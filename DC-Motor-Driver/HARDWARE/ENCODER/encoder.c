#include "encoder.h"

u16 count_A = 0;         //A相捕获通道捕获的上升沿
u16 count_AA = 0;        //A相捕获通道捕获上升沿达到65535溢出次数

u16 count_B = 0;         //B相捕获通道捕获的上升沿
u16 count_BB = 0;        //B相捕获通道捕获上升沿达到65535溢出次数

//编码器A相，TIM3 通道1，输入捕获，初始化
//arr：自动重装值 固定为 (299+1) = 300，pwm周期为300/10000 = 30ms
//psc：时钟预分频数  固定为（7199+1）=7200，频率为7200/72000000 = 10KHZ
void Encoder_A_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM3_ICInitStructure; //定时器3通道1输入捕获配置
	NVIC_InitTypeDef NVIC_InitStructure;//中断分组设置

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3完全重映射  TIM3_CH1->PC6(编码器A相)  TIM3_CH2->PC7(编码器B相) 
 
   //设置该引脚为复用捕获输入,TIM3_CH1捕获A相编码器方波
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = 299; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIM3时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIM3的时间基数单位
	
	//初始化TIM3输入捕获参数
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_1; // 选择CH1为输入通道
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //直接映射到CH1上
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频，即每个上升沿记录1次 
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);//允许CC1IE捕获中断	
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3	
}


//编码器B相，TIM3 通道2，输入捕获，初始化
//arr：自动重装值 固定为 (299+1) = 300，pwm周期为300/10000 = 30ms
//psc：时钟预分频数  固定为（7199+1）=7200，频率为7200/72000000 = 10KHZ
void Encoder_B_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM3_ICInitStructure; //定时器3通道2输入捕获配置
	NVIC_InitTypeDef NVIC_InitStructure;//中断分组设置

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3完全重映射  TIM3_CH1->PC6(编码器A相)  TIM3_CH2->PC7(编码器B相) 
 
   //设置该引脚为复用捕获输入,TIM3_CH1捕获A相编码器方波
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = 299; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIM3时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIM3的时间基数单位
	
	//初始化TIM3输入捕获参数
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; // 选择CH2为输入通道
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //直接映射到CH2上
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频，即每个上升沿记录1次 
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);//允许CC2IE捕获中断	
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3	
}


//定时器5中断服务程序	 
void TIM3_IRQHandler()
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{

		count_A++;
		if(count_A == 65535)
		{
			count_AA++;
			count_A = 0;
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);//清除通道1捕获中断位
	}
	else if (TIM_GetITStatus(TIM3,TIM_IT_CC2) != RESET)
	{
		count_B++;
		if(count_B == 65535)
		{
			count_BB++;
			count_B = 0;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2); //清除通道2捕获中断标志位
	}
}
