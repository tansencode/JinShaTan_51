#include<reg52.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;

void main()
{
   ENLED=0;	 //EA使能端，取0有效
   ADDR3=1;	  //EA使能端，取1有效

   ADDR2=1;	   //R2、R1、R0相当于C、B、A端，000，第一个亮，001，第二个亮
   ADDR1=0;
   ADDR0=1;

   P0=0xC0;	 //共阳，显示0
   while(1);
}