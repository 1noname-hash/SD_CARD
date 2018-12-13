#include "spi.h"

void SD_LowLevel_DeInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	SPI_Cmd(SD_SPI1, DISABLE); /*!< SD_SPI disable */
	SPI_DeInit(SD_SPI1);   /*!< DeInitializes the SD_SPI */
	
	RCC_APB2PeriphClockCmd(SD_SPI1_CLK, DISABLE); //ʱ��ʧ��
	//����spi����
	GPIO_InitStructure.GPIO_Pin = SD_SPI1_SCK_PIN|SD_SPI1_MISO_PIN|SD_SPI1_MOSI_PIN|SD_SPI1_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SD_SPI1_GPIO_PORT, &GPIO_InitStructure);
}

void SD_LowLevel_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
	//GPIO��SPIʱ��ʹ��
	RCC_AHBPeriphClockCmd(SD_SPI1_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(SD_SPI1_CLK, ENABLE); 
	//����SPI����
	GPIO_InitStructure.GPIO_Pin = SD_SPI1_SCK_PIN|SD_SPI1_MISO_PIN|SD_SPI1_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(SD_SPI1_GPIO_PORT, &GPIO_InitStructure);
	//CS����
	GPIO_InitStructure.GPIO_Pin = SD_SPI1_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(SD_SPI1_GPIO_PORT, &GPIO_InitStructure);
	//���ñ��ù���
   /* Connect PXx to SD_SPI_SCK */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
 
   /* Connect PXx to SD_SPI_MISO */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); 
 
   /* Connect PXx to SD_SPI_MOSI */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);	
	
	/*!< SD_SPI Config */
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
 
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SD_SPI1, &SPI_InitStructure);
   
   SPI_Cmd(SD_SPI1, ENABLE); /*!< SD_SPI enable */
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256��Ƶ (SPI 281.25K@sys 72M) 
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI1,ENABLE); 

} 


//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������

	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����					    
}
