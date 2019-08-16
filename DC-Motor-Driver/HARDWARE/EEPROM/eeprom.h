#ifndef __24CXX_H
#define __24CXX_H
#include "iic.h"   

//24C02大小为 256个字节
#define AT24C02		255
					  
u8 AT24C02_ReadOneByte(u16 ReadAddr);							//指定地址读取一个字节
void AT24C02_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void AT24C02_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 AT24C02_ReadLenByte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据
void AT24C02_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据，从数组中写入
void AT24C02_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据，读入数组中

u8 AT24C02_Check(void);  //检查器件 
void AT24C02_Init(void); //初始化IIC

void PID_NumToArray(void);//PID参数 由实数转换为数组
void Read_PID(void);  //读取PID参数
void Write_PID(void); //写入PID参数
#endif
