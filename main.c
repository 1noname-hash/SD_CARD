#include "main.h"
#include "usart.h"
#include "led.h"
#include "stdlib.h"
#include "string.h"
#include "delay.h"
#include "spi.h"
#include "sd.h"
#include "ff.h"
#include "myfatfs.h"

/**
  ******************************************************************************
  *                            ������չ����
  ******************************************************************************
  */
extern FATFS fs;													/* FatFs�ļ�ϵͳ���� */
extern FIL fnew;													/* �ļ����� */
extern FRESULT res_sd;                /* �ļ�������� */
extern UINT fnum;            					  /* �ļ��ɹ���д���� */
extern BYTE WriteBuffer[];
extern BYTE ReadBuffer[512];


int main(void)
{		 
//	uint32_t sd_size;
//	u8 t=0;	 
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	LED_D1_1;
	LED_D2_1;
	
		printf("\r\n****** ����һ��SD�� �ļ�ϵͳʵ�� ******\r\n");
	//���ⲿSD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SDIO�豸��ʼ��
	
	SD_FAT_Initialize("0:");
	SD_FAT_DeleteFile("TEMP1.CSV");

	SD_FAT_WriteInitialize("0:TEMP1.CSV","ID.Noumber,Year/Month/Day Hour:Minute:Second,Temp1,Temp2,Temp3,Temp4,Temp5\n");
	SD_FAT_WriteExisiting("0:TEMP1.CSV",WriteBuffer);
	SD_FAT_Read("0:TEMP1.CSV");

	
	SD_FAT_DISMOUNT("0:");
	
  
  
	while(1)
	{
	}    
	
}




#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

