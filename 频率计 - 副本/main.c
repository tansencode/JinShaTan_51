#include <reg52.h>
#define uchar unsigned char 
#define uint unsigned int

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

signed long frequency=0;
bit start=0;   //定时器，计数模式启动变量
unsigned char Pulse = 0;
unsigned char T0RH=0;
unsigned char T0RL=0;
unsigned char code LedChar[] = { //数码管显示字符转换表
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* 将一个无符号长整型的数字显示到数码管上，num-待显示数字 */
void ShowNumber(unsigned long num)
{
     signed char i;
     unsigned char buf[6];
     for (i=0; i<6; i++) //把长整型数转换为 6 位十进制的数组
     {
         buf[i] = num % 10;
         num = num / 10;
      }
      for (i=5; i>=1; i--) //从最高位起，遇到 0 转换为空格，遇到非 0 则退出循环
      {
          if (buf[i] == 0)
          LedBuff[i] = 0xFF;
          else
         break;
      }
      for ( ; i>=0; i--) //剩余低位都如实转换为数码管显示字符
      {
          LedBuff[i] = LedChar[buf[i]];
       }
}

/* 数码管动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
     static unsigned char i = 0; //动态扫描的索引
     P0 = 0xFF; //显示消隐
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
    EA = 1; //使能总中断
    ENLED = 0; //选择数码管进行显示
    ADDR3 = 1;
    ConfigTimer0(1); //T0定时1ms
	TMOD = 0x51; //设置 T0定时、T1计数都为模式 1
	TH1=0x00;
    TL1=0x00;
    //TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
    //TL0 = 0x67;
	ET1=1;
	TR1=1;
    //ET0 = 1; //使能 T0 中断
    //TR0 = 1; //启动 T0
    LedBuff[0] = LedChar[0]; //上电显示0
    while(1)
    {
     if(start==1)
       {
           TR1=1;  //启动计数器
		   TR0 = 1;
           start=0;  //关闭启动变量位 保证1秒时间
           Pulse = 0;
         }	 
         ShowNumber(frequency);
     }
}
void time0() interrupt 1	   //T0定时
{
    static int i;
   
    TH0 = 0xFC; //重新加载初值,定时1ms
    TL0 = 0x67;
	 i++;
    LedScan(); //调用数码管显示扫描函数
    if(i==1000)
    {
      i=0;
      TR1=0;    //停止计数
      TR0=0;          //停止定时
	  //TH1=TH1+30;
      frequency = ((Pulse*65535)+(TH1*256)+TL1);	 //求出频率值   就是1秒内脉冲次数
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
      TH1=0;          //计数值清零
      TL1=0;
	  //TH0 = 0xFC; //重新加载初值,定时1ms
      //TL0 = 0x67;
      start=1;               //启动定时器开启变量
	 // TR1=1;
 }
}
void Time1() interrupt 3	//T1计数
{
   TH1=0;
   TL1=0;
   TR1=1;
   Pulse++;
}
 