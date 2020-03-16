#include <reg52.h>
sbit PWMOUT = P0^0;
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
unsigned char HighRH = 0; //高电平重载值的高字节
unsigned char HighRL = 0; //高电平重载值的低字节
unsigned char LowRH = 0; //低电平重载值的高字节
unsigned char LowRL = 0; //低电平重载值的低字节
void ConfigPWM(unsigned int fr, unsigned char dc);
void ClosePWM();
void main()
{
unsigned int i;
EA = 1; //开总中断
ENLED = 0; //使能独立 LED
ADDR3 = 1;
ADDR2 = 1;
ADDR1 = 1;
ADDR0 = 0;
P0=0x00;
while (1)
{
ConfigPWM(100, 10); //频率 100Hz，占空比 10%
for (i=0; i<40000; i++);
ClosePWM();
ConfigPWM(100, 40); //频率 100Hz，占空比 40%
for (i=0; i<40000; i++);
ClosePWM();
ConfigPWM(100, 90); //频率 100Hz，占空比 90%
for (i=0; i<40000; i++);
ClosePWM(); //关闭 PWM，相当于占空比 100%
for (i=0; i<40000; i++);
}
}
/* 配置并启动 PWM，fr-频率，dc-占空比 */
void ConfigPWM(unsigned int fr, unsigned char dc)
{
unsigned int high, low;
unsigned long tmp;
tmp = (11059200/12) / fr; //计算一个周期所需的计数值
high = (tmp*dc) / 100; //计算高电平所需的计数值
low = tmp - high; //计算低电平所需的计数值
high = 65536 - high + 12; //计算高电平的重载值并补偿中断延时
low = 65536 - low + 12; //计算低电平的重载值并补偿中断延时
HighRH = (unsigned char)(high>>8); //高电平重载值拆分为高低字节
HighRL = (unsigned char)high;
LowRH = (unsigned char)(low>>8); //低电平重载值拆分为高低字节
LowRL = (unsigned char)low;
TMOD &= 0xF0; //清零 T0 的控制位
TMOD |= 0x01; //配置 T0 为模式 1
TH0 = HighRH; //加载 T0 重载值
TL0 = HighRL;
ET0 = 1; //使能 T0 中断
TR0 = 1; //启动 T0
PWMOUT = 1; //输出高电平
}
/* 关闭 PWM */
void ClosePWM()
{
TR0 = 0; //停止定时器
ET0 = 0; //禁止中断
PWMOUT = 1; //输出高电平
}
/* T0 中断服务函数，产生 PWM 输出 */
void InterruptTimer0() interrupt 1
{
if (PWMOUT == 1) //当前输出为高电平时，装载低电平值并输出低电平
{
TH0 = LowRH;
TL0 = LowRL;
PWMOUT = 0;
}
else //当前输出为低电平时，装载高电平值并输出高电平
{
TH0 = HighRH;
TL0 = HighRL;
PWMOUT = 1;
}
}