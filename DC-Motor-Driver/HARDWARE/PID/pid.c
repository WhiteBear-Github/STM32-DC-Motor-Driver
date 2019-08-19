#include "pid.h"
#include "encoder.h"
#include "pwm.h"
#include "usart.h"
#include "wave.h"
#include "delay.h"

PID_DEFINE  PID;

u8 mode = 0;   //测速，与测角度转换标志位，0：转速，1：角度

double angle;          //每秒应转动角度
double angle_sum = 0;  //当前已转过的总角度

s16 error;      //当次误差
u8 start = 0;   //电机开始工作标志位
u8 t = 0;  //TFLCD上显示t个分点

u8 kp[5];  //存放Kp数组
u8 ki[5];  //存放Ki数组
u8 kd[5];  //存放Kd数组

//arr：自动重装值 固定为 (999+1) = 1000，pwm周期为1000/10000 = 100ms
//psc：时钟预分频数  固定为（7199+1）=7200，频率为7200/72000000 = 10KHZ
void TIM4_CNT_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = 999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
}

//PID变量初始化
void PID_Init()
{
	PID.Rin = 45;
	
	PID.Ek = 0;
	PID.Ek1 = 0;
	
	PID.Kp = 2.8;  
	PID.Ki = 2.5;
	PID.Kd = 0.3;
	
	PID.OUT = 0;
	
	ShowKp();
	ShowKi();
	ShowKd();
	ShowAN_V();
}

//定时器4中断函数
//0.1s进入一次
void TIM4_IRQHandler ()
{
	if(start)  //电机启动后开始PID计算
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		{
			if(mode)  
			{
				angle = PID.Rin; //计算每秒应转读数，1s转完
				if(angle_sum >= PID.Rin)
				{
					angle = 0;
				}
				PID.Rout = count_A+count_AA*65535;           //实际每0.1s计数次数
				error = angle/10*334/360 - PID.Rout;         //0.1s误差计数次数
				count_AA = 0;
				count_A = 0;
				angle_sum = count_A_TEMP/334.0*360; //实际已经转过角度  
				if(angle_sum > 80) //角度转动超过80度才在LCD上显示
				{
					t += 1;
					if(t>=119)
					{
						t = 119;   //屏幕最多显示最近12秒的速度
					}
					wave();
				}
				PID_Calculate(error);
				
				PID.OUT += PID.OUTR;
				if(PID.OUT >= 9999)
				{
					PID.OUT = 9999;
				}						
				if(PID.OUT <= 0)
				{
					PID.OUT = 0;
				}
				if(angle_sum >= PID.Rin)
				{
					PID.OUT = 0;
				}
				printf("目标角度为%lf 度\n",PID.Rin);
				printf("1s转完目标速度为%lf r/s\n",angle/360);
				printf("当前转过角度为%lf 度\n",angle_sum);
				printf("当前转速为%lf r/s\n",PID.Rout/334*10);
				printf("输出占空比为%lf \n",PID.OUT/9999*100);
				PWM(1,PID.OUT);
			
				PID.Rout = 0;
		
			}
			else
			{
				t += 1;
				if(t>=119)
				{
					t = 119;   //屏幕最多显示最近12秒的速度
				}					
				PID.Rout = count_A+count_AA*65535;           //实际每0.1s计数次数
				wave();//显示波形
				error = PID.Rin*334/10 - PID.Rout;           //0.1s的误差计数次数
				count_AA = 0;
				count_A = 0;
				PID_Calculate(error);
				if(PID.OUTR>25 ||PID.OUTR<-25)      //如果增量小于一定值输出不改变，减小波动
				{
					PID.OUT += PID.OUTR;
				}
				if(PID.OUT >= 9999)
				{
					PID.OUT = 9999;
				}						
				if(PID.OUT <= 0)
				{
					PID.OUT = 0;
				}
				printf("目标转速为%lf r/s\n",PID.Rin);
				printf("当前转速为%lf r/s\n",PID.Rout/334*10);
				printf("输出占空比为%lf \n",PID.OUT/9999*100);
				PID.Rout  = 0;
				PWM(1,PID.OUT);	
			}		
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}

//增量式PID计算

void PID_Calculate(s16 error)
{
	PID.OUTR = PID.Kp*(error-PID.Ek) + PID.Ki*error + PID.Kd *(error-2*PID.Ek+PID.Ek1);  //增量式PID计算公式	
	PID.Ek = error;
	PID.Ek1 = PID.Ek;
}

