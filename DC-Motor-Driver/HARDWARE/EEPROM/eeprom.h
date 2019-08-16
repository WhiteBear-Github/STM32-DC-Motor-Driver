#ifndef __24CXX_H
#define __24CXX_H
#include "iic.h"   

//24C02��СΪ 256���ֽ�
#define AT24C02		255
					  
u8 AT24C02_ReadOneByte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void AT24C02_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
void AT24C02_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 AT24C02_ReadLenByte(u16 ReadAddr,u8 Len);					//ָ����ַ��ʼ��ȡָ����������
void AT24C02_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ����ݣ���������д��
void AT24C02_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ����ݣ�����������

u8 AT24C02_Check(void);  //������� 
void AT24C02_Init(void); //��ʼ��IIC

void PID_NumToArray(void);//PID���� ��ʵ��ת��Ϊ����
void Read_PID(void);  //��ȡPID����
void Write_PID(void); //д��PID����
#endif
