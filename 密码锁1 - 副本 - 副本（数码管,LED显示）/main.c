/*
*******************************************************************************
* 1、用0～9的数字键输入密码，允许的密码最大位数由PSW_MAX_LEN宏定义给出
* 2、用户设定的密码由EEPROM保存，并且用“累加和”校验来保证数据的有效性
* 3、程序首次运行时EEPROM中的数据无效，此时会提示“设置密码”
* 4、设置密码时需要用户输入两次相同的密码，以保证设置成功，设置后密码自动保存
* 5、两次输入密码不相同时，将导致密码设置失败，按回车键重新设置
* 6、密码设置成功后，每次开机都会要求用户输入密码，用户键入密码后，按回车键，
     系统将键入的密码与EEPROM中保存的密码做对比，相同则系统解锁，否则继续锁定
* 7、密码输入错误系统锁定时，按回车键可以重新开始输入密码
* 8、密码输入正确系统解锁时，按回车键可以进入修改密码的操作
* 9、当忘记密码时，可通过修改PSW_MAX_LEN或PSW_SAVE_ADDR宏定义的值来重置密码
*******************************************************************************
*/

#include <reg52.h>

#define PSW_MAX_LEN    9    //用宏定义出密码最大长度（最大不超过16）
#define PSW_SAVE_ADDR  0x20  //用宏定义出密码在EEPROM中的保存地址

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

sbit MC0 = P3^0;
sbit MC1 = P3^1;
sbit MC2 = P3^2;
sbit MC3 = P3^3;

unsigned char code LedChar[] = { //数码管显示字符转换表
0x89, 0x89, 0x89, 0x89, 0x89,0x89,0x89, 0x89,
0x89, 0x89, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char LedBuff[6] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

signed long beats = 0; //电机转动节拍总数
unsigned char status;  //系统状态字：0-输入密码，1-设置密码，2-设置密码确认，
                       //3-密码设置失败，4-输入的密码正确，5-输入的密码错误
static unsigned long num=0; //用于保存输入的数
unsigned char PswCnt;  //当前输入密码位数的计数器
unsigned char pdata PswSet[PSW_MAX_LEN];   //已设定的密码数据
unsigned char pdata PswBuf1[PSW_MAX_LEN];  //密码输入缓冲区1
unsigned char pdata PswBuf2[PSW_MAX_LEN];  //密码输入缓冲区2

unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

static unsigned char j ; //动态扫描的索引
void ledon();
extern void KeyScan();
extern void KeyDriver();
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);

void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len);
void ConfigTimer0(unsigned int ms);
void ShowNumber(unsigned long num);
bit  ReadPassword();

void main()
{
    EA = 1;           //开总中断
	ENLED = 0; //选择数码管进行显示
    ADDR3 = 1;
		
	TMOD = 0x01; //设置 T0 为模式 1
    TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
    TL0 = 0x67;
    ET0 = 1; //使能 T0 中断
    TR0 = 1; //启动 T0
    ConfigTimer0(1);  //配置T0定时1ms
	LedBuff[0] = LedChar[10]; //上电无显示，等待输入密码
    InitLcd1602();    //初始化液晶
    if (ReadPassword())  //读取密码并判断是否有效
    {
	    j=2;
        status = 0;	
    }
    else
    {
	    j=0;
        status = 1;
    }
    FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
    FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
    PswCnt = 0;
    while (1)
    {
        KeyDriver();
    }
}

/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(signed long angle)
{
  //在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
  EA = 0;
  beats = (angle * 4076)/360 ; //实测为 4076 拍转动一圈
  EA = 1;
}
/* 步进电机停止函数 */
void StopMotor()
{
   EA = 0;
   beats = 0;
   EA = 1;
}


/* 将一段内存数据拷贝到另一处，dest-目的地址，src-源地址，len-数据长度 */
void CopyMemory(unsigned char *dest, unsigned char *src, unsigned char len)
{
    while (len--)
    {
        *dest++ = *src++;
    }
}
/* 用一个指定字节填充一段内存，dest-目的地址，byt-填充用字节，len-数据长度 */
void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len)
{
    while (len--)
    {
        *dest++ = byt;
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
/* 将一段数据按字节累加，返回获得的累加和，dat-数据指针，len-数据长度 */
unsigned char GetChkSum(unsigned char *dat, unsigned char len)
{
    unsigned char sum = 0;
    
    while (len--)
    {
        sum += *dat++;
    }
    return sum;
}
/* 从EEPROM中读取保存的密码数据，并对密码进行“累加和”校验，
   返回值-校验通过返回1，否则返回0 */
bit ReadPassword()
{
    unsigned char sum;
    unsigned char buf[PSW_MAX_LEN+1];
    
    E2Read(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //读取密码数据和校验字节
    sum = GetChkSum(buf, PSW_MAX_LEN); //计算密码数据的累加和
    if (buf[PSW_MAX_LEN] == ~sum)      //校验通过，即密码有效时，拷贝密码
    {
        CopyMemory(PswSet, buf, PSW_MAX_LEN);
        return 1;
    }
    else  //密码数据无效时，缓冲区默认成全0
    {
        FillMemory(PswSet, '\0', PSW_MAX_LEN);
        return 0;
    }
}
/* 将当前的密码数据保存到EEPROM中，同时添加校验字节 */
void SavePassword()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    CopyMemory(buf, PswSet, PSW_MAX_LEN);  //复制密码数据到缓冲区中
    buf[PSW_MAX_LEN] = ~GetChkSum(buf, PSW_MAX_LEN);  //填充最后的累加和校验字节
    E2Write(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //将密码数据保存到EEPROM中
}
/* 在液晶上显示与当前输入密码位数相同的*，以指示当前输入的密码位数 */
void ShowPswCnt()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    FillMemory(buf, '*', PswCnt);
    FillMemory(buf+PswCnt, '\0', sizeof(buf)-PswCnt);
  //  LcdShowStr(0, 1, buf);
}
/* 数字键动作函数，keycode-数字按键的ASCII值 */
void KeyNumAction(unsigned char keycode)
{
    switch (status)
    {
        case 0: //输入密码状态时，将输入字符填充到PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 1: //设置密码状态时，将输入字符填充到PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 2: //设置密码确认状态时，将输入字符填充到PswBuf2
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf2[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        default: //其它状态不予响应
            break;
    }
}

/* 将一个无符号长整型的数字显示到数码管上，num-待显示数字 */
void ShowNumber(unsigned long num)
{
    signed char i;
    unsigned char buf[6];
    for (i=0; i<6; i++) //把长整型数转换为 6 位十进制的数组
    {
       buf[i] = num % 10;
       num = num / 10;
    }
    for (i=5; i>=1; i--) //从最高位起，遇到 0 转换为空格，遇到非 0 则退出循环
    {
      if (buf[i] == 0)
      LedBuff[i] = 0xFF;
      else
      break;
     }
     for ( ; i>=0; i--) //剩余低位都如实转换为数码管显示字符
     {
        LedBuff[i] = LedChar[buf[i]];
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

void LedScan1()	 //led提示设置密码
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
	case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
	case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0x7F; break;
	default: break;
  }
}

void LedScan2()			 //led提示再次输入验证密码
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
		 case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
		 case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xBF; break;
		 default: break;
     }
}
void LedScan3()	  //led提示输入密码
{
     static unsigned char i = 0; //动态扫描的索引
     static unsigned int n; //动态扫描的索引
     P0 = 0xFF; //显示消隐
     switch (i)
     {
         case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
         case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
         case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
         case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
         case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
         case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
         case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xCF; break;
         default: break;
     }
}

void LedScan4()	  //密码输入正确提示
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
       case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
       case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xDF; break;
       default: break;
    }
}
void LedScan5()					 //密码输入错误提示
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
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xEF; break;
        default: break;
    }
}

void ShowNumber1(unsigned long num)	//数码管无显示等待输入密码
{
    signed char i;
    unsigned char buf[6];
    for (i=0; i<6; i++) //把长整型数转换为 6 位十进制的数组
    {
       buf[i] = num % 10;
       num = num / 10;
    }
    for (i=5; i>=1; i--) //从最高位起，遇到 0 转换为空格，遇到非 0 则退出循环
    {
       if (buf[i] == 0)
       LedBuff[i] = 0xFF;
       else
       LedBuff[i] = 0xFF;
	}
    for ( ; i>=0; i--) //剩余低位都如实转换为数码管显示字符
    {
        LedBuff[i] =0xFF; 
     } 
}
/* 回车键动作函数 */
void KeyEnterAction()
{
    
    switch (status)
    {
        case 0: //检查输入的密码是否正确
            LcdClearScreen();
            if (CmpMemory(PswSet, PswBuf1, PSW_MAX_LEN))
            {   //正确时显示解锁的提示
				StartMotor(90);
				j=3;
                status = 4;  //进入解锁状态
            }
            else
            {   //错误时显示已锁定的提示
				StartMotor(-90);
				j=4;
                status = 5;  //进入锁定状态
            }
            break;
        case 1: //进入密码设置的确认步骤，即再输入一次密码
		    num=0;
		    ShowNumber1(num);
		    j=1;
            PswCnt = 0;
            status = 2;
            break;
        case 2: //检查两次输入的密码是否一致
            if (CmpMemory(PswBuf1, PswBuf2, PSW_MAX_LEN))
            {   //一致时保存设置的密码
				num=0;
		        ShowNumber1(num);
                CopyMemory(PswSet, PswBuf1, PSW_MAX_LEN);
                SavePassword();
                FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
				j=2;
                PswCnt = 0;
                status = 0;  //进入密码输入状态
            }
            else
            {   //不一致时提示密码设置失败
               	num=0;
		        ShowNumber1(num);
			    j=1;
                status = 3;  //进入密码设置失败状态
            }
            break;
        case 3: //重新提示设置密码，进入密码设置状态
		    num=0;
		    ShowNumber1(num);
		    j=0;
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        case 4: //重新设置即修改密码
		    num=0;
		    ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
			j=0;
            PswCnt = 0;
            status = 1;
            break;
        case 5: //重新输入密码
		   num=0;
		   ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
			j=2;
            PswCnt = 0;
            status = 0;
            break;
        default: //不予响应
            break;
    }
}
/* Esc键动作函数 */
void KeyEscAction()
{
     static unsigned char i = 0; //动态扫描的索引
    switch (status)
    {
        case 0: //输入密码状态时，清空当前输入以重新开始
		    num=0;
			ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
			j=2;
            PswCnt = 0;
            break;
        case 1: //设置密码状态时，清空当前输入以重新开始
           	num=0;
			ShowNumber1(num);
		    j=0;
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            break;
        case 2: //设置密码确认状态时，清空输入并返回密码设置
		    num=0;
			ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
			j=0;
            PswCnt = 0;
            status = 1;
            break;
        default: //其它状态不予响应
            break;
    }
}

/* 电机转动控制函数 */
void TurnMotor()
{
unsigned char tmp; //临时变量
static unsigned char index = 0; //节拍输出索引
unsigned char code BeatCode[8] = { //步进电机节拍对应的 IO 控制代码
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
if (beats != 0) //节拍数不为 0 则产生一个驱动节拍
{
if (beats > 0) //节拍数大于 0 时正转
{
index++; //正转时节拍输出索引递增
index = index & 0x07; //用&操作实现到 8 归零
beats--; //正转时节拍计数递减
}
else //节拍数小于 0 时反转
{
index--; //反转时节拍输出索引递减
index = index & 0x07; //用&操作同样可以实现到-1 时归 7
beats++; //反转时节拍计数递增
}
tmp = P3; //用 tmp 把 P1 口当前值暂存
tmp = tmp & 0xF0; //用&操作清零低 4 位
tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低 4 位
P3= tmp; //把低 4 位的节拍代码和高 4 位的原值送回 P1
}
else //节拍数为 0 则关闭电机所有的相
{
P3 = P3 | 0x0F;
}
}


/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    
	static unsigned int i=0;
    if  ((keycode>='0') && (keycode<='9'))  //输入字符
    {
        KeyNumAction(keycode);
		num = (num*10)+(keycode-0x30); //整体十进制左移，新数字进入个位
		ShowNumber(num); //运算结果显示到数码管
    }
    else if (keycode == 0x0D)  //回车键
    {
	    
        KeyEnterAction();
    }
    else if (keycode == 0x1B)  //Esc键
    {
        KeyEscAction();
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
/* T0中断服务函数，执行按键扫描 */
void InterruptTimer0() interrupt 1
{
    static bit div=0;
	static int n=90000;
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
	LedScan(); //调用数码管显示扫描函数
    KeyScan();   //按键扫描
	switch(j)
	{
	   case 0:
       
	   LedScan1();
	break;
      case 1:
    
	   LedScan2();
		break;
       case 2:
	  LedScan3();
		break;
		case 3:
	   LedScan4();
		break;
	case 4:
	   LedScan5();
		break;
		default:break;
		}
	div = ~div;
   if (div == 1)
   {
      TurnMotor();
   }
}
