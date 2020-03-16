#include <reg51.h>
#define uchar unsigned char 
#define uint unsigned int

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

//uint ff;   //����Ƶ��ֵ
signed long frequency=0;
uchar start=1;   //��ʱ��������ģʽ��������
bit Flag = 0;
unsigned char Counter = 0,Hdata = 0,Ldata = 0,Pulse = 0;

code uchar seg[]={0x28,0x7e,0xa2,0x62,0x74,0x61,0x21,0x7a,0x20,0x60};
code uchar tab[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};

unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
{ 0x31, 0x32, 0x33, 0x26 }, //���ּ� 1�����ּ� 2�����ּ� 3�����ϼ�
{ 0x34, 0x35, 0x36, 0x25 }, //���ּ� 4�����ּ� 5�����ּ� 6�������
{ 0x37, 0x38, 0x39, 0x28 }, //���ּ� 7�����ּ� 8�����ּ� 9�����¼�
{ 0x30, 0x1B, 0x0D, 0x27 } //���ּ� 0��ESC ���� �س����� ���Ҽ�
};

void delay(uint k)  //��ʱ����
{
    int p;
    while(k--)
	for(p=0;p<110;p++);     
}
/*void show(uint k)  //��ֵ��ʾ����
{
  static uchar i,j;
  uint ss[5];
  i=0;
  do
  {
   ss[i]=k%10;
   i++;
  }while(k/=10);
  for(j=0;j<i;j++)
  {
   P0=seg[ss[j]];
   P2=tab[j];
   delay(100);
   P0=0xff;
   P2=0xff;
  }  
}	  */


void Time1() interrupt 3
{
   TH1=0;
   TL1=0;
   TR1=1;
   Pulse++;
}  
void init()
{
TMOD = 0x51;
TH0 = 0x4c;
TL0 = 0x00;
TH1 = 0x00;
TL1 = 0x00;
ET0 = 1;
ET1 = 1;
EA = 1;
PT1 = 1;
TR0 = 1;
TR1 = 1;
}

/* ���������ʾһλ���֣�index-�����λ����(���ҵ����Ӧ0��5)��
**     num-����ʾ�����֣�point-�����Ƿ���ʾ��λ�ϵ�С���� */
void ShowLedNumber(unsigned int index, unsigned int num,unsigned int point)
{
   P0=0xFF;
   LedBuff[index] = LedChar[num];  //��������ת��Ϊ������ַ�0��F
    if (point != 0)
    {
       LedBuff[index] &= 0x7F;  //point��Ϊ0ʱ������ǰλ��С����
    }
}
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
/*void time0_init()
{
 TMOD|=0x01;  //��ʱ��0 ��ʱģʽ ������Χ65536
 TH0 = 0xFC; //���¼��س�ֵ
  TL0 = 0x67;
 TR0=0;     //��ͣ��ʱ��0
}  
void time1_init()
{
 TMOD|=0x50;     //��ʱ��1  ����ģʽ��������Χ65536
 TH1=0;      //������ֵ0
 TL1=0;
 TR1=0;    //��ͣ������
}
void str_init()    //��ʱ���ж�
{
   EA=1;
   ET0=1;
}  	 */
void time0_event() interrupt 1
{
    static uchar i;
   
    TH0 = 0xFC; //���¼��س�ֵ,��ʱ1ms
    TL0 = 0x67;
	 i++;
    LedScan(); //�����������ʾɨ�躯��
    if(i==1000)
    {
      i=0;
	 // TR0=0;
      TR1=0;    //ֹͣ����
     // TR0=0;          //ֹͣ��ʱ
      frequency = ((Pulse*65535)+(TH1*256)+TL1);
      //frequency=(TH1*256+TL1);   //���Ƶ��ֵ   ����1�����������
	  Pulse=0;
      TH1=0;          //����ֵ����
      TL1=0;
      //TH0 = 0xFC; //���¼��س�ֵ
     //TL0 = 0x67;
      start=1;               //������ʱ����������
	  TR1=1;
 }
}
 
void main()
{
  //ff=60000;
  //unsigned long frequency;
   init();
    EA = 1; //ʹ�����ж�
    ENLED = 0; //ѡ������ܽ�����ʾ
    ADDR3 = 1;
    P3 = 0xFF;
    TMOD = 0x01; //���� T0 Ϊģʽ 1
//	TMOD = 0x51; //���� T0 Ϊģʽ 1
	TH1=0x00;
    TL1=0x00;
    TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
    TL0 = 0x67;
	ET1=1;
	TR1=1;
    ET0 = 1; //ʹ�� T0 �ж�
    TR0 = 1; //���� T0
    LedBuff[0] = LedChar[0]; //�ϵ���ʾ 0
   // time0_init();
   // time1_init();
   // str_init();
    while(1)
    {
     if(start==1)
       {
           //TR0=1;  //������ʱ��
           TR1=1;  //����������
           Hdata = TH1;
           Ldata = TL1;
           //P3^5=frequency;
           frequency = ((Pulse*65535)+(TH1*256)+TL1);
   //Flag = 0;
           start=0;  //�ر���������λ ��֤1��ʱ��
           Pulse = 0;
//LcdDisplay(frequency);
         }	 
         ShowNumber(frequency);
		  //ShowLedNumber(5,(frequency/100000)%10,0);			   //ʮλ
	   	  //ShowLedNumber(4,(frequency/10000)%10,0);	 //��λ
		  //ShowLedNumber(3,(frequency/1000)%10,0);	//ʮ��λ
		 // ShowLedNumber(2,(frequency/100)%10,0);  //�ٷ�λ
		 // ShowLedNumber(1,(frequency/10)%10,0);		//ǧ��λ
		 // ShowLedNumber(0,frequency%10,0);		//ǧ��λ
		 // delay(50);
     }
}

/*void InterruptTimer0() interrupt 1
{
TH0 = 0xFC; //���¼��س�ֵ
TL0 = 0x67;
LedScan(); //�����������ʾɨ�躯��
} */
/*void Time0() interrupt 1
{
Counter++;
LedScan(); //�����������ʾɨ�躯��
if (Counter == 1)
TR0 = 0;
TR1 = 0;
Counter = 0;
start = 1;
}  */
