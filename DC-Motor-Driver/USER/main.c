#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "pwm.h"
#include "usart.h"
#include "encoder.h"
#include "pid.h"
#include "lcd.h"
#include "wave.h"
#include "eeprom.h"
#include "upper.h"

 int main(void)
{

	u8 lcd_id[12];			//���LCD ID�ַ���
	
	delay_init();             //��ʱ������ʼ��
	uart_init(115200);        //���ڲ���������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     //�ж����ȼ�������2
	
	LCD_Init();                //��ʼ��LCD��������ʾ����ɫΪ��ɫ
	line();                    //��LCD������
	POINT_COLOR = BLUE;      //LCD������ɫ����Ϊ��ɫ
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//��LCD ID��ӡ��lcd_id���顣 
	PID_Init();               //pid������ʼ��

	Aim();                    //��Ŀ�����
	LCD_ShowString(305,1,20,20,12,"Kp");//��ʼʱ��������Ŀ��ΪKp������LCD���Ͻ���ʾ
	
	TIM5_PWM_Init();          //PWM�����ʼ��
	Encoder_A_Init();         //������A��ʼ��

	AT24C02_Init();						//IIC��ʼ�� 
	KEY_EXTI_Init();          //�����ⲿ�жϳ�ʼ��
	
	while(AT24C02_Check())   //��ⲻ��24c02
	{
		POINT_COLOR = RED;      //LCD������ɫ����Ϊ��ɫ
		LCD_ShowString(300,1,5,20,12,"!");//LCD���Ͻ���ʾ��ɫ��̾��
		POINT_COLOR = BLUE;     //���ʻ�����ɫ�����ڻ�����
	}
	
	PWM(1,0);                 //���ó�ʼPWMΪ��ת��ռ�ձ�Ϊ0
	start = 1;                //���ת����־λ��1��PID�㷨����
	TIM4_CNT_Init();          //100ms��ʱ������
	
	Read_PID();               //��ȡeeprom
	while(1);
}

