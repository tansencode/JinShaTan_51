/*----------------------------------------------- 
 ���ܣ�������벢��ʾ 
------------------------------------------------*/  
#include <reg52.h>  
#include <intrins.h>  
//#include "delay.h"  
  
#define uint unsigned int  
#define uchar unsigned char  
  
sbit IR=P3^3;  //����ӿڱ�־  
char buff[10];  
  
void DisplayListChar(uchar X,uchar Y, unsigned char *DData);
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern bit irflag;
extern void InitLcd1602();
extern unsigned char ircode[4];
extern void InitInfrared();  
void initLCM(void);  
void cheng(char disdata);  
  
/*------------------------------------------------ 
                ȫ�ֱ������� 
------------------------------------------------*/  
  
unsigned char  irtime;//������ȫ�ֱ���  
  
bit irpro_ok,irok;  
unsigned char IRcord[4];  
unsigned char irdata[33];  
  
unsigned char TempData[16];  
/*------------------------------------------------ 
                  �������� 
------------------------------------------------*/  
void Ir_work(void);  
void Ircordpro(void);  
  
/*------------------------------------------------ 
                  ��ʱ��0�жϴ��� 
------------------------------------------------*/  
  
void tim0_isr (void) interrupt 1 using 1  
{  
  irtime++;  //���ڼ���2���½���֮���ʱ��  
}  
  
/*------------------------------------------------ 
                  �ⲿ�ж�1�жϴ��� 
------------------------------------------------*/  
void EX1_ISR (void) interrupt 2 //�ⲿ�ж�1������  
{  
  static unsigned char  i;             //���պ����źŴ���  
  static bit startflag;                //�Ƿ�ʼ�����־λ  
  
if(startflag)                           
   {  
    if(irtime<63&&irtime>=33)//������ TC9012��ͷ�룬9ms+4.5ms  
                        i=0;  
            irdata[i]=irtime;//�洢ÿ����ƽ�ĳ���ʱ�䣬�����Ժ��ж���0����1  
            irtime=0;  
            i++;  
             if(i==33)  
                {  
                 irok=1;  
                 i=0;  
                }  
          }  
     else  
        {  
        irtime=0;  
        startflag=1;  
        }  
  
}  
  
/*------------------------------------------------ 
                ��ʱ��0��ʼ�� 
------------------------------------------------*/  
void TIM0init(void)//��ʱ��0��ʼ��  
{  
  
  TMOD=0x02;//��ʱ��0������ʽ2��TH0����װֵ��TL0�ǳ�ֵ  
  TH0=0x00; //����ֵ  
  TL0=0x00; //��ʼ��ֵ  
  ET0=1;    //���ж�  
  TR0=1;      
}  
/*------------------------------------------------ 
                  �ⲿ�ж�1��ʼ�� 
------------------------------------------------*/  
void EX1init(void)  
{  
 IT1 = 1;   //ָ���ⲿ�ж�1�½��ش�����INT1 (P3.3)  
 EX1 = 1;   //ʹ���ⲿ�ж�  
 EA = 1;    //�����ж�  
}  
/*------------------------------------------------ 
                  ��ֵ���� 
------------------------------------------------*/  
  
void Ir_work(void)  
{  
     cheng(IRcord[2]);  
     LcdShowStr(12,0,buff);  
     cheng(IRcord[1]);  
     LcdShowStr(12,1,buff);  
     irpro_ok=0;//������ɱ�־  
  
  }  
/*------------------------------------------------ 
                ������ֵ���� 
------------------------------------------------*/  
void Ircordpro(void)//������ֵ������  
{   
  unsigned char i, j, k;  
  unsigned char cord,value;  
  
  k=1;  
  for(i=0;i<4;i++)      //����4���ֽ�  
     {  
      for(j=1;j<=8;j++) //����1���ֽ�8λ  
         {  
          cord=irdata[k];  
          if(cord>7)//����ĳֵΪ1������;����о��Թ�ϵ������ʹ��12M���㣬��ֵ������һ�����  
             value|=0x80;  
          if(j<8)  
            {  
             value>>=1;  
            }  
           k++;  
         }  
     IRcord[i]=value;  
     value=0;       
     }   
     irpro_ok=1;//������ϱ�־λ��1  
}  
  
/*------------------------------------------------ 
                 ������ 
------------------------------------------------*/  
void main(void)  
{  
 EX1init();            //��ʼ���ⲿ�ж�1  
 TIM0init();           //  
 //initLCM();
 //InitInfrared();   //��ʼ�����⹦��
 InitLcd1602();  
 LcdShowStr(0, 0,"Temperature:");  
 LcdShowStr(0, 1,"Humidity:");  
 while(1)//��ѭ��  
   {  
    //LcdShowStr(0, 0,"Temperature:");
	 //if (irflag)  //���յ���������ʱˢ����ʾ
    // irflag = 0;
    if(irok)                        //������պ��˽��к��⴦��  
      {     
       Ircordpro();  
       irok=0;  
      }  
  
    if(irpro_ok)                   //�������ú���й��������簴��Ӧ�İ�������ʾ��Ӧ�����ֵ�  
      {  
       Ir_work();  
      }  
   }  
}  
  
  
  
  
void cheng(char disdata)  
{  
  buff[0]=disdata/1000+48;  
  buff[1]=disdata%1000/100+48;  
  buff[2]=disdata%100/10+48;  
  buff[3]=disdata%10+48;  
}  