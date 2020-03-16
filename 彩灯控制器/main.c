#include <reg52.h>

bit flag300ms = 1;       //300ms定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char RxdByte = 0;  //串口接收到的字节

void ConfigTimer0(unsigned int ms);
unsigned char GetADCValue(unsigned char chn);
void ValueToString(unsigned char *str, unsigned long val);
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
//extern void Led7();
//extern void Led8();
//extern void Led9();

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
void UsartConfiguration();
//unsigned char CNCHAR[10] = "V  ";
//void ConfigUART(unsigned int baud);

//unsigned char val;
static char str[10]={0,0,0,0,0,0,0,0};
void main()
{
    unsigned char val;
    //unsigned char str[10];
    
    EA = 1;            //开总中断
    ConfigTimer0(10);  //配置T0定时10ms
	ConfigUART(9600);   //配置波特率为9600
    InitLcd1602();     //初始化液晶  
    while (1)
    {
        if (flag300ms)
        {
            flag300ms = 0;
			//显示通道0的电压
			LcdShowStr(9, 0, "TJ 50");  //显示通道指示
            val=GetADCValue(0);  //获取ADC通道0的转换值
			LcdShowStr(0, 0, "Mode=");  //显示通道指示
            ValueToString(str, val);  //转为字符串格式的电压值			 
			LcdShowStr(0, 1, "Voltage=");    //显示到液晶上
            LcdShowStr(8, 1, str);    //显示到液晶
			TI=1;

			KeyDriver();  //调用按键驱动
			UartDriver();  //调用串口驱动
			//ValueToString();

            //显示通道1的电压
            //val = GetADCValue(1);
            //ValueToString(str, val);
            //LcdShowStr(6, 1, str);
            //显示通道3的电压
           // val = GetADCValue(3);
            //ValueToString(str, val);
            //LcdShowStr(12, 1, str);
        }
    }
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
    str[0] = (val/1000)%10 + '0';  //整数位字符

    str[1] = '.';             //小数点

    str[2] = (val/100)%10 + '0';  //小数位字符
	
	str[3] = (val/10)%10 +'0';
	
	str[4] = (val%10) + '0';

    str[5] = 'V';             //电压单位

    str[6] = '\0';            //结束符
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
    unsigned char code cmdLen[] = {          //命令长度汇总表
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {         //命令指针汇总表
        &cmd0[0],  &cmd1[0],  &cmd2[0],
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
			Led1();
            break;
        case 1:
            //flagBuzzOn = 0; //关闭蜂鸣器
			Led2();
            break;
        case 2:
           // buf[len] = '\0';  //为接收到的字符串添加结束符
           // LcdShowStr(0, 0, buf+cmdLen[2]);  //显示命令后的字符串
           // i = len - cmdLen[2];              //计算有效字符个数
           // if (i < 16)  //有效字符少于16时，清除液晶上的后续字符位
           // {
           //     LcdAreaClear(i, 0, 16-i);
           // }
		   Led3();
            break;
        default:   //未找到相符命令时，给上机发送“错误命令”的提示
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r';  //有效命令被执行后，在原命令帧之后添加
    buf[len++] = '\n';  //回车换行符后返回给上位机，表示已执行
    UartWrite(buf, len);
}

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    if  (keycode == 0x31)   //输入字符
    {
	    
		LcdShowStr(5, 0,"1");
        Led1();
    }
	 else if (keycode == 0x32)  //向上键，+
    {
	    LcdShowStr(5, 0,"2");
        Led2();
    }
	 else if (keycode == 0x33)  //向上键，+
    {
	   	LcdShowStr(5, 0,"3");
        Led3();
    }
	else if (keycode == 0x34)  //向上键，+
    {
	   	LcdShowStr(5, 0,"4");
        Led4();
    }
	else if (keycode == 0x35)  //向上键，+
    {
	   	LcdShowStr(5, 0,"5");
        Led5();
    }
	else if (keycode == 0x36)  //向上键，+
    {
	   	LcdShowStr(5, 0,"6");
        Led6();
    }
	/*else if (keycode == 0x37)  //向上键，+
    {
	   	LcdShowStr(5, 0,"7");
        Led7();
    }
	else if (keycode == 0x38)  //向上键，+
    {
	   	LcdShowStr(5, 0,"8");
        Led8();
    }
		else if (keycode == 0x39)  //向上键，+
    {
	   	LcdShowStr(5, 0,"9");
        Led9();
    } */
    else if (keycode == 0x26)  //向上键，+
    {
        //OprtKeyAction(0);
    }
    else if (keycode == 0x28)  //向下键，-
    {
       // OprtKeyAction(1);
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
        //GetResult();
    }
    else if (keycode == 0x1B)  //Esc键，清除
    {
        P0=0xFF;
    }
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

/* 串口配置函数，baud-通信波特率 */
/*void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    ES  = 1;       //使能串口中断
    TR1 = 1;       //启动T1
}  */

/* T0中断服务函数，执行300ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	KeyScan();   //按键扫描
	UartRxMonitor(1);  //串口接收监控
    tmr300ms++;
    if (tmr300ms >= 30)  //定时300ms
    {
        tmr300ms = 0;
        flag300ms = 1;
    }
}

/*void InterruptUART() interrupt 4
{
   	 //unsigned char i;
	 SBUF = ' ' ;//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = ' ' ;//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = str[0];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
   	 SBUF =str[1];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = str[2];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = str[3];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = str[4];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = str[5];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = ' ';//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;		 
	 //for(i=0; i<6; i++)
	 //{
	 //	SBUF = CNCHAR[i];//将接收到的数据放入到发送寄存器
	//	while (!TI);			         //等待发送数据完成
	//	TI = 0;
	// } 
    //SBUF ='0';  //接收到的数据又直接发回，叫作-"echo" 
	//SBUF = str;//将接收到的数据放入到发送寄存器                 
    //if (TI)  //字节发送完毕
    //{
       // TI = 0;  //手动清零发送中断标志位
    //}
} */