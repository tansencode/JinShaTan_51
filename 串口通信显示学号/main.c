#include <reg52.h>

sbit MC0 = P3^0;
sbit MC1 = P3^1;
sbit MC2 = P3^2;
sbit MC3 = P3^3;

signed long beats = 0; //电机转动节拍总数

flagmotor=0;

unsigned char T0RH = 0; //T0 重载值的高字节
unsigned char T0RL = 0; //T0 重载值的低字节

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdWriteCmd(unsigned char cmd);
extern void StartMotor(unsigned long angle);


void StartMotor(signed long angle);
void StopMotor();
void main()
{
EA = 1; //开总中断
TMOD = 0x00; //设置 T0 为模式 0
TH0 = T0RH; //加载 T0 重载值
TL0 = T0RL;
ConfigTimer0(2); //配置 T0 定时 2ms
ConfigUART(9600); //配置波特率为 9600
InitLcd1602(); //初始化液晶
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

/* 串口动作函数，根据接收到的命令帧执行响应的动作
buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
unsigned char i;
unsigned char code cmd0[] = "motorrun"; //电机转动命令
unsigned char code cmd1[] = "motorstop"; //电机停止命令
unsigned char code cmd2[] = "number"; //显示学号
unsigned char code cmd3[] = "zhuanye"; //显示专业
unsigned char code cmd4[] = "time"; //显示时间

unsigned char code cmdLen[] = { //命令长度汇总表
sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,sizeof(cmd3)-1,sizeof(cmd4)-1,
};
unsigned char code *cmdPtr[] = { //命令指针汇总表
&cmd0[0], &cmd1[0], &cmd2[0],&cmd3[0],&cmd4[0],
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
LcdWriteCmd(0x01); //清屏
StartMotor(60); //开启步进电机
break;

case 1:
StopMotor();//关闭步进电机
break;

case 2:
LcdShowStr(0,0,"0903160218"); //显示学号, 0,0第一行显示
break;

case 3:
LcdWriteCmd(0x01); //清屏
LcdShowStr(0, 1,"DianZiXinXi"); //显示专业， 0,1第二行显示
break;

case 4:
LcdWriteCmd(0x01); //清屏
buf[len] = '\0'; //为接收到的字符串添加结束符
LcdShowStr(0, 1, buf+cmdLen[4]); //显示命令后的字符串， 0,1第二行显示
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

/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(signed long angle)
{
//在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
EA = 0;
beats = (angle * 4076)/360 ; //实测为 4076 拍转动一圈
EA = 1;
}

/* 步进电机停止函数 */
void StopMotor()
{
EA = 0;
beats = 0;
EA = 1;
}

void TurnMotor()
{
unsigned char tmp; //临时变量
static unsigned char index = 0; //节拍输出索引
unsigned char code BeatCode[8] = { //步进电机节拍对应的 IO 控制代码
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
if (beats != 0) //节拍数不为 0 则产生一个驱动节拍
{
if (beats > 0) //节拍数大于 0 时正转
{
index++; //正转时节拍输出索引递增
index = index & 0x07; //用&操作实现到 8 归零
beats--; //正转时节拍计数递减
}
else //节拍数小于 0 时反转
{
index--; //反转时节拍输出索引递减
index = index & 0x07; //用&操作同样可以实现到-1 时归 7
beats++; //反转时节拍计数递增
}
tmp = P3; //用 tmp 把 P1 口当前值暂存
tmp = tmp & 0xF0; //用&操作清零低 4 位
tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低 4 位
P3= tmp; //把低 4 位的节拍代码和高 4 位的原值送回 P1
}
else //节拍数为 0 则关闭电机所有的相
{
P3 = P3 | 0x0F;
}
}
/* T0 中断服务函数，执行串口接收监控和蜂鸣器驱动 */
void InterruptTimer0() interrupt 1
{
static bit div = 0;
TH0 = T0RH; //加载 T0 重载值
TL0 = T0RL;
div = ~div;
TurnMotor();
UartRxMonitor(1);
}
