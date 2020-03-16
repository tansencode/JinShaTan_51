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
unsigned char T1RH = 0;    //T1重载值的高字节
unsigned char T1RL = 0;    //T1重载值的低字节
bit flag1s = 1;       //300ms定时标志
static unsigned char cnt=0;
static unsigned int t=0;
unsigned char leddd[7]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB};
    //其实就是定义了一个亮灯状态表,由中间到两边，再由两边倒中间,然后P0循环此数组
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111

extern void ConfigTimer0(unsigned int ms);
void ConfigTimer1(unsigned int ms);
/* 软件延时函数，延时时间(t*10)us */
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
  EA = 1; //使能总中断,打开总开关
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
        P0=~(0x01<<cnt);		//逻辑左移,取反，0000 0001——1111 1100
		
		}
		
     
	 /*for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0xC0>>cnt);		//逻辑右移,取反，1100 0000——0011 1111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
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
    	P0=~(0x80>>cnt);		//逻辑右移,取反，1000 0000——0011 1111
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
	 for(cnt=0;cnt<7;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x03<<cnt);		//逻辑左移,取反，0000 0011——1111 1100
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0xC0>>cnt);		//逻辑右移,取反，1100 0000——0011 1111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
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
	 for(cnt=0;cnt<8;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x03<<cnt);		//逻辑左移,取反，0000 0011——1111 1100
		for(i=0;i<25000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<8;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0xC0>>cnt);		//逻辑右移,取反，1100 0000——0011 1111
		for(i=0;i<25000;i++);	  //延时函数，不断执行空循环
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
	 for(cnt=0;cnt<7;cnt=cnt+2)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x08<<cnt);		//逻辑左移,取反，0000 1000——1111 0111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt=cnt+2)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0x10>>cnt);		//逻辑右移,取反，0001 0000——1110 1111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
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

/* 配置并启动T1，ms-定时时间 */
void ConfigTimer1(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 12;           //补偿中断响应延时造成的误差
    T1RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //清零T1的控制位
    TMOD |= 0x10;   //配置T1为模式1
    TH1 = T1RH;     //加载T1重载值
    TL1 = T1RL;
    ET1 = 1;        //使能T1中断
    TR1 = 1;        //启动T1
}

/* T1中断服务函数，定时动态调整占空比 */
void InterruptTimer1() interrupt 3
{
    static unsigned int tmr1s=0;
	static bit dir = 0;
    static unsigned char index = 0;
    TH1 = T1RH;  //重新加载T1重载值
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