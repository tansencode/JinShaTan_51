#include <reg52.h>
#include <intrins.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;

extern bit irflag;
extern unsigned char ircode[16];
extern void InitInfrared();
sbit BUZZ = P1^6; //蜂鸣器控制引脚
bit flagBuzzOn = 0; //蜂鸣器启动标志
bit flag300ms = 1;       //300ms定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char T1RH = 0;    //T1重载值的高字节
unsigned char T1RL = 0;    //T1重载值的低字节
bit flag1s = 1;       //300ms定时标志
bit flagUart=0;
bit d=0;
static unsigned int cnt=0;
unsigned int t=0;
static unsigned int m=1;
static unsigned char c=0;

static unsigned char i = 0;  //动态扫描的索引
    static unsigned int  tmr = 0;  //1s软件定时器
    static unsigned char index = 9;  //图片刷新索引
unsigned char RxdByte = 0;  //串口接收到的字节

void ConfigTimer0(unsigned int ms);
void ConfigTimer1(unsigned int ms);
unsigned char GetADCValue(unsigned char chn);
void ValueToString(unsigned char *str, unsigned long val);

void Infra();
extern void KeyScan();
extern void KeyDriver();

extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);

extern void Led1();
extern void Led2();
extern void Led3();
extern void Led4();
extern void Led5();
extern void Led6();
extern void Led7();
extern void Led8();
extern void Led9();
extern void Led0();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
void UsartConfiguration();
unsigned char code LedChar[] = { //数码管显示字符转换表
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = { //数码管+独立 LED 显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


unsigned char code L1[7]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB};
    //其实就是定义了一个亮灯状态表,由中间到两边，再由两边倒中间,然后P0循环此数组
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111
unsigned char code L2[30]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB,
                         0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,
						 0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xEF}; //22
unsigned char code L3[50]={0xEF,0xDF,0xBF,0x7F, 0xF7,0xFB,0xFD,0xFE,
                         0xEF,0xDF,0xBF,0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,
                         0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xBF,0xDF};  //28	
unsigned char ledd3[20]={0xFC,0xF9,0xE7,0xCF,0x9F,0x3F,0x7F,0x3F,0x9F,0xCF,0xE7,0xF9,0xFC,0xFE}; //14 
unsigned char code ledd4[25]={0x7F,0x3F,0x1F,0x0F,0x87,0xC3,0xE1,
                         0xF0,0xF8,0xFC,0xFE,0xFC,0xF8,0xF0,0xE1,0xC3,
						 0x87,0x0F,0x1F,0x3F,0x7F}; //   21  	
unsigned char code ledd5[20]={0xFA,0xFD,0xF5,0xF4,0xEB,0xF7,0xD7,0xEF,0xAF,0xDF,0x5F,0xBF}; 
//unsigned char val;

static char str[10]={0,0,0,0,0,0,0,0};
void main()
{
    unsigned char val;
    EA = 1;            //开总中断
	ENLED=0;
	ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
	InitInfrared();   //初始化红外功能
    ConfigTimer0(1);  //配置T0定时10ms
	ConfigTimer1(1);
	ConfigUART(9600);   //配置波特率为9600
    InitLcd1602();     //初始化液晶
	//led1();  
    while (1)
    {
	switch(m)
			{
			     case 1:	LcdShowStr(5, 0,"1");Led1();c=0;break;

				 case 2:    LcdShowStr(5, 0,"2");Led2();c=0;break;
				 case 3:    LcdShowStr(5, 0,"3");Led3();c=0;break;
				 case 4:    LcdShowStr(5, 0,"4");Led4();c=0;break;
				 case 5:    LcdShowStr(5, 0,"5");Led5();c=0;break;
				 case 6:    LcdShowStr(5, 0,"6");Led6();c=0;break;
				 case 7:    LcdShowStr(5, 0,"7");Led7();c=0;break;
				 case 8:    LcdShowStr(5, 0,"8");Led8();c=0;break;
				 case 9:    LcdShowStr(5, 0,"9");Led9();c=0;break;
				 case 0:    LcdShowStr(5, 0,"0");Led0();c=0;break;
				 default:
				 P0=0xFF;break;
		     }
	 ENLED=1;	  //关闭LED使能端，防止闪烁
	 KeyDriver();  //调用按键驱动
	 UartDriver();  //调用串口驱动  
        //if (flag300ms)
        {
            flag300ms = 0;
			//显示通道0的电压
			LcdShowStr(9, 0, "TJ 50");  //显示通道指示
            val=GetADCValue(0);  //获取ADC通道0的转换值
			LcdShowStr(0, 0, "Mode=");  //显示通道指示
			ADDR2=1;
            ADDR1=1;
            ADDR0=0;
			P0=0xFF;
            ValueToString(str, val);  //转为字符串格式的电压值			 
			LcdShowStr(0, 1, "Voltage=");    //显示到液晶上
            LcdShowStr(8, 1, str);    //显示到液晶
			TI=1;
			if(flagUart)
			{
			      UartWrite(str,8);
				  //UartWrite('\n',1);

			}
		}	
      }
	  Infra();
      ENLED=0;	  //打开LED使能端
}
/* 读取当前的ADC转换值，chn-ADC通道号0~3 */
unsigned char GetADCValue(unsigned char chn)
{
    signed long val;
    
    I2CStart();
    if (!I2CWrite(0x48<<1))  //寻址PCF8591，如未应答，则停止操作并返回0
    {
        I2CStop();
        return 0;
    }
    I2CWrite(0x40|chn);        //写入控制字节，选择转换通道
    I2CStart();
    I2CWrite((0x48<<1)|0x01);  //寻址PCF8591，指定后续为读操作    
    I2CReadACK();              //先空读一个字节，提供采样转换时间
    val = I2CReadNAK();        //读取刚刚转换完的值
    I2CStop();
    
    return val;
}
/* ADC转换值转为实际电压值的字符串形式，str-字符串指针，val-AD转换值 */
void ValueToString(unsigned char *str, unsigned long val)
{
    //电压值=转换结果*2.5V/255，式中的25隐含了一位十进制小数
    val = (val*2500) / 255;
	t=val*6;
    str[0] = (val/1000)%10 + '0';  //整数位字符

    str[1] = '.';             //小数点

    str[2] = (val/100)%10 + '0';  //小数位字符
	
	str[3] = (val/10)%10 +'0';
	
	str[4] = (val%10) + '0';

    str[5] = 'V';             //电压单位

    str[6] = '\0';            //结束符
}

void Infra()
{
     if (irflag)  //接收到红外数据时刷新显示
        {
            irflag = 0;
			if (ircode[2] == 0x16) //  0
			{
			   m=0;
			}
			 else if (ircode[2] == 0x0C) //  1
			{
			  m=1;
			}
			else if (ircode[2] == 0x18)	  //  2
			{
			   m=2;
			}
				else if (ircode[2] == 0x5E)		//  3
			{
			  m=3;
			}
				else if (ircode[2] == 0x08)		//  4
			{
			  m=4;
			}
				else if (ircode[2] == 0x1C)	   //  5
			{
			   m=5;
			}
				else if (ircode[2] == 0x5A)	   //  6
			{
			   m=6;
			}
				else if (ircode[2] == 0x42)		//  7
			{
			   m=7;
			}
				else if (ircode[2] == 0x52)	  //  8
			{
			   m=8;
			}
			else if (ircode[2] == 0x4A)		//  9
			{
			   m=9;
			}
			else if (ircode[2] == 0x07)		//  -1
			{
			  	
			}
			else if(ircode[2] == 0x09) 	// '+' ，加号
            {
                 
            }
            else if (ircode[2] == 0x15)  //	'-' ，减号
            {
               
            }
			else if (ircode[2] == 0x0D)  //	'*'	，乘号
            {
               
			}
			else if (ircode[2] == 0x19)  // '/'	 ，除号
            {
                
			}

	       	else if (ircode[2] == 0x43)   //回车键，得出结果
            {
                
			}
			else if(ircode[2] == 0x44)
			{
			   
			}	 
			else if ((ircode[2] == 0x40)|| (ircode[2] == 0x45)) //Ens清零键
            {
               
			}
		    else if (ircode[2] == 0x46)  //  蜂鸣器开启
            {  
			     flagBuzzOn=1;
			}
			else if (ircode[2] == 0x47)  //  蜂鸣器关闭
            {  
			     flagBuzzOn=0;
			}
        }
}
/* 内存比较函数，比较两个指针所指向的内存数据是否相同，
   ptr1-待比较指针1，ptr2-待比较指针2，len-待比较长度
   返回值-两段内存数据完全相同时返回1，不同返回0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //遇到不相等数据时即刻返回0
        {
            return 0;
        }
    }
    return 1;  //比较完全部长度数据都相等则返回1
}
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char code cmd0[] = "1";   //开蜂鸣器命令
    unsigned char code cmd1[] = "2";  //关蜂鸣器命令
    unsigned char code cmd2[] = "3";  //字符串显示命令
	unsigned char code cmd3[] = "4";  //字符串显示命令
	unsigned char code cmd4[] = "5";  //字符串显示命令
	unsigned char code cmd5[] = "6";  //字符串显示命令
	unsigned char code cmd6[] = "7";  //字符串显示命令
	unsigned char code cmd7[] = "8";  //字符串显示命令
	unsigned char code cmd8[] = "9";  //字符串显示命令
    unsigned char code cmdLen[] = {          //命令长度汇总表
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,	 sizeof(cmd3)-1, sizeof(cmd4)-1, sizeof(cmd5)-1,
		sizeof(cmd6)-1,sizeof(cmd7)-1,sizeof(cmd8)-1,
    };
    unsigned char code *cmdPtr[] = {         //命令指针汇总表
        &cmd0[0],  &cmd1[0],  &cmd2[0],	&cmd3[0],&cmd4[0],&cmd5[0],&cmd6[0],&cmd7[0],&cmd8[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  //遍历命令列表，查找相同命令
    {
        if (len >= cmdLen[i])  //首先接收到的数据长度要不小于命令长度
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  //比较相同时退出循环
            {
                break;
            }
        }
    }
    switch (i)  //循环退出时i的值即是当前命令的索引值
    {
        case 0:
            //flagBuzzOn = 1; //开启蜂鸣器
		    m=1;
            break;
        case 1:
            //flagBuzzOn = 0; //关闭蜂鸣器
			m=2;
            break;
        case 2:
            m=3;
            break;
		case 3:
            m=4;
            break;
		case 4:
            m=5;
            break;
		case 5:
            m=6;
            break;
		case 6:
            m=7;
            break;
		case 7:
            m=8;
            break;
		case 8:
            m=9;
            break;
        default:   //未找到相符命令时，给上机发送“错误命令”的提示
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r';  //有效命令被执行后，在原命令帧之后添加
    buf[len++] = '\n';  //回车换行符后返回给上位机，表示已执行
    UartWrite(buf, len);
}

void Led1()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  //while(1)
  {
	 for(cnt=0;cnt<8;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x01<<cnt);		//逻辑左移,取反，0000 0011——1111 1100
		for(i=0;i<t;i++);	  //延时函数，不断执行空循环
		if(c==1)
		break;
     }
	 P0=0xFF;
  }
}



void Led2()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  //while(1)
  {
	 for(cnt=0;cnt<8;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0x80>>cnt);		//逻辑右移,取反，1100 0000——0011 1111
		for(i=0;i<t;i++);	  //延时函数，不断执行空循环
		if(c==1)
		break;
	  }
  }
}

void Led3()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;
  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  //while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x01<<cnt);		//逻辑左移,取反，0000 0001——1111 1110
		for(i=0;i<t;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0x80>>cnt);		//逻辑右移,取反，1000 0000——0111 1111
		for(i=0;i<t;i++);	  //延时函数，不断执行空循环
	  }
  }
}
void Led4()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<6;cnt++)
		{
            P0=L1[cnt];
			for(i=0;i<t;i++);
		}
  }
}
void Led5()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<22;cnt++)
		{
            P0=L2[cnt];
			for(i=0;i<t;i++);
		}
  }
}
void Led6()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<8;cnt++)
		{
            P0=L3[cnt];
			for(i=0;i<t;i++);
		}
  }
}	 
void Led7()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<14;cnt++)
		{
            P0=ledd3[cnt];
			for(i=0;i<t;i++);
		}
  }
}

void Led8()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<11;cnt++)
		{
            P0=ledd5[cnt];
			for(i=0;i<t;i++);
		}
  }
}
void Led9()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<11;cnt++)
		{
            P0=ledd4[cnt];
			for(i=0;i<t;i++);
		}
  }
}
void Led0()
{
   ENLED=1;
   P0=0xFF;
}
/*void dianscan()
{
    P0 = 0xFF;   //显示消隐
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image[index][0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image[index][1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image[index][2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image[index][3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image[index][4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image[index][5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image[index][6]; break;
        case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image[index][7]; break;
        default: break;
    }
} */
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    static unsigned char u=0;
    if  (keycode == 0x31)   //输入字符
    {
	    
		//LcdShowStr(5, 0,"1");
	    
		
        //Led1();
		//c=1;
		m=1;
		
    }
	 else if (keycode == 0x32)  //向上键，+
    {
	    //LcdShowStr(5, 0,"2");

        //Led2();
		//c=2;
		m=2;
    }
	 else if (keycode == 0x33)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"3");
        //Led3();
		m=3;
    }
	else if (keycode == 0x34)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"4");
        //Led4();
		m=4;
    }
	else if (keycode == 0x35)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"5");
        //Led5();
		m=5;
    }
	else if (keycode == 0x36)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"6");
       // Led6();
	   m=6;
    }
	else if (keycode == 0x37)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"7");
        //Led7();
		m=7;
    }
	else if (keycode == 0x38)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"8");
        //Led8();
		m=8;
    }
		else if (keycode == 0x39)  //向上键，+
    {
	   	//LcdShowStr(5, 0,"9");
        //Led9();
		m=9;
    } 
	
		else if (keycode == 0x30)  //向上键，+
    {
	   	m=0;
    } 
    else if (keycode == 0x26)  //向上键，+
    {
        //OprtKeyAction(0);
		t=t-10000;
    }
    else if (keycode == 0x28)  //向下键，-
    {
       // OprtKeyAction(1);
	   t=t+10000;
    }
    else if (keycode == 0x25)  //向左键，*
    {
        //OprtKeyAction(2);
    }
    else if (keycode == 0x27)  //向右键，÷
    {
       // OprtKeyAction(3);
    }
    else if (keycode == 0x0D)  //回车键，计算结果
    {
	   switch(u)
			{
              case  0:flagUart=1;u++;break;
			  case  1:flagUart=0;u=0;break;
			  default:break;
			}

    }
    else if (keycode == 0x1B)  //Esc键，清除
    {
	    c=1;
        P0=0xFF;
    }
}

/* LED动态扫描刷新函数，需在定时中断中调用 */
/*void LedScan()
{
    static unsigned char i = 0;  //动态扫描索引
    
    P0 = 0xFF;             //关闭所有段选位，显示消隐
    P1 = (P1 & 0xF8) | i;  //位选索引值赋值到P1口低3位
    P0 = LedBuff[i];       //缓冲区中索引位置的数据送到P0口
    if (i < 5)             //索引递增循环，遍历整个缓冲区
        i++;
    else
        i = 0;
}  */
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 12;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}

/* T0中断服务函数，执行300ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	KeyScan();   //按键扫描
	//LedScan();
	UartRxMonitor(1);  //串口接收监控
	if(flagBuzzOn)
	   BUZZ=~BUZZ;
	else
	  BUZZ=1;
    tmr300ms++;
    if (tmr300ms >= 1)  //定时300ms
    {
        tmr300ms = 0;
        flag300ms = 1;
    }
}

/* 配置并启动T1，ms-定时时间 */
void ConfigTimer1(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 12;           //补偿中断响应延时造成的误差
    T1RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //清零T1的控制位
    TMOD |= 0x10;   //配置T1为模式1
    TH1 = T1RH;     //加载T1重载值
    TL1 = T1RL;
    ET1 = 1;        //使能T1中断
    TR1 = 1;        //启动T1
}

/* T1中断服务函数，定时动态调整占空比 */
void InterruptTimer1() interrupt 3
{
    static unsigned int tmr1s=0;
	static bit dir = 0;
    static unsigned char index = 0;
    TH1 = T1RH;  //重新加载T1重载值
    TL1 = T1RL;

	tmr1s++;
	if(tmr1s>=10)
	{
	    tmr1s=0;
		flag1s=1;
	//	cnt++;
	//	t++;
	//	P0=~(0x03<<cnt);
		//if(cnt>=8)
	//	cnt=0;

	} 
} 