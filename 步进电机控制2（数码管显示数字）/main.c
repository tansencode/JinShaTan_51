#include<at89x52.h>

sbit KEY_IN_1 = P2^4;	   //���尴��IO�ڵ�ַ
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

unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
{ 0x31, 0x32, 0x33, 0x26 }, //���ּ� 1�����ּ� 2�����ּ� 3�����ϼ�
{ 0x34, 0x35, 0x36, 0x25 }, //���ּ� 4�����ּ� 5�����ּ� 6�������
{ 0x37, 0x38, 0x39, 0x28 }, //���ּ� 7�����ּ� 8�����ּ� 9�����¼�
{ 0x30, 0x1B, 0x0D, 0x27 } //���ּ� 0��ESC ���� �س����� ���Ҽ�
};

unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

unsigned char KeySta[4][4] = { //ȫ�����󰴼��ĵ�ǰ״̬
{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};

signed long beats = 0; //���ת����������
void KeyDriver();

void main()
{
EA = 1; //ʹ�����ж�,���ܿ���
ENLED = 0; //ѡ������ܽ�����ʾ
ADDR3 = 1;
T2MOD = 0x00; //���� T2 Ϊģʽ 0
T2CON=0x00;
TH2 = 0xFC; //Ϊ T2 ����ֵ 0xFC67����ʱ 1ms
TL2 = 0x67;
RCAP2H=0xFC;
RCAP2L=0x67;
ET2 = 1; //ʹ�� T2 �ж�
TR2 = 1; //���� T2
LedBuff[0] = LedChar[0]; //�ϵ���ʾ 0
while (1)
{
KeyDriver(); //���ð�����������
}
}

/* ��һ���޷��ų����͵�������ʾ��������ϣ�num-����ʾ���� */
void ShowNumber(unsigned long num)
{
signed char i;
unsigned char buf[6];
for (i=0; i<6; i++) //�ѳ�������ת��Ϊ 6 λʮ���Ƶ�����
{
buf[i] = num % 10;
num = num / 10;
}
for (i=5; i>=1; i--) //�����λ������ 0 ת��Ϊ�ո������� 0 ���˳�ѭ��
{
if (buf[i] == 0)
LedBuff[i] = 0xFF;
else
break;
}
for ( ; i>=0; i--) //ʣ���λ����ʵת��Ϊ�������ʾ�ַ�
{
LedBuff[i] = LedChar[buf[i]];
}
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

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
static bit dirMotor = 0; //���ת������
static signed long num=0;		  //��������ĽǶ�ֵ
if ((keycode>=0x30) && (keycode<=0x39)) //���Ƶ��ת��
{
ENLED = 0; //ѡ������ܽ�����ʾ
ADDR3 = 1;
num = (num*10)+(keycode-0x30); //����ʮ�������ƣ������ֽ����λ
ShowNumber(num); //��������ʾ�������
}
else if(keycode==0x0D)	   //���س�����ʼת��
{
if (dirMotor == 0)	  //��ת
{
ShowNumber(num); //��������ʾ�������
StartMotor(num);

num=0;

}
else
{
ShowNumber(num); //��������ʾ�������
StartMotor(-1*(num));	  //��ת

num=0;

}
}
else if (keycode == 0x26) //���ϼ�������ת������Ϊ��ת
{
dirMotor = 0;
}
else if (keycode == 0x28) //���¼�������ת������Ϊ��ת
{
dirMotor = 1;
}
else if (keycode == 0x25) //��������̶���ת 360 ��
{
StartMotor(360);
}
else if (keycode == 0x27) //���Ҽ����̶���ת 360 ��
{
StartMotor(-360);
}
else if (keycode == 0x1B) //Esc ����ֹͣת��
{
num=0;
ShowNumber(num); //�����ļ�����ʾ�������
StopMotor();
}
}


/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
unsigned char i, j;
static unsigned char backup[4][4] = { //����ֵ���ݣ�����ǰһ�ε�ֵ
{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};
for (i=0; i<4; i++) //ѭ����� 4*4 �ľ��󰴼�
{
for (j=0; j<4; j++)
{
if (backup[i][j] != KeySta[i][j]) //��ⰴ������
{
if (backup[i][j] != 0) //��������ʱִ�ж���
{
KeyAction(KeyCodeMap[i][j]); //���ð�����������
}
backup[i][j] = KeySta[i][j]; //ˢ��ǰһ�εı���ֵ
}
}
}
}

/* ����ɨ�躯�������ڶ�ʱ�ж��е��ã��Ƽ����ü�� 1ms */
void KeyScan()
{
unsigned char i;
static unsigned char keyout = 0; //���󰴼�ɨ���������
static unsigned char keybuf[4][4] = { //���󰴼�ɨ�軺����
{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}
};
//��һ�е� 4 ������ֵ���뻺����
keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
//��������°���״̬
for (i=0; i<4; i++) //ÿ�� 4 ������������ѭ�� 4 ��
{
if ((keybuf[keyout][i] & 0x0F) == 0x00)
{ //���� 4 ��ɨ��ֵΪ 0���� 4*4ms �ڶ��ǰ���״̬ʱ������Ϊ�������ȶ��İ���
KeySta[keyout][i] = 0;
}
else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
{ //���� 4 ��ɨ��ֵΪ 1���� 4*4ms �ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
KeySta[keyout][i] = 1;
}
}
//ִ����һ�ε�ɨ�����
keyout++; //�����������
keyout = keyout & 0x03; //����ֵ�ӵ� 4 ������
switch (keyout) //�����������ͷŵ�ǰ������ţ������´ε��������
{
case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
default: break;
}
}

/* ���ת�����ƺ��� */
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

/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
static unsigned char i = 0; //��̬ɨ�������
P0 = 0xFF; //��ʾ����
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

/* T2 �жϷ����������ڰ���ɨ������ת������ */
void InterruptTimer2() interrupt 5
{
static bit div = 0;
TF2=0;				//�жϱ�־λ���㣬T2�Զ����س�ֵ
LedScan(); //�����������ʾɨ�躯��
KeyScan(); //ִ�а���ɨ��
//��һ����̬ bit ����ʵ�ֶ���Ƶ���� 2ms ��ʱ�����ڿ��Ƶ��
div = ~div;
if (div == 1)
{
TurnMotor();
}
}