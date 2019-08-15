#ifndef __PWM_H
#define __PWM_H

#include "sys.h"
void TIM5_PWM_Init(void);   //PWM波TIM5初始化
void PWMA_Change(double pwma); //PWMA波占空比改变函数 ,pwma值范围0~9999 
void PWMB_Change(double pwmb); //PWMB波占空比改变函数 ,pwmb值范围0~9999 
void PWM(u8 dir ,double pwm);//电机操控函数

extern u8 start ;   //电机开始工作标志位

#endif
