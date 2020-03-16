#include<reg52.h>
#include<intrins.h>             //_nop_();延时函数用
#define uchar unsigned char
#define uint unsigned int
#define key P2
sbit remoteout=P3^5;              //  遥控输出口
uchar temp;  //key;
uint m;
uchar keyvol;
extern void KeyScan();
extern void KeyDriver();
void KeyAction(unsigned char keycode);
void ConfigTimer0(unsigned int ms);
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

uchar code table[]={
0x45,0x46,0x47,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};

/***********延时化函数**********/
void delayms(uint xms)
{
uint i,j;
for(i=xms;i>0;i--)
for(j=120;j>0;j--);
}
/***********初始化函数**********/
init()
{
P2=0xFF;
keyvol=0;    //脉冲个数标记清零
remoteout=0;              //关遥控输出
IE=0x00;              //IE 单片机寄存器              此句表示单片机复位
IP=0x01;              //开启外部中断 0
TMOD=0x20;              //单片机工作在方式2 即 8 位自动重装 模式
TH1=0xf3;              //40KHZ 初值
TL1=0xf3;              //当 TF1 计数溢出时，常数 TH0里的常数重装入 TL1
EA=1;              //开总中断
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
/***********按键显示函数**********/
void display(uchar num)
{
P0=table[num];
}
/**********发射函数************/
sed()
{
ET1=1;              //开计数器 1 中断
TR1=1;              //开定时器 1
delayms(3);
ET1=0;              //关
TR1=0;
remoteout=1;              //40KHZ 发 3 毫秒
for(m=keyvol;m>0;m--)
{
delayms(1);              //停 1 毫秒
ET1=1; 
TR1=1; 
delayms(1);
ET1=0; 
TR1=0;
remoteout=1;              //40KHZ 发 1 毫秒
}
delayms(10);
}

tx()              //这个子函数是在发射脉冲个数，不同的按键装入不同的按键个数
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
/***********按键扫描函数**********/
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
/***********主函数**********/
void main()
{
    init();
	EA = 1;           //开总中断
	ConfigTimer0(1);  //配置T0定时1ms
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

/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 28;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行按键扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
}

/*********40KHZ 发生器***********/
//              定时中断 T1
void time_intt1(void) interrupt 3
{
remoteout=~remoteout;              //遥控开关取反
}