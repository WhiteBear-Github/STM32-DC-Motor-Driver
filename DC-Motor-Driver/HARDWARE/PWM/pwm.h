#ifndef __PWM_H
#define __PWM_H

#include "sys.h"
void TIM5_PWM_Init(void);   //PWM��TIM5��ʼ��
void PWMA_Change(double pwma); //PWMA��ռ�ձȸı亯�� ,pwmaֵ��Χ0~9999 
void PWMB_Change(double pwmb); //PWMB��ռ�ձȸı亯�� ,pwmbֵ��Χ0~9999 
void PWM(u8 dir ,double pwm);//����ٿغ���

extern u8 start ;   //�����ʼ������־λ

#endif
