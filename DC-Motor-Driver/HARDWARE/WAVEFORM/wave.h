#ifndef __WAVE_H
#define __WAVE_H

#include "sys.h"

void line(void);  //坐标轴线	，PID参数标志，单位描点划分
void Aim(void);   //目标红线设置
void Clean_Aim(void);	//清楚目标红线
void wave(void);  //画波浪曲线
void PID_ToShow(double pid);//将PID参数转换成字符串形式，用于LCD显示
void ShowKp(void);//在LCD上显示参数Kp
void ShowKi(void);//在LCD上显示参数Ki
void ShowKd(void);//在LCD上显示参数Kd

#endif
