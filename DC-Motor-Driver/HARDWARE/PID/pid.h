#ifndef __PID_H
#define __PID_H

#include "sys.h"

void TIM4_CNT_Init(void);
void PID_Calculate(s16 error);
void  PID_Init(void);//PID变量初始化
void TIM4_IRQHandler (void); //定时器4中断函数

typedef struct
{
	double Kp;       //比例系数
//	double Ti;       //积分参数1/Ti
//	double Td;       //微分参数Td
//	double T;        //计算周期
 	double Ki;       //积分系数  Ki = Kp * T/Ti
	double Kd;       //微分系数  Kd = Kp * Td/T
	double Rin;      //目标设定值单位r/s
	double Rout;     //此次实际测量值
	double Ek;       //上一次偏差值
	double Ek1;      //上上一次偏差值
	double OUT;      //实际输出占空比
	double OUTR;     //占空比增量
}PID_DEFINE;

extern PID_DEFINE PID;
extern u8 t;  //计算次数纪录变量

extern u8 kp[5];  //存放Kp数组
extern u8 ki[5];  //存放Ki数组
extern u8 kd[5];  //存放Kd数组

extern u8 mode;  //转速、角度模式选择
extern double angle_sum ;  //当前已转过的总角度

#endif
