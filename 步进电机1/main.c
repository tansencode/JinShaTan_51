#include <reg52.h>
void TurnMotor(unsigned long angle);
void main()
{
TurnMotor(360*25); //360 度*25，即 25 圈
while (1);
}
/* 软件延时函数，延时约 2ms */
void delay()
{
unsigned int i = 200;
while (i--);
}
/* 步进电机转动函数，angle-需转过的角度 */
void TurnMotor(unsigned long angle)
{
unsigned char tmp; //临时变量
unsigned char index = 0; //节拍输出索引
unsigned long beats = 0; //所需节拍总数
unsigned char code BeatCode[8] = { //步进电机节拍对应的 IO 控制代码
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
beats = (angle*4096) / 360; //计算需要的节拍总数，4096 拍对应一圈
while (beats--) //判断 beats 不为 0 时执行循环，然后自减 1
{
tmp = P1; //用 tmp 把 P1 口当前值暂存
tmp = tmp & 0xF0; //用&操作清零低 4 位
tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低 4 位
P1 = tmp; //把低 4 位的节拍代码和高 4 位的原值送回 P1
index++; //节拍输出索引递增
index = index & 0x07; //用&操作实现到 8 归零
delay(); //延时 2ms，即 2ms 执行一拍
}
P1 = P1 | 0x0F; //关闭电机所有的相
}