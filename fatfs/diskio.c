/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"		/* FatFs lower layer API */
#include "sd.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "stm32l1xx.h"
/* Definitions of physical drive number for each drive */
#define SD_CARD		0	/* Example: Map MMC/SD card to physical drive 0 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
//��ȡ����״̬
DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case SD_CARD:	/* SD CARD */
			status &= ~STA_NOINIT;
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
//��ʼ������
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	u8 res=0;	    
		switch(pdrv)
		{
			case SD_CARD://SD��
				res = SD_Initialize();//SD_Initialize() 
				if(res)//STM32 SPI��bug,��sd������ʧ�ܵ�ʱ�������ִ����������,���ܵ���SPI��д�쳣
				{
					SD_SPI_SpeedLow();
					SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
					SD_SPI_SpeedHigh();
				}
					break;
			default:
				res=1; 
		}		 
		if(res)return  STA_NOINIT;
		else return 0; //��ʼ���ɹ�
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
//������
//drv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res=0; 
			if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
		switch(pdrv)
		{
			case SD_CARD://SD��
				res=SD_ReadDisk(buff,sector,count);	 
				if(res)//STM32 SPI��bug,��sd������ʧ�ܵ�ʱ�������ִ����������,���ܵ���SPI��д�쳣
				{
					SD_SPI_SpeedLow();
					SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
					SD_SPI_SpeedHigh();
				}
				break;
			default:
				res=1; 
		}
		 //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
			if(res==0x00)return RES_OK;	 
			else return RES_ERROR;	
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
//д����
//drv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд���������
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res=0;  
			if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
		switch(pdrv)
		{
			case SD_CARD://SD��
				res=SD_WriteDisk((u8*)buff,sector,count);
				break;
			default:
				res=1; 
		}
			//������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
			if(res == 0x00)return RES_OK;	 
			else return RES_ERROR;	
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
//����������Ļ��
 //drv:���̱��0~9
 //ctrl:���ƴ���
 //*buff:����/���ջ�����ָ��
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						  			     
	if(pdrv==SD_CARD)//SD��
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				SD_CS_LOW();
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				SD_CS_HIGH();
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res=RES_ERROR;//�����Ĳ�֧��
    return res;
}
#endif

//���ʱ��
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
////��̬�����ڴ�
//void *ff_memalloc (UINT size)			
//{
//	return (void*)mymalloc(size);
//}
////�ͷ��ڴ�
//void ff_memfree (void* mf)		 
//{
//	myfree(mf);
//}

