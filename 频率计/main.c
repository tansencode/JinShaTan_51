#include <reg51.h>
#define uchar unsigned char 
#define uint unsigned int

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

//uint ff;   //接受频率值
signed long frequency=0;
uchar start=1;   //定时器，计数模式启动变量
bit Flag = 0;
unsigned char Counter = 0,Hdata = 0,Ldata = 0,Pulse = 0;

code uchar seg[]={0x28,0x7e,0xa2,0x62,0x74,0x61,0x21,0x7a,0x20,0x60};
code uchar tab[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};

unsigned char code LedChar[] = { //数码管显示字符转换表
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
{ 0x31, 0x32, 0x33, 0x26 }, //数字键 1、数字键 2、数字键 3、向上键
{ 0x34, 0x35, 0x36, 0x25 }, //数字键 4、数字键 5、数字键 6、向左键
{ 0x37, 0x38, 0x39, 0x28 }, //数字键 7、数字键 8、数字键 9、向下键
{ 0x30, 0x1B, 0x0D, 0x27 } //数字键 0、ESC 键、 回车键、 向右键
};

void delay(uint k)  //延时函数
{
    int p;
    while(k--)
	for(p=0;p<110;p++);     
}
/*void show(uint k)  //数值显示函数
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

/* 数码管上显示一位数字，index-数码管位索引(从右到左对应0～5)，
**     num-待显示的数字，point-代表是否显示此位上的小数点 */
void ShowLedNumber(unsigned int index, unsigned int num,unsigned int point)
{
   P0=0xFF;
   LedBuff[index] = LedChar[num];  //输入数字转换为数码管字符0～F
    if (point != 0)
    {
       LedBuff[index] &= 0x7F;  //point不为0时点亮当前位的小数点
    }
}
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
/*void time0_init()
{
 TMOD|=0x01;  //定时器0 定时模式 计数范围65536
 TH0 = 0xFC; //重新加载初值
  TL0 = 0x67;
 TR0=0;     //暂停定时器0
}  
void time1_init()
{
 TMOD|=0x50;     //定时器1  计数模式，计数范围65536
 TH1=0;      //计数初值0
 TL1=0;
 TR1=0;    //暂停计数器
}
void str_init()    //定时器中断
{
   EA=1;
   ET0=1;
}  	 */
void time0_event() interrupt 1
{
    static uchar i;
   
    TH0 = 0xFC; //重新加载初值,定时1ms
    TL0 = 0x67;
	 i++;
    LedScan(); //调用数码管显示扫描函数
    if(i==1000)
    {
      i=0;
	 // TR0=0;
      TR1=0;    //停止计数
     // TR0=0;          //停止定时
      frequency = ((Pulse*65535)+(TH1*256)+TL1);
      //frequency=(TH1*256+TL1);   //求出频率值   就是1秒内脉冲次数
	  Pulse=0;
      TH1=0;          //计数值清零
      TL1=0;
      //TH0 = 0xFC; //重新加载初值
     //TL0 = 0x67;
      start=1;               //启动定时器开启变量
	  TR1=1;
 }
}
 
void main()
{
  //ff=60000;
  //unsigned long frequency;
   init();
    EA = 1; //使能总中断
    ENLED = 0; //选择数码管进行显示
    ADDR3 = 1;
    P3 = 0xFF;
    TMOD = 0x01; //设置 T0 为模式 1
//	TMOD = 0x51; //设置 T0 为模式 1
	TH1=0x00;
    TL1=0x00;
    TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
    TL0 = 0x67;
	ET1=1;
	TR1=1;
    ET0 = 1; //使能 T0 中断
    TR0 = 1; //启动 T0
    LedBuff[0] = LedChar[0]; //上电显示 0
   // time0_init();
   // time1_init();
   // str_init();
    while(1)
    {
     if(start==1)
       {
           //TR0=1;  //启动定时器
           TR1=1;  //启动计数器
           Hdata = TH1;
           Ldata = TL1;
           //P3^5=frequency;
           frequency = ((Pulse*65535)+(TH1*256)+TL1);
   //Flag = 0;
           start=0;  //关闭启动变量位 保证1秒时间
           Pulse = 0;
//LcdDisplay(frequency);
         }	 
         ShowNumber(frequency);
		  //ShowLedNumber(5,(frequency/100000)%10,0);			   //十位
	   	  //ShowLedNumber(4,(frequency/10000)%10,0);	 //个位
		  //ShowLedNumber(3,(frequency/1000)%10,0);	//十分位
		 // ShowLedNumber(2,(frequency/100)%10,0);  //百分位
		 // ShowLedNumber(1,(frequency/10)%10,0);		//千分位
		 // ShowLedNumber(0,frequency%10,0);		//千分位
		 // delay(50);
     }
}

/*void InterruptTimer0() interrupt 1
{
TH0 = 0xFC; //重新加载初值
TL0 = 0x67;
LedScan(); //调用数码管显示扫描函数
} */
/*void Time0() interrupt 1
{
Counter++;
LedScan(); //调用数码管显示扫描函数
if (Counter == 1)
TR0 = 0;
TR1 = 0;
Counter = 0;
start = 1;
}  */
