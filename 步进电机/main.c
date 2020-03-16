#include <reg52.h>

unsigned char code BeatCode[8] = { //步进电机节拍对应的 IO 控制代码
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};

void delay();

void main()
{
unsigned char tmp; //定义一个临时变量
unsigned char index = 0; //定义节拍输出索引
while (1)
{
tmp = P1; //用 tmp 把 P1 口当前值暂存
tmp = tmp & 0xF0; //用&操作清零低 4 位
tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低 4 位
P1 = tmp; //把低 4 位的节拍代码和高 4 位的原值送回 P1
index++; //节拍输出索引递增
index = index & 0x07; //用&操作实现到 8 归零
delay(); //延时 2ms，即 2ms 执行一拍
}
}

/* 软件延时函数，延时约 2ms */
void delay()
{
unsigned int i = 200;
{
while(i--);
}
}