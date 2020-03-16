#include <reg52.h>
unsigned long beats = 0; //电机转动节拍总数
void StartMotor(unsigned long angle);
void motor()
{
EA = 1; //使能总中断
TMOD = 0x01; //设置 T0 为模式 1
TH0 = 0xF8; //为 T0 赋初值 0xF8CD，定时 2ms
TL0 = 0xCD;
ET0 = 1; //使能 T0 中断
TR0 = 1; //启动 T0
StartMotor(360*2+180); //控制电机转动 2 圈半
while (1);
}
/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(unsigned long angle)
{
//在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
EA = 0;
beats = (angle * 4076) / 360; //实测为 4076 拍转动一圈
EA = 1;
}

void StopMotor()
{
EA = 0;
beats = 0;
EA = 1;
}
/* T0 中断服务函数，用于驱动步进电机旋转 */
void InterruptTimer0() interrupt 1
{
unsigned char tmp; //临时变量
static unsigned char index = 0; //节拍输出索引
unsigned char code BeatCode[8] = { //步进电机节拍对应的 IO 控制代码
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
TH0 = 0xF8; //重新加载初值
TL0 = 0xCD;
if (beats != 0) //节拍数不为 0 则产生一个驱动节拍
{
tmp = P1; //用 tmp 把 P1 口当前值暂存
tmp = tmp & 0xF0; //用&操作清零低 4 位
tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低 4 位
P1 = tmp; //把低 4 位的节拍代码和高 4 位的原值送回 P1
index++; //节拍输出索引递增
index = index & 0x07; //用&操作实现到 8 归零
beats--; //总节拍数-1
}
else //节拍数为 0 则关闭电机所有的相
{
P1 = P1 | 0x0F;
}
}