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



 int main(void)
{
	u8 lcd_id[12];			//存放LCD ID字符串
	
	delay_init();             //延时函数初始化
	uart_init(115200);        //串口波特率设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     //中断优先级级分组2
	LCD_Init();                //初始化LCD，横屏显示，底色为白色
	line();                    //画LCD坐标轴
	POINT_COLOR = 0x001F;      //LCD画笔颜色调整为蓝色
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。 
	PID_Init();                 //pid参数初始化
	
	Aim();                    //画目标红线
	TIM5_PWM_Init();          //PWM输出初始化
	Encoder_A_Init();         //编码器A初始化

	KEY_EXTI_Init();          //按键外部中断初始化
	PWM(1,0);                 //设置初始PWM为正转，占空比为0
	start = 1;                //电机转动标志位置1，PID算法开启
	TIM4_CNT_Init();          //100ms定时器开启
	while(1);
}

