#include "pid.h"
#include "encoder.h"
#include "pwm.h"
#include "usart.h"
#include "wave.h"

PID_DEFINE  PID;

u8 start = 0;   //�����ʼ������־λ
u8 t = 0;  //TFLCD����ʾt���ֵ�
u8 kp[5];  //���Kp����
u8 ki[5];  //���Ki����
u8 kd[5];  //���Kd����

//arr���Զ���װֵ �̶�Ϊ (999+1) = 1000��pwm����Ϊ1000/10000 = 100ms
//psc��ʱ��Ԥ��Ƶ��  �̶�Ϊ��7199+1��=7200��Ƶ��Ϊ7200/72000000 = 10KHZ
void TIM4_CNT_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 999; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
}

//PID������ʼ��
void PID_Init()
{
	PID.Rin = 45;
	
	PID.Ek = 0;
	PID.Ek1 = 0;
	
	PID.Kp = 2.0;  
	PID.Ki = 2.0;
	PID.Kd = 0.3;
	
	PID.OUT = 0;
	
	ShowKp();
	ShowKi();
	ShowKd();
}

//��ʱ��4�жϺ���
void TIM4_IRQHandler ()
{
	s16 error;
	if(start)  //���������ʼPID����
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		{
			t += 1;
			if(t>=119)
			{
				t = 119;   //��Ļ�����ʾ���12����ٶ�
			}					
			PID.Rout = count_A+count_AA*65535;           //ʵ��ÿ0.1s���Ȧ��
			wave();//��ʾ����
			error = PID.Rin*334/10 - PID.Rout;           //0.1s�����Ȧ��
			count_AA = 0;
			count_A = 0;
			PID_Calculate(error);
			if(PID.OUTR>30 ||PID.OUTR<-30)      //�������С��һ��ֵ������ı䣬��С����
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
			printf("Ŀ��ת��Ϊ%lf r/s\n",PID.Rin);
			printf("��ǰת��Ϊ%lf r/s\n",PID.Rout/334*10);
			printf("���ռ�ձ�Ϊ%lf \n",PID.OUT/9999*100);
			PID.Rout  = 0;
			PWM(1,PID.OUT);	
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}

//����ʽPID����

void PID_Calculate(s16 error)
{
	PID.OUTR = PID.Kp*(error-PID.Ek) + PID.Ki*error + PID.Kd *(error-2*PID.Ek+PID.Ek1);  //����ʽPID���㹫ʽ
	PID.Ek = error;
	PID.Ek1 = PID.Ek;
}

