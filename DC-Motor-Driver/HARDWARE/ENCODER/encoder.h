#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"
void Encoder_A_Init(void);//������A�࣬TIM5 ͨ��2�����벶�񣬳�ʼ��
void Encoder_B_Init(void);//������B�࣬TIM5 ͨ��3�����벶�񣬳�ʼ��
void TIM3_IRQHandler(void); //��ʱ��5�жϷ������

extern u16 count_A;       //A�ಶ��ͨ�������������
extern u16 count_AA;      //A�ಶ��ͨ�����������شﵽ65535�������
extern u16 count_A_TEMP;        //A����������ݣ��ݴ�ֵ�����ڴ洢����ת���Ƕ�

extern u16 count_B;       //B�ಶ��ͨ�������������
extern u16 count_BB;      //B�ಶ��ͨ�����������شﵽ65535�������

#endif
