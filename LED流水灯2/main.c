#include<reg52.h>
#include<intrins.h>
sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;

void main()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;
  ADDR2=1;
  ADDR1=1;
  ADDR0=0; 
  P0=~0x01;		   //需在外部定义，移动函数只负责移动，不能直接点亮第一个小灯
  for(i=0;i<40000;i++);			  //下一步，移位
  while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
	   
        P0=_crol_(P0,1);		// //循环左移，i=0开始移动，所以i<7
		for(i=0;i<40000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {	 
	 
        P0=_cror_(P0,1);	//循环右移，i=0开始移动，所以i<7
		for(i=0;i<40000;i++);	  //延时函数，不断执行空循环
	  }
  }
}