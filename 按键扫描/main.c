#include <reg52.h>
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit LED9 = P0^7;
sbit LED8 = P0^6;
sbit LED7 = P0^5;
sbit LED6 = P0^4;
sbit KEY1 = P2^4;
sbit KEY2 = P2^5;
sbit KEY3 = P2^6;
sbit KEY4 = P2^7;
void main()
{
ENLED = 0; //选择独立 LED 进行显示
ADDR3 = 1;
ADDR2 = 1;
ADDR1 = 1;
ADDR0 = 0;
P2 = 0xF7; //P2.3 置 0，即 KeyOut1 输出低电平
while (1)
{
//将按键扫描引脚的值传递到 LED 上
LED9 = KEY1; //按下时为 0，对应的 LED 点亮
LED8 = KEY2;
LED7 = KEY3;
LED6 = KEY4;
}
}