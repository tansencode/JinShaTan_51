#include <reg52.h>
#define uchar unsigned char 
#define uint unsigned int

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

signed long frequency=0;
bit start=0;   //��ʱ��������ģʽ��������
unsigned char Pulse = 0;
unsigned char T0RH=0;
unsigned char T0RL=0;
unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* ��һ���޷��ų����͵�������ʾ��������ϣ�num-����ʾ���� */
void ShowNumber(unsigned long num)
{
     signed char i;
     unsigned char buf[6];
     for (i=0; i<6; i++) //�ѳ�������ת��Ϊ 6 λʮ���Ƶ�����
     {
         buf[i] = num % 10;
         num = num / 10;
      }
      for (i=5; i>=1; i--) //�����λ������ 0 ת��Ϊ�ո������� 0 ���˳�ѭ��
      {
          if (buf[i] == 0)
          LedBuff[i] = 0xFF;
          else
         break;
      }
      for ( ; i>=0; i--) //ʣ���λ����ʵת��Ϊ�������ʾ�ַ�
      {
          LedBuff[i] = LedChar[buf[i]];
       }
}

/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
     static unsigned char i = 0; //��̬ɨ�������
     P0 = 0xFF; //��ʾ����
     switch (i)
     {
          case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
          case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
          case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
          case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
          case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
          case 5: ADDR2=1; ADDR1=0; ADDR0=1; i=0; P0=LedBuff[5]; break;
          default: break;
      }
}

void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;
	tmp=11059200/12;
	tmp=65536-tmp;
	tmp=tmp+23;
	T0RH=(unsigned char)(tmp>>8);
	T0RL=(unsigned char)tmp;
	TMOD&=0x0F;
	TMOD|=0x10;
	TH0=T0RH;
	TL0=T0RL;
	ET0=1; 
	TR0=1;
}
void main()
{
	P3 = 0xFF;
    EA = 1; //ʹ�����ж�
    ENLED = 0; //ѡ������ܽ�����ʾ
    ADDR3 = 1;
    ConfigTimer0(1); //T0��ʱ1ms
	TMOD = 0x51; //���� T0��ʱ��T1������Ϊģʽ 1
	TH1=0x00;
    TL1=0x00;
    //TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
    //TL0 = 0x67;
	ET1=1;
	TR1=1;
    //ET0 = 1; //ʹ�� T0 �ж�
    //TR0 = 1; //���� T0
    LedBuff[0] = LedChar[0]; //�ϵ���ʾ0
    while(1)
    {
     if(start==1)
       {
           TR1=1;  //����������
		   TR0 = 1;
           start=0;  //�ر���������λ ��֤1��ʱ��
           Pulse = 0;
         }	 
         ShowNumber(frequency);
     }
}
void time0() interrupt 1	   //T0��ʱ
{
    static int i;
   
    TH0 = 0xFC; //���¼��س�ֵ,��ʱ1ms
    TL0 = 0x67;
	 i++;
    LedScan(); //�����������ʾɨ�躯��
    if(i==1000)
    {
      i=0;
      TR1=0;    //ֹͣ����
      TR0=0;          //ֹͣ��ʱ
	  //TH1=TH1+30;
      frequency = ((Pulse*65535)+(TH1*256)+TL1);	 //���Ƶ��ֵ   ����1�����������
	  if(frequency>800&&frequency<1100)
	  {
	     frequency=frequency+50;
	   }
	  else if(frequency>8000&&frequency<11000)
	  {
	     frequency=frequency+1210;
	   }
	  else if(frequency>80000&&frequency<110000)
	  {
	     frequency=frequency+12900;
	   }
	  else if(frequency>190000&&frequency<210000)
	  {
	   frequency=frequency+8400;
	   }
	  Pulse=0;
      TH1=0;          //����ֵ����
      TL1=0;
	  //TH0 = 0xFC; //���¼��س�ֵ,��ʱ1ms
      //TL0 = 0x67;
      start=1;               //������ʱ����������
	 // TR1=1;
 }
}
void Time1() interrupt 3	//T1����
{
   TH1=0;
   TL1=0;
   TR1=1;
   Pulse++;
}
 