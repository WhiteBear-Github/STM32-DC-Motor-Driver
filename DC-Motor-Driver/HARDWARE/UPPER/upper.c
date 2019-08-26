#include "upper.h"
#include "usart.h"
#include "pid.h"
#include "wave.h"
#include "key.h"
#include "encoder.h"

u8 len; //���յ�����λ������ȣ����200��
u8 i = 0 ;
u16 compare = 0;  //����Ƚϱ�����1~2λΪkpĿ��ָ��ָʾ��3~4Ϊki��5~6Ϊkd,7Ϊv��8~12λΪangle��13λΪ����Ŀ��ָ���־λ
double data; //���յ���λ������������

const u8 command1[] = "kp";
const u8 command2[] = "ki";
const u8 command3[] = "kd";
const u8 command4[] = "v";
const u8 command5[] = "angle";

//��λ��ָ���
void Upper_Computer(void)
{
	if(USART_RX_STA&0x8000)
	{					   
		len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
		printf("\r\n\r\n�ֽڳ���Ϊ%d\r\n\r\n",len);
		if(len != 2 && len != 1 && len != 5 && len != 3)
		{
			printf("\r\n��������������������\r\n\r\n");
			USART_RX_STA=0; //����˴����������ɱ�����׼������Ŀ������
			len = 0;
		}
		else
		{
			printf("\r\n���ݳ�����ȷ\r\n\r\n");
			if((compare & 0x1000) == 0)//û�еõ�Ŀ��ָ��
			{
				if(len == 2)
				{
					for(i=0;i<len;i++)
					{
						if(USART_RX_BUF[i] == command1[i])
							compare = compare | (0x0000 | (0x0001 << i));  //��compare 1~2bit Ϊ11 ʱ�������յ���kpĿ��ָ��
						if(USART_RX_BUF[i] == command2[i])
							compare = compare | (0x0000 | (0x0001 << (i+2)));//��compare 3~4bit Ϊ11 ʱ�������յ���kiĿ��ָ��
						if(USART_RX_BUF[i] == command3[i])
							compare = compare | (0x0000 | (0x0001 << (i+4)));//��compare 5~6bit Ϊ11 ʱ�������յ���kdĿ��ָ��
					}		
				}
				else if (len == 1)
				{
					if(USART_RX_BUF[0] == command4[0])
						compare = compare | (0x0000 | (0x0001 << (i+6)));//��compare 7bit Ϊ1 ʱ�������յ���vĿ��ָ��
				}
				else if(len == 5)
				{
					for(i=0;i<len;i++)
					{
						if(USART_RX_BUF[i] == command5[i])
							compare = compare | (0x0000 | (0x0001 << (i+7)));//��compare 8~12bit Ϊ11111 ʱ�������յ���angleĿ��ָ��
					}
				}
				//�ж������Ŀ��ָ��
				if((compare & 0x0003) == 0x0003)
				{
					printf("\r\n�����͵�Ŀ��ָ��Ϊ: kp\r\n\r\n");
					compare |= 0x1000;  //��־Ŀ��ָ�����������
				}
				else if((compare & 0x000c) == 0x000c)
				{
					printf("\r\n�����͵�Ŀ��ָ��Ϊ: ki\r\n\r\n");
					compare |= 0x1000; //��־Ŀ��ָ�����������
				}
				else if((compare & 0x0030) == 0x0030)
				{
					printf("\r\n�����͵�Ŀ��ָ��Ϊ: kd\r\n\r\n");
					compare |= 0x1000; //��־Ŀ��ָ�����������
				}
				else if((compare & 0x0040) == 0x0040)
				{
					if(!mode) //����ģʽ��
					{
						printf("\r\n�����͵�Ŀ��ָ��Ϊ: v\r\n\r\n");
						compare |= 0x1000; //��־Ŀ��ָ�����������
					}
					else
					{
						printf("\r\n���ڵ��ٽ����·��ʹ�����,����������\r\n\r\n");
					}
				}
				else if((compare & 0x0f80) == 0x0f80)
				{
					if(mode)//���Ƕ�ģʽ��
					{
						printf("\r\n�����͵�Ŀ��ָ��Ϊ: angle\r\n\r\n");
						compare |= 0x1000; //��־Ŀ��ָ�����������
					}
					else
						printf("\r\n���ڵ��ǶȽ����·��ʹ�����,����������\r\n\r\n");
				}
				else
				{
					printf("\r\nû�з��ϵ�Ŀ��ָ�����������\r\n\r\n");
				}		
				USART_RX_STA=0; //����˴����������ɱ�����׼�������ձ�����
				len = 0;
			}
			else if((USART_RX_STA&0x8000) && (compare & 0x1000)) //�ѽ��յ�Ŀ��ָ��
			{
				if(len == 2)  //����Ϊ2�����ݴ�����λ������
				{
					data = (u16)((USART_RX_BUF[0]-'0')*10) + (u16)(USART_RX_BUF[1]-'0');
				}
				else if(len == 3)  //����Ϊ3�����ݴ�����λ������С����
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
					printf("\r\n�����͵�Ŀ��ָ��Ϊ: kp\r\n\r\n");
					printf("\r\n�����͵�Ŀ������Ϊ: %f\r\n\r\n",data);
					PID.Kp = data;
					Check();
					ShowKp();
				}
				else if((compare & 0x000C) == 0x000C)
				{
					printf("\r\n�����͵�Ŀ��ָ��Ϊ: ki\r\n\r\n");
					printf("\r\n�����͵�Ŀ������Ϊ: %f\r\n\r\n",data);
					PID.Ki = data;     //Ŀ��ֵ��ֵ
					Check();           //��鸳ֵȡֵ��Χ
					ShowKi();          //��ʾKi
				}
				else if((compare & 0x0030) == 0x0030)
				{
					printf("\r\n�����͵�Ŀ��ָ��Ϊ: kd\r\n\r\n");
					printf("\r\n�����͵�Ŀ������Ϊ: %f\r\n\r\n",data);
					PID.Kd = data;     //Ŀ��ֵ��ֵ
					Check();           //��鸳ֵȡֵ��Χ
					ShowKd();          //��ʾKD
				}
				else if((compare & 0x0040) == 0x0040)
				{
					if(!mode) //����ģʽ��
					{
						printf("\r\n�����͵�Ŀ��ָ��Ϊ: v\r\n\r\n");
						printf("\r\n�����͵�Ŀ������Ϊ: %f\r\n\r\n",data);
						Clean_Aim();    //���Ŀ�����
						PID.Rin = data; //Ŀ��ֵ��ֵ
						Check();        //��鸳ֵȡֵ��Χ
						Aim();          //ˢ����ʾĿ�����
						ShowAN_V();     //ˢ����ʾ�ٶ�
					}
					else
						printf("\r\n���ڵ��ٽ����·��ʹ�����\r\n\r\n");
				}
				else if((compare & 0x0f80) == 0x0f80)
				{
					if(mode)//���Ƕ�ģʽ��
					{
						printf("\r\n�����͵�Ŀ��ָ��Ϊ: angle\r\n\r\n");
						printf("\r\n�����͵�Ŀ������Ϊ: %f\r\n\r\n",data);
						Clean_Aim();   //���Ŀ�����
						PID.Rin = data;//Ŀ��ֵ��ֵ
						count_A_TEMP=0;//�����ת�Ƕȱ�������ȡֵ
						PID.OUT=0;     //������ֵ
						Check();       //��鸳ֵȡֵ��Χ
						Aim();         //ˢ����ʾĿ�����
						ShowAN_V();    //ˢ����ʾ�Ƕ�
					}
					else
						printf("\r\n���ڵ��ǶȽ����·��ʹ�����\r\n\r\n");
				}
				USART_RX_STA=0;     //��ս���״̬��־
				compare = 0;        //�� 0 Ŀ��Ƚϱ���
				len = 0;
			}	
		}
	}
}
