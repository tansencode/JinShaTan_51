#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
void main()
{
ENLED = 0; //U3、U4 两片 74HC138 总使能
ADDR3 = 0; //使能 U4 使之正常输出

ADDR2 = 0; //经 U4 的 Y1 输出开启三极管 Q11
ADDR1 = 0;
ADDR0 = 0;

P0 = 0x00; //向 P0 写入 0 来点亮一行

while(1); //程序停止在这里
}