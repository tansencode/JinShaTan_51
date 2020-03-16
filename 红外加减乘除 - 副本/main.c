#include <reg52.h>

unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

sbit BUZZ=P1^6;
bit flagBuzzOn=0;
unsigned char step = 0;  //操作步骤
unsigned char oprt = 0;  //运算类型
//static signed long num1 = 0;    //操作数1
//static signed long num2 = 0;    //操作数2
static float result2 = 0;  //运算结果
static float result = 0.000000;  //运算结果
static signed long res = 0;  //运算结果
//signed char result[16];

static  float num1 = 0;    //操作数1
static  float num2 = 0;    //操作数2
//static  float result = 0;  //运算结果

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdFullClear();
extern void LcdWriteDat(unsigned char dat);
extern bit irflag;
extern unsigned char ircode[16];
extern void InitInfrared();
void NumKeyAction1();
void ConfigTimer0(unsigned int ms);
void OprtKeyAction(unsigned char type);
void Reset();
void GetResult();
void NumKeyAction(unsigned char n);
void NumKeyAction2();
static unsigned char len;
static unsigned char str[16];
static int j=0;

void main()
{
    static signed int i=0;
    EA = 1;      //开总中断
    InitInfrared();   //初始化红外功能
    ConfigTimer0(2);  //配置T0定时2ms
    InitLcd1602();    //初始化液晶
    LcdShowStr(15, 1, "0");  //初始显示一个数字0
    while (1)
    {
        if (irflag)  //接收到红外数据时刷新显示
        {
            irflag = 0;
            // if ((ircode[2] >= '0')&&(ircode[2] <= '9'))   //0x09为+键码，正转1圈
		//	{
              //    NumKeyAction(ircode[2] - '0');
            //}
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
			else if (ircode[2] == 0x07)		//  -1
			{
			  	//OprtKeyAction(1);
				//NumKeyAction(-0x01);
				 NumKeyAction2();
				 //LcdWriteDat(0x2E);
				 //LcdShowStr(15, 1, ".");     //显示到液晶第二行上
			}
			else if(ircode[2] == 0x09) 	// '+' ，加号
            {
                 OprtKeyAction(0);
            }
            else if (ircode[2] == 0x15)  //	'-' ，减号
            {
                OprtKeyAction(1);
            }
			else if (ircode[2] == 0x0D)  //	'*'	，乘号
            {
                OprtKeyAction(2);
			}
			else if (ircode[2] == 0x19)  // '/'	 ，除号
            {
                OprtKeyAction(3);
			}

	       	else if (ircode[2] == 0x43)   //回车键，得出结果
            {
                GetResult();
			}
			else if(ircode[2] == 0x44)
			{
			    NumKeyAction1();
			}	 
			else if ((ircode[2] == 0x40)|| (ircode[2] == 0x45)) //Ens清零键
            {
               Reset();
               LcdShowStr(15, 1, "0");
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

/* 显示运算符，显示位置y，运算符类型type */
void ShowOprt(unsigned char y, unsigned char type)
{
    switch (type)
    {
        case 0: LcdShowStr(0, y, "+"); break;  //0代表+
        case 1: LcdShowStr(0, y, "-"); break;  //1代表-
        case 2: LcdShowStr(0, y, "*"); break;  //2代表*
        case 3: LcdShowStr(0, y, "/"); break;  //3代表/
        default: break;
    }
}
/* 计算器复位，清零变量值，清除屏幕显示 */
void Reset()
{
    num1 = 0;
    num2 = 0;
    step = 0;
    LcdFullClear();
}
/* 数字键动作函数，n-按键输入的数值 */
void NumKeyAction(signed char n)
{
   // unsigned char len;
    //unsigned char str[16];
    
    if (step > 1)  //如计算已完成，则重新开始新的计算
    {
        Reset();
    }
    if (step == 0)  //输入第一操作数
    {
	    if(j=0)
		{
        num1 = num1*(10.0) + n;             //输入数值累加到原操作数上
        str[len]=num1+'0';
		len = LongToString(str, num1);  //新数值转换为字符串
	
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
		}
		else if(j=1)
		{
        num1 = num1*(10.0) + n;             //输入数值累加到原操作数上
		str[0]=num1/10+'0';
	    str[1]='.';
		str[2]=num1 / 10+'0';
        //len = LongToString(str, num1);  //新数值转换为字符串
	
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
		j=0;
		}

	
    }
    else            //输入第二操作数
    {
        num2 = num2*(10.0) + n;             //输入数值累加到原操作数上
        len = LongToString(str, num2);  //新数值转换为字符串
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
}

void NumKeyAction1()
{
    //unsigned char len;
    //unsigned char str[16];
    
    if (step > 1)  //如计算已完成，则重新开始新的计算
    {
        Reset();
    }
    if (step == 0)  //输入第一操作数
    {
        num1 = num1/10;             //输入数值累加到原操作数上
        len = LongToString(str, num1);  //新数值转换为字符串
		 LcdAreaClear(1, 1, str);        //清除第二行中间的字符位
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
    else            //输入第二操作数
    {
        num2 = num2/10;             //输入数值累加到原操作数上
        len = LongToString(str, num2);  //新数值转换为字符串
		 LcdAreaClear(1, 1,str);        //清除第二行中间的字符位
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
}

void NumKeyAction2()
{
    //unsigned char len;
    //unsigned char str[16];
    
    if (step > 1)  //如计算已完成，则重新开始新的计算
    {
        Reset();
    }
    if (step == 0)  //输入第一操作数
    {
        //num1 = num1/10;             //输入数值累加到原操作数上
        len = LongToString(str, num1);  //新数值转换为字符串
		str[len++]='.';
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
		j=1;
		//len=len+1;
		//num1=num1*100;
    }
    else            //输入第二操作数
    {
        //num2 = num2/10;             //输入数值累加到原操作数上
			str[len++]='.';
        len = LongToString(str, num2);  //新数值转换为字符串
	//	str[len++]='.';
		LcdAreaClear(1, 1,str);        //清除第二行中间的字符位
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
		//len=len+1;
    }
}


/*unsigned char LongToString1(unsigned char *str, signed long dat)
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
    return len;   //返回字符串长度
}		 */

/*void NumKeyAction1(signed char n)
{
    unsigned char len;
    unsigned char str[16];
    static unsigned int i=0;
    if (step > 1)  //如计算已完成，则重新开始新的计算
    {
        Reset();
    }
    if (step == 0)  //输入第一操作数
    {
	    switch(i)
		{
		case 0:num1=n;break;
		case 1:
          num1 = num1*n;             //输入数值累加到原操作数上
		  break;
		}
        len = LongToString1(str, num1);  //新数值转换为字符串
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
    else            //输入第二操作数
    {
        num2 = num2*10 + n;             //输入数值累加到原操作数上
        len = LongToString1(str, num2);  //新数值转换为字符串
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
}		 */

/* 运算符按键动作函数，运算符类型type */
void OprtKeyAction(unsigned char type)
{
    unsigned char len;
    unsigned char str[16];
    
    if (step == 0)  //第二操作数尚未输入时响应，即不支持连续操作
    {
        len = LongToString(str, num1); //第一操作数转换为字符串
        LcdAreaClear(0, 0, 16-len);    //清除第一行左边的字符位
        LcdShowStr(16-len, 0, str);    //字符串靠右显示在第一行
        ShowOprt(1, type);             //在第二行显示操作符
        LcdAreaClear(1, 1, 14);        //清除第二行中间的字符位
        LcdShowStr(15, 1, "0");        //在第二行最右端显示0
        oprt = type;                   //记录操作类型
        step = 1;
    }
}

/* 计算结果函数 */
void GetResult()
{
    unsigned char len;
    unsigned char str[16];
    
    if (step == 1) //第二操作数已输入时才执行计算
    {
        step = 2;
        switch (oprt)  //根据运算符类型计算结果，未考虑溢出问题
        {
            case 0: result = num1 + num2; break;
            case 1: result = num1 - num2; break;
            case 2: result = num1 * num2; break;
            case 3: result = num1 / num2; break;
            default: break;
        }
     /* result1 =result1*1000000;
      res=result1;
	  result[1]=' ';
      result[2]=' ';
	  result[3]=' ';
	  result[4]=res/10000000000+0x30;
	  result[5]=res/1000000000%10+0x30;
	  result[6]=res/100000000%10+0x30;
	  result[7]=res/10000000%10+0x30;
	  result[8]=res/1000000%10+0x30;
	  result[9]='.';		  
	  result[10]=res/100000%10+0x30;
	  result[11]=res/10000%10+0x30;
	  result[12]=res/1000%10+0x30;
	  result[13]=res/100%10+0x30;
	  result[14]=res/10%10+0x30;
	  result[15]=res%10+0x30;  */

        len = LongToString(str, num2);  //原第二操作数和运算符显示到第一行
        ShowOprt(0, oprt);
        LcdAreaClear(1, 0, 16-1-len);
        LcdShowStr(16-len, 0, str);
        len = LongToString(str, result);  //计算结果和等号显示在第二行
        LcdShowStr(0, 1, "=");
        LcdAreaClear(1, 1, 16-1-len);
        LcdShowStr(16-len, 1, str);
    }
}

/* 步进电机启动函数，angle-需转过的角度 */
/*void StartMotor(signed long angle)
{
    //在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
    EA = 0;
    beats = (angle * 4076) / 360; //实测为4076拍转动一圈
    EA = 1;
}	 */
/* 步进电机停止函数 */
/*void StopMotor()
{
    EA = 0;
    beats = 0;
    EA = 1;
}  */
/* 电机转动控制函数 */
/*void TurnMotor()
{
    unsigned char tmp;  //临时变量
    static unsigned char index = 0;  //节拍输出索引
    unsigned char code BeatCode[8] = {  //步进电机节拍对应的IO控制代码
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };
    
    if (beats != 0)  //节拍数不为0则产生一个驱动节拍
    {
        if (beats > 0)  //节拍数大于0时正转
        {
            index++;               //正转时节拍输出索引递增
            index = index & 0x07;  //用&操作实现到8归零
            beats--;               //正转时节拍计数递减
        }
        else            //节拍数小于0时反转
        {
            index--;               //反转时节拍输出索引递减
            index = index & 0x07;  //用&操作同样可以实现到-1时归7
            beats++;               //反转时节拍计数递增
        }
        tmp = P1;                    //用tmp把P1口当前值暂存
        tmp = tmp & 0xF0;            //用&操作清零低4位
        tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低4位
        P1  = tmp;                   //把低4位的节拍代码和高4位的原值送回P1
    }
    else  //节拍数为0则关闭电机所有的相
    {
        P1 = P1 | 0x0F;
    }
}	 */
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 13;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行数码管扫描显示 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	if(flagBuzzOn)
	   BUZZ=~BUZZ;
	else
	  BUZZ=1;
    //TurnMotor(); //执行电机驱动
}
