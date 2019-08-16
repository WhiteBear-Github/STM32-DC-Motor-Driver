#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "pid.h"
#include "usart.h"
#include "wave.h"
#include "lcd.h"
#include "eeprom.h"

 u8 j;      //����ѡ��ָ��ֵ
 
 
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE4,3

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
	
}

//�����ⲿ�жϺ���
void KEY_EXTI_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  KEY_Init();	 //	�����˿ڳ�ʼ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ���ⲿ�жϹ���ʱ��

	//GPIOE.3	  �ж����Լ��жϳ�ʼ������ �½��ش��� //KEY1
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  //GPIOE.4	  �ж����Լ��жϳ�ʼ������  �½��ش���	//KEY0
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  //GPIOA.0	  �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
  EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0�� 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure); 
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0 
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�1 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
 	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ���KEY0���ڵ��ⲿ�ж�ͨ��
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0 
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2 
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
}

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	static u8 i = 0;
	delay_ms(10);//����
	if(WK_UP==1)	 	 //WK_UP����
	{	
		switch(i)
		{
			case 0 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Kp");break;//��ǰ����Ŀ��ΪKp������LCD���Ͻ���ʾ
			case 1 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Ki");break;//��ǰ����Ŀ��ΪKi������LCD���Ͻ���ʾ
			case 2 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Kd");break;//��ǰ����Ŀ��ΪKd������LCD���Ͻ���ʾ
			case 3 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(310,1,20,20,12,"V");break;//��ǰ����Ŀ��ΪV������LCD���Ͻ���ʾ
			case 4 : j = i; i=0;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"WR");break;//��ǰ����Ŀ��ΪEEPROM��д������LCD���Ͻ���ʾ
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}

//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY1==0)	 //����KEY1
	{				 
		switch(j)
		{
			case 0 : PID.Kp += 0.1;printf("��ǰKpΪ%f",PID.Kp);ShowKp();break;
			case 1 : PID.Ki += 0.1;printf("��ǰKiΪ%f",PID.Ki);ShowKi();break;
			case 2 : PID.Kd += 0.1;printf("��ǰKdΪ%f",PID.Kd);ShowKd();break;
			case 3 : Clean_Aim(); PID.Rin += 5;Aim();break;
			case 4 : Write_PID();break;//�����ʵ�������ɫ�����ڻ�����
		}				
	}
	
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY0==0)	 //����KEY0
	{
		switch(j)
		{
			case 0 : PID.Kp -= 0.1;printf("��ǰKpΪ%f",PID.Kp);ShowKp();break;
			case 1 : PID.Ki -= 0.1;printf("��ǰKiΪ%f",PID.Ki);ShowKi();break;
			case 2 : PID.Kd -= 0.1;printf("��ǰKdΪ%f",PID.Kd);ShowKd();break;
			case 3 : Clean_Aim(); PID.Rin -= 5;Aim();break;
			case 4 : Read_PID();break;//��LCD���Ͻ���ʾ��ROK����־
		}
	}		
	
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}

