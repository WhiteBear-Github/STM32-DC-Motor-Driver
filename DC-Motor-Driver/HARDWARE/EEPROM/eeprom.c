#include "eeprom.h"
#include "delay.h"
#include "pid.h"
#include "lcd.h"
#include "usart.h"
#include "wave.h"

//24C02����256���ֽڴ�С����ַΪ0~255
//�������ݣ���ַд���С����д
/************************************************************
24C02��255λ������־λ����Ҫ�ڴ�λд���������ݣ����������������
*************************************************************/


//��ʼ��IIC�ӿ�
void AT24C02_Init(void)
{
	IIC_Init();
}

//��AT24C02ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  ��ַ��ΧΪ0~255
//����ֵ  :����������
u8 AT24C02_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;	
	
  IIC_Start();  
	IIC_Send_Byte(0XA0);   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack(); 
	
  IIC_Send_Byte(ReadAddr%256);   //����24C02�ڲ���ַ����256���ֽڣ�256����ַ��0~255�����趨�����
	IIC_Wait_Ack();	  
  
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
	
  temp=IIC_Read_Byte(0);	  //��ȡһ���ֽ����ݣ�������NACK	   
  IIC_Stop();//����һ��ֹͣ����	
	
	return temp;
}

//��AT24C02ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24C02_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start(); 
	IIC_Send_Byte(0XA0);   //����������ַ0XA0,д����  
	IIC_Wait_Ack();
	
  IIC_Send_Byte(WriteAddr%256);   //����24C02�ڲ���ַ����256���ֽڣ�256����ַ��0~255�����趨�����
	IIC_Wait_Ack(); 	 						
	
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
	
  IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

//��AT24C02�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24C02_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24C02_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24C02�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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

//���AT24C02�Ƿ�����
//��������2402�����һ����ַ(255)���洢��־��.
//����1:���ʧ��
//����0:���ɹ�
u8 AT24C02_Check(void)
{
	u8 temp;
	temp=AT24C02_ReadOneByte(255);//����ÿ�ο�����дAT24C02			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24C02_WriteOneByte(255,0X55);
	    temp=AT24C02_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24C02�����ָ����ַ��ʼ����ָ������������,����������
//ReadAddr :��ʼ�����ĵ�ַ ��24C02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24C02_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24C02_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24C02�����ָ����ַ��ʼд��ָ�����������ݣ���������д��
//WriteAddr :��ʼд��ĵ�ַ ��24C02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24C02_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24C02_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

//PID���� ��ʵ��ת��Ϊ����
void PID_NumToArray(void)
{
	u8 integer;   //����
	u8 dec;//С��
	
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
	static u8 ok =0;//�л�д��ɹ���־"ok"��ɫ����������
	
	PID_NumToArray();  //��PID����ת��������
	
	AT24C02_Write(0,kp,5); //��PID����д��24C02
	AT24C02_Write(6,ki,5);
	AT24C02_Write(11,kd,5); 
	if(ok)  //��LCD���Ͻ���ʾ������ɫ��WOK����־
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
	POINT_COLOR = BLUE;//�����ʵ�������ɫ�����ڻ�����
}

void Read_PID()
{
	AT24C02_Read(0,kp,5);       //�ֱ��ȡ24C02�洢��PID����(������ת��Ϊ������)
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
	
	printf("��ǰKpΪ%f",PID.Kp);  //�ڴ��ں�LCD����ʾ����ֵ
	printf("��ǰKiΪ%f",PID.Ki);
	printf("��ǰKdΪ%f",PID.Kd);
	ShowKp();
	ShowKi();
	ShowKd(); 
	
	LCD_ShowString(285,1,15,20,12,"ROK");//��LCD���Ͻ���ʾ��ROK����־
}
	
