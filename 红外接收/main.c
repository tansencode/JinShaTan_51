#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
#define disout P2 
sbit  dula=P3^7; 
uchar code table[]={
0x45,0x46,0x47,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};
sbit remotein=P3^4;// 遥控输入
sbit sin=P3^0;   //基准正弦波相位输入
uint i,j,n0=1,n1=1,k,n,a=1;
uint keyvol,PWM_T=0;
sbit K1=P0^0;

sbit LED=P1^0;
sbit LED1=P1^1;

sbit  K2=P0^1;


sbit K3=P0^2;


sbit K4=P0^3;


sbit K5=P0^4;


sbit K6=P0^5;


sbit K7=P0^6;


sbit K8=P0^7;


uint i,j,m,n,k,s=1;
uint keyvol;              //值存放
//
/*********1 毫秒延时程序**********/
delay1ms(uint t)
{
for(i=0;i<t;i++)
for(j=0;j<120;j++);
}
/***********初始化函数**********/
clearmen()
{ 
dula=0; 
EX0=1;
EA=1;              //开总中断
TMOD=0x02;
TH0=0xFF;
TL0=0xE8;
ET0=1;
}
//

/**********函数************/
loop()
{
switch(disout&0x07)
{
case 0:{s=1;break;}
case 1:{s=2;break;} case 2:{s=3;break;} case 3:{s=4;break;} case 4:{s=5;break;} case 5:{s=6;break;} case 6:{s=7;break;} case 7:{s=8;break;} default:break;}
}
xianshi()
{
dula=1; P1=table[keyvol-2] ; dula=0;
}


//
//



/***********主函数***************/
main()
{
clearmen();              //初始化
//loop();
P2=0xff;
while(1);
//{
//while(sin==1); delay1ms(s); K8=0;delay1ms(1);K8=1;
//}
}
/************外中断遥控接收函数**************/
//外中断 0
void intt0(void) interrupt 0
{
EX0=0;keyvol=0;
if(remotein==0)
{delay1ms(1);
if(remotein==0)
{
while(1)
{
while(remotein==0);
keyvol++;k=0;
while(remotein==1)
{delay1ms(1);k++;
if(k>2)
{ 
xianshi();goto OOUUTT; xianshi();
};
}
}
OOUUTT:
switch(keyvol)
{
case 2:

TR0=0;
if(n0%2!=0)
{
LED=1;
}
else
LED=0;
if(n0==10)
{
n0=1;}
break;

//{K1=~K1;break;}
case 3:
    
	   	TR0=0;
if(n1%2!=0)
{
LED1=1;
}
else
LED=0;
if(n1==10)
{
n1=1;}
break;

	  //{K2=~K2;break;} case 4:{K3=~K3;break;} case 5:{K4=~K4;break;} case 6:{K5=~K5;break;} case 7:{K6=~K6;break;} case 8:{K7=~K7;break;}
//case 9:{if(disout==0x00){disout=0xff;}else{disout--;}loop();break;}
default:break;
}
}
}
EX0=1;
}