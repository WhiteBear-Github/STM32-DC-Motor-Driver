#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define WK_UP PAin(0)	//PA0  WK_UP

#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 

 

#define KEY0_PRES 	1	//KEY0按下
#define KEY1_PRES	  2	//KEY1按下
#define WKUP_PRES   3	//KEY_UP按下(即WK_UP/KEY_UP)


void KEY_Init(void);//IO初始化
void KEY_EXTI_Init(void);//按键外部中断函数
void Angle_Init(void);//角度模式初始化
void Speed_Init(void);//速度模式初始化
void Check(void);//pid参数及目标设定值变量操作有效性判断
void Stop_Init(void);//电机停止，PID清0初始化

#endif

