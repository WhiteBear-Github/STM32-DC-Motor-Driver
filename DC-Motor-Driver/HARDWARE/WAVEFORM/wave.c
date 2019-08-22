
////////////////////////////////2.8寸显示屏 分辨率240*320/////////////////////////////////////

#include "lcd.h"
#include "wave.h"
#include "pid.h"
#include "usart.h"

u8 speed[120];      //各时刻速度纪录数组，每100ms计算一次
u16	angle_rem[120];     //各时刻角度纪录数组，每100ms计算一次  
u8 kpid[5];         //Kp Ki Kd转换为字符串，LCD显示用


//显示坐标轴，单位大小划分
//显示PID参数标志
void line()
{
	u16 i;
	LCD_Scan_Dir(3);
	
	LCD_DrawLine(20,220,320,220);
	LCD_DrawLine(20,220,20,20);
//横坐标设置单位/s
//25个分辨率为1s
//每秒画10个点满屏120,每个点分辨率为2.5
	for(i=45;i<=300;i+=25)                      //横坐标打点
	{
		LCD_Fast_DrawPoint(i,220,YELLOW);
	}

//	LCD_ShowxNum(45,221,1.1,1,12,1);         
//	
//	LCD_ShowxNum(70,221,2,1,12,1);     

//	LCD_ShowxNum(95,221,3,1,12,1);    
//	
//	LCD_ShowxNum(120,221,4,1,12,1);
//	
//	LCD_ShowxNum(145,221,5,1,12,1);
//	
//	LCD_ShowxNum(170,221,6,1,12,1);
//	
//	LCD_ShowxNum(195,221,7,1,12,1);
//	
//	LCD_ShowxNum(220,221,8,1,12,1);
//	
//	LCD_ShowxNum(245,221,9,1,12,1);
//	
	LCD_ShowString(45,221,15,20,12,"0.1"); 
	LCD_ShowString(70,221,15,20,12,"0.2");
	LCD_ShowString(95,221,15,20,12,"0.3"); 
	LCD_ShowString(120,221,15,20,12,"0.4"); 	
	LCD_ShowString(145,221,15,20,12,"0.5"); 
	LCD_ShowString(170,221,15,20,12,"0.6"); 
	LCD_ShowString(195,221,15,20,12,"0.7"); 
	LCD_ShowString(220,221,15,20,12,"0.8"); 
	LCD_ShowString(245,221,15,20,12,"0.9"); 
	LCD_ShowString(270,221,15,20,12,"1.0"); 
	LCD_ShowString(295,221,15,20,12,"1.1"); 

	LCD_ShowString(25,1,20,20,12,"Kp;");     //显示Kp参数标志
	LCD_ShowString(85,1,20,20,12,"Ki;");     //显示Ki参数标志
	LCD_ShowString(145,1,20,20,12,"Kd;");    //显示Kd参数标志
	if(mode)
		LCD_ShowString(205,1,20,20,12,"AN:");  //显示设定的角度标志
	else
		LCD_ShowString(205,1,20,20,12," V:");  //显示设定的速度标志
//	LCD_ShowString(270,221,10,20,12,"10");
//	
//	LCD_ShowString(295,221,10,20,12,"11");
//	
//	LCD_ShowString(315,221,10,20,12,"s");
	

//////////////////////////////////////////////
///////////设置纵坐标单位r/或度////////////////
	
		for(i=210;i>20;i-=10)
	{	
			LCD_Fast_DrawPoint(20,i,YELLOW);
	}
	if(!mode)
	{
		LCD_ShowxNum(10,210,5,1,12,1);
	
		LCD_ShowString(5,200,10,20,12,"10");
 	
		LCD_ShowString(5,190,10,20,12,"15");
	
		LCD_ShowString(5,180,10,20,12,"20");
	
		LCD_ShowString(5,170,10,20,12,"25");
	
		LCD_ShowString(5,160,10,20,12,"30");
	
		LCD_ShowString(5,150,10,20,12,"35");
	
		LCD_ShowString(5,140,10,20,12,"40");
	
		LCD_ShowString(5,130,10,20,12,"45");
	
		LCD_ShowString(5,120,10,20,12,"50");
	
		LCD_ShowString(5,110,10,20,12,"55");
	
		LCD_ShowString(5,100,10,20,12,"60");
	
		LCD_ShowString(5,90,10,20,12,"65");
	
		LCD_ShowString(5,80,10,20,12,"70");
	
		LCD_ShowString(5,70,10,20,12,"75");
	
		LCD_ShowString(5,60,10,20,12,"80");
	
		LCD_ShowString(5,50,10,20,12,"85");

		LCD_ShowString(5,40,10,20,12,"90");

		LCD_ShowString(5,30,10,20,12,"95");

		LCD_ShowString(0,20,15,20,12,"100");
	
		LCD_ShowString(0,1,20,20,12,"r/s");
	}
	else
	{
		LCD_ShowString(5,210,15,20,12,"80");
	
		LCD_ShowString(5,200,15,20,12,"90");
 	
		LCD_ShowString(0,190,15,20,12,"100");
	
		LCD_ShowString(0,180,15,20,12,"110");
	
		LCD_ShowString(0,170,15,20,12,"120");
	
		LCD_ShowString(0,160,15,20,12,"130");
	
		LCD_ShowString(0,150,15,20,12,"140");
	
		LCD_ShowString(0,140,15,20,12,"150");
	
		LCD_ShowString(0,130,15,20,12,"160");
	
		LCD_ShowString(0,120,15,20,12,"170");
	
		LCD_ShowString(0,110,15,20,12,"180");
	
		LCD_ShowString(0,100,15,20,12,"190");
	
		LCD_ShowString(0,90,15,20,12,"200");
	
		LCD_ShowString(0,80,15,20,12,"210");
	
		LCD_ShowString(0,70,15,20,12,"220");
	
		LCD_ShowString(0,60,15,20,12,"230");
	
		LCD_ShowString(0,50,15,20,12,"240");

		LCD_ShowString(0,40,15,20,12,"250");

		LCD_ShowString(0,30,15,20,12,"260");

		LCD_ShowString(0,20,15,20,12,"270");
	}
	
}

//目标值红线显示
void Aim(void)
{
	u8 real;
	if(!mode)
		real = 240-(20+PID.Rin*2);
	else
		real = 240-(20+PID.Rin-70);
	if(real<=20)
		real = 20;
	if(real>220)
		real = 220;
		
	LCD_Fill(21,real,320,real,RED);

}
//清除目标值红线
void Clean_Aim()
{
	u8 real;
	if(!mode)
		real = 240-(20+PID.Rin*2);
	else
		real = 240-(20+PID.Rin-70);
	if(real<=20)                              //目标速度最低设定值为0 r/s
		real = 20;
	if(real>220)                              //目标速度最高设定值为100 r/s
		real = 220;
		
	LCD_Fill(21,real,320,real,WHITE);

}

//波形显示
void wave(void)
{
	u8 V;//当前转速
	s16 AN; //当前角度
	s8 i;//for循环语句变量
	
	if(!mode)
	{
		V = PID.Rout/334*20;                   //计算实际当前转速
		LCD_Fill(21,20,320,219,WHITE);          //速度显示区域清空
		Aim();                                 //显示当前目标值红线

		for(i=t;i>0;i--)//计算本次速度对应图像
		{
			if(speed[i-1]<=0)
			{
				speed[i-1] = 1;        //如果转速为0，显示时向上移动一个分辨率，便于清屏处理
			}
			else if(speed[i-1]>=100)
			{
				speed[i-1] = 100;
			}
			speed[i] = speed[i-1];  //将上次速度向后推移一位
		}
	
		speed[0] = V;
		
		for(i=t;i>=0;i--) //显示本次对应图像
		{
			if(i>1)              //丢弃计算的第一个点
			{
				LCD_DrawLine(20+(i*2.5),240-(20+2*speed[i]),20+((i-1)*2.5),240-(20+2*speed[i-1]));
			}	
			else if(i==1)
			{
				LCD_DrawLine(20+(i*2.5),240-(20+2*speed[i]),21,240-(20+2*speed[i-1])); //最新一个计算数值显示向右偏差1个像素点，便于清屏处理
			}
		}
	}
	else
	{
		AN = angle_sum;
		LCD_Fill(21,20,320,219,WHITE);          //速度显示区域清空
		Aim();                                 //显示当前目标值红线
		
		for(i=t;i>0;i--)//计算本次速度对应图像
		{
			if(angle_rem[i-1]<=70)        //控制显示范围
				angle_rem[i-1] = 71;        //如果转速为0，显示时向上移动一个分辨率，便于清屏处理
			else if (angle_rem[i-1]>=270)
				angle_rem[i-1] = 270;
			angle_rem[i] = angle_rem[i-1];  //将上次速度向后推移一位
		}
	
		angle_rem[0] = AN;
		
		for(i=t;i>=0;i--) //显示本次对应图像
	 {
			if(i>1)              //丢弃计算的第一个点
			{
				LCD_DrawLine(20+(i*2.5),240-(angle_rem[i]-50),20+((i-1)*2.5),240-(angle_rem[i-1]-50));
			}	
			else if(i==1)
			{
				LCD_DrawLine(20+(i*2.5),240-(angle_rem[i]-50),21,240-(angle_rem[i-1]-50)); //最新一个计算数值显示向右偏差1个像素点，便于清屏处理
			}
		}
	}

}

//将PID参数转换成字符串形式，用于LCD显示
//参数大小不超过两位数
                         //***************************************************************//
//****************************存在BUG：当数据转换到0时会出现0和-0两次，造成显示数据偏差0.1************************//
                         //***************************************************************//
void PID_ToShow(double pid)
{
	u8 integer;   //整数
	u8 dec;//小数
	
	if(pid < 0)
	{
		kpid[0] = '-';
		pid = -pid;
	}
	else
		kpid[0] = ' ';
	
	integer = (u8)pid;
	dec = (u8)(pid*10)%10;
	
	kpid[1] = integer + '0';
	kpid[2] = '.';
	kpid[3] = dec + '0';	
	kpid[4] = '\0';
}

//显示当前Kp参数值
void ShowKp(void)
{
	PID_ToShow(PID.Kp);
	LCD_Fill(45,1,65,19,WHITE);
	LCD_ShowString(45,1,20,20,12, kpid);	
}

//显示当前Ki参数值
void ShowKi(void)
{
	PID_ToShow(PID.Ki);
	LCD_Fill(105,1,125,19,WHITE);
	LCD_ShowString(105,1,20,20,12, kpid);	
}

//显示当前Kd参数值
void ShowKd(void)
{
	PID_ToShow(PID.Kd);
	LCD_Fill(165,1,185,19,WHITE);
	LCD_ShowString(165,1,20,20,12, kpid);	
}

//显示当前设定速度/角度
void ShowAN_V(void)
{
	u8 AN_V[4];
	LCD_Fill(225,1,245,19,WHITE);
	if((u8)PID.Rin/100%10 == 0)
	{
		AN_V[0] = ' ';
	}
	else
	{
		AN_V[0] = (u8)PID.Rin/100%10 + '0';
	}
	AN_V[1] = (u8)PID.Rin/10%10 + '0';
	AN_V[2] = (u8)PID.Rin%10 + '0';
	AN_V[3] = '\0';
	LCD_ShowString(225,1,20,20,12, AN_V);		
}
