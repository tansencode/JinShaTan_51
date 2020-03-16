#include <reg52.h>

sbit LED=P0^0;
sbit LED1=P0^1;
sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;


sbit BUZZ = P1^6; //��������������
bit flagBuzzOn = 0; //������������־

unsigned char T0RH = 0; //T0 ����ֵ�ĸ��ֽ�
unsigned char T0RL = 0; //T0 ����ֵ�ĵ��ֽ�
void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main()
{
EA = 1; //�����ж�
ConfigTimer0(1); //���� T0 ��ʱ 1ms
ConfigUART(9600); //���ò�����Ϊ 9600
while (1)
{
UartDriver(); //���ô�������
}
}

/* �ڴ�ȽϺ������Ƚ�����ָ����ָ����ڴ������Ƿ���ͬ��
ptr1-���Ƚ�ָ�� 1��ptr2-���Ƚ�ָ�� 2��len-���Ƚϳ���
����ֵ-�����ڴ�������ȫ��ͬʱ���� 1����ͬ���� 0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
while (len--)
{
if (*ptr1++ != *ptr2++) //�������������ʱ���̷��� 0
{
return 0;
}
}
return 1; //�Ƚ���ȫ���������ݶ�����򷵻� 1
}
void ledshanshuo()			//LED2��˸��LED1��
{
   unsigned int i=0;
   ENLED=0;
   ADDR3=1;

   ADDR2=1;
   ADDR1=1;
   ADDR0=0;
   while(1)
   {
    LED=0;
     LED1=0;
	 for(i=0;i<3000;i++);
	 LED1=1;
	 for(i=0;i<3000;i++);
    }
}

void led()
{
unsigned int i=0;
   ENLED=0;
   ADDR3=1;

   ADDR2=1;
   ADDR1=1;
   ADDR0=0;
   LED=0;
   while(1);
}

/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
unsigned char i;
unsigned char code cmd0[] = "1"; //led��˸��led��
unsigned char code cmd1[] = "2"; //�򿪷���������
unsigned char code cmd2[] = "3"; //�رշ���������
unsigned char code cmdLen[] = { //����Ȼ��ܱ�
sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
};
unsigned char code *cmdPtr[] = { //����ָ����ܱ�
&cmd0[0], &cmd1[0], &cmd2[0],
};
for (i=0; i<sizeof(cmdLen); i++) //���������б�������ͬ����
{
if (len >= cmdLen[i]) //���Ƚ��յ������ݳ���Ҫ��С�������
{
if (CmpMemory(buf, cmdPtr[i], cmdLen[i])) //�Ƚ���ͬʱ�˳�ѭ��
{
break;
}
}
}
switch (i) //ѭ���˳�ʱ i ��ֵ���ǵ�ǰ���������ֵ
{
case 0:
ledshanshuo();	 //LED2��˸��LED1��
led();

break;
case 1:
flagBuzzOn = 1; //����������
break;
case 2:
break;

default: //δ�ҵ��������ʱ�����ϻ����͡������������ʾ
UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
return;
}
buf[len++] = '\r'; //��Ч���ִ�к���ԭ����֮֡�����
buf[len++] = '\n'; //�س����з��󷵻ظ���λ������ʾ��ִ��
UartWrite(buf, len);
}

/* ���ò����� T0��ms-T0 ��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
unsigned long tmp; //��ʱ����
tmp = 11059200 / 12; //��ʱ������Ƶ��
tmp = (tmp * ms) / 1000; //��������ļ���ֵ
tmp = 65536 - tmp; //���㶨ʱ������ֵ
tmp = tmp + 33; //�����ж���Ӧ��ʱ��ɵ����
T0RH = (unsigned char)(tmp>>8); //��ʱ������ֵ���Ϊ�ߵ��ֽ�
T0RL = (unsigned char)tmp;
TMOD &= 0xF0; //���� T0 �Ŀ���λ
TMOD |= 0x01; //���� T0 Ϊģʽ 1
TH0 = T0RH; //���� T0 ����ֵ
TL0 = T0RL;
ET0 = 1; //ʹ�� T0 �ж�
TR0 = 1; //���� T0
}
/* T0 �жϷ�������ִ�д��ڽ��ռ�غͷ��������� */
void InterruptTimer0() interrupt 1
{
TH0 = T0RH; //���¼�������ֵ
TL0 = T0RL;
if (flagBuzzOn) //ִ�з��������л�ر�
BUZZ = ~BUZZ;
else
BUZZ = 1;
UartRxMonitor(1); //���ڽ��ռ��
}
