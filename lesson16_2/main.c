/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������16�� �¶ȴ�����DS18B20ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������16��16.4��
*         ����DS18B20�����¶Ȳ����¶�ֵ��ʾ��Һ����
*******************************************************************************
*/

#include <reg52.h>

bit flag1s = 0;          //1s��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

#define IR_CODE_0     20                  //�������0  
#define IR_CODE_1     50  
#define IR_CODE_START 240                 //��������ʼ  
#define IR_CODE_SIZI  42                  //��������� 

void ConfigTimer0(unsigned int ms);
unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void DataConversion(unsigned int num,unsigned char* irdata)  
{   unsigned char i=IR_CODE_SIZI-3,n=1,f=1;  
    IrcodeReset();                        //�洢����ʼ��  
    for(n=3;n<18;n+=2)                     //ǰ8�����У����10101010 0xAA   
    {   if(f)  
            {     
            irdata[n] = IR_CODE_1;  
            f=0;  
            }         
        else  
            {   irdata[n] = IR_CODE_0;f=1;}       
    }  
    while(num!=0)                                 //�������� ʮ����ת������  
    {  
        if(num%2)   {  irdata[i] = IR_CODE_1; }                             
          
        else        {  irdata[i] = IR_CODE_0; }      
      
        num = num/2;  
        i-=2;  
    }     
}  

/**************���Ⲩ���亯��**************  
***������ ��SendIrWave()  
***�� ��  ���������������ݵ��׵�ַirdata  
***�� ��  ���Ժ���ķ�ʽ����irdata �ĺ��Ⲩ������  
***����ֵ ����  
*****************************************/  
void SendIrWave(unsigned char* irdata)  
{   unsigned char count=0;  
    unsigned char i=0;  
  
    while(1)                                 //���������� У���� ������   
    {  
        ON_CARR;  
        for(i=irdata[count]; i>0; i--)        //�ز�������ʱ  
            {     
            CarrierWave_DELAY;    
            }                                 
                OFF_CARR;  
                IR_SEND_OFF;                         //�͵�ƽ����      
                count++;  
                                
        for(i=irdata[count]; i>0; i--)        //�͵�ƽ��ʱ  
            {     
            Low_DELAY;    
  
            }  
  
        if(count == IR_CODE_SIZI-1)          //�������  
            {   break;       }    
        count++;              
    }  
}  

void InitIrReceive(void)  
{  
/*------------------------------------------------  
                ��ʱ��0��ʼ��  
------------------------------------------------*/  
    TMOD|= 0x02;//��ʱ��0������ʽ2��TH0����װֵ��TL0�ǳ�ֵ  
    //8λ�Զ���װ  
    TH0  = 0x00;//����ֵ  
    TL0  = 0x00;//��ʼ��ֵ    
    ET0  = 1;   //���ж�  
    TR0  = 1;   //��ʱ��/����������  
/*------------------------------------------------  
                  �ⲿ�ж�0��ʼ��  
------------------------------------------------*/  
    IT0  = 1;   //ָ���ⲿ�ж�0�½��ش�����INT0 (P3.2)  
    EX0  = 1;   //ʹ���ⲿ�ж�  
    EA   = 1;  
}
void IRTimeCount() interrupt 1  
   {  Ir_Time_count++; }  
/*************����������жϺ���***********  
***�������� IRReceive(void)  
***�� ����  ��  
***�������������պ����룬����IR_CODE  
***����ֵ�� ��  
*****************************************/  
void IRReceive() interrupt 0  
{  
    static startflag=0,bitnum=0;  
    if(startflag)  
    {  if(Ir_Time_count<IR_START_TIEM_MAX && Ir_Time_count>IR_START_TIEM_MIN)   
       //�������ж�  
         {bitnum=0;  }  
       IR_CODE[bitnum]=Ir_Time_count;  
       //������ TC9012��ͷ�룬9ms+4.5ms  
       Ir_Time_count=0;    bitnum++;  
                
       if(bitnum==IR_CODE_SIZI)        //�������  
         {IR_RECEIVE_OK=1;  bitnum=0;}             
    }    
  
    else                               //��һ�������ź�  
    {Ir_Time_count=0;   startflag=1;    }         
} 

/************����������ת������***********  
***�������� DataConversion()  
***�� ����  ��  
***�������������պ����룬����IR_CODE  
***����ֵ�� ������������  
*****************************************/  
unsigned int DataConversion(void)  
{  
      unsigned char i=9;  
      unsigned int num=0;  
      for(i=9;i<IR_CODE_SIZI-1;i++)  
      {  
        if(IR_CODE[i]>IR_CODE_1)  
        {  
            num+=pow(2,IR_CODE_SIZI-2-i);//������תʮ����  
        }  
      }  
      return num;  
}  
/*************����������У��***********  
***�������� CheckSucceed()  
***�� ����  ��  
***����������ת��IR_CODE 1����8��У���룬�������趨ֵ���бȽ��ж�  
***����ֵ�� �ɹ�1 ʧ��0  
*****************************************/  
unsigned char CheckSucceed(void)  
{  
    unsigned char i=1;  
    unsigned int num=0;  
    for(i=1;i<9;i++)                  //У����λ��1����8  
        {     
            if(IR_CODE[i]>IR_CODE_1)    
            {     
             num+=pow(2,8-i); //ת��        
            }      
        }             
  
    if(num==CHECK_NUM && IR_CODE[0]>IR_START_TIEM_MIN &&  IR_CODE[0]<IR_START_TIEM_MAX)  
    //У������������У��  
     {    
      return 1;   
     }        
    else  
    {     
     return 0;   
    }     
} 

void main()
{
    bit res;
    int temp;        //��ȡ���ĵ�ǰ�¶�ֵ
    int intT, decT;  //�¶�ֵ��������С������
    unsigned char len;
    unsigned char str[12];

    EA = 1;            //�����ж�
    ConfigTimer0(10);  //T0��ʱ10ms
    Start18B20();      //����DS18B20
    InitLcd1602();     //��ʼ��Һ��
    
    while (1)
    {
        if (flag1s)  //ÿ�����һ���¶�
        {
            flag1s = 0;
            res = Get18B20Temp(&temp);  //��ȡ��ǰ�¶�
            if (res)                    //��ȡ�ɹ�ʱ��ˢ�µ�ǰ�¶���ʾ
            {
                intT = temp >> 4;             //������¶�ֵ��������
                decT = temp & 0xF;            //������¶�ֵС������
                len = IntToString(str, intT); //��������ת��Ϊ�ַ���
                str[len++] = '.';             //���С����
                decT = (decT*10) / 16;        //�����Ƶ�С������ת��Ϊ1λʮ����λ
                str[len++] = decT + '0';      //ʮ����С��λ��ת��ΪASCII�ַ�
                while (len < 6)               //�ÿո��뵽6���ַ�����
                {
                    str[len++] = ' ';
                }
                str[len] = '\0';              //����ַ���������
                LcdShowStr(0, 0, str);        //��ʾ��Һ������
            }
            else                        //��ȡʧ��ʱ����ʾ������Ϣ
            {
                LcdShowStr(0, 0, "error!");
            }
            Start18B20();               //����������һ��ת��
        }
    }
}
/* ������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //���Ϊ����������ȡ����ֵ������ָ������Ӹ���
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ����������1�붨ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr1s = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr1s++;
    if (tmr1s >= 100)  //��ʱ1s
    {
        tmr1s = 0;
        flag1s = 1;
    }
}
