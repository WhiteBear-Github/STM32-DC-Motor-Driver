#include "pwm.h"

//TIM5 PWM���ֳ�ʼ�� �趨ͨ��2ͨ��3ΪPWM�����Ӧ����PA1��PA2
//PWM�����ʼ��
//arr���Զ���װֵ �̶�Ϊ��9999+1�� = 10000��pwm����Ϊ10000/1000000 = 10ms
//psc��ʱ��Ԥ��Ƶ��  �̶�Ϊ��71+1��=72��Ƶ��Ϊ72/72000000 = 1MHZ
void TIM5_PWM_Init()
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//ʹ�ܶ�ʱ��5ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIOAʱ��   
 
   //���TIM5 CH2��PWM���岨��	GPIOA.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2; //TIM5_CH2  TIM5_CH3  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
 
   //��ʼ��TIM5
	TIM_TimeBaseStructure.TIM_Period = 9999; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =71; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM5 ͨ��2��ͨ��3 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1��С��CCR2Ϊ��Ч��ƽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM5 OC2
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM5 OC3

	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  //ʹ��TIM5��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //ʹ��TIM5��CCR3�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIM5
}

//�ı�TIM5 ͨ��2 ��PWMռ�ձ�
//���� pwma���Ĵ���CCR2��ֵ��ȡֵ��Χ0~9999,��ȡ9999�ǵ��ռ�ձ�Ϊ100
void PWMA_Change(double pwma)
{
	TIM_SetCompare2(TIM5,pwma);
}

//�ı�TIM5 ͨ��3 ��PWMռ�ձ�
//���� pwmb���Ĵ���CCR2��ֵ��ȡֵ��Χ0~9999 ,��ȡ9999�ǵ��ռ�ձ�Ϊ100
void PWMB_Change(double pwmb)
{
	TIM_SetCompare3(TIM5,pwmb);
}


//����ٿغ���
//dir����ʾ���� ��0����ת������ۿ����˳ʱ��ת������1����ת
void PWM(u8 dir ,double pwm)
{
	if(!dir)              
	{
		PWMB_Change(0);    //ͨ��3�����ѹΪ0
		PWMA_Change(pwm);  //ͨ��2���PWM
	}
	else
	{
		PWMA_Change(0);    //ͨ��2�����ѹΪ0
		PWMB_Change(pwm);  //ͨ��3���PWM
	}

}
