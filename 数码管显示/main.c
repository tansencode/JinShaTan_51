#include<reg52.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;

void main()
{
   ENLED=0;	 //EAʹ�ܶˣ�ȡ0��Ч
   ADDR3=1;	  //EAʹ�ܶˣ�ȡ1��Ч

   ADDR2=1;	   //R2��R1��R0�൱��C��B��A�ˣ�000����һ������001���ڶ�����
   ADDR1=0;
   ADDR0=1;

   P0=0xC0;	 //��������ʾ0
   while(1);
}