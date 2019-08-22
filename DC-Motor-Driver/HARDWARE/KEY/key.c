#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "pid.h"
#include "usart.h"
#include "wave.h"
#include "lcd.h"
#include "eeprom.h"
#include "pwm.h"
#include "encoder.h"

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

//电机停止，PID清0初始化
void Stop_Init()
{
	PID.Ek = 0;
	PID.Ek1 = 0;
	PID.Kd = 0;
	PID.Ki = 0; 
	PID.Kp = 0;
	PID.OUT = 0;
	PID.OUTR = 0;
	PID.Rin = 0;
	PID.Rout = 0;
}

//角度模式初始化
void Angle_Init(void)
{
	PID.Ek = 0;
	PID.Ek1 = 0;
	
	PID.Kp = 3.4;
	PID.Ki = 6.9;
	PID.Kd = 0.0;
	
	PID.Rin = 180;
	PID.Rout = 0;
	PID.OUT = 0;
	PID.OUTR = 0;
	
	Read_PID();
	
	LCD_Clear(WHITE);
	Aim();
	LCD_ShowString(305,1,20,20,12,"MO");//开始时按键调整目标为MO，并在LCD右上角显示
	POINT_COLOR = BLACK;
	line();
	POINT_COLOR = BLUE;
	ShowKp();
	ShowKi();
	ShowKd();
	ShowAN_V();

}

//速度模式初始化
void Speed_Init(void)
{
	PID.Ek = 0;
	PID.Ek1 = 0;
	
	PID.Kp = 1.8;
	PID.Ki = 1.4;
	PID.Kd = 1.4;
	
	PID.Rin = 45;
	PID.Rout = 0;
	PID.OUT = 0;
	PID.OUTR = 0;
	Read_PID();
	LCD_Clear(WHITE);
	Aim();
	LCD_ShowString(305,1,10,20,12,"MO");//开始时按键调整目标为MO，并在LCD右上角显示
	POINT_COLOR = BLACK;
	line();
	POINT_COLOR = BLUE;
	ShowKp();
	ShowKi();
	ShowKd();
	ShowAN_V();
}

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	static u8 i = 0;
	delay_ms(10);//消抖
	if(WK_UP==1)//WK_UP按键
	{	
		switch(i)
		{
			case 0 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Kp");break;//当前调整目标为Kp，并在LCD右上角显示
			case 1 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Ki");break;//当前调整目标为Ki，并在LCD右上角显示
			case 2 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"Kd");break;//当前调整目标为Kd，并在LCD右上角显示
			case 3 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);if(!mode){LCD_ShowString(310,1,20,20,12,"V");}else{LCD_ShowString(305,1,20,20,12,"AN");}break;//当前调整目标为V/AN，并在LCD右上角显示
			case 4 : j = i; i++;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"MO");break;//当前调整目标为模式mode，并在LCD右上角显示	
			case 5 : j = i; i=0;LCD_Fill(305,1,320,20,WHITE);LCD_ShowString(305,1,20,20,12,"WR");break;//当前调整目标为EEPROM读写，并在LCD右上角显示
			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}


//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	static u8 stop = 1;   //电机停止标志位，1停止，0旋转
	
	delay_ms(10);//消抖
	if(KEY1==0)	 //按键KEY1
	{				 
		switch(j)
		{
			case 0 : PID.Kp += 0.1;Check();printf("当前Kp为%f",PID.Kp);ShowKp();break;
			case 1 : PID.Ki += 0.1;Check();printf("当前Ki为%f",PID.Ki);ShowKi();break;
			case 2 : PID.Kd += 0.1;Check();printf("当前Kd为%f",PID.Kd);ShowKd();break;
			case 3 : Clean_Aim();if(!mode){PID.Rin += 5;}else{PID.Rin += 10;count_A_TEMP=0;PID.OUT=0;}Check();Aim();ShowAN_V();break;
			case 4 : if(stop){Stop_Init();PWM(1,0);stop=0;}//电机停机
							 else{mode = 1;Angle_Init();count_A_TEMP=0;t = 0;angle_sum=0;stop=1;}break;//角度模式
			case 5 : Write_PID();break;//将画笔调整回蓝色，用于画波形
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
			case 0 : PID.Kp -= 0.1;Check();printf("当前Kp为%f",PID.Kp);ShowKp();break;
			case 1 : PID.Ki -= 0.1;Check();printf("当前Ki为%f",PID.Ki);ShowKi();break;
			case 2 : PID.Kd -= 0.1;Check();printf("当前Kd为%f",PID.Kd);ShowKd();break;
			case 3 : Clean_Aim();if(!mode){PID.Rin -= 5;}else{PID.Rin -= 10;count_A_TEMP=0;PID.OUT=0;}Check();Aim();ShowAN_V();break;
			case 4 : mode = 0;Speed_Init();count_A_TEMP=0;PWM(1,0);delay_ms(100);t = 0;PID.Rout=0;break;//速度模式
			case 5 : Read_PID();break;//在LCD右上角显示“ROK”标志
		}
	}		
		      
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}

//按键操纵变量有效性判断
void Check(void)
{
	switch(j)
	{
		case 0: if(PID.Kp >= 9.9) PID.Kp = 9.9;else if(PID.Kp <= -9.9)PID.Kp = -9.9;
		case 1: if(PID.Ki >= 9.9)PID.Ki = 9.9;else if(PID.Ki <= -9.9)PID.Ki = -9.9;
		case 2: if(PID.Kd >= 9.9 )PID.Kd = 9.9;else if(PID.Kd <= -9.9)PID.Kd = -9.9;
		case 3: if(mode){if(PID.Rin > 260)PID.Rin = 260.;else if(PID.Rin<=90)PID.Rin = 90;}
						else{if(PID.Rin > 100)PID.Rin = 100;else if(PID.Rin<=1)PID.Rin = 1;}
	}
}
