#include "pid.h"
#include "encoder.h"
#include "pwm.h"
#include "usart.h"
#include "wave.h"
#include "delay.h"

PID_DEFINE  PID;

u8 mode = 0;   //���٣����Ƕ�ת����־λ��0��ת�٣�1���Ƕ�

double angle;          //ÿ��Ӧת���Ƕ�
double angle_sum = 0;  //��ǰ��ת�����ܽǶ�

s16 error;      //�������
u8 start = 0;   //�����ʼ������־λ
u8 t = 0;  //TFLCD����ʾt���ֵ�

u8 kp[5];  //���Kp����
u8 ki[5];  //���Ki����
u8 kd[5];  //���Kd����
u8 pidout[5]; //����ٶ�PID.OUTֵ������λ

//arr���Զ���װֵ �̶�Ϊ (499+1) = 500��pwm����Ϊ500/10000 = 50ms
//psc��ʱ��Ԥ��Ƶ��  �̶�Ϊ��7199+1��=7200��Ƶ��Ϊ7200/72000000 = 10KHZ
void TIM4_CNT_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 499; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
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
	
	PID.Kp = 1.8;  
	PID.Ki = 1.4;
	PID.Kd = 1.4;
	
	PID.OUT = 0;
	
	ShowKp();
	ShowKi();
	ShowKd();
	ShowAN_V();
}

//��ʱ��4�жϺ���
//0.05s����һ��
void TIM4_IRQHandler ()
{
	if(start)  //���������ʼPID����
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		{
			if(mode)  
			{
				angle = (PID.Rin - angle_sum); //����ÿ��Ӧת������1sת��
				if(angle/360 < 0.06 && angle >=5.0)     //�Ƕȹ�Сʱ�������
				{
					angle = 21.6;
				}
				PID.Rout = count_A+count_AA*65535;           //ʵ��ÿ0.05s��������
				error = angle/20*334/360 - PID.Rout;         //0.05s����������
				count_AA = 0;
				count_A = 0;
				angle_sum = count_A_TEMP/334.0*360; //ʵ���Ѿ�ת���Ƕ�  
				if(angle_sum > 70) //�Ƕ�ת������70�Ȳ���LCD����ʾ
				{
					t += 1;
					if(t>=119)
					{
						t = 119;   //��Ļ�����ʾ���1.2����ٶ�
					}
					
					wave();
				}
				PID_Calculate(error);
				
				PID.OUT += PID.OUTR;
				if(PID.OUT >= 4999)
				{
					PID.OUT = 4999;
				}						
				if(PID.OUT <= 0)
				{
					PID.OUT = 0;
				}
				if(angle_sum >= PID.Rin)
				{
					PID.OUT = 0;
				}
				printf("Ŀ��Ƕ�Ϊ%lf ��\n",PID.Rin);
				printf("��ǰת���Ƕ�Ϊ%lf ��\n",angle_sum);
				printf("Ŀ���ٶ�Ϊ%lf r/s\n",angle/360);
				printf("��ǰת��Ϊ%lf r/s\n",PID.Rout/334*20);
				printf("���ռ�ձ�Ϊ%lf \n",PID.OUT/4999*100);
				PWM(1,PID.OUT);
			
				PID.Rout = 0;
		
			}
			else
			{
				t += 1;
				if(t>=119)
				{
					t = 119;   //��Ļ�����ʾ���1.2����ٶ�
				}					
				PID.Rout = count_A+count_AA*65535;           //ʵ��ÿ0.05s��������
				wave();//��ʾ����
				error = PID.Rin*334/20 - PID.Rout;           //0.05s������������
				count_AA = 0;
				count_A = 0;
				PID_Calculate(error);
				if(PID.OUTR>25 ||PID.OUTR<-25)      //�������С��һ��ֵ������ı䣬��С����
				{
					PID.OUT += PID.OUTR;
				}
				if(PID.OUT >= 4999)
				{
					PID.OUT = 4999;
				}						
				if(PID.OUT <= 0)
				{
					PID.OUT = 0;
				}
				printf("Ŀ��ת��Ϊ%lf r/s\n",PID.Rin);
				printf("��ǰת��Ϊ%lf r/s\n",PID.Rout/334*20);
				printf("���ռ�ձ�Ϊ%lf \n",PID.OUT/4999*100);
				PID.Rout  = 0;
				PWM(1,PID.OUT);	
			}		
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

