#include <reg52.h>  
  
sbit  P3_4 = P3^4;  
  
static bit OP;        //红外发射管的亮灭  
  
static unsigned int ncount;       //延时计数器  
  
static unsigned int endncount; //终止延时计数  
  
static unsigned char flag;      //红外发送标志  
  
char iraddr1;  //十六位地址的第一个字节  
  
char iraddr2;  //十六位地址的第二个字节  
  
void SendIRdata(char p_irdata);  
  
void delay();  
  
void main(void)  
  
{  
  
    ncount = 0;  
  
    flag = 0;  
  
    OP = 0;  
  
    P3_4 = 0;  
  
    EA = 1; //允许CPU中断  
  
    TMOD = 0x11; //设定时器0和1为16位模式1  
  
    ET0 = 1; //定时器0中断允许  
  
    TH0 = 0xFF;  
  
    TL0 = 0xE8; //设定时值0为38K 也就是每隔26us中断一次   
  
    TR0 = 1;//开始计数  
  
    iraddr1=3;  
  
    iraddr2=252;  
  
    do{  
  
        //unsigned char i;  
  
        delay();  
  
        SendIRdata(12);  
  
    }while(1);  
  
}  
  
//定时器0中断处理  
  
void timeint(void) interrupt 1  
  
{  
  
    TH0=0xFF;  
  
    TL0=0xE8; //设定时值为38K 也就是每隔26us中断一次  
  
    ncount++;  
  
    if (flag==1)  
  
    {  
  
        OP=~OP;  
  
    }  
  
    else  
  
    {  
  
        OP = 0;  
  
    }  
  
    P3_4 = OP;  
  
}  
  
void SendIRdata(char p_irdata)  
  
{  
  
    int i;  
  
    char irdata=p_irdata;  
  
    //发送9ms的起始码  
  
    endncount=223;  
  
    flag=1;  
  
    ncount=0;  
  
    do{}while(ncount<endncount);  
  
    //发送4.5ms的结果码  
  
    endncount=117;  
  
    flag=0;  
  
    ncount=0;  
  
    do{}while(ncount<endncount);  
  
    //发送十六位地址的前八位  
  
    irdata=iraddr1;  
  
    for(i=0;i<8;i++)  
  
    {  
  
        //先发送0.56ms的38KHZ红外波（即编码中0.56ms的低电平）  
  
        endncount=10;  
  
        flag=1;  
  
        ncount=0;  
  
        do{}while(ncount<endncount);  
  
        //停止发送红外信号（即编码中的高电平）  
  
        if(irdata-(irdata/2)*2)  //判断二进制数个位为1还是0  
  
        {  
  
            endncount=41;  //1为宽的高电平  
  
        }  
        else  
        {  
            endncount=15;   //0为窄的高电平  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    //发送十六位地址的后八位  
    irdata=iraddr2;  
    for(i=0;i<8;i++)  
    {  
        endncount=10;  
        flag=1;  
        ncount=0;  
        do{}while(ncount<endncount);  
        if(irdata-(irdata/2)*2)  
        {  
            endncount=41;  
        }  
        else  
        {  
            endncount=15;  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    //发送八位数据  
    irdata=p_irdata;  
    for(i=0;i<8;i++)  
    {  
        endncount=10;  
        flag=1;  
        ncount=0;  
        do{}while(ncount<endncount);  
        if(irdata-(irdata/2)*2)  
        {  
            endncount=41;  
        }  
        else  
        {  
            endncount=15;  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    //发送八位数据的反码  
    irdata=~p_irdata;  
    for(i=0;i<8;i++)  
    {  
        endncount=10;  
        flag=1;  
        ncount=0;  
        do{}while(ncount<endncount);  
        if(irdata-(irdata/2)*2)  
        {  
            endncount=41;  
        }  
        else  
        {  
            endncount=15;  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    endncount=10;  
    flag=1;  
    ncount=0;  
    do{}while(ncount<endncount);  
    flag=0;  
}  
  
void delay()  
  
{  
  
    int i,j;  
  
    for(i = 0; i < 400; i++){  
  
        for(j = 0; j < 200; j++){  
  
        }  
  
    }  
  
}  