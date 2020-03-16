#include<reg52.h>
#include<intrins.h>             //_nop_();��ʱ������
#define uchar unsigned char
#define uint unsigned int
#define key P2
sbit remoteout=P3^5;              //  ң�������
uchar temp;  //key;
uint m;
uchar keyvol;
extern void KeyScan();
extern void KeyDriver();
void KeyAction(unsigned char keycode);
void ConfigTimer0(unsigned int ms);
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

uchar code table[]={
0x45,0x46,0x47,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};

/***********��ʱ������**********/
void delayms(uint xms)
{
uint i,j;
for(i=xms;i>0;i--)
for(j=120;j>0;j--);
}
/***********��ʼ������**********/
init()
{
P2=0xFF;
keyvol=0;    //��������������
remoteout=0;              //��ң�����
IE=0x00;              //IE ��Ƭ���Ĵ���              �˾��ʾ��Ƭ����λ
IP=0x01;              //�����ⲿ�ж� 0
TMOD=0x20;              //��Ƭ�������ڷ�ʽ2 �� 8 λ�Զ���װ ģʽ
TH1=0xf3;              //40KHZ ��ֵ
TL1=0xf3;              //�� TF1 �������ʱ������ TH0��ĳ�����װ�� TL1
EA=1;              //�����ж�
}

void KeyAction(unsigned char keycode)
{  
    if(keycode==0x31)
	{
	   delayms(10);
	   while(keycode==0x31)
	   {
	      keyvol=1;
		}
	}
	if(keycode==0x32)
	{
	   delayms(10);
	   while(keycode==0x32)
	   {
	      keyvol=2;
		}
	}
	if(keycode==0x33)
	{
	   delayms(10);
	   while(keycode==0x33)
	   {
	      keyvol=3;
		}
	}
}
/***********������ʾ����**********/
void display(uchar num)
{
P0=table[num];
}
/**********���亯��************/
sed()
{
ET1=1;              //�������� 1 �ж�
TR1=1;              //����ʱ�� 1
delayms(3);
ET1=0;              //��
TR1=0;
remoteout=1;              //40KHZ �� 3 ����
for(m=keyvol;m>0;m--)
{
delayms(1);              //ͣ 1 ����
ET1=1; 
TR1=1; 
delayms(1);
ET1=0; 
TR1=0;
remoteout=1;              //40KHZ �� 1 ����
}
delayms(10);
}

tx()              //����Ӻ������ڷ��������������ͬ�İ���װ�벻ͬ�İ�������
{
switch(key)
{
case 0:key=key+1;sed();break; 
case 1:key=key+1;sed();break; 
case 2:key=key+1;sed();break; 
case 3:key=key+1;sed();break; 
case 4:key=key+1;sed();break; 
case 5:key=key+1;sed();break; 
case 6:key=key+1;sed();break; 
case 7:key=key+1;sed();break; 
case 8:key=key+1;sed();break; 
case 9:key=key+1;sed();break; 
case 10:key=key+1;sed();break; 
case 11:key=key+1;sed();break; 
case 12:key=key+1;sed();break; 
case 13:key=key+1;sed();break; 
case 14:key=key+1;sed();break; 
case 15:key=key+1;sed();break; 
default:break;
}
}
/***********����ɨ�躯��**********/
void matrixkeyscan()
{
P2=0xfe; temp= P2; temp=temp&0xf0; if(temp!=0xf0)
{
delayms(10);
temp= P2; temp=temp&0xf0; if(temp!=0xf0)
{ temp=P2; 
switch(temp)
{
case 0xee:   key=0;break; 
case 0xde:    key=1;break;
case 0xbe:    key=2;break; 
case 0x7e:    key=3;break;
}
while(temp!=0xf0)
{ temp=P2; temp=temp&0xf0;
}
display(key);



tx();
}
}
P2=0xfd; temp=P2; temp=temp&0xf0; if(temp!=0xf0)
{
delayms(10);
temp=P2; temp=temp&0xf0; if(temp!=0xf0)
{ temp=P2; switch(temp)
{
case 0xed:    key=4;break; case 0xdd:   key=5;break; case 0xbd:   key=6;break; case 0x7d:   key=7;break;
}
while(temp!=0xf0)
{
temp=P2;temp=temp&0xf0;
}
display(key);
tx();
}

} P2=0xfb;temp=P2;temp=temp&0xf0; if(temp!=0xf0)
{ delayms(10);temp=P2;temp=temp&0xf0; if(temp!=0xf0)
{ temp=P2; switch(temp)
{
case 0xeb:              key=8;break; 
case 0xdb:              key=9;break; 
case 0xbb:              key=10;break; 
case 0x7b:              key=11;break;
}
while(temp!=0xf0)
{
temp=P2;temp=temp&0xf0;
}
display(key);
tx();
}
} P2=0xf7;temp=P2;temp=temp&0xf0; if(temp!=0xf0)
{
delayms(10);
temp=P2;



temp=temp&0xf0;
if(temp!=0xf0)
{
temp=P2;
switch(temp)
{
case 0xe7:    key=12;break; 
case 0xd7:   key=13;break; 
case 0xb7:    key=14;break; 
case 0x77:    key=15;break;
}
while(temp!=0xf0)
{ temp=P2; temp=temp&0xf0;
} display(key); tx();
}
}
}
/***********������**********/
void main()
{
    init();
	EA = 1;           //�����ж�
	ConfigTimer0(1);  //����T0��ʱ1ms
    while(1)
    {
	   KeyDriver();
       //keywork();
       if(keyvol!=0)
       {
          send();
          keyvol=0;
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
    tmp = tmp + 28;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�а���ɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    KeyScan();   //����ɨ��
}

/*********40KHZ ������***********/
//              ��ʱ�ж� T1
void time_intt1(void) interrupt 3
{
remoteout=~remoteout;              //ң�ؿ���ȡ��
}