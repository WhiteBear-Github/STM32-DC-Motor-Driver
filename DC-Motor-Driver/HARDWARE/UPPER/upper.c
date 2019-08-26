#include "upper.h"
#include "usart.h"
#include "pid.h"
#include "wave.h"
#include "key.h"
#include "encoder.h"

u8 len; //接收到的上位机命令长度（最大200）
u8 i = 0 ;
u16 compare = 0;  //命令比较变量，1~2位为kp目标指令指示，3~4为ki，5~6为kd,7为v，8~12位为angle，13位为进入目标指令标志位
double data; //接收到上位机传来的数据

const u8 command1[] = "kp";
const u8 command2[] = "ki";
const u8 command3[] = "kd";
const u8 command4[] = "v";
const u8 command5[] = "angle";

//上位机指令函数
void Upper_Computer(void)
{
	if(USART_RX_STA&0x8000)
	{					   
		len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
		printf("\r\n\r\n字节长度为%d\r\n\r\n",len);
		if(len != 2 && len != 1 && len != 5 && len != 3)
		{
			printf("\r\n输入错误命令，请重新输入\r\n\r\n");
			USART_RX_STA=0; //清除此次命令接收完成变量，准备接收目标数据
			len = 0;
		}
		else
		{
			printf("\r\n数据长度正确\r\n\r\n");
			if((compare & 0x1000) == 0)//没有得到目标指令
			{
				if(len == 2)
				{
					for(i=0;i<len;i++)
					{
						if(USART_RX_BUF[i] == command1[i])
							compare = compare | (0x0000 | (0x0001 << i));  //当compare 1~2bit 为11 时表明接收到了kp目标指令
						if(USART_RX_BUF[i] == command2[i])
							compare = compare | (0x0000 | (0x0001 << (i+2)));//当compare 3~4bit 为11 时表明接收到了ki目标指令
						if(USART_RX_BUF[i] == command3[i])
							compare = compare | (0x0000 | (0x0001 << (i+4)));//当compare 5~6bit 为11 时表明接收到了kd目标指令
					}		
				}
				else if (len == 1)
				{
					if(USART_RX_BUF[0] == command4[0])
						compare = compare | (0x0000 | (0x0001 << (i+6)));//当compare 7bit 为1 时表明接收到了v目标指令
				}
				else if(len == 5)
				{
					for(i=0;i<len;i++)
					{
						if(USART_RX_BUF[i] == command5[i])
							compare = compare | (0x0000 | (0x0001 << (i+7)));//当compare 8~12bit 为11111 时表明接收到了angle目标指令
					}
				}
				//判断输入的目标指令
				if((compare & 0x0003) == 0x0003)
				{
					printf("\r\n您发送的目标指令为: kp\r\n\r\n");
					compare |= 0x1000;  //标志目标指令已设置完成
				}
				else if((compare & 0x000c) == 0x000c)
				{
					printf("\r\n您发送的目标指令为: ki\r\n\r\n");
					compare |= 0x1000; //标志目标指令已设置完成
				}
				else if((compare & 0x0030) == 0x0030)
				{
					printf("\r\n您发送的目标指令为: kd\r\n\r\n");
					compare |= 0x1000; //标志目标指令已设置完成
				}
				else if((compare & 0x0040) == 0x0040)
				{
					if(!mode) //调速模式下
					{
						printf("\r\n您发送的目标指令为: v\r\n\r\n");
						compare |= 0x1000; //标志目标指令已设置完成
					}
					else
					{
						printf("\r\n请在调速界面下发送此命令,请重新输入\r\n\r\n");
					}
				}
				else if((compare & 0x0f80) == 0x0f80)
				{
					if(mode)//调角度模式下
					{
						printf("\r\n您发送的目标指令为: angle\r\n\r\n");
						compare |= 0x1000; //标志目标指令已设置完成
					}
					else
						printf("\r\n请在调角度界面下发送此命令,请重新输入\r\n\r\n");
				}
				else
				{
					printf("\r\n没有符合的目标指令，请重新输入\r\n\r\n");
				}		
				USART_RX_STA=0; //清除此次命令接收完成变量，准备接收收标数据
				len = 0;
			}
			else if((USART_RX_STA&0x8000) && (compare & 0x1000)) //已接收到目标指令
			{
				if(len == 2)  //长度为2的数据处理（两位整数）
				{
					data = (u16)((USART_RX_BUF[0]-'0')*10) + (u16)(USART_RX_BUF[1]-'0');
				}
				else if(len == 3)  //长度为3的数据处理（三位整数或小数）
				{
					if(USART_RX_BUF[1] == '.')
					{
						data = (u16)(USART_RX_BUF[0]-'0') + (double)((USART_RX_BUF[2]-'0')/10.0);
					}
					else
					{
						data = (u16)((USART_RX_BUF[0]-'0')*100) + (u16)((USART_RX_BUF[1]-'0')*10) + (u16)(USART_RX_BUF[2]-'0');
					}
				}
				else if(len == 1)
				{
					data = (u16)(USART_RX_BUF[0]-'0');
				}
				
				if((compare & 0x0003) == 0x0003)
				{
					printf("\r\n您发送的目标指令为: kp\r\n\r\n");
					printf("\r\n您发送的目标数据为: %f\r\n\r\n",data);
					PID.Kp = data;
					Check();
					ShowKp();
				}
				else if((compare & 0x000C) == 0x000C)
				{
					printf("\r\n您发送的目标指令为: ki\r\n\r\n");
					printf("\r\n您发送的目标数据为: %f\r\n\r\n",data);
					PID.Ki = data;     //目标值赋值
					Check();           //检查赋值取值范围
					ShowKi();          //显示Ki
				}
				else if((compare & 0x0030) == 0x0030)
				{
					printf("\r\n您发送的目标指令为: kd\r\n\r\n");
					printf("\r\n您发送的目标数据为: %f\r\n\r\n",data);
					PID.Kd = data;     //目标值赋值
					Check();           //检查赋值取值范围
					ShowKd();          //显示KD
				}
				else if((compare & 0x0040) == 0x0040)
				{
					if(!mode) //调速模式下
					{
						printf("\r\n您发送的目标指令为: v\r\n\r\n");
						printf("\r\n您发送的目标数据为: %f\r\n\r\n",data);
						Clean_Aim();    //清除目标红线
						PID.Rin = data; //目标值赋值
						Check();        //检查赋值取值范围
						Aim();          //刷新显示目标红线
						ShowAN_V();     //刷新显示速度
					}
					else
						printf("\r\n请在调速界面下发送此命令\r\n\r\n");
				}
				else if((compare & 0x0f80) == 0x0f80)
				{
					if(mode)//调角度模式下
					{
						printf("\r\n您发送的目标指令为: angle\r\n\r\n");
						printf("\r\n您发送的目标数据为: %f\r\n\r\n",data);
						Clean_Aim();   //清除目标红线
						PID.Rin = data;//目标值赋值
						count_A_TEMP=0;//清除已转角度编码器读取值
						PID.OUT=0;     //清空输出值
						Check();       //检查赋值取值范围
						Aim();         //刷新显示目标红线
						ShowAN_V();    //刷新显示角度
					}
					else
						printf("\r\n请在调角度界面下发送此命令\r\n\r\n");
				}
				USART_RX_STA=0;     //清空接收状态标志
				compare = 0;        //置 0 目标比较变量
				len = 0;
			}	
		}
	}
}
