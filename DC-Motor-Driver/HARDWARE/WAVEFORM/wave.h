#ifndef __WAVE_H
#define __WAVE_H

#include "sys.h"

void line(void);  //��������	��PID������־����λ��㻮��
void Aim(void);   //Ŀ���������
void Clean_Aim(void);	//���Ŀ�����
void wave(void);  //����������
void PID_ToShow(double pid);//��PID����ת�����ַ�����ʽ������LCD��ʾ
void ShowKp(void);//��LCD����ʾ����Kp
void ShowKi(void);//��LCD����ʾ����Ki
void ShowKd(void);//��LCD����ʾ����Kd

#endif
