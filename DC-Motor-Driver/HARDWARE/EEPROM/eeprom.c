#include "eeprom.h"
#include "delay.h"
#include "pid.h"
#include "lcd.h"
#include "usart.h"
#include "wave.h"

//24C02共有256个字节大小，地址为0~255
//连续数据，地址写入从小往大写
/************************************************************
24C02的255位用作标志位，不要在此位写入其他数据，特殊情况另做处理
*************************************************************/


//初始化IIC接口
void AT24C02_Init(void)
{
	IIC_Init();
}

//在AT24C02指定地址读出一个数据
//ReadAddr:开始读数的地址  地址范围为0~255
//返回值  :读到的数据
u8 AT24C02_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;	
	
  IIC_Start();  
	IIC_Send_Byte(0XA0);   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack(); 
	
  IIC_Send_Byte(ReadAddr%256);   //发送24C02内部地址（共256个字节，256个地址，0~255），设定超额保护
	IIC_Wait_Ack();	  
  
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
	
  temp=IIC_Read_Byte(0);	  //读取一个字节数据，并返回NACK	   
  IIC_Stop();//产生一个停止条件	
	
	return temp;
}

//在AT24C02指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24C02_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start(); 
	IIC_Send_Byte(0XA0);   //发送器件地址0XA0,写数据  
	IIC_Wait_Ack();
	
  IIC_Send_Byte(WriteAddr%256);   //发送24C02内部地址（共256个字节，256个地址，0~255），设定超额保护
	IIC_Wait_Ack(); 	 						
	
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
	
  IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

//在AT24C02里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24C02_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24C02_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24C02里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24C02_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24C02_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

//检查AT24C02是否正常
//这里用了2402的最后一个地址(255)来存储标志字.
//返回1:检测失败
//返回0:检测成功
u8 AT24C02_Check(void)
{
	u8 temp;
	temp=AT24C02_ReadOneByte(255);//不必每次开机都写AT24C02			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24C02_WriteOneByte(255,0X55);
	    temp=AT24C02_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24C02里面的指定地址开始读出指定个数的数据,读入数组中
//ReadAddr :开始读出的地址 对24C02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24C02_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24C02_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24C02里面的指定地址开始写入指定个数的数据，从数组中写入
//WriteAddr :开始写入的地址 对24C02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24C02_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24C02_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

//PID参数 由实数转换为数组
void PID_NumToArray(void)
{
	u8 integer;   //整数
	u8 dec;//小数
	
	if(PID.Kp  < 0)
	{
		kp[0] = '-';
		PID.Kp = -PID.Kp;
	}
	else
		kp[0] = ' ';
	
	integer = (u8)PID.Kp;
	dec = (u8)(PID.Kp*10)%10;
	
	kp[1] = integer + '0';
	kp[2] = '.';
	kp[3] = dec + '0';	
	kp[4] = '\0';
	
	if(PID.Ki  < 0)
	{
		ki[0] = '-';
		PID.Ki = -PID.Ki;
	}
	else
		ki[0] = ' ';
	
	integer = (u8)PID.Ki;
	dec = (u8)(PID.Ki*10)%10;
	
	ki[1] = integer + '0';
	ki[2] = '.';
	ki[3] = dec + '0';	
	ki[4] = '\0';
	
	if(PID.Kd  < 0)
	{
		kd[0] = '-';
		PID.Kd = -PID.Kd;
	}
	else
		kd[0] = ' ';
	
	integer = (u8)PID.Kd;
	dec = (u8)(PID.Kd*10)%10;
	
	kd[1] = integer + '0';
	kd[2] = '.';
	kd[3] = dec + '0';	
	kd[4] = '\0';
}

void Write_PID()
{
	static u8 ok =0;//切换写入成功标志"ok"颜色，便于区分
	
	PID_NumToArray();  //将PID参数转换成数组
	
	AT24C02_Write(0,kp,5); //将PID参数写入24C02
	AT24C02_Write(6,ki,5);
	AT24C02_Write(11,kd,5); 
	if(ok)  //在LCD右上角显示红蓝两色“WOK”标志
	{
		POINT_COLOR = RED;
		LCD_ShowString(285,1,15,20,12,"WOK");
		ok=!ok;
	}	
	else  
  {
		POINT_COLOR = BLUE;
		LCD_ShowString(285,1,15,20,12,"WOK");
		ok=!ok;
	} 
	POINT_COLOR = BLUE;//将画笔调整回蓝色，用于画波形
}

void Read_PID()
{
	AT24C02_Read(0,kp,5);       //分别读取24C02存储的PID参数(将数组转换为浮点数)
	PID.Kp = (u8)(kp[1] - '0') + (double)((kp[3] - '0')/10.0); 
	if(kp[0] == '-')
	{
		PID.Kp = -PID.Kp;
	}
	
	AT24C02_Read(6,ki,5);
	PID.Ki = (u8)(ki[1] - '0') + (double)((ki[3] - '0')/10.0); 
	if(ki[0] == '-')
	{
		PID.Ki = -PID.Ki;
	}
	
	AT24C02_Read(11,kd,5); 
	PID.Kd = (u8)(kd[1] - '0') + (double)((kd[3] - '0')/10.0); 
	if(kd[0] == '-')
	{
		PID.Kd = -PID.Kd;
	}
	
	printf("当前Kp为%f",PID.Kp);  //在串口和LCD上显示参数值
	printf("当前Ki为%f",PID.Ki);
	printf("当前Kd为%f",PID.Kd);
	ShowKp();
	ShowKi();
	ShowKd(); 
	
	LCD_ShowString(285,1,15,20,12,"ROK");//在LCD右上角显示“ROK”标志
}
	
