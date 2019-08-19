#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"
void Encoder_A_Init(void);//编码器A相，TIM5 通道2，输入捕获，初始化
void Encoder_B_Init(void);//编码器B相，TIM5 通道3，输入捕获，初始化
void TIM3_IRQHandler(void); //定时器5中断服务程序

extern u16 count_A;       //A相捕获通道捕获的上升沿
extern u16 count_AA;      //A相捕获通道捕获上升沿达到65535溢出次数
extern u16 count_A_TEMP;        //A相编码器数据，暂存值，用于存储计算转过角度

extern u16 count_B;       //B相捕获通道捕获的上升沿
extern u16 count_BB;      //B相捕获通道捕获上升沿达到65535溢出次数

#endif
