#include<reg52.h>
#include <intrins.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;
extern void KeyScan();
extern void KeyDriver();
extern void KeyAction(unsigned char keycode);
unsigned char T1RH = 0;    //T1����ֵ�ĸ��ֽ�
unsigned char T1RL = 0;    //T1����ֵ�ĵ��ֽ�
bit flag1s = 1;       //300ms��ʱ��־
static unsigned char cnt=0;
static unsigned int t=0;
unsigned char leddd[7]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB};
    //��ʵ���Ƕ�����һ������״̬��,���м䵽���ߣ��������ߵ��м�,Ȼ��P0ѭ��������
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111

extern void ConfigTimer0(unsigned int ms);
void ConfigTimer1(unsigned int ms);
/* �����ʱ��������ʱʱ��(t*10)us */
/*void DelayX10us(unsigned char t)
{
    do {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--t);
} 	*/
void Led10()
{
  EA = 1; //ʹ�����ж�,���ܿ���
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  //P0=0xFF;
  P0=0xFF;
  ConfigTimer1(10);
  while(1)
  {
	 	if(flag1s)
		{
		flag1s=0;
        P0=~(0x01<<cnt);		//�߼�����,ȡ����0000 0001����1111 1100
		
		}
		
     
	 /*for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0xC0>>cnt);		//�߼�����,ȡ����1100 0000����0011 1111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }	 */
  }
}

void Led11()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
	  if(flag1s)
	{
	    flag1s=0;
    	P0=~(0x80>>cnt);		//�߼�����,ȡ����1000 0000����0011 1111
	  }
	  P0=0xFF;
}
  

void Led12()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x03<<cnt);		//�߼�����,ȡ����0000 0011����1111 1100
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0xC0>>cnt);		//�߼�����,ȡ����1100 0000����0011 1111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}
void Led13()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  //while(1)
  {
	 for(cnt=0;cnt<8;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x03<<cnt);		//�߼�����,ȡ����0000 0011����1111 1100
		for(i=0;i<25000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<8;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0xC0>>cnt);		//�߼�����,ȡ����1100 0000����0011 1111
		for(i=0;i<25000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}
void Led14()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt=cnt+2)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x08<<cnt);		//�߼�����,ȡ����0000 1000����1111 0111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt=cnt+2)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0x10>>cnt);		//�߼�����,ȡ����0001 0000����1110 1111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}  

void Led15()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   	P0=0xFF;
    while(1)
    {
        for(cnt=0;cnt<6;cnt++)
		{
            P0=leddd[cnt];
			for(i=0;i<30000;i++);
		}
  }
}

/* ���ò�����T1��ms-��ʱʱ�� */
void ConfigTimer1(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T1RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //����T1�Ŀ���λ
    TMOD |= 0x10;   //����T1Ϊģʽ1
    TH1 = T1RH;     //����T1����ֵ
    TL1 = T1RL;
    ET1 = 1;        //ʹ��T1�ж�
    TR1 = 1;        //����T1
}

/* T1�жϷ���������ʱ��̬����ռ�ձ� */
void InterruptTimer1() interrupt 3
{
    static unsigned int tmr1s=0;
	static bit dir = 0;
    static unsigned char index = 0;
    TH1 = T1RH;  //���¼���T1����ֵ
    TL1 = T1RL;

	tmr1s++;
	if(tmr1s>=100)
	{
	    tmr1s=0;
		flag1s=1;
		cnt++;
		t++;
		if(cnt>=8)
		cnt=0;

	}
}