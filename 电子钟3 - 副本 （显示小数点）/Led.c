#include<at89x52.h>
#define  _LED_BUZZER_C
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};

struct sLedBuff ledBuff; //LED显示缓冲区，默认初值全0，正好达到上电全亮的效果

/* LED初始化函数，初始化IO、配置定时器 */
void InitLed()
{
    //初始化IO口
    P0 = 0xFF;
    ENLED = 0;
    //配置T2作为动态扫描定时
   T2MOD = 0x00; //设置 T2 为模式 0
   T2CON=0x00;
   TH2 = 0xFC; //为 T2 赋初值 0xFC67，定时 1ms
   TL2 = 0x67;
   RCAP2H=0xFC;
   RCAP2L=0x67;
   ET2 = 1; //使能 T2 中断
   TR2 = 1; //启动 T2
}

/* 数码管上显示一位数字，index-数码管位索引(从右到左对应0～5)，
**     num-待显示的数字，point-代表是否显示此位上的小数点 */
void ShowLedNumber(unsigned int index, unsigned int num, unsigned int point)
{
    ledBuff.number[index] = LedChar[num];  //输入数字转换为数码管字符0～F
    if (point != 0)
    {
        ledBuff.number[index] &= 0x7F;  //point不为0时点亮当前位的小数点
    }
}

/* T2中断服务函数，LED动态扫描、蜂鸣器控制 */
void InterruptTimer2() interrupt 5
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
}
