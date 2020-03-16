#include<reg52.h>
#include<intrins.h>

#define uint unsigned int   
#define uchar unsigned char 
bit flag300ms = 1;       //300ms定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char RxdByte = 0;  //串口接收到的字节

void ConfigTimer0(unsigned int ms);

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;
static int cnt=0;
static int n=0;                //循环移位变量
unsigned char ledd[30]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB,0xE7,
              0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,
			  0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xEF};
    //其实就是定义了一个亮灯状态表,由中间到两边，再由两边倒中间,然后P0循环此数组
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111
void main()
{
    unsigned int i=0;
	EA=1;
	//static int cnt=0;
//	ConfigTimer0(10);  //配置T0定时10ms
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
	 ConfigTimer0(10);  //配置T0定时10ms
	 //P0=~0x01;		   //需在外部定义，移动函数只负责移动，不能直接点亮第一个小灯
   	//P0=0xFF;
//	ConfigTimer0(10);  //配置T0定时10ms
   while(1)
	
    {
	   //ConfigTimer0(10);  //配置T0定时10ms
	   if(flag300ms)
	    {
		  flag300ms=0;
		   
				 //P0=~(0x01<<n);
				 P0=ledd[n];
				 //for(cnt=0;cnt<8;cnt++)    //移位判断，向左移7位，第一个不记做移位
				 //if(n==8)
				// {
				// n=0;
				// P0=~0x01;
				// }
   
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
    tmp = tmp + 12;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}

/* T0中断服务函数，执行300ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    //while (TF0 == 0);  //当TF0等于0时一直执行空循环，即停在这里直到T0溢出
    //TF0 = 0;           //T0溢出后，清零中断标志 
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	
    tmr300ms++;
    if (tmr300ms >= 100)  //定时300ms
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