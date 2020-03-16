#include<reg51.h>
#include<intrins.h>

    /***********宏定义*******************/
    #define uint unsigned int   
    #define uchar unsigned char 
    #define long_time0 40    //定时器0定时时间，每次50ms

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;
    /************定义全局变量************/
    uint n=0;                //循环移位变量

    /************子函数声明**************/
    void init_time0(void);   //定时器0初始化子函数

    /*************主函数*****************/
    void main(void)
    {
	    	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   	//P0=0xFF;
        init_time0();

    }

    /*********定时器0初始化函数**********/
    void init_time0(void)
    {
            EA=1;                   //开总中断
            ET0=1;                  //开定时器0中断
            TMOD=0X01;              //选择方式1
            TH0=(65536-50000)/256;  //每次定时50ms
            TL0=(65526-50000)%256;
            TR0=1;                  //开启定时器
            while(1);               //等待定时器0中断
    }
    /************定时器中断函数**********/
    void time0(void) interrupt 1 using 0
    {
        static uint i=0;
        i++;
        if(long_time0==i)
        {
            P0=~(0x01<<n);          //P1=~(0xfe<<n);
            if(++n==8)
			
			n=0;
            i=0;                    //定时时间到清零
			
        }
        TH0=(65536-50000)/256;      //重新装入初值
        TL0=(65526-50000)%256;
    }