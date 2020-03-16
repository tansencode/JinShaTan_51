#include <reg52.h>

bit flag300ms = 1;       //300ms定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char RxdByte = 0;  //串口接收到的字节

void ConfigTimer0(unsigned int ms);
unsigned char GetADCValue(unsigned char chn);
void ValueToString(unsigned char *str, unsigned long val);
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
void UsartConfiguration();
unsigned char CNCHAR[10] = "V  ";
void ConfigUART(unsigned int baud);

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
            val=GetADCValue(0);  //获取ADC通道0的转换值
			LcdShowStr(0, 0, "50  TJ");  //显示通道指示
            ValueToString(str, val);  //转为字符串格式的电压值			 
			LcdShowStr(0, 1, "Voltage=");    //显示到液晶上
            LcdShowStr(8, 1, str);    //显示到液晶
			TI=1;
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

/* ADC转换值转为实际电压值的字符串形式，str-字符串指针，val-AD转换值 */
/*void ValueToString(unsigned char *str, unsigned long val)
{
    unsigned char i;
	//unsigned char str1[]= {0, 0, 0, 0, 0, 0}; 
    //电压值=转换结果*2.5V/255，式中的25隐含了一位十进制小数
    val = (val*2500) / 255;
    str[0] = (val/1000)%10 + '0';  //整数位字符
	//SBUF = str[0];//将接收到的数据放入到发送寄存器
	//while (!TI);			         //等待发送数据完成
	//TI = 0;

    str[1] = '.';             //小数点
	//SBUF = '.';//将接收到的数据放入到发送寄存器
//	while (!TI);			         //等待发送数据完成
//	TI = 0;

    str[2] = (val/100)%10 + '0';  //小数位字符
	//SBUF = str[2];//将接收到的数据放入到发送寄存器
	//while (!TI);			         //等待发送数据完成
	//TI = 0;
	str[3] = (val/10)%10 +'0';
	//SBUF = str[3];//将接收到的数据放入到发送寄存器
//	while (!TI);			         //等待发送数据完成
//	TI = 0;
	str[4] = (val%10) + '0';
	//SBUF = str[4];//将接收到的数据放入到发送寄存器
	//while (!TI);			         //等待发送数据完成
	//TI = 0;
    str[5] = 'V';             //电压单位
	//SBUF = str[5];//将接收到的数据放入到发送寄存器
	//while (!TI);			         //等待发送数据完成
	//TI = 0;
    str[6] = '\0';            //结束符
//	TI=1;
	
		 
	/*SBUF = str[0];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	SBUF = '.';//将接收到的数据放入到发送寄存器
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
	TI = 0;	 */
//	for(i=0; i<6; i++)
//	{
//	 	SBUF = CNCHAR[i];//将接收到的数据放入到发送寄存器
//		while (!TI);			         //等待发送数据完成
//		TI = 0;
//	} 

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
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    ES  = 1;       //使能串口中断
    TR1 = 1;       //启动T1
} 

/* T0中断服务函数，执行300ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    tmr300ms++;
    if (tmr300ms >= 30)  //定时300ms
    {
        tmr300ms = 0;
        flag300ms = 1;
    }
}

void InterruptUART() interrupt 4
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
}