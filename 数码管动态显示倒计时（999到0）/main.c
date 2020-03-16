#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = { 
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};		//�������ʾ�ַ�ת����,����

unsigned char LedBuff[6] = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};		 //�������ʾ����������ֵ 0xFF ȷ������ʱ������

unsigned char i = 0; //��̬ɨ�������
unsigned int cnt = 0; //��¼ T0 �жϴ���
unsigned char flag1s = 0; //1 �붨ʱ��־

void main()
{
unsigned long sec = 999; //��¼��������������999��ʼ����ʱ
EA = 1; //ʹ�����ж�
ENLED = 0; //ʹ�� U3��ѡ����������
ADDR3 = 1; //��Ϊ��Ҫ��̬�ı� ADDR0-2 ��ֵ�����Բ���Ҫ�ٳ�ʼ����

TMOD = 0x01; //���� T0 Ϊģʽ 1
TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
TL0 = 0x67;
ET0 = 1; //ʹ�� T0 �ж�
TR0 = 1; //���� T0

while (1)
{
if (flag1s == 1) //�ж� 1 �붨ʱ��־
{
//���´��뽫 sec ��ʮ����λ�ӵ͵���������ȡ��תΪ�������ʾ�ַ�
LedBuff[0] = LedChar[sec%10];				//��λ

LedBuff[2] = LedChar[sec/10%10];			//ʮλ

LedBuff[4] = LedChar[sec/100%10];			//��λ

flag1s = 0; //1 �붨ʱ��־����
sec--; //������Լ�1����ʾ999���ٿ�ʼ��
}
}
}

/* ��ʱ�� 0 �жϷ����� */
void InterruptTimer0() interrupt 1	 //����������
{
TH0 = 0xFC; //���¼��س�ֵ
TL0 = 0x67;
cnt++; //�жϴ�������ֵ�� 1
if (cnt >= 1000) //�ж� 1000 �μ� 1 ��
{
cnt = 0; //�������ֵ�����¿�ʼ�� 1 ���ʱ
flag1s = 1; //���� 1 �붨ʱ��־Ϊ 1
}
//���´����������ܶ�̬ɨ��ˢ��
P0 = 0xFF; //��ʾ����
switch (i)		  //i++���úܺã��������i=0
{
case 0: ADDR2=0; ADDR1=0; ADDR0=0; i=i+2; P0=LedBuff[0]; break;	   //��λ

case 2: ADDR2=0; ADDR1=1; ADDR0=0; i=i+2; P0=LedBuff[2]; break;		 //ʮλ

case 4: ADDR2=1; ADDR1=0; ADDR0=0; i=0; P0=LedBuff[4]; break;		  //��λ

default: break;
}
}