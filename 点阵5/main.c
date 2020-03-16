#include <reg52.h>
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
s0bit ENLED = P1^4;
unsigned char code image[30][8] = {
{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //动画帧 1
{0xFF,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F}, //动画帧 2
{0xFF,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F}, //动画帧 3
{0xFF,0x1F,0x3F,0x3F,0x3F,0x3F,0x3F,0x1F}, //动画帧 4
{0xFF,0x0F,0x9F,0x9F,0x9F,0x9F,0x9F,0x0F}, //动画帧 5
{0xFF,0x87,0xCF,0xCF,0xCF,0xCF,0xCF,0x87}, //动画帧 6
{0xFF,0xC3,0xE7,0xE7,0xE7,0xE7,0xE7,0xC3}, //动画帧 7
{0xFF,0xE1,0x73,0x73,0x73,0xF3,0xF3,0xE1}, //动画帧 8
{0xFF,0x70,0x39,0x39,0x39,0x79,0xF9,0xF0}, //动画帧 9
{0xFF,0x38,0x1C,0x1C,0x1C,0x3C,0x7C,0xF8}, //动画帧 10
{0xFF,0x9C,0x0E,0x0E,0x0E,0x1E,0x3E,0x7C}, //动画帧 11
{0xFF,0xCE,0x07,0x07,0x07,0x0F,0x1F,0x3E}, //动画帧 12
{0xFF,0x67,0x03,0x03,0x03,0x07,0x0F,0x9F}, //动画帧 13
{0xFF,0x33,0x01,0x01,0x01,0x03,0x87,0xCF}, //动画帧 14
{0xFF,0x99,0x00,0x00,0x00,0x81,0xC3,0xE7}, //动画帧 15
{0xFF,0xCC,0x80,0x80,0x80,0xC0,0xE1,0xF3}, //动画帧 16
{0xFF,0xE6,0xC0,0xC0,0xC0,0xE0,0xF0,0xF9}, //动画帧 17
{0xFF,0x73,0x60,0x60,0x60,0x70,0x78,0xFC}, //动画帧 18
{0xFF,0x39,0x30,0x30,0x30,0x38,0x3C,0x7E}, //动画帧 19
{0xFF,0x9C,0x98,0x98,0x98,0x9C,0x1E,0x3F}, //动画帧 20
{0xFF,0xCE,0xCC,0xCC,0xCC,0xCE,0x0F,0x1F}, //动画帧 21
{0xFF,0x67,0x66,0x66,0x66,0x67,0x07,0x0F}, //动画帧 22
{0xFF,0x33,0x33,0x33,0x33,0x33,0x03,0x87}, //动画帧 23
{0xFF,0x99,0x99,0x99,0x99,0x99,0x81,0xC3}, //动画帧 24
{0xFF,0xCC,0xCC,0xCC,0xCC,0xCC,0xC0,0xE1}, //动画帧 25
{0xFF,0xE6,0xE6,0xE6,0xE6,0xE6,0xE0,0xF0}, //动画帧 26
{0xFF,0xF3,0xF3,0xF3,0xF3,0xF3,0xF0,0xF8}, //动画帧 27
{0xFF,0xF9,0xF9,0xF9,0xF9,0xF9,0xF8,0xFC}, //动画帧 28
{0xFF,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFE}, //动画帧 29
{0xFF,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF} //动画帧 30
};
void main()
{
EA = 1; //使能总中断
ENLED = 0; //使能 U4，选择 LED 点阵
ADDR3 = 0;
TMOD = 0x01; //设置 T0 为模式 1
TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
TL0 = 0x67;
ET0 = 1; //使能 T0 中断
TR0 = 1; //启动 T0
while (1);
}
/* 定时器 0 中断服务函数 */
void InterruptTimer0() interrupt 1
{
static unsigned char i = 0; //动态扫描的索引
static unsigned char tmr = 0; //250ms 软件定时器
static unsigned char index = 0; //图片刷新索引
TH0 = 0xFC; //重新加载初值
TL0 = 0x67;
//以下代码完成 LED 点阵动态扫描刷新
P0 = 0xFF; //显示消隐
switch (i)
{
case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image[index][0]; break;
case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image[index][1]; break;
case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image[index][2]; break;
case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image[index][3]; break;
case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image[index][4]; break;
case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image[index][5]; break;
case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image[index][6]; break;
case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image[index][7]; break;
default: break;
}
//以下代码完成每 250ms 改变一帧图像
tmr++;
if (tmr >= 250) //达到 250ms 时改变一次图片索引
{
tmr = 0;
index++;
if (index >= 30) //图片索引达到 30 后归零
{
index = 0;
}
}
}