#include "pwm.h"

//TIM5 PWM部分初始化 设定通道2通道3为PWM输出对应引脚PA1和PA2
//PWM输出初始化
//arr：自动重装值 固定为（9999+1） = 10000，pwm周期为10000/1000000 = 10ms
//psc：时钟预分频数  固定为（71+1）=72，频率为72/72000000 = 1MHZ
void TIM5_PWM_Init()
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//使能定时器5时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIOA时钟   
 
   //输出TIM5 CH2的PWM脉冲波形	GPIOA.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2; //TIM5_CH2  TIM5_CH3  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA
 
   //初始化TIM5
	TIM_TimeBaseStructure.TIM_Period = 9999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =71; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM5 通道2、通道3 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1，小于CCR2为有效电平
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5 OC2
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5 OC3

	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR3上的预装载寄存器
 
	TIM_Cmd(TIM5, ENABLE);  //使能TIM5
}

//改变TIM5 通道2 上PWM占空比
//参数 pwma：寄存器CCR2的值，取值范围0~9999,当取9999是电机占空比为100
void PWMA_Change(double pwma)
{
	TIM_SetCompare2(TIM5,pwma);
}

//改变TIM5 通道3 上PWM占空比
//参数 pwmb：寄存器CCR2的值，取值范围0~9999 ,当取9999是电机占空比为100
void PWMB_Change(double pwmb)
{
	TIM_SetCompare3(TIM5,pwmb);
}


//电机操控函数
//dir：表示方向 ，0：正转（背面观看电机顺时针转动），1：反转
void PWM(u8 dir ,double pwm)
{
	if(!dir)              
	{
		PWMB_Change(0);    //通道3输出电压为0
		PWMA_Change(pwm);  //通道2输出PWM
	}
	else
	{
		PWMA_Change(0);    //通道2输出电压为0
		PWMB_Change(pwm);  //通道3输出PWM
	}

}
