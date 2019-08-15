#include "encoder.h"

u16 count_A = 0;         //A�ಶ��ͨ�������������
u16 count_AA = 0;        //A�ಶ��ͨ�����������شﵽ65535�������

u16 count_B = 0;         //B�ಶ��ͨ�������������
u16 count_BB = 0;        //B�ಶ��ͨ�����������شﵽ65535�������

//������A�࣬TIM3 ͨ��1�����벶�񣬳�ʼ��
//arr���Զ���װֵ �̶�Ϊ (299+1) = 300��pwm����Ϊ300/10000 = 30ms
//psc��ʱ��Ԥ��Ƶ��  �̶�Ϊ��7199+1��=7200��Ƶ��Ϊ7200/72000000 = 10KHZ
void Encoder_A_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM3_ICInitStructure; //��ʱ��3ͨ��1���벶������
	NVIC_InitTypeDef NVIC_InitStructure;//�жϷ�������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3��ȫ��ӳ��  TIM3_CH1->PC6(������A��)  TIM3_CH2->PC7(������B��) 
 
   //���ø�����Ϊ���ò�������,TIM3_CH1����A�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = 299; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIM3ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIM3��ʱ�������λ
	
	//��ʼ��TIM3���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_1; // ѡ��CH1Ϊ����ͨ��
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ֱ��ӳ�䵽CH1��
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ����ÿ�������ؼ�¼1�� 
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

	TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);//����CC1IE�����ж�	
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3	
}


//������B�࣬TIM3 ͨ��2�����벶�񣬳�ʼ��
//arr���Զ���װֵ �̶�Ϊ (299+1) = 300��pwm����Ϊ300/10000 = 30ms
//psc��ʱ��Ԥ��Ƶ��  �̶�Ϊ��7199+1��=7200��Ƶ��Ϊ7200/72000000 = 10KHZ
void Encoder_B_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM3_ICInitStructure; //��ʱ��3ͨ��2���벶������
	NVIC_InitTypeDef NVIC_InitStructure;//�жϷ�������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3��ȫ��ӳ��  TIM3_CH1->PC6(������A��)  TIM3_CH2->PC7(������B��) 
 
   //���ø�����Ϊ���ò�������,TIM3_CH1����A�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = 299; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIM3ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIM3��ʱ�������λ
	
	//��ʼ��TIM3���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; // ѡ��CH2Ϊ����ͨ��
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ֱ��ӳ�䵽CH2��
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ����ÿ�������ؼ�¼1�� 
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

	TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);//����CC2IE�����ж�	
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3	
}


//��ʱ��5�жϷ������	 
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
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);//���ͨ��1�����ж�λ
	}
	else if (TIM_GetITStatus(TIM3,TIM_IT_CC2) != RESET)
	{
		count_B++;
		if(count_B == 65535)
		{
			count_BB++;
			count_B = 0;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2); //���ͨ��2�����жϱ�־λ
	}
}
