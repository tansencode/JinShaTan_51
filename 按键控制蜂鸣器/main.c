#include<reg52.h> 
#define uint unsigned int //宏定义
sbit SPK=P1^6;            //定义喇叭端口
sbit key=P2^4;            //开发板上对应的是s18按键

void delay(uint z)
{
uint x,y;
for(x=z;x>0;x--)
for(y=110;y>0;y--);
}

void main()
{
    while(1)
    { 
if(key==0)
{
       delay(100); 
   SPK=0;
delay(100);
       SPK=1;
}
else
{
SPK=1;
}
    }
}
