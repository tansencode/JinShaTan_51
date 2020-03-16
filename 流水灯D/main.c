#include<reg52.h>
#include<intrins.h>

#define uint unsigned int   
#define uchar unsigned char 
bit flag300ms = 1;       //300ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char RxdByte = 0;  //���ڽ��յ����ֽ�

void ConfigTimer0(unsigned int ms);

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;
static int cnt=0;
static int n=0;                //ѭ����λ����
unsigned char ledd[30]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB,0xE7,
              0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,
			  0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xEF};
    //��ʵ���Ƕ�����һ������״̬��,���м䵽���ߣ��������ߵ��м�,Ȼ��P0ѭ��������
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111
void main()
{
    unsigned int i=0;
	EA=1;
	//static int cnt=0;
//	ConfigTimer0(10);  //����T0��ʱ10ms
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
	 ConfigTimer0(10);  //����T0��ʱ10ms
	 //P0=~0x01;		   //�����ⲿ���壬�ƶ�����ֻ�����ƶ�������ֱ�ӵ�����һ��С��
   	//P0=0xFF;
//	ConfigTimer0(10);  //����T0��ʱ10ms
   while(1)
	
    {
	   //ConfigTimer0(10);  //����T0��ʱ10ms
	   if(flag300ms)
	    {
		  flag300ms=0;
		   
				 //P0=~(0x01<<n);
				 P0=ledd[n];
				 //for(cnt=0;cnt<8;cnt++)    //��λ�жϣ�������7λ����һ����������λ
				 //if(n==8)
				// {
				// n=0;
				// P0=~0x01;
				// }
   
		}
	
  }
}

/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}

/* T0�жϷ�������ִ��300ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    //while (TF0 == 0);  //��TF0����0ʱһֱִ�п�ѭ������ͣ������ֱ��T0���
    //TF0 = 0;           //T0����������жϱ�־ 
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	
    tmr300ms++;
    if (tmr300ms >= 100)  //��ʱ300ms
    {
        
	n++;
	    tmr300ms = 0;
	 flag300ms = 1;
	 if(n==6)
				 {
				 n=0;
				 }
    }
}