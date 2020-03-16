#include <reg52.h>

sbit MC0 = P3^0;
sbit MC1 = P3^1;
sbit MC2 = P3^2;
sbit MC3 = P3^3;

signed long beats = 0; //���ת����������

flagmotor=0;

unsigned char T0RH = 0; //T0 ����ֵ�ĸ��ֽ�
unsigned char T0RL = 0; //T0 ����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdWriteCmd(unsigned char cmd);
extern void StartMotor(unsigned long angle);


void StartMotor(signed long angle);
void StopMotor();
void main()
{
EA = 1; //�����ж�
TMOD = 0x00; //���� T0 Ϊģʽ 0
TH0 = T0RH; //���� T0 ����ֵ
TL0 = T0RL;
ConfigTimer0(2); //���� T0 ��ʱ 2ms
ConfigUART(9600); //���ò�����Ϊ 9600
InitLcd1602(); //��ʼ��Һ��
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

/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
unsigned char i;
unsigned char code cmd0[] = "motorrun"; //���ת������
unsigned char code cmd1[] = "motorstop"; //���ֹͣ����
unsigned char code cmd2[] = "number"; //��ʾѧ��
unsigned char code cmd3[] = "zhuanye"; //��ʾרҵ
unsigned char code cmd4[] = "time"; //��ʾʱ��

unsigned char code cmdLen[] = { //����Ȼ��ܱ�
sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,sizeof(cmd3)-1,sizeof(cmd4)-1,
};
unsigned char code *cmdPtr[] = { //����ָ����ܱ�
&cmd0[0], &cmd1[0], &cmd2[0],&cmd3[0],&cmd4[0],
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
LcdWriteCmd(0x01); //����
StartMotor(60); //�����������
break;

case 1:
StopMotor();//�رղ������
break;

case 2:
LcdShowStr(0,0,"0903160218"); //��ʾѧ��, 0,0��һ����ʾ
break;

case 3:
LcdWriteCmd(0x01); //����
LcdShowStr(0, 1,"DianZiXinXi"); //��ʾרҵ�� 0,1�ڶ�����ʾ
break;

case 4:
LcdWriteCmd(0x01); //����
buf[len] = '\0'; //Ϊ���յ����ַ�����ӽ�����
LcdShowStr(0, 1, buf+cmdLen[4]); //��ʾ�������ַ����� 0,1�ڶ�����ʾ
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

/* �����������������angle-��ת���ĽǶ� */
void StartMotor(signed long angle)
{
//�ڼ���ǰ�ر��жϣ���ɺ��ٴ򿪣��Ա����жϴ�ϼ�����̶���ɴ���
EA = 0;
beats = (angle * 4076)/360 ; //ʵ��Ϊ 4076 ��ת��һȦ
EA = 1;
}

/* �������ֹͣ���� */
void StopMotor()
{
EA = 0;
beats = 0;
EA = 1;
}

void TurnMotor()
{
unsigned char tmp; //��ʱ����
static unsigned char index = 0; //�����������
unsigned char code BeatCode[8] = { //����������Ķ�Ӧ�� IO ���ƴ���
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
if (beats != 0) //��������Ϊ 0 �����һ����������
{
if (beats > 0) //���������� 0 ʱ��ת
{
index++; //��תʱ���������������
index = index & 0x07; //��&����ʵ�ֵ� 8 ����
beats--; //��תʱ���ļ����ݼ�
}
else //������С�� 0 ʱ��ת
{
index--; //��תʱ������������ݼ�
index = index & 0x07; //��&����ͬ������ʵ�ֵ�-1 ʱ�� 7
beats++; //��תʱ���ļ�������
}
tmp = P3; //�� tmp �� P1 �ڵ�ǰֵ�ݴ�
tmp = tmp & 0xF0; //��&��������� 4 λ
tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д���� 4 λ
P3= tmp; //�ѵ� 4 λ�Ľ��Ĵ���͸� 4 λ��ԭֵ�ͻ� P1
}
else //������Ϊ 0 ��رյ�����е���
{
P3 = P3 | 0x0F;
}
}
/* T0 �жϷ�������ִ�д��ڽ��ռ�غͷ��������� */
void InterruptTimer0() interrupt 1
{
static bit div = 0;
TH0 = T0RH; //���� T0 ����ֵ
TL0 = T0RL;
div = ~div;
TurnMotor();
UartRxMonitor(1);
}
