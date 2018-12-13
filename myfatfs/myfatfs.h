#ifndef __MYFATFS_H
#define __MYFATFS_H

#include "ff.h"
#include "usart.h"
#include "led.h"

//����������
void SD_FAT_Initialize(const TCHAR* path);  //�ļ�ϵͳ��ʼ������,����SD��
void SD_FAT_WriteInitialize(const TCHAR* path,const char* filetitle);//�ļ�д����ԣ�ͬʱ��������д��
void SD_FAT_WriteExisiting(	const TCHAR* path,const void *buff);//�ļ�׷�����ݺ���
void SD_FAT_Read(	const TCHAR* path/*,char* buff*/);//�ļ����ݶ�ȡ����
void SD_FAT_DeleteFile(TCHAR* path);//�ļ�ɾ������
void SD_FAT_DISMOUNT(const TCHAR* path);//ȡ�����غ���
//void SD_FAT_STAT();







#endif


