#include <reg52.h>
extern bit irflag;
extern unsigned char ircode[4];
extern void InitInfrared();
signed char i = 0;
unsigned char len = 0;
unsigned char str[16];

unsigned char code SinWave[] = {  //正弦波波表
    127, 152, 176, 198, 217, 233, 245, 252,
    255, 252, 245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,   2,   9,  21,  37,  56,  78, 102,
	  127,152,176,198,217,233,245,252,255,252,
	  245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,2,   9,  21,  37,  56,  78, 102,

	  127,152,176,198,217,233,245,252,255,252,
	  245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,   2,   9,  21,  37,  56,  78, 102, 
};
unsigned char code SquWave[] = {  //方波波表
    255, 255, 255, 255,255, 255, 255, 255,
   0, 0, 0, 0,  0,  0,  0,   0,
      255, 255, 255, 255,255, 255, 255, 255, 
	  0, 0, 0, 0,  0,  0,  0,   0,
	  255, 255, 255, 255,255, 255, 255, 255,
   0, 0, 0, 0,  0,  0,  0,   0,
      255, 255, 255, 255,255, 255, 255, 255, 
	  0, 0, 0, 0,  0,  0,  0,   0,
	  255, 255, 255, 255,255, 255, 255, 255,
   0, 0, 0, 0,  0,  0,  0,   0,
      255, 255, 255, 255,255, 255, 255, 255, 
	  0, 0, 0, 0,  0,  0,  0,   0,

};
unsigned char code TriWave[] = {  //三角波波表
      0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
	 0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
	 0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
};
unsigned char code SawWave[] = {  //锯齿波表
      0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
	0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
	0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
};
unsigned char fWave = 10;   //波形频率
unsigned char code *pWave;  //波表指针
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char T1RH = 1;  //T1重载值的高字节
unsigned char T1RL = 1;  //T1重载值的低字节

void ConfigTimer0(unsigned int ms);
void SetWaveFreq(unsigned char freq);

void NumKeyAction(unsigned char n);
unsigned char LongToString(unsigned char *str, signed long dat);
extern void KeyScan();
extern void KeyDriver();
extern void I2CStart();
extern void I2CStop();
extern bit I2CWrite(unsigned char dat);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdFullClear();
extern void LcdWriteDat(unsigned char dat);
void main()
{    
    EA = 1;           //开总中断
	InitInfrared();   //初始化红外功能
    ConfigTimer0(1);  //配置T0定时1ms
    pWave = SinWave;  //默认正弦波
    SetWaveFreq(10);  //默认频率10Hz
	fWave=10;
    InitLcd1602();    //初始化液晶
	LcdShowStr(0,0,"SinWave");
    LcdShowStr(0, 1, "10");  //初始显示一个数字10
	LcdShowStr(2, 1, "Hz");  //初始显示一个数字10
    while (1)
    {
        KeyDriver();  //调用按键驱动
		if (irflag)  //接收到红外数据时刷新显示
        {
            irflag = 0;
            if (ircode[2] == 0x09)  //0x09为+键码
            {
                  if (fWave < 25) //达到最大值25之前可递增频率值
                  {
                        fWave++;
                         SetWaveFreq(fWave);

                   }
            }
            else if (ircode[2] == 0x15)  //0x15为-键码
            {
                   if (fWave > 2) //达到最小值2之前可递减频率值
                   {
                        fWave--;
                        SetWaveFreq(fWave);
						len = LongToString(str, fWave);  //新数值转换为字符串
		                LcdAreaClear(0, 1, str);        //清除第二行中间的字符位
                        LcdShowStr(6, 1, str);     //显示到液晶第二行上
                    }
            }
				if (ircode[2] == 0x16) //  0
			{
			   //num=ircode[2];
			   NumKeyAction(0x00);
			}
			 else if (ircode[2] == 0x0C) //  1
			{
			   //num=0;
			   NumKeyAction(0x01);
			}
			else if (ircode[2] == 0x18)	  //  2
			{
			   NumKeyAction(0x02);
			}
				else if (ircode[2] == 0x5E)		//  3
			{
			   NumKeyAction(0x03);
			}
				else if (ircode[2] == 0x08)		//  4
			{
			   NumKeyAction(0x04);
			}
				else if (ircode[2] == 0x1C)	   //  5
			{
			   NumKeyAction(0x05);
			}
				else if (ircode[2] == 0x5A)	   //  6
			{
			   NumKeyAction(0x06);
			}
				else if (ircode[2] == 0x42)		//  7
			{
			   NumKeyAction(0x07);
			}
				else if (ircode[2] == 0x52)	  //  8
			{
			   NumKeyAction(0x08);
			}
			else if (ircode[2] == 0x4A)		//  9
			{
			   NumKeyAction(0x09);
			}
        }
    }
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    static unsigned char i = 0;
    
    if (keycode == 0x26)  //向上键，切换波形
    {
        //在4种波形间循环切换
        if (i == 0)
        {
            i = 1;
            pWave = SquWave;
			LcdShowStr(0,0,"SquWave");
        }
		else if (i == 1)
        {
            i = 2;
            pWave =TriWave;
			LcdShowStr(0,0,"TriWave");
        }
        else if (i == 2)
        {
            i = 3;
            pWave = SawWave;
			LcdShowStr(0,0,"SawWave");
        }
        else
        {
            i = 0;
            pWave = SinWave;
			LcdShowStr(0,0,"SinWave");
        }

    }

	 if (keycode == 0x28)  //向下键，切换波形
    {
        //在4种波形间循环切换
        if (i == 0)
        {
            i = 1;
            pWave = SquWave;
			LcdShowStr(0,0,"SquWave");
        }
		else if (i == 1)
        {
            i = 2;
            pWave =TriWave;
			LcdShowStr(0,0,"TriWave");
        }
        else if (i == 2)
        {
            i = 3;
            pWave = SawWave;
			LcdShowStr(0,0,"SawWave");
        }
        else
        {
            i = 0;
            pWave = SinWave;
			LcdShowStr(0,0,"SinWave");
        }

    }
}

/* 长整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char LongToString(unsigned char *str, signed long dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[16];
    
    if (dat < 0)  //如果为负数，首先取绝对值，并在指针上添加负号
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //先转换为低位在前的十进制数组
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i最后的值就是有效字符的个数
    while (i-- > 0)   //将数组值转换为ASCII码反向拷贝到接收指针上
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //添加字符串结束符
    
    return len;   //返回字符串长度
}
void NumKeyAction(signed char n)
{
    unsigned char len;
    unsigned char str[16];
    
    
       fWave = fWave*10 + n;             //输入数值累加到原操作数上

        len = LongToString(str, fWave);  //新数值转换为字符串
		 LcdAreaClear(0, 1, str);        //清除第二行中间的字符位
        LcdShowStr(6, 1, str);     //显示到液晶第二行上
  
}
/* 设置DAC输出值，val-设定值 */
void SetDACOut(unsigned char val)
{
    I2CStart();
    if (!I2CWrite(0x48<<1)) //寻址PCF8591，如未应答，则停止操作并返回
    {
        I2CStop();
        return;
    }
    I2CWrite(0x40);         //写入控制字节
    I2CWrite(val);          //写入DA值  
    I2CStop();
}
/* 设置输出波形的频率，freq-设定频率 */
void SetWaveFreq(unsigned char freq)
{
    unsigned long tmp;
    
    tmp = (11059200/12) / (freq*96); //定时器计数频率，是波形频率的32倍
    tmp = 65536 - tmp;               //计算定时器重载值
    tmp = tmp + 36;                  //修正中断响应延时造成的误差
    T1RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //清零T1的控制位
    TMOD |= 0x10;   //配置T1为模式1
    TH1 = T1RH;     //加载T1重载值
    TL1 = T1RL;
    ET1 = 1;        //使能T1中断
    PT1 = 1;        //设置为高优先级
    TR1 = 1;        //启动T1
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
/* T0中断服务函数，执行按键扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
}
/* T1中断服务函数，执行波形输出 */
void InterruptTimer1() interrupt 3
{
    static unsigned char i = 0;
    
    TH1 = T1RH;  //重新加载重载值
    TL1 = T1RL;
    //循环输出波表中的数据
    SetDACOut(pWave[i]);
    i++;
    if (i >= 96)
    {
        i = 0;
    }
}
