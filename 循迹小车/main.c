/*****************ZY08-C智能循迹、遥控、避障、寻光机器人*****************
//=====================================================================//
                   ZY08-C循迹、红外避障、遥控综合程序演示                         //
//  本程序主要将循迹程序、红外避障程序、遥控器程序综合在一起，通过主板按键切换
，蓝色小主板是右边按键S4切换，如果客户买的是绿色大主板 ，那么就是主板左边的按键S1
切换，数码管显示1，就是循迹模式，按一下按键数码管显示2，就是避障模式，按一下按键
数码管显示3，就是红外遥控程序，周而复始切换模式。
//---------------------------------------------------------------------//

*  平台：ZY08-C智能循迹、遥控、避障、寻光机器人 + Keil U4 + STC89C52
*  程序名称：5、ZY08-C循迹、红外避障、遥控综合程序
*  公司：湖南智宇科教设备有限公司
*  编写：智宇公司研发一部
*  交流:智能车QQ:1018973181 
*  晶振:11.0592MHZ
*  售后技术说明：免费开源，不支持客户自己修改程序分析与改正，可以提供我公司程序说明
*  视频教程：本小车配套学习C语言详细视频教程，资料统一网盘下载
******************************************************************/  

#include <reg52.h> //包含51单片机相关的头文件
#include <intrins.h>
sbit LeftLed=P2^0;                        //定义前方左侧指示灯端口
sbit RightLed=P0^7;                       //定义前方右侧指示灯端口
sbit FontLled=P1^7;

sbit LeftIR=P3^5;                         //定义前方左侧红外探头端口
sbit RightIR=P3^6;                        //定义前方右侧红外探头端口
sbit FontIR=P3^7;                         //定义前方正前方红外探头端口

sbit M1A=P0^0;                            //定义左侧电机驱动A端
sbit M1B=P0^1;                            //定义左侧电机驱动B端

sbit M2A=P0^2;                            //定义右侧电机驱动A端
sbit M2B=P0^3;                            //定义右侧电机驱动B端

sbit B1=P0^4;                             //定义语音识识别传感器端口
sbit SB1=P0^6;                            //定义蜂鸣器端口
sbit IRIN=P3^3;                                       //定义红外接收端口

unsigned char code  LedShowData[]={0x03,0x9F,0x25,0x0D,0x99,  //定义数码管显示数据
                                   0x49,0x41,0x1F,0x01,0x19};//0,1,2,3,4,5,6,7,8,9
unsigned char code  RecvData[]={0x19,0x46,0x15,0x43,0x44,0x40,0x0D,0x0E,0x00,0x0F};
unsigned char IRCOM[7];
static unsigned int LedFlash;                         //定义闪动频率计数变量
bit EnableLight=0;                                    //定义指示灯使能位
#define ShowPort P2                                   //定义数码管显示端口
unsigned char temp = 1;
void tingzhi()
{
   M1A=0;                                   //将M1电机A端初始化为0
   M1B=0;                                   //将M1电机B端初始化为0
   M2A=0;                                   //将M2电机A端初始化为0
   M2B=0;

}

void qianjin()
{
   M1A=1;                                   
   M1B=0;                                   
   M2A=1;                                   
   M2B=0;
}

void houtui()
{
   M1A=0;                                   
   M1B=1;                                   
   M2A=0;                                   
   M2B=1;
}

void zuozhuan()
{
   M1A=0;                                   
   M1B=1;                                   
   M2A=1;                                   
   M2B=0;
}

void youzhuan()
{
   M1A=1;                                   
   M1B=0;                                   
   M2A=0;                                   
   M2B=1;
}

void Delay1ms(unsigned int i) 
{ 
unsigned char j,k; 
do{ 
  j = 10; 
  do{ 
   k = 50; 
   do{ 
    _nop_(); 
   }while(--k);     
  }while(--j); 
}while(--i); 

} 

void delay_nus(unsigned int i)  //延时:i>=12 ,i的最小延时单12 us
{ 
  i=i/10;
  while(--i);
}   
void delay_nms(unsigned int n)  //延时n ms
{ 
  n=n+1;
  while(--n)  
  delay_nus(900);         //延时 1ms,同时进行补偿
  
}  

void delayms(unsigned char x)                         //0.14mS延时程序
{
  unsigned char i;                                    //定义临时变量
  while(x--)                                          //延时时间循环
  {
    for (i = 0; i<13; i++) {}                         //14mS延时
  }
}

void Delay()                                          //定义延时子程序
{ 
  unsigned int DelayTime=30000;                       //定义延时时间变量
  while(DelayTime--);                                 //开始进行延时循环
  return;                                             //子程序返回
}
void ControlCar(unsigned char ConType)    //定义电机控制子程序
{
 
  tingzhi();
 switch(ConType)                          //判断用户设定电机形式
 {
  case 1:  //前进                         //判断用户是否选择形式1
  { 
    //tingzhi();                                                      //进入前进之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
        //Delay1ms(50);
        LeftLed = 0 ;
        qianjin();
    break;
  }
  case 2: //后退                          //判断用户是否选择形式2
  { 
   // tingzhi();                                                              //进入后退之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
   // Delay1ms(50);
    LeftLed = 1 ;          
        houtui();                                //M2电机反转
    break;
  }
  case 3: //左转                          //判断用户是否选择形式3
  { 
     //tingzhi();                                                                  //进入左转之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
        // Delay1ms(50); 
         zuozhuan();                               //M2电机正转
         break;
  }
  case 4: //右转                          //判断用户是否选择形式4
  { 
   // tingzhi();                                                                  //进入右转之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
//        Delay1ms(50);
        youzhuan();                                //M1电机正转
                                          //M2电机反转
        break;
  }
  case 8: //停止                          //判断用户是否选择形式8
  {
    tingzhi();
        break;                                //退出当前选择
  }
 }
}

void ControlCar_yaokong(unsigned char ConType)    //定义电机控制子程序        (红外遥控单独设置一个 switch  case  语句  )
{
 
  tingzhi();
 switch(ConType)                          //判断用户设定电机形式
 {
  case 1:  //前进                         //判断用户是否选择形式1
  { 
    tingzhi();                                                      //进入前进之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
        Delay1ms(150);
        LeftLed = 0 ;
        qianjin();
    break;
  }
  case 2: //后退                          //判断用户是否选择形式2
  { 
    tingzhi();                                                              //进入后退之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
    Delay1ms(150);
    LeftLed = 1 ;          
        houtui();                                //M2电机反转
    break;
  }
  case 3: //左转                          //判断用户是否选择形式3
  { 
     tingzhi();                                                                  //进入左转之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
         Delay1ms(150); 
         zuozhuan();                               //M2电机正转
         break;
  }
  case 4: //右转                          //判断用户是否选择形式4
  { 
     tingzhi();                                                                  //进入右转之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
         Delay1ms(150);
     youzhuan();                                //M1电机正转  //M2电机反转
          break;
  }
  case 8: //停止                          //判断用户是否选择形式8
  {
    tingzhi();
        break;                                //退出当前选择
  }
 }
}
void Robot_Avoidance()                   //机器人避障子程序
{

   LeftLed=LeftIR;                        //前方左侧指示灯指示出前方左侧红外探头状态
   RightLed=RightIR;                      //前方右侧指示灯指示出前方右侧红外探头状态
   FontLled= FontIR; 
   SB1=FontIR;

    if(FontIR == 0)                          //如果前面避障传感器检测到障碍物
   {
      ControlCar(8);                       //停止
      delay_nms (300);                                           //停止300MS   防止电机反相电压冲击  导致系统复位
          ControlCar(2);                                           //后退  
          delay_nms (1000);                                           //后退1500MS  
          ControlCar(3);                                           //
          delay_nms (1800);        
  }
   else
   {
      ControlCar(1);                      //右侧没有信号时,开始向左转一定的角度
      delay_nms (10);
   }
    ControlCar(1);
}
//机器人循迹子程序
void Robot_Traction()                     //机器人循迹子程序
{
   LeftLed=LeftIR;                        //前方左侧指示灯指示出前方左侧红外探头状态
   RightLed=RightIR;                      //前方右侧指示灯指示出前方右侧红外探头状态
   FontLled= FontIR; 
   SB1=LeftIR;
   if(LeftIR == 0 && RightIR == 0)    //三个红外检测到黑线，就前进
   {
      ControlCar(1);                      //左侧没有信号时，开始向右转一定的角度
      delay_nms (10);
   }
   
   else if(LeftIR == 0 && RightIR == 1)
   {
      ControlCar(3);                      //右侧没有信号时,开始向左转一定的角度
      delay_nms (10);
   }

   else if(LeftIR == 1 && RightIR == 0)
   {
      ControlCar(4);                      //右侧没有信号时,开始向左转一定的角度
      delay_nms (10);
   }
    ControlCar(8);
}

void main()                               //主程序入口
{
  
  bit RunFlag=0;                          //定义小车运行标志位
  LedFlash=3000;                                       //对闪灯数据进行初始化
  //EX1=1;                                               //同意开启外部中断1
  IT1=1;                                               //设定外部中断1为低边缘触发类型
  EA=1;                                                //总中断开启
  ControlCar(8);                          //初始化小车运行状态
  while(1)                                //程序主循环
  {
             if(P3^2 == 0)
          {
           delay_nms(10);
           if(P3^2 == 0)
           {
                     temp++;
                  while(!P3^2);
           }
          }
          if(temp > 3)
          {
          temp = 1;
          }
          switch(temp)
          {
                   case 1:        ShowPort = LedShowData[1];Robot_Traction();EX1 = 0;break;
                case 2: ShowPort = LedShowData[2];Robot_Avoidance();EX1 = 0;break;
                case 3: ShowPort = LedShowData[3];EX1 = 1;break;
          }
  }
}
//----------红外遥控-------------------------------------------------------------
void IR_IN() interrupt 2 using 0                      //定义INT2外部中断函数
{
  unsigned char j,k,N=0;                              //定义临时接收变量
   
  EX1 = 0;                                            //关闭外部中断,防止再有信号到达   
  delayms(15);                                        //延时时间，进行红外消抖
  if (IRIN==1)                                        //判断红外信号是否消失
  {  
     EX1 =1;                                          //外部中断开
         return;                                          //返回
  } 
                           
  while (!IRIN)                                       //等IR变为高电平，跳过9ms的前导低电平信号。
  {
      delayms(1);                                     //延时等待
  }

  for (j=0;j<4;j++)                                   //采集红外遥控器数据
  { 
    for (k=0;k<8;k++)                                 //分次采集8位数据
    {
       while (IRIN)                                   //等 IR 变为低电平，跳过4.5ms的前导高电平信号。
       {
         delayms(1);                                  //延时等待
       }
       
       while (!IRIN)                                  //等 IR 变为高电平
       {
         delayms(1);                                  //延时等待
       }
   
       while (IRIN)                                   //计算IR高电平时长
       {
         delayms(1);                                  //延时等待
         N++;                                         //计数器加加
         if (N>=30)                                   //判断计数器累加值
             { 
           EX1=1;                                     //打开外部中断功能
               return;                                    //返回
         }                   
       }
                                       
      IRCOM[j]=IRCOM[j] >> 1;                         //进行数据位移操作并自动补零
     
      if (N>=8)                                       //判断数据长度 
      {
         IRCOM[j] = IRCOM[j] | 0x80;                  //数据最高位补1
      } 
      N=0;                                            //清零位数计录器
    }
  }
   
  if (IRCOM[2]!=~IRCOM[3])                            //判断地址码是否相同
  { 
     EX1=1;                                           //打开外部中断
     return;                                          //返回
  }

  for(j=0;j<10;j++)                                   //循环进行键码解析
   {
      if(IRCOM[2]==RecvData[j])                       //进行键位对应
      {
       // ControlCar(j);
                ControlCar_yaokong(j);                               //数码管显示相应数码
      }
   } 
   EX1 = 1;                                           //外部中断开 
} 
//------------------------------------------------------------------------------------------------------- 