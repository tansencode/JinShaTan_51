#include <reg52.h>

sbit ADDR0 = P1^0;	//定义led使能端及位选端
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

sbit Buzz=P1^6;	  //蜂鸣器控制引脚
bit flagBuzzon=0;
sbit KEY_IN_1  = P2^4;  //按键控制引脚
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;


bit flag300ms = 1;  //200ms定时标志

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;
unsigned char i=0;
unsigned int j=0;
static unsigned int cnt=0;
unsigned char T0RH=0;
unsigned char T0RL=0;
static unsigned char keyout = 0;   //矩阵按键扫描输出索引

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartRead(unsigned char *buf, unsigned char len);
extern void UartWrite(unsigned char *buf, unsigned char len);
unsigned char code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
    { 0x31, 0x32, 0x33, 0x26 }, //数字键1、数字键2、数字键3、向上键
    { 0x34, 0x35, 0x36, 0x25 }, //数字键4、数字键5、数字键6、向左键
    { 0x37, 0x38, 0x39, 0x28 }, //数字键7、数字键8、数字键9、向下键
    { 0x30, 0x1B, 0x0D, 0x27 }  //数字键0、ESC键、  回车键、 向右键
};
unsigned char KeySta[4][4] = {  //全部矩阵按键的当前状态
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

unsigned long pdata KeyDownTime[4][4] = {  //每个按键按下的持续时间，单位ms
    {0, 0, 0, 0},  {0, 0, 0, 0},  {0, 0, 0, 0},  {0, 0, 0, 0}
};
//unsigned char i, j;
void KeyDriver();

void main()
{
    unsigned char psec=0xAA;  
    EA = 1;       //使能总中断
	TMOD = 0x01; //设置 T0 为模式 1
    TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
    TL0 = 0x67;
    ET0 = 1; //使能 T0 中断
    TR0 = 1; //启动 T0
	ConfigTimer0(1); //配置 T0 定时 1ms
    ConfigUART(9600); //配置波特率为 9600
	InitDS1302();     //初始化实时时钟
    while (1)
    {
        KeyDriver();   //调用按键驱动函数
		//UartDriver(); //调用串口驱动
		UartWrite(KeyDownTime,10);
    }
}

void ledon()		//LED亮与灭控制函数
{
    EA = 1;       //使能总中断
    ENLED = 0;    
    ADDR3 = 1;
	ADDR2=1;
	ADDR1=1;
	ADDR0=0;
	switch(i)
	{
	     case 0:P0=0xfe;i++;break;
		 case 1:P0=0xff;i=0;break;
		 default:break;
	}

}


void delay(u16 j)
{
	while(j--);	
}

void buzz()	  //蜂鸣器启动和停止控制函数
{
    unsigned int cnt=0;
	switch(cnt)
	{
       case 0:
	   flagBuzzon=1;  //蜂鸣器开启
		 ++cnt;
		   break;
		case 1:
		  flagBuzzon=0;	   //蜂鸣器关闭
		   cnt=0;
		   break;
		 default:break;	       
	}
}

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    // unsigned int cnt=0;
     if (keycode == 0x31 )  //K1按键控制蜂鸣器启动和停止
    {  
	   buzz();
    }
    else if (keycode == 0x32 )  //K2按键控制led1亮和灭
    {
		ledon();
    }
}
/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
   // unsigned char i, j;
    static unsigned char pdata backup[4][4] = {  //按键值备份，保存前一次的值
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    static unsigned long pdata TimeThr[4][4] = {  //快速输入执行的时间阈值
        {1000, 1000, 1000, 1000},  {1000, 1000, 1000, 1000},
        {1000, 1000, 1000, 1000},  {1000, 1000, 1000, 1000}
    };
    
    for (i=0; i<4; i++)  //循环扫描4*4的矩阵按键
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])     //检测按键动作
            {
                if (backup[i][j] != 0)            //按键按下时执行动作
                {
                    KeyAction(KeyCodeMap[i][j]);  //调用按键动作函数
                }
                backup[i][j] = KeySta[i][j];      //刷新前一次的备份值
            }
            if (KeyDownTime[i][j] > 0)            //检测执行快速输入
            {
                if (KeyDownTime[i][j] >= TimeThr[i][j])
                {                                 //达到阈值时执行一次动作
                    KeyAction(KeyCodeMap[i][j]);  //调用按键动作函数
                    TimeThr[i][j] += 200; //时间阈值增加200ms，以准备下次执行
                }
            }
            else   //按键弹起时复位阈值时间
            {
                TimeThr[i][j] = 1000;  //恢复1s的初始阈值时间
            }
        }
    }
}
/* 按键扫描函数，需在定时中断中调用 */
void KeyScan()
{
    unsigned char i;
    static unsigned char keyout = 0;   //矩阵按键扫描输出索引
    static unsigned char keybuf[4][4] = {  //矩阵按键扫描缓冲区
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
    };

    //将一行的4个按键值移入缓冲区
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //消抖后更新按键状态
    for (i=0; i<4; i++)  //每行4个按键，所以循环4次
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   //连续4次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
            KeySta[keyout][i] = 0;
            KeyDownTime[keyout][i] += 4;  //按下的持续时间累加
			//UartWrite(KeyDownTime[keyout][i],10);
		//	SBUF =KeyDownTime[keyout][i] ;//将接收到的数据放入到发送寄存器
	      //  while (!TI);			         //等待发送数据完成
	     //   TI = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[keyout][i] = 1;
            KeyDownTime[keyout][i] = 0;   //按下的持续时间清零
        }
    }
    //执行下一次的扫描输出
    keyout++;        //输出索引递增
    keyout &= 0x03;  //索引值加到4即归零
    switch (keyout)  //根据索引，释放当前输出引脚，拉低下次的输出引脚
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}

/* 配置并启动 T0，ms-T0 定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp; //临时变量
    tmp = 11059200 / 12; //定时器计数频率
    tmp = (tmp * ms) / 1000; //计算所需的计数值
    tmp = 65536 - tmp; //计算定时器重载值
    tmp = tmp + 33; //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8); //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0; //清零 T0 的控制位
    TMOD |= 0x01; //配置 T0 为模式 1
    TH0 = T0RH; //加载 T0 重载值
    TL0 = T0RL;
    ET0 = 1; //使能 T0 中断
    TR0 = 1; //启动 T0
}


/* T0中断服务函数，用于数码管显示扫描与按键扫描 */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmrls=0;
    TH0 = 0xFC; 
    TL0 = 0x67;		
	tmrls++;
	if(	tmrls>=300)
	{
	   flag300ms=1;
	}

    KeyScan();   //调用按键扫描函数
	if(flagBuzzon)    //执行蜂鸣器鸣叫或关闭
    	Buzz=~Buzz;
	else
	    Buzz=1;
	UartRxMonitor(1);


}
/*void InterruptUART() interrupt 4
{
   	 unsigned char i;
	 SBUF = ' ' ;//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF = ' ' ;//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
	 SBUF =KeyDownTime[keyout][i];//将接收到的数据放入到发送寄存器
	 while (!TI);			         //等待发送数据完成
	 TI = 0;
}*/	 