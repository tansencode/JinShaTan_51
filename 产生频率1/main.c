#include <reg52.h>
#include<intrins.h>
#define uchar unsigned char 
#define uint unsigned int
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit led=P2^0;
unsigned char T0RH=0;
unsigned char T0RL=0;
void delay()
{
   
       _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	  // _nop_();
	   //_nop_();
	   //_nop_();
	   //_nop_();
}

void delay1(unsigned char t)
{
   do{
       _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	  }while(--t);
}

void delay2()
{
       _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   //_nop_();
	   
}
void delay3()
{
       delay2();
	   delay2();
	   delay2();
	   delay2();
	   delay2();
	   delay2();
	   delay2();
	   delay2();
	   delay2();
	   //delay2();	   
}

void delay4()
{
   	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
	delay3();
}

void delay5()
{
   	delay4();
	delay4();
	delay4();
	delay4();
	delay4();
	delay4();
	delay4();
    delay4();
}

void delay6()
{
   delay5();
   delay5();
   delay5();
   delay5();
   delay5();
   delay5();
   delay5();
   delay5();
   delay5();
   delay5();
}

void delay7()
{
   delay6();
   delay6();
   delay6();
   delay6();
   delay6();
   delay6();
   delay6();
   delay6();
   delay6();
}
/*void main()			//1
{
  //EA=1;
  //ConfigTimer0(50);
   //led=1;
  while(1)
   {
   led=1;
   delay7();
   led=0;
    //delay(1);
   } 
} */
void main()			//1 0
{
  //EA=1;
  //ConfigTimer0(50);
   //led=1;
  while(1)
   {
   led=1;
   delay7();
   led=0;
   } 
}
/*void main()			//1 00
{
  //EA=1;
  //ConfigTimer0(50);
   //led=1;
  while(1)
   {
   led=1;
   delay5();
   led=0;
    //delay(1);
   } 
}  */

/*void main()			//1 000
{
  led=1;
  while(1)
   {
   led=1;
  	delay1(100);
   	//delay4();
    led=0;
   } 
}  */
/*void main()			//1 0000
{
  // unsigned int i;
  //EA=1;
  //ConfigTimer0(50);
  //led=1;
  while(1)
   {
   led=1;
  	 delay1(10);
  // delay2();
   //_nop_();
   
    led=0;
    //delay(1);
   } 
}  */
	
/*void main()			//10 0000
{
  // unsigned int i;
  //EA=1;
  //ConfigTimer0(50);
  //led=1;
  while(1)
   {
   led=1;
    _nop_();
	_nop_();
	_nop_();
    _nop_();
	_nop_();
	_nop_();
	_nop_();
    led=0;
   } 
}	   */


/*void main()	   //20 0000
{
  // unsigned int i;
  //EA=1;
  //ConfigTimer0(50);
  //led=1;
  while(1)
   {
   led=1;
   _nop_();
    led=0;
    //delay(1);
   } 
}*/	

/*void time() interrupt 1
{
   static int i=0;
   TH0=T0RH;
   TL0=T0RL;
   //i++;
  // if(i>=100)
  // {
     //  i=0;
	   led=~led;
    //}
}  */

      