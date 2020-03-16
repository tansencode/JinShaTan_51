#include<reg52.h>
/*********************第一部分*********************/ 
//以下是电机驱动芯片L298N 管脚位声明 
sbit PWM1=P1^0;              //电机A/B使能端 
sbit PWM2=P1^1; 
sbit IN1=P1^2;                 //电机控制端 
sbit IN2=P1^3;
sbit IN3=P1^4; 
sbit IN4=P1^5; 
sbit RP1=P2^0;                //传感器管脚位声明
sbit RP2=P2^1; 
sbit RP3=P2^2; 
sbit RP4=P2^3;
int count1,count2;              //用于中断计数
//延时函数，延时z秒
void delay(int z) 
{ 
int x,y; 
for(x=z;x>0;x--); 
for(y=124;y>0;y--); 
} 
/*********************第二部分 电机控制子函数*********************/
void forward1()                //电机1前进 
{ 
IN1=0; 
IN2=1; 
} 
void forward2()                //电机2前进 
{ 
IN3=0; 
IN4=1; 
}
void back1()                  //电机1后退
{ 
IN1=1; 
IN2=0; 
} 
void back2()                  //电机2后退
{ 
IN3=1; 
IN4=0; 
} 
/*********************电机的速度控制 用PWM调速*********************/
void speed(int cnt1,int sd1,int cnt2,int sd2) 
{ 
if(cnt1<sd1)                  //电机1 PWM调速 
PWM1=1; 
else 
PWM1=1; 
if(cnt2<sd2)                  //电机2 PWM调速
PWM2=1;  
else 
PWM2=1; 
}
/**************电机的前进、左转、右转控制***************/
void turn(int cnt1,int sd1,int cnt2,int sd2)
{ 
forward1(); 
forward2(); 
speed(cnt1,sd1,cnt2,sd2); 
} 
/*********************电机的后退控制*********************/
void back(int cnt1,int sd1,int cnt2,int sd2) 
{ 
back1(); 
back2(); 
speed(cnt1,sd1,cnt2,sd2); 
} 
/*********************第三部分 主函数*********************/
void main() 
{ 
int num=0;
TMOD=0x01;                    //中断模式设置 
EA=1; 
TH0=(65536-1000)/256;            //定时 1ms 
TL0=(65536-1000)%256;  
ET0=1; 
TR0=1; 
turn(count1,250,count2,250);         //使小车冲上黑色轨道 
delay(350);                       //延时等待小车冲上黑色轨道
while(1) 
{ 
if(RP1==1&&RP2==1&&RP3==1&&RP4==1) 
back(count1,50,count2,50);           //1111 当四个LED都没检测到黑线时，小车后退 
if(RP1==1&&RP2==0&&RP3==0&&RP4==1) 
turn(count1,200,count2,200);         //1001 当第二、第三个LED检测到黑线时，小车前进
if(RP1==0&&RP2==1&&RP3==1&&RP4==0)
turn(count1,150,count2,150);         // 0110 当第一、第四个LED检测到黑线时，小车减速前进 
/*********************小车左转*********************/
if(RP1==0&&RP2==1&&RP3==1&&RP4==1) 
turn(count1,0,count2,150);          // 0111 当只有第一个LED检测到黑线时，小车左转
if(RP1==1&&RP2==0&&RP3==1&&RP4==1)
turn(count1,80,count2,150);         // 1011 当第二个LED检测到黑线时，小车偏左转 
if(RP1==0&&RP2==0&&RP3==1&&RP4==1) 
turn(count1,0,count2,100);          // 0011 当第一、二个LED检测到黑线时，小车左转
if(RP1==0&&RP2==0&&RP3==0&&RP4==1) 
turn(count1,0,count2,200);          // 0001 当第一、二、三个LED检测到黑线时，小车左大转 
/*********************小车右转*********************/
if(RP1==1&&RP2==1&&RP3==0&&RP4==1)
turn(count1,150,count2,80);          // 1101 当第三个LED检测到黑线时，小车偏右转
if(RP1==1&&RP2==1&&RP3==1&&RP4==0) 
turn(count1,180,count2,0);          // 1110 当第四个LED检测到黑线时，小车右转 
if(RP1==1&&RP2==1&&RP3==0&&RP4==0) 
turn(count1,100,count2,0);          // 1100 当第三、四个LED检测到黑线时，小车右转 
if(RP1==1&&RP2==0&&RP3==0&&RP4==0)
turn(count1,200,count2,0);          // 1000当第二、三、四个LED检测到黑线时，小车右大转 
/****************小车检测到全黑情况的处理****************/
if(RP1==0&&RP2==0&&RP3==0&&RP4==0) 
{ 
if(num==0)
{ 
back1();                        // 0000 当四个LED第一次检测到黑线时，左轮后退右轮前进，小车左转 
forward2(); 
delay(100);                      //延时100ms，等待小车转过弯道
} 
if(num>0 && num<6) 
{ 
turn(count1,200,count2,200);     // 0000 当四个LED再次检测到黑线的次数小于6次时                             //即到达黑线时小车前进速度大于后退速度 
} 
if(num>=6)
{ 
PWM1=0; 
PWM2=0;                      //当四个LED第六次检测到黑线时，小车停止
} 
num++; 
} 
} 
}
/*************第四部分 中断服务函数*************/ 
void time0()interrupt 1 
{ 
TH0=(65536-1000)/256;            //定时1ms 
TL0=(65536-1000)%256; 
count1++; 
count2++; 
if(count1>=500)                  //周期是500ms 
count1=0; 
if(count2>=500)                  //周期是500ms 
count2=0; 
}