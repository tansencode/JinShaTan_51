#include<at89x52.h>

sbit KEY_IN_1 = P2^4;	   //定义按键IO口地址
sbit KEY_IN_2 = P2^5;
sbit KEY_IN_3 = P2^6;
sbit KEY_IN_4 = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

sbit MC0 = P3^0;
sbit MC1 = P3^1;
sbit MC2 = P3^2;
sbit MC3 = P3^3;

unsigned char code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
{ 0x31, 0x32, 0x33, 0x26 }, //数字键 1、数字键 2、数字键 3、向上键
{ 0x34, 0x35, 0x36, 0x25 }, //数字键 4、数字键 5、数字键 6、向左键
{ 0x37, 0x38, 0x39, 0x28 }, //数字键 7、数字键 8、数字键 9、向下键
{ 0x30, 0x1B, 0x0D, 0x27 } //数字键 0、ESC 键、 回车键、 向右键
};

unsigned char code LedChar[] = { //数码管显示字符转换表
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //数码管显示缓冲区
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

unsigned char KeySta[4][4] = { //全部矩阵按键的当前状态
{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};

signed long beats = 0; //电机转动节拍总数
void KeyDriver();

void main()
{
EA = 1; //使能总中断,打开总开关
ENLED = 0; //选择数码管进行显示
ADDR3 = 1;
T2MOD = 0x00; //设置 T2 为模式 0
T2CON=0x00;
TH2 = 0xFC; //为 T2 赋初值 0xFC67，定时 1ms
TL2 = 0x67;
RCAP2H=0xFC;
RCAP2L=0x67;
ET2 = 1; //使能 T2 中断
TR2 = 1; //启动 T2
LedBuff[0] = LedChar[0]; //上电显示 0
while (1)
{
KeyDriver(); //调用按键驱动函数
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

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
static bit dirMotor = 0; //电机转动方向
static signed long num=0;		  //保存输入的角度值
if ((keycode>=0x30) && (keycode<=0x39)) //控制电机转动
{
ENLED = 0; //选择数码管进行显示
ADDR3 = 1;
num = (num*10)+(keycode-0x30); //整体十进制左移，新数字进入个位
ShowNumber(num); //运算结果显示到数码管
}
else if(keycode==0x0D)	   //按回车键开始转动
{
if (dirMotor == 0)	  //正转
{
ShowNumber(num); //运算结果显示到数码管
StartMotor(num);

num=0;

}
else
{
ShowNumber(num); //运算结果显示到数码管
StartMotor(-1*(num));	  //反转

num=0;

}
}
else if (keycode == 0x26) //向上键，控制转动方向为正转
{
dirMotor = 0;
}
else if (keycode == 0x28) //向下键，控制转动方向为反转
{
dirMotor = 1;
}
else if (keycode == 0x25) //向左键，固定正转 360 度
{
StartMotor(360);
}
else if (keycode == 0x27) //向右键，固定反转 360 度
{
StartMotor(-360);
}
else if (keycode == 0x1B) //Esc 键，停止转动
{
num=0;
ShowNumber(num); //清零后的加数显示到数码管
StopMotor();
}
}


/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
unsigned char i, j;
static unsigned char backup[4][4] = { //按键值备份，保存前一次的值
{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};
for (i=0; i<4; i++) //循环检测 4*4 的矩阵按键
{
for (j=0; j<4; j++)
{
if (backup[i][j] != KeySta[i][j]) //检测按键动作
{
if (backup[i][j] != 0) //按键按下时执行动作
{
KeyAction(KeyCodeMap[i][j]); //调用按键动作函数
}
backup[i][j] = KeySta[i][j]; //刷新前一次的备份值
}
}
}
}

/* 按键扫描函数，需在定时中断中调用，推荐调用间隔 1ms */
void KeyScan()
{
unsigned char i;
static unsigned char keyout = 0; //矩阵按键扫描输出索引
static unsigned char keybuf[4][4] = { //矩阵按键扫描缓冲区
{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}
};
//将一行的 4 个按键值移入缓冲区
keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
//消抖后更新按键状态
for (i=0; i<4; i++) //每行 4 个按键，所以循环 4 次
{
if ((keybuf[keyout][i] & 0x0F) == 0x00)
{ //连续 4 次扫描值为 0，即 4*4ms 内都是按下状态时，可认为按键已稳定的按下
KeySta[keyout][i] = 0;
}
else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
{ //连续 4 次扫描值为 1，即 4*4ms 内都是弹起状态时，可认为按键已稳定的弹起
KeySta[keyout][i] = 1;
}
}
//执行下一次的扫描输出
keyout++; //输出索引递增
keyout = keyout & 0x03; //索引值加到 4 即归零
switch (keyout) //根据索引，释放当前输出引脚，拉低下次的输出引脚
{
case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
default: break;
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

/* T2 中断服务函数，用于按键扫描与电机转动控制 */
void InterruptTimer2() interrupt 5
{
static bit div = 0;
TF2=0;				//中断标志位清零，T2自动加载初值
LedScan(); //调用数码管显示扫描函数
KeyScan(); //执行按键扫描
//用一个静态 bit 变量实现二分频，即 2ms 定时，用于控制电机
div = ~div;
if (div == 1)
{
TurnMotor();
}
}