#include <reg52.h>

sbit LED=P0^0;
sbit LED1=P0^1;
sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;


sbit BUZZ = P1^6; //蜂鸣器控制引脚
bit flagBuzzOn = 0; //蜂鸣器启动标志

unsigned char T0RH = 0; //T0 重载值的高字节
unsigned char T0RL = 0; //T0 重载值的低字节
void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main()
{
EA = 1; //开总中断
ConfigTimer0(1); //配置 T0 定时 1ms
ConfigUART(9600); //配置波特率为 9600
while (1)
{
UartDriver(); //调用串口驱动
}
}

/* 内存比较函数，比较两个指针所指向的内存数据是否相同，
ptr1-待比较指针 1，ptr2-待比较指针 2，len-待比较长度
返回值-两段内存数据完全相同时返回 1，不同返回 0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
while (len--)
{
if (*ptr1++ != *ptr2++) //遇到不相等数据时即刻返回 0
{
return 0;
}
}
return 1; //比较完全部长度数据都相等则返回 1
}
void ledshanshuo()			//LED2闪烁和LED1亮
{
   unsigned int i=0;
   ENLED=0;
   ADDR3=1;

   ADDR2=1;
   ADDR1=1;
   ADDR0=0;
   while(1)
   {
    LED=0;
     LED1=0;
	 for(i=0;i<3000;i++);
	 LED1=1;
	 for(i=0;i<3000;i++);
    }
}

void led()
{
unsigned int i=0;
   ENLED=0;
   ADDR3=1;

   ADDR2=1;
   ADDR1=1;
   ADDR0=0;
   LED=0;
   while(1);
}

/* 串口动作函数，根据接收到的命令帧执行响应的动作
buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
unsigned char i;
unsigned char code cmd0[] = "1"; //led闪烁、led亮
unsigned char code cmd1[] = "2"; //打开蜂鸣器命令
unsigned char code cmd2[] = "3"; //关闭蜂鸣器命令
unsigned char code cmdLen[] = { //命令长度汇总表
sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
};
unsigned char code *cmdPtr[] = { //命令指针汇总表
&cmd0[0], &cmd1[0], &cmd2[0],
};
for (i=0; i<sizeof(cmdLen); i++) //遍历命令列表，查找相同命令
{
if (len >= cmdLen[i]) //首先接收到的数据长度要不小于命令长度
{
if (CmpMemory(buf, cmdPtr[i], cmdLen[i])) //比较相同时退出循环
{
break;
}
}
}
switch (i) //循环退出时 i 的值即是当前命令的索引值
{
case 0:
ledshanshuo();	 //LED2闪烁和LED1亮
led();

break;
case 1:
flagBuzzOn = 1; //开启蜂鸣器
break;
case 2:
break;

default: //未找到相符命令时，给上机发送“错误命令”的提示
UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
return;
}
buf[len++] = '\r'; //有效命令被执行后，在原命令帧之后添加
buf[len++] = '\n'; //回车换行符后返回给上位机，表示已执行
UartWrite(buf, len);
}

/* 配置并启动 T0，ms-T0 定时时间 */
void ConfigTimer0(unsigned int ms)
{
unsigned long tmp; //临时变量
tmp = 11059200 / 12; //定时器计数频率
tmp = (tmp * ms) / 1000; //计算所需的计数值
tmp = 65536 - tmp; //计算定时器重载值
tmp = tmp + 33; //补偿中断响应延时造成的误差
T0RH = (unsigned char)(tmp>>8); //定时器重载值拆分为高低字节
T0RL = (unsigned char)tmp;
TMOD &= 0xF0; //清零 T0 的控制位
TMOD |= 0x01; //配置 T0 为模式 1
TH0 = T0RH; //加载 T0 重载值
TL0 = T0RL;
ET0 = 1; //使能 T0 中断
TR0 = 1; //启动 T0
}
/* T0 中断服务函数，执行串口接收监控和蜂鸣器驱动 */
void InterruptTimer0() interrupt 1
{
TH0 = T0RH; //重新加载重载值
TL0 = T0RL;
if (flagBuzzOn) //执行蜂鸣器鸣叫或关闭
BUZZ = ~BUZZ;
else
BUZZ = 1;
UartRxMonitor(1); //串口接收监控
}
