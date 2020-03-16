#include <reg52.h>

sbit IR_INPUT = P3^3;  //红外接收引脚

bit irflag = 0;  //红外接收标志，收到一帧正确数据后置1
unsigned char ircode[16];  //红外代码接收缓冲区

/* 初始化红外接收功能 */
void InitInfrared()
{
    IR_INPUT = 1;  //确保红外接收引脚被释放
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x10;  //配置T1为模式1
    TR1 = 0;       //停止T1计数
    ET1 = 0;       //禁止T1中断
    IT1 = 1;       //设置INT1为负边沿触发
    EX1 = 1;       //使能INT1中断
}
/* 获取当前高电平的持续时间 */
unsigned int GetHighTime()
{
    TH1 = 0;  //清零T1计数初值
    TL1 = 0;
    TR1 = 1;  //启动T1计数
    while (IR_INPUT)  //红外输入引脚为1时循环检测等待，变为0时则结束本循环
    {
        if (TH1 >= 0x40)
        {            //当T1计数值大于0x4000，即高电平持续时间超过约18ms时，
            break;   //强制退出循环，是为了避免信号异常时，程序假死在这里。
        }
    }
    TR1 = 0;  //停止T1计数

    return (TH1*256 + TL1);  //T1计数值合成为16bit整型数，并返回该数
}
/* 获取当前低电平的持续时间 */
unsigned int GetLowTime()
{
    TH1 = 0;  //清零T1计数初值
    TL1 = 0;
    TR1 = 1;  //启动T1计数
    while (!IR_INPUT)  //红外输入引脚为0时循环检测等待，变为1时则结束本循环
    {
        if (TH1 >= 0x40)
        {            //当T1计数值大于0x4000，即低电平持续时间超过约18ms时，
            break;   //强制退出循环，是为了避免信号异常时，程序假死在这里。
        }
    }
    TR1 = 0;  //停止T1计数

    return (TH1*256 + TL1);  //T1计数值合成为16bit整型数，并返回该数
}
/* INT1中断服务函数，执行红外接收及解码 */
void EXINT1_ISR() interrupt 2
{
    unsigned char i, j;
    unsigned char byt;
    unsigned int time;
    
    //接收并判定引导码的9ms低电平
    time = GetLowTime();
    if ((time<7833) || (time>8755))  //时间判定范围为8.5～9.5ms，
    {                                //超过此范围则说明为误码，直接退出
        IE1 = 0;   //退出前清零INT1中断标志
        return;
    }
    //接收并判定引导码的4.5ms高电平
    time = GetHighTime();
    if ((time<3686) || (time>4608))  //时间判定范围为4.0～5.0ms，
    {                                //超过此范围则说明为误码，直接退出
        IE1 = 0;
        return;
    }
    //接收并判定后续的4字节数据
    for (i=0; i<4; i++)  //循环接收4个字节
    {
        for (j=0; j<8; j++)  //循环接收判定每字节的8个bit
        {
            //接收判定每bit的560us低电平
            time = GetLowTime();
            if ((time<313) || (time>718)) //时间判定范围为340～780us，
            {                             //超过此范围则说明为误码，直接退出
                IE1 = 0;
                return;
            }
            //接收每bit高电平时间，判定该bit的值
            time = GetHighTime();
            if ((time>313) && (time<718)) //时间判定范围为340～780us，
            {                             //在此范围内说明该bit值为0
                byt >>= 1;   //因低位在先，所以数据右移，高位为0
            }
            else if ((time>1345) && (time<1751)) //时间判定范围为1460～1900us，
            {                                    //在此范围内说明该bit值为1
                byt >>= 1;   //因低位在先，所以数据右移，
                byt |= 0x80; //高位置1
            }
            else  //不在上述范围内则说明为误码，直接退出
            {
                IE1 = 0;
                return;
            }
        }
        ircode[i] = byt;  //接收完一个字节后保存到缓冲区
    }
    irflag = 1;  //接收完毕后设置标志
    IE1 = 0;     //退出前清零INT1中断标志
}
