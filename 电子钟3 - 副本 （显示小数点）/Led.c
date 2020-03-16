#include<at89x52.h>
#define  _LED_BUZZER_C
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};

struct sLedBuff ledBuff; //LED��ʾ��������Ĭ�ϳ�ֵȫ0�����ôﵽ�ϵ�ȫ����Ч��

/* LED��ʼ����������ʼ��IO�����ö�ʱ�� */
void InitLed()
{
    //��ʼ��IO��
    P0 = 0xFF;
    ENLED = 0;
    //����T2��Ϊ��̬ɨ�趨ʱ
   T2MOD = 0x00; //���� T2 Ϊģʽ 0
   T2CON=0x00;
   TH2 = 0xFC; //Ϊ T2 ����ֵ 0xFC67����ʱ 1ms
   TL2 = 0x67;
   RCAP2H=0xFC;
   RCAP2L=0x67;
   ET2 = 1; //ʹ�� T2 �ж�
   TR2 = 1; //���� T2
}

/* ���������ʾһλ���֣�index-�����λ����(���ҵ����Ӧ0��5)��
**     num-����ʾ�����֣�point-�����Ƿ���ʾ��λ�ϵ�С���� */
void ShowLedNumber(unsigned int index, unsigned int num, unsigned int point)
{
    ledBuff.number[index] = LedChar[num];  //��������ת��Ϊ������ַ�0��F
    if (point != 0)
    {
        ledBuff.number[index] &= 0x7F;  //point��Ϊ0ʱ������ǰλ��С����
    }
}

/* T2�жϷ�������LED��̬ɨ�衢���������� */
void InterruptTimer2() interrupt 5
{
    static i = 0;  //LEDλѡ����
    
    TF2 = 0;  //����T2�жϱ�־
    //ȫ��LED��̬ɨ����ʾ
    if (ENLED == 0)  //LEDʹ��ʱ�Ž��ж�̬ɨ��
    {
        P0 = 0xFF;                       //�ر����ж�ѡλ����ʾ����
        P1 = (P1 & 0xF0) | i;            //λѡ����ֵ��ֵ��P1�ڵ�4λ
        P0 = *((data*)&ledBuff+i); //������������λ�õ������͵�P0��
        if (i < (sizeof(ledBuff)-1))     //��������ѭ������������������
            i++;
        else
            i = 0;
    }
}
