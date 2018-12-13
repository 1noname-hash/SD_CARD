#include "sd.h"
#include "usart.h"

u8  SD_Type=0;//SD�������� 
////////////////////////////////////��ֲ�޸���///////////////////////////////////
//��ֲʱ��Ľӿ�
//data:Ҫд�������
//����ֵ:����������
u8 SD_SPI_ReadWriteByte(u8 data)
{
	return SPI1_ReadWriteByte(data);
}	  

//SD����ʼ����ʱ��,��Ҫ����
void SD_SPI_SpeedLow(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_128);//���õ�����ģʽ	
}

//SD������������ʱ��,���Ը�����
void SD_SPI_SpeedHigh(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//���õ�����ģʽ	
}

void SD_SPI_Init(void)
{
	SD_LowLevel_Init();
	//SD��ƬѡCS�ø�
	SD_CS_HIGH();
}
///////////////////////////////////////////////////////////////////////////////////
//ȡ��ѡ��,�ͷ�SPI����
void SD_DisSelect(void)
{
	SD_CS_HIGH();
 	SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
}
//ѡ��sd��,���ҵȴ���׼��OK
//����ֵ:0,�ɹ�;1,ʧ��;
u8 SD_Select(void)
{
	SD_CS_LOW();
	if(SD_WaitReady()==0)return 0;//�ȴ��ɹ�
	SD_DisSelect();
	return 1;//�ȴ�ʧ��
}
//�ȴ���׼����
//����ֵ:0,׼������;����,�������
u8 SD_WaitReady(void)
{
	u32 t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
		t++;		  			
	}while(t<0XFFFFFF);//�ȴ� 
	return 1;
}
//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//    ����,�õ���Ӧֵʧ��
u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFFFF;//�ȴ�����	   						  
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//�ȴ��õ�׼ȷ�Ļ�Ӧ  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//�õ���Ӧʧ��   
	else return MSD_RESPONSE_NO_ERROR;//��ȷ��Ӧ
}
//��sd����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�;����,ʧ��;	
u8 SD_RecvData(u8*buf,u16 len)
{			  	  
	if(SD_GetResponse(0xFE))return 1;//�ȴ�SD������������ʼ����0xFE
    while(len--)//��ʼ��������
    {
        *buf=SPI1_ReadWriteByte(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);									  					    
    return 0;//��ȡ�ɹ�
}
//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
//����ֵ:0,�ɹ�;����,ʧ��;	
u8 SD_SendBlock(u8*buf,u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady())return 1;//�ȴ�׼��ʧЧ
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//���ǽ���ָ��
	{
		for(t=0;t<512;t++)SPI1_ReadWriteByte(buf[t]);//����ٶ�,���ٺ�������ʱ��
	    SD_SPI_ReadWriteByte(0xFF);//����crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//������Ӧ
		if((t&0x1F)!=0x05)return 2;//��Ӧ����									  					    
	}						 									  					    
  return 0;//д��ɹ�
}
//��SD������һ������
//����: u8 cmd   ���� 
//      u32 arg  �������
//      u8 crc   crcУ��ֵ	   
//����ֵ:SD�����ص���Ӧ			
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
  u8 r1;	
	u8 Retry=0; 
	SD_DisSelect();//ȡ���ϴ�Ƭѡ
	if(SD_Select())return 0XFF;//ƬѡʧЧ 
	//����
    SD_SPI_ReadWriteByte(cmd | 0x40);//�ֱ�д������
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
	Retry=0X1F;
	do
	{
		r1=SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 
	//����״ֵ̬
    return r1;
}		    						
//��ȡSD����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0��NO_ERR
//		 1������														   
u8 SD_GetCID(u8 *cid_data)
{
    u8 r1;	   
    //��CMD10�����CID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����	 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}																				  
//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:0��NO_ERR
//		 1������														   
u8 SD_GetCSD(u8 *csd_data)
{
    u8 r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
    if(r1==0)
	  {
    	r1=SD_RecvData(csd_data, 16);//����16���ֽڵ����� 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}  
//��ȡSD����������������������   
//����ֵ:0�� ȡ�������� 
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.														  
u32 SD_GetSectorCount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;	
	//���SD�����ݿ��С��������д�룬��ȡ
		printf("\r\nSD�����д���ݿ�Ĵ�С�� %d B\r\n",1<<(((csd[12]&0x03)<<2)|((csd[13]&0xc0)>>6)));
		printf("SD�����ֿ�д������ %x��������\r\n",(csd[13]&0x20));
		printf("\r\nSD���������ݿ�Ĵ�С�� %d B\r\n",1<<(csd[5]&0x0F));
		printf("SD�����ֿ��ȡ���� %x��������\r\n",(csd[6]&0x80));
		printf("\r\n����������ʹ�ܣ� %x \r\n",(csd[10]&0x40)>>6);
		printf("����������С�� %d B\r\n",(((csd[10]&0x3F)<<1)|(csd[11]&0x80)>>7));

    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//�õ�������	 		   
    }else//V1.XX�Ŀ�
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//�õ�������   
    }
    return Capacity;
}
//��ʼ��SD��
u8 SD_Initialize(void)
{
	u8 r1;      // ���SD���ķ���ֵ
	u16 retry;  // �������г�ʱ����
	u8 buf[4];  
	u16 i;	
	
	//��ʼ��SPI
	SD_SPI_Init();
	
	SD_SPI_SpeedLow();	//���õ�����ģʽ
	for(i=0;i<10;i++)SD_SPI_ReadWriteByte(0XFF);//��������74������
	retry=20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//����IDLE״̬
	}while((r1!=0X01) && retry--);//�ж�CMD0����Ӧ�ǲ���0x01��ȷ�����λ�Ƿ���λ��1�����ڿ���״̬.
 	SD_Type=0;//Ĭ���޿�
	if(r1==0X01)  //SD�����ڿ���ģʽ
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0    0x1AA���趨��ѹ��Χ2.7~3.6V��ȷ�����ģʽΪ��AA��  //�����֧��CMD8���ͻ�����Ӧ
		{
			/***********NOTE******************/
			//����ΪʲôCMD8����ӦֵҪ��1�أ�
			//��ΪCMD8����Ӧ��R7��R7��Ӧ��5���ֽڣ�����ֽ���R1��Ӧ��ͬ��R1��Ӧ��SPIģʽ�½���1���ֽڣ���
			//���⣬���ݴ���ĸ�ʽ���ȷ��͸��ֽڣ����͵��ֽڣ����Դ��ڿ���ģʽ��SD����R1��Ӧ��ֵ����1;
			/*********************************/
			//���濪ʼ�ж�R7��Ӧ�ĺ���4���ֽڣ�
			//SD�����ݸ�ʽ�������ǳ������ݸ�ʽ���������ô��ģʽ��MSB��Ҳ����˵���͵�ַ�洢���ֽڣ��ߵ�ַ�洢���ֽڣ�ÿһ�ֽ������Чλ�ȴ���
			for(i=0;i<4;i++) buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp; ����iΪ4������ΪR7��5Byte������һ���ֽ���R1��ͬ		
			if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
			{
				retry=0XFFFE;
				//ACMD41ָ����Ӧ��R1���������ʼ����ɣ�R1��Ӧ������״̬λ�����λ�������㣬�Ը�������SD����ʼ��������
				//�����λ��Ϊ0����ѭ������ACMD41����ȡR1��Ӧ��ֱ����λΪ0.
				//ACMD41�Ĺ��ܣ��˳�����״̬��
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55  ��ӦΪR1
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//���� ACMD41		��������HCS���Ƿ�֧�ָ�������		
				}while(r1&&retry--);	
				//����if����жϵĻ���R3��Ӧ�ĵ�һ���ֽڣ���R1��ͬ������ʱ�����ǿ���״̬�������ж��Ƿ�Ϊ0
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
				{					
					for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//�õ�OCRֵ
					//���CCSֵ
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //���CCSλΪ1������SDHC����/***SD�������ͣ�SD_TYPE_V2HC******/
					else SD_Type=SD_TYPE_V2;               //���CCSλΪ0������SDSC����/***SD�������ͣ�SD_TYPE_V2******/
				}
			}else printf("\r\n��֧�ָõ�ѹ��Χ!\r\n");
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);		//����CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//����ACMD41
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1; /***SD�������ͣ�SD_TYPE_V1******/
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
				}while(r1&&retry--);
			}else//MMC����֧��CMD55+CMD41ʶ��
			{
				SD_Type=SD_TYPE_MMC;/***SD�������ͣ�MMC V3******/
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//����CMD1
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;/***SD�������ͣ�����Ŀ� ******/
		}
	}
	
	//��ӡSD������
//	switch(SD_Type)
//	{
//		case SD_TYPE_ERR:printf("Card Type:SD_TYPE_ERR\r\n");break;
//		case SD_TYPE_MMC:printf("Card Type:SD_TYPE_MMC\r\n");break;
//		case SD_TYPE_V1:printf("Card Type:SD_TYPE_V1\r\n");break;
//		case SD_TYPE_V2:printf("Card Type:SD_TYPE_V2\r\n");break;
//		case SD_TYPE_V2HC:printf("Card Type:SD_TYPE_V2HC\r\n");break;
//		default: break;
//	}
	
	SD_DisSelect();//ȡ��Ƭѡ
	SD_SPI_SpeedHigh();//����
	
	if(SD_Type)return 0;
	else if(r1)return r1; 	   
	return 0xaa;//��������
}
//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//ת��Ϊ�ֽڵ�ַ   ������SD�����Կ�Ϊ��λ��������Ǹ������ģ���Ҫ����ת��Ϊ���ֽ�Ϊ��λ��
	if(cnt==1)  //�������ݶ�ȡ
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//������   CMD17ָ����ӦΪR1��
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�	   
		}
	}else
	 {
			r1=SD_SendCmd(CMD18,sector,0X01);//����������
			do
			{
				r1=SD_RecvData(buf,512);//����512���ֽ�	 
				buf+=512;  
			}while(--cnt && r1==0); 	
			SD_SendCmd(CMD12,0,0X01);	//����ֹͣ����
	 }   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}
//дSD��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);//������
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_SendBlock(buf,0xFE);//д512���ֽ�	   
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//����Ԥ����ָ��ACMD23����ָ�����SD����Ч����MMC����Ч��
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//����������
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//����512���ֽ�	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//����512���ֽڣ����д���ݽ�������0xFD��
		}
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}	

//���룺��������ʼ������ַ������������ַ
//����ֵ�������Ƿ�ɹ�
u8 SD_erase_sector(u32 sector_start, u32 sector_stop)	//�����ʼ�ͽ�����ַ��ͬһֵʱ��ֻ������һ������
{
    
	SD_DisSelect(); //��Ч��һ��Ƭѡ
	
	//����CMD32�����ò�����������ʼ��ַ
	//����Ӧ��Ϊ00
	if(SD_SendCmd(CMD32,sector_start,0xff))
	   {
		  SD_DisSelect(); //��Ч��һ��Ƭѡ
		  return MSD_ERASE_SECTOR_ERROR;
	   }
 
	//����CMD33�����ò�����������ֹ��ַ
	//����Ӧ��Ϊ00
	if(SD_SendCmd(CMD33,sector_stop,0xff))
	   {
		  SD_DisSelect();  //��Ч��һ��Ƭѡ
		  return MSD_ERASE_SECTOR_ERROR;
	   }
	//����CMD38����������
	//����Ӧ��Ϊ00
	if(SD_SendCmd(CMD38,0,0xff))
	   {
		  SD_DisSelect();  //��Ч��һ��Ƭѡ
		  return MSD_ERASE_SECTOR_ERROR;
	   }
	SD_DisSelect();
  return MSD_ERASE_SECTOR_SUCCESS;
}



