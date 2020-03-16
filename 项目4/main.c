#include <reg52.h>
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit BUZZ = P1^6; //蜂鸣器控制引脚
bit flagBuzzOn = 0; //蜂鸣器启动标志
unsigned char code LedChar[] = { //数码管显示字符转换表
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

bit flag1s = 0;          //1s定时标志	 
//bit flag200ms = 1;  //200ms定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
extern void DelayX10us(unsigned char t);
//extern void InitLcd1602();
//extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
//void ShowNumber(unsigned long num);

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
    
    bit res;
    int temp;        //读取到的当前温度值
    int intT, decT;  //温度值的整数和小数部分
    unsigned char len;
    unsigned char str[12];

    EA = 1;            //开总中断
	EA = 1; //使能总中断
    ENLED = 0; //选择数码管进行显示
    ADDR3 = 1;
	TMOD = 0x01; //设置 T0 为模式 1
    TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
    TL0 = 0x67;
    ET0 = 1; //使能 T0 中断
    TR0 = 1; //启动 T0
    ConfigTimer0(1);  //T0定时10ms
    Start18B20();      //启动DS18B20

	DelayX10us(100000);
    //InitLcd1602();     //初始化液晶
    while (1)
    {
        if (flag1s)  //每秒更新一次温度
        {
            flag1s = 0;
			P0=0xFF;
		//	DelayX10us(100000);
            res = Get18B20Temp(&temp);  //读取当前温度
			
            if (res)                    //读取成功时，刷新当前温度显示
            {
                intT = temp >> 4;             //分离出温度值整数部分
                decT = temp & 0xF;            //分离出温度值小数部分
               // len = IntToString(str, intT); //整数部分转换为字符串
               // str[len++] = '.';             //添加小数点
                decT = (decT*10) / 16;        //二进制的小数部分转换为1位十进制位
                //str[len++] = decT + '0';      //十进制小数位再转换为ASCII字符
                //while (len < 6)               //用空格补齐到6个字符长度
               // {
                    //str[len++] = ' ';
               // }
                //str[len] = '\0';              //添加字符串结束符
               // LcdShowStr(0, 0, str);        //显示到液晶屏上
			    ShowLedNumber(5,intT/10,0);
				ShowLedNumber(4,(intT-(intT/10)*10),1);
				ShowLedNumber(3,decT,0);
				//ShowLedNumber(2,decT,0);
				//ShowLedNumber(1,decT,0);
				if(intT>29)
				flagBuzzOn = 1; //开启蜂鸣器
				else
				flagBuzzOn = 0; //开启蜂鸣器
            }
            else                        //读取失败时，提示错误信息
            {
              //  LcdShowStr(0, 0, "error!");
			  ShowLedNumber(5,0,0);
			  ShowLedNumber(4,0,1);
			  ShowLedNumber(3,0,0);
            }
            Start18B20();               //重新启动下一次转换
			DelayX10us(100000);
        }
    }
}


/* 数码管动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
     static unsigned char i = 0; //动态扫描的索引
     P0 = 0xFF; //显示消隐
     switch (i)
     {
         case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
         case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
         case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
         case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
         case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
         case 5: ADDR2=1; ADDR1=0; ADDR0=1; i=0; P0=LedBuff[5]; break;
         default: break;
      }
}
/* 整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
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
/* T0中断服务函数，完成1秒定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr1s = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	LedScan(); //调用数码管显示扫描函数
    tmr1s++;
    if (tmr1s++>=1)  //定时1ms
    {
        tmr1s = 0;
        flag1s = 1;
    }
	if (flagBuzzOn) //执行蜂鸣器鸣叫或关闭
    BUZZ = ~BUZZ;
    else
    BUZZ = 1;
}
