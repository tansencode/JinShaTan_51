#include <reg52.h>
#include<intrins.h>
#define uchar unsigned char 
#define uint unsigned int
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

void delay(unsigned char t)
{
   do{
       _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	  // _nop_();
	  // _nop_();
	  // _nop_();
	  // _nop_();
	  }while(--t);
}
void main()
{
  // unsigned int i;
   while(1)
   {
   P2=0xFE;
   delay(1);
   //for(i=0;i<10000;i++);
   P2=0xFF;
   delay(1);
   //for(i=0;i<10000;i++);
   }
}
