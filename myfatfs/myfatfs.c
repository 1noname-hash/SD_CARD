#include "myfatfs.h"
#include "string.h"
/**********************************
				�ļ�ϵͳ��������
**********************************/
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_sd;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
FILINFO* filinfo;
/**********************************
					���ݻ�����
**********************************/
BYTE ReadBuffer[512]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
{"ID01,2018/12/03 11:00,25,27,26,31,30,\nID02,2018/12/03 11:21,24,44,32,31,25,\nID03,2018/12/03 11:35,53,34,54,23,54,\nID04,2018/12/03 11:55,33,42,15,34,23\n"
};  



/***********�ļ�ϵͳ����***********************************************/

/**********************************
�ļ�ϵͳ��ʼ������
���ܣ�����SD�����أ����sd�Ƿ�����ļ�ϵͳ��
���������path    (Logical drive number to be mounted/unmounted )
����ֵ���ޣ�
ʾ����"0:"
**********************************/
void SD_FAT_Initialize(
	const TCHAR* path	/* Logical drive number to be mounted/unmounted */
)
{
	res_sd = f_mount(&fs,path,1);  //����SD��
	/*----------------------- ��ʽ������ ---------------------------*/  
		/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
		if(res_sd == FR_NO_FILESYSTEM)
		{
			Uart1_SendStr("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
			/* ��ʽ�� */
			res_sd=f_mkfs(path,0,0);							
			
			if(res_sd == FR_OK)
			{
				Uart1_SendStr("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
				/* ��ʽ������ȡ������ */
				res_sd = f_mount(NULL,path,1);			
				/* ���¹���	*/			
				res_sd = f_mount(&fs,path,1);
			}
			else
			{
				LED_D1_0;
				Uart1_SendStr("������ʽ��ʧ�ܡ�����\r\n");
				while(1);
			}
		}
		else 
			if(res_sd!=FR_OK)
			{
				printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
				Uart1_SendStr("��������ԭ��SD����ʼ�����ɹ���\r\n");
				while(1);
			}
			else
			{
				Uart1_SendStr("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
			}
}		


/**********************************
д������ʼ������
���ܣ��ļ�д����ԣ��������ļ���д�����ݵı�����
���������path��(Pointer to the file name)
					filetitle  �ļ���������
����ֵ���ޣ�
ʾ����"0:TEMP1.CSV"
			"ID.Noumber,Year/Month/Day Hour:Minute:Second,Temp1,Temp2,Temp3,Temp4,Temp5\n"
**********************************/
/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
void SD_FAT_WriteInitialize(
	const TCHAR* path,/* Pointer to the file name */
	const char* filetitle /*�ļ���������*/
)
{
	/* ���ļ�������ļ��������򴴽��� */
	Uart1_SendStr("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_sd = f_open(&fnew, path,FA_CREATE_NEW | FA_WRITE ); //�����ļ�����Ϊ���������������
	if ( res_sd == FR_OK )
	{
		Uart1_SendStr("����/����FatFs��д�����ļ�.csv�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		//д���ļ�������,�Զ������ֽ���
		res_sd=f_write(&fnew,filetitle,strlen(filetitle),&fnum);
    if(res_sd==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_sd);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);	
	}
	else
	{	
		LED_D1_0;
		Uart1_SendStr("������/�����ļ�ʧ�ܡ�\r\n");
	}
}


/**********************************
�ļ�׷�����ݺ���
���ܣ�����д�뵽ָ���ļ�������ʵ���������ļ�׷�����ݣ�
���������path��(Pointer to the file name)
					buff  ���ݻ�������ַ
����ֵ���ޣ�
ʾ����"0:TEMP1.CSV"
			WriteBuffer
**********************************/
void SD_FAT_WriteExisiting(
	const TCHAR* path,/* Pointer to the file name */
	const void *buff	/* Pointer to the data to be written */
//	UINT btw
)
{
		res_sd=f_open(&fnew, path,FA_OPEN_EXISTING | FA_WRITE );//���Ѿ����ڵ��ļ���ִ��д�����
		res_sd=f_lseek(&fnew,f_size(&fnew)); //����ָ�룬ʵ������׷��
		res_sd=f_write(&fnew,buff,strlen(buff),&fnum);
		f_close(&fnew);			//�ļ�д�����֮����Ҫ�ر��ļ�
}


/**********************************
�ļ����ݶ�ȡ����
���ܣ����ݶ�ȡ��ָ��������
���������path��(Pointer to data buffer)
					buff  ���ݻ�������ַ
����ֵ���ޣ�
ʾ����"0:TEMP1.CSV"
			ReadBuffer
**********************************/
/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
void SD_FAT_Read(
	const TCHAR* path/* Pointer to the file name */
//	char* buff	/* Pointer to data buffer */
)
{
	
	Uart1_SendStr("****** ���������ļ���ȡ����... ******\r\n");
	res_sd = f_open(&fnew, path, FA_OPEN_EXISTING | FA_READ); 	 
	if(res_sd == FR_OK)
	{
		LED_D2_0;
		Uart1_SendStr("�����ļ��ɹ���\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      Uart1_SendStr("�����ļ���ȡʧ�ܣ�\n");
    }		
	}
	else
	{
		LED_D1_0;
		Uart1_SendStr("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	

}



/**********************************
//SD���ļ��м��ļ�ɾ������
//���ܣ�ɾ���ļ�
//����������ַ��������ļ�·��
//����ֵ����
**********************************/
void SD_FAT_DeleteFile(TCHAR* path)
{
	FS_DeleteFolderOrFile(path);
}



/**********************************
ȡ�����غ���
���ܣ�����ʹ���ļ�ϵͳ
���������path��(Pointer to data buffer)
����ֵ���ޣ�
ʾ����"0:"
**********************************/
void SD_FAT_DISMOUNT(const TCHAR* path)
{
		/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,path,1);
	/* ������ɣ�ͣ�� */
}





















//void SD_FAT_STAT()
//{
//		res_sd = f_stat("TEMP1.CSV", filinfo); 
//		if( res_sd )
//		printf("newname.CSV err : %d\r\n", res_sd);
//		else
//		{
//		printf("newname.CSV size : %lu\r\n",filinfo->fsize);
//		printf("fdate : %d\r\n",filinfo->fdate); 
//		printf("ftime : %d\r\n",filinfo->ftime);
//		printf("fattrib : %d\r\n",filinfo->fattrib);
//		}
//}

