#include <reg52.h>
//#define  _LED_BUZZER_C
#define _TIME_C
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
/*unsigned char LedBuff1[] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};	*/
unsigned char Leddp[]={
0x7F,0x7F,0x7F,0x7F,0x7F,0x7F};

//struct sLedBuff ledBuff;
struct sTime CurTime;
struct sTime {  //日期时间结构体定义
    unsigned int  year;
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char week;
};

bit flag200ms = 1;  //200ms定时标志
struct sTime bufTime;  //日期时间缓冲区
unsigned char setIndex = 0;  //时间设置索引
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned int index;
unsigned int num;
unsigned int point;

void ConfigTimer0(unsigned int ms);
void RefreshTimeShow();
void RefreshTime();
extern void InitDS1302();
extern void GetRealTime(struct sTime *time);
extern void SetRealTime(struct sTime *time);
extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdSetCursor(unsigned char x, unsigned char y);
extern void LcdOpenCursor();
extern void LcdCloseCursor();


/* 数码管上显示一位数字，index-数码管位索引(从右到左对应0～5)，
**     num-待显示的数字，point-代表是否显示此位上的小数点 */
void ShowLedNumber(unsigned int index, unsigned int num,unsigned int point)
{
   P0=0xFF;
   LedBuff[index] = LedChar[num];  //输入数字转换为数码管字符0～F
    if (point != 0)
    {
       LedBuff[index] &= 0x7F;  //point不为0时点亮当前位的小数点
    }
}

void main()
{
    unsigned char psec=0xAA;  //秒备份，初值AA确保首次读取时间后会刷新显示
    EA = 1;           //开总中断
	ENLED = 0; //选择数码管进行显示
    ADDR3 = 1;
	ET0 = 1; //使能 T0 中断
	TR0 = 1; //启动 T0
    ConfigTimer0(1);  //T0定时1ms
	P0=0xFF;
    InitDS1302();     //初始化实时时钟
    InitLcd1602();    //初始化液晶
	 RefreshTime();
    
    //初始化屏幕上固定不变的内容
    //LcdShowStr(3, 0, "20  -  -  ");
    
    while (1)
    {
        KeyDriver();  //调用按键驱动
        if (flag200ms && (setIndex == 0))
        {                             //每隔200ms且未处于设置状态时，
            flag200ms = 0;
			
			RefreshTime();
			P0=0xFF;
            GetRealTime(&bufTime);    //获取当前时间
            if (psec != bufTime.sec)  //检测到时间有变化时刷新显示
            {
               // RefreshTimeShow();
				RefreshTime();
                psec = bufTime.sec;   //用当前值更新上次秒数
			
            }
        }
    }
}

/* 将一个BCD码字节显示到屏幕上，(x,y)-屏幕起始坐标，bcd-待显示BCD码 */
/*void ShowBcdByte(unsigned char x, unsigned char y, unsigned char bcd)
{
    unsigned char str[4];
    
    str[0] = (bcd >> 4) ;
    str[1] = (bcd&0x0F);
    ShowLedNumber(index,str,0);
	RefreshTime();

}	 */

/* 获取当前日期时间，并刷新时间和星期的显示 */
void RefreshTime()
{
    GetRealTime(&bufTime);                  //获取当前日期时间
    ShowLedNumber(5,bufTime.hour>>4, 0);   //时
    ShowLedNumber(4, bufTime.hour&0xF,1);
    ShowLedNumber(3, bufTime.min>>4,  0);   //分
    ShowLedNumber(2, bufTime.min&0xF, 1);
    ShowLedNumber(1, bufTime.sec>>4,  0);   //秒
    ShowLedNumber(0, bufTime.sec&0xF, 0);
} 

void RefreshTime0()
{
   // GetRealTime(&bufTime);                  //获取当前日期时间
    ShowLedNumber(5,bufTime.hour>>4, 0);   //时
    ShowLedNumber(4, bufTime.hour&0xF,0);
    ShowLedNumber(3, bufTime.min>>4,  0);   //分
    ShowLedNumber(2, bufTime.min&0xF, 0);
    ShowLedNumber(1, bufTime.sec>>4,  0);   //秒
    ShowLedNumber(0, bufTime.sec&0xF, 0);
} 
/* 日期刷新函数，ops-刷新选项：为0时只当日期变化才刷新，非0则立即刷新 */
/*void RefreshDate(uint8 ops)
{
    uint8 pdata str[12];
    static uint8 backup = 0;
    
    if ((backup!=CurTime.day) || (ops!=0))
    {
        str[0] = ((CurTime.year>>12) & 0xF) + '0';  //4位数年份
        str[1] = ((CurTime.year>>8) & 0xF) + '0';
        str[2] = ((CurTime.year>>4) & 0xF) + '0';
        str[3] = (CurTime.year & 0xF) + '0';
        str[4] = '-';                        //分隔符
        str[5] = (CurTime.mon >> 4) + '0';   //月份
        str[6] = (CurTime.mon & 0xF) + '0';
        str[7] = '-';                        //分隔符
        str[8] = (CurTime.day >> 4) + '0';   //日期
        str[9] = (CurTime.day & 0xF) + '0';
        str[10] = '\0';         //字符串结束符
        LcdShowStr(0, 0, str);  //显示到液晶上
        backup = CurTime.day;   //刷新上次日期值
    }
} */
/* 刷新日期时间的显示 */
/*void RefreshTimeShow()
{
    ShowBcdByte(5,  0, bufTime.year);
    ShowBcdByte(8,  0, bufTime.mon);
    ShowBcdByte(11, 0, bufTime.day);
    ShowBcdByte(4,  1, bufTime.hour);
    ShowBcdByte(7,  1, bufTime.min);
    ShowBcdByte(10, 1, bufTime.sec);
} */
/*void RefreshTimeShow()
{
    ShowBcdByte(5,  0, bufTime.year);
    ShowBcdByte(8,  0, bufTime.mon);
    ShowBcdByte(11, 0, bufTime.day);
    ShowBcdByte(4,  1, bufTime.hour);
    ShowBcdByte(7,  1, bufTime.min);
    ShowBcdByte(10, 1, bufTime.sec);
} */  

/* 数码管动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
   static unsigned char i = 0; //动态扫描的索引
   P0 = 0xFF; //显示消隐
   P1=(P1&0xF8)|i;		 //重点，P1口低三位是定义ADDR0~R2的，进行位选
   P0=LedBuff[i];
   if(i<5)
     i++;
   else
     i=0;
}


/* 刷新当前设置位的光标指示 */
/*void RefreshSetShow()
{
    switch (setIndex)
    {
        case 1:  LcdSetCursor(5,  0); break;
        case 2:  LcdSetCursor(6,  0); break;
        case 3:  LcdSetCursor(8,  0); break;
        case 4:  LcdSetCursor(9,  0); break;
        case 5:  LcdSetCursor(11, 0); break;
        case 6:  LcdSetCursor(12, 0); break;
        case 7:  LcdSetCursor(4,  1); break;
        case 8:  LcdSetCursor(5,  1); break;
		case 9:  LcdSetCursor(7,  1); break;
		case 10: LcdSetCursor(8,  1); break;
		case 11: LcdSetCursor(10,  1); break;
		case 12: LcdSetCursor(11,  1); break;
        default:  break;
    }
}  */
/* 递增一个BCD码的高位 */
unsigned char IncBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) < 0x90)
        bcd += 0x10;
    else
        bcd &= 0x0F;
    
    return bcd;
}
/* 递增一个BCD码的低位 */
unsigned char IncBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) < 0x09)
        bcd += 0x01;
    else
        bcd &= 0xF0;
    
    return bcd;
}
/* 递减一个BCD码的高位 */
unsigned char DecBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) > 0x00)
        bcd -= 0x10;
    else
        bcd |= 0x90;
    
    return bcd;
}
/* 递减一个BCD码的低位 */
unsigned char DecBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) > 0x00)
        bcd -= 0x01;
    else
        bcd |= 0x09;
    
    return bcd;
}
/* 递增时间当前设置位的值 */
void IncSetTime()
{
    switch (setIndex)
    {
        //case 1:  bufTime.year = IncBcdHigh(bufTime.year); break;
        //case 2:  bufTime.year = IncBcdLow(bufTime.year);  break;
        //case 3:  bufTime.mon  = IncBcdHigh(bufTime.mon);  break;
        //case 4:  bufTime.mon  = IncBcdLow(bufTime.mon);   break;
        //case 5:  bufTime.day  = IncBcdHigh(bufTime.day);  break;
        //case 6:  bufTime.day  = IncBcdLow(bufTime.day);   break;
        case 1:  bufTime.hour = IncBcdHigh(bufTime.hour); ShowLedNumber(5,bufTime.hour>>4, 1);  break;
        case 2:  bufTime.hour = IncBcdLow(bufTime.hour); ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3:  bufTime.min  = IncBcdHigh(bufTime.min); ShowLedNumber(3, bufTime.min>>4,  1);  break;
        case 4:  bufTime.min  = IncBcdLow(bufTime.min); ShowLedNumber(2, bufTime.min&0xF, 1);  break;
        case 5:  bufTime.sec  = IncBcdHigh(bufTime.sec);ShowLedNumber(1, bufTime.sec>>4,  1);   break;
        case 6:  bufTime.sec  = IncBcdLow(bufTime.sec); ShowLedNumber(0, bufTime.sec&0xF, 1);  break;
        default:  break;
    }
    //RefreshTimeShow();
	//RefreshTimeShow();
    // RefreshTime();
}
/* 递减时间当前设置位的值 */
void DecSetTime()
{
    switch (setIndex)
    {
        //case 1:  bufTime.year = DecBcdHigh(bufTime.year); break;
        //case 2:  bufTime.year = DecBcdLow(bufTime.year);  break;
        //case 3:  bufTime.mon  = DecBcdHigh(bufTime.mon);  break;
        //case 4:  bufTime.mon  = DecBcdLow(bufTime.mon);   break;
        //case 5:  bufTime.day  = DecBcdHigh(bufTime.day);  break;
        //case 6:  bufTime.day  = DecBcdLow(bufTime.day);   break;
        case 1:  bufTime.hour = DecBcdHigh(bufTime.hour);ShowLedNumber(5,bufTime.hour>>4, 1); break;
        case 2:  bufTime.hour = DecBcdLow(bufTime.hour); ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3:  bufTime.min  = DecBcdHigh(bufTime.min); ShowLedNumber(3, bufTime.min>>4,  1); break;
        case 4:  bufTime.min  = DecBcdLow(bufTime.min);   ShowLedNumber(2, bufTime.min&0xF, 1); break;
        case 5:  bufTime.sec  = DecBcdHigh(bufTime.sec); ShowLedNumber(1, bufTime.sec>>4,  1);  break;
        case 6:  bufTime.sec  = DecBcdLow(bufTime.sec);   ShowLedNumber(0, bufTime.sec&0xF, 1); break;
        default:  break;
    }
    //RefreshTimeShow();
	//RefreshTimeShow();
   //	RefreshTime();
}
/* 右移时间设置位 */
void RightShiftTimeSet()
{
        unsigned char *p;
    	if(setIndex!=0)
		  {
        if (setIndex <6)
         setIndex++;
		  
        else
            setIndex = 1;
		}
			switch(setIndex)
		{ 
		case 1: RefreshTime0(); ShowLedNumber(5,bufTime.hour>>4, 1); break;
        case 2: RefreshTime0();ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3: RefreshTime0();ShowLedNumber(3, bufTime.min>>4,  1); break;
        case 4: RefreshTime0(); ShowLedNumber(2, bufTime.min&0xF, 1); break;
        case 5: RefreshTime0();ShowLedNumber(1, bufTime.sec>>4,  1);  break;
        case 6: RefreshTime0();ShowLedNumber(0, bufTime.sec&0xF, 1);break;
           // setIndex--;
		  }
        //RefreshSetShow();
		//RefreshTime();
}

/* 左移时间设置位 */
void LeftShiftTimeSet()
{
    	if(setIndex!=0)
		{
        if (setIndex >1)
		{
	
		setIndex--;	
	
		  }
        else
            setIndex = 6;
		  	switch(setIndex)
		{ 
		case 1:  RefreshTime0();ShowLedNumber(5,bufTime.hour>>4, 1); break;
        case 2:  RefreshTime0();ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3:  RefreshTime0();ShowLedNumber(3, bufTime.min>>4,  1); break;
        case 4:  RefreshTime0(); ShowLedNumber(2, bufTime.min&0xF, 1); break;
        case 5:  RefreshTime0();ShowLedNumber(1, bufTime.sec>>4,  1);  break;
        case 6:  RefreshTime0(); ShowLedNumber(0, bufTime.sec&0xF, 1);break;
           // setIndex--;
		  }
		}
       // RefreshSetShow();
	   //RefreshTime();
}
/* 进入时间设置状态 */
void EnterTimeSet()
{
    //static unsigned char i = 5; //动态扫描的索引
    setIndex = 1;       //把设置索引设置为5，即可进入设置状态
	ShowLedNumber(4, bufTime.hour&0xF,0);
	ShowLedNumber(2, bufTime.min&0xF, 0);
	ShowLedNumber(5,bufTime.hour>>4, 1);   //时
	//ShowLedNumber(5,bufTime.hour>>4, 1);
	//ShowLedNumber(5,bufTime.hour>>4, 1);   //时
    //RightShiftTimeSet(); //再利用现成的左移操作移到位置1并完成显示刷新
    //LcdOpenCursor();    //打开光标闪烁效果
}
/* 退出时间设置状态，save-是否保存当前设置的时间值 */
void ExitTimeSet(bit save)
{
    setIndex = 0;     //把设置索引设置为0，即可退出设置状态
    if (save)         //需保存时即把当前设置时间写入DS1302
    {
        SetRealTime(&bufTime);
    }
    //LcdCloseCursor(); //关闭光标显示
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    static unsigned char i = 0; //动态扫描的索引
    if  ((keycode>='0') && (keycode<='9'))  //本例中不响应字符键
    {
    }
    else if (keycode == 0x26)  //向上键，递增当前设置位的值
    {
        IncSetTime();
		//RefreshTime();
    }
    else if (keycode == 0x28)  //向下键，递减当前设置位的值
    {
        DecSetTime();
		//RefreshTime();
    }
    else if (keycode == 0x25)  //向左键，向左切换设置位
    {
        LeftShiftTimeSet();
    }
    else if (keycode == 0x27)  //向右键，向右切换设置位
    {
	    
        RightShiftTimeSet();
    }
    else if (keycode == 0x0D)  //回车键，进入设置模式/启用当前设置值
    {
        if (setIndex == 0)  //不处于设置状态时，进入设置状态
        {
            EnterTimeSet();
        }
        else                //已处于设置状态时，保存时间并退出设置状态
        {
            ExitTimeSet(1);
        }
    }
    else if (keycode == 0x1B)  //Esc键，取消当前设置
    {
        ExitTimeSet(0);
    }
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 28;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行按键扫描和200ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
	static int i=0;

    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
/*	if(ENLED==0)
	{
	  P0=0xff;
	  P1=(P1&0xF0)|i;
	  P0=*((int data*)&LedBuff+i);
	  if(i<(sizeof(LedBuff)-1))
	  i++;
	  else
	  i=0;
	}
	*/
	KeyScan();   //按键扫描
	LedScan(); //调用数码管显示扫描函数
    tmr200ms++;
    if (tmr200ms >= 200)  //定时200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }

}

/* T2中断服务函数，LED动态扫描、蜂鸣器控制 */
/*void InterruptTimer2() interrupt 5
{
    static i = 0;  //LED位选索引
    
    TF2 = 0;  //清零T2中断标志
    //全部LED动态扫描显示
    if (ENLED == 0)  //LED使能时才进行动态扫描
    {
        P0 = 0xFF;                       //关闭所有段选位，显示消隐
        P1 = (P1 & 0xF0) | i;            //位选索引值赋值到P1口低4位
        P0 = *((data*)&ledBuff+i); //缓冲区中索引位置的数据送到P0口
        if (i < (sizeof(ledBuff)-1))     //索引递增循环，遍历整个缓冲区
            i++;
        else
            i = 0;
    }
}  */