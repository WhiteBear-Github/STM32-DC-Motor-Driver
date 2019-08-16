#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "pid.h"
#include "usart.h"
#include "wave.h"
#include "lcd.h"
#include "eeprom.h"

 u8 j;      //按键选择指引值
 
 
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4,3

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
	
}

//按键外部中断函数
void KEY_EXTI_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  KEY_Init();	 //	按键端口初始化

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能外部中断功能时钟

	//GPIOE.3	  中断线以及中断初始化配置 下降沿触发 //KEY1
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  //GPIOE.4	  中断线以及中断初始化配置  下降沿触发	//KEY0
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  //GPIOA.0	  中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
  EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键WK_UP所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0， 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); 
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//使能按键KEY1所在的外部中断通道
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0 
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级1 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
 	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能按键KEY0所在的外部中断通道
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0 
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级2 
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
}

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	static u8 i = 0;
	delay_ms(10);//消抖
	if(WK_UP==1)	 	 //WK_UP按键
	{	
		switch(i)
		{
			case 0 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Kp");break;//当前调整目标为Kp，并在LCD右上角显示
			case 1 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Ki");break;//当前调整目标为Ki，并在LCD右上角显示
			case 2 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Kd");break;//当前调整目标为Kd，并在LCD右上角显示
			case 3 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(310,1,20,20,12,"V");break;//当前调整目标为V，并在LCD右上角显示
			case 4 : j = i; i=0;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"WR");break;//当前调整目标为EEPROM读写，并在LCD右上角显示
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}

//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY1==0)	 //按键KEY1
	{				 
		switch(j)
		{
			case 0 : PID.Kp += 0.1;printf("当前Kp为%f",PID.Kp);ShowKp();break;
			case 1 : PID.Ki += 0.1;printf("当前Ki为%f",PID.Ki);ShowKi();break;
			case 2 : PID.Kd += 0.1;printf("当前Kd为%f",PID.Kd);ShowKd();break;
			case 3 : Clean_Aim(); PID.Rin += 5;Aim();break;
			case 4 : Write_PID();break;//将画笔调整回蓝色，用于画波形
		}				
	}
	
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY0==0)	 //按键KEY0
	{
		switch(j)
		{
			case 0 : PID.Kp -= 0.1;printf("当前Kp为%f",PID.Kp);ShowKp();break;
			case 1 : PID.Ki -= 0.1;printf("当前Ki为%f",PID.Ki);ShowKi();break;
			case 2 : PID.Kd -= 0.1;printf("当前Kd为%f",PID.Kd);ShowKd();break;
			case 3 : Clean_Aim(); PID.Rin -= 5;Aim();break;
			case 4 : Read_PID();break;//在LCD右上角显示“ROK”标志
		}
	}		
	
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}

