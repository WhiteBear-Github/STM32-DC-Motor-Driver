#ifndef __PID_H
#define __PID_H

#include "sys.h"

void TIM4_CNT_Init(void);
void PID_Calculate(s16 error);
void  PID_Init(void);//PID������ʼ��
void TIM4_IRQHandler (void); //��ʱ��4�жϺ���

typedef struct
{
	double Kp;       //����ϵ��
//	double Ti;       //���ֲ���1/Ti
//	double Td;       //΢�ֲ���Td
//	double T;        //��������
 	double Ki;       //����ϵ��  Ki = Kp * T/Ti
	double Kd;       //΢��ϵ��  Kd = Kp * Td/T
	double Rin;      //Ŀ���趨ֵ��λr/s
	double Rout;     //�˴�ʵ�ʲ���ֵ
	double Ek;       //��һ��ƫ��ֵ
	double Ek1;      //����һ��ƫ��ֵ
	double OUT;      //ʵ�����ռ�ձ�
	double OUTR;     //ռ�ձ�����
}PID_DEFINE;

extern PID_DEFINE PID;
extern u8 t;  //���������¼����

extern u8 kp[5];  //���Kp����
extern u8 ki[5];  //���Ki����
extern u8 kd[5];  //���Kd����

extern u8 mode;  //ת�١��Ƕ�ģʽѡ��
extern double angle_sum ;  //��ǰ��ת�����ܽǶ�

#endif
