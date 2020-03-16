#include <reg52.h>
unsigned long beats = 0; //���ת����������
void StartMotor(unsigned long angle);
void motor()
{
EA = 1; //ʹ�����ж�
TMOD = 0x01; //���� T0 Ϊģʽ 1
TH0 = 0xF8; //Ϊ T0 ����ֵ 0xF8CD����ʱ 2ms
TL0 = 0xCD;
ET0 = 1; //ʹ�� T0 �ж�
TR0 = 1; //���� T0
StartMotor(360*2+180); //���Ƶ��ת�� 2 Ȧ��
while (1);
}
/* �����������������angle-��ת���ĽǶ� */
void StartMotor(unsigned long angle)
{
//�ڼ���ǰ�ر��жϣ���ɺ��ٴ򿪣��Ա����жϴ�ϼ�����̶���ɴ���
EA = 0;
beats = (angle * 4076) / 360; //ʵ��Ϊ 4076 ��ת��һȦ
EA = 1;
}

void StopMotor()
{
EA = 0;
beats = 0;
EA = 1;
}
/* T0 �жϷ������������������������ת */
void InterruptTimer0() interrupt 1
{
unsigned char tmp; //��ʱ����
static unsigned char index = 0; //�����������
unsigned char code BeatCode[8] = { //����������Ķ�Ӧ�� IO ���ƴ���
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
TH0 = 0xF8; //���¼��س�ֵ
TL0 = 0xCD;
if (beats != 0) //��������Ϊ 0 �����һ����������
{
tmp = P1; //�� tmp �� P1 �ڵ�ǰֵ�ݴ�
tmp = tmp & 0xF0; //��&��������� 4 λ
tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д���� 4 λ
P1 = tmp; //�ѵ� 4 λ�Ľ��Ĵ���͸� 4 λ��ԭֵ�ͻ� P1
index++; //���������������
index = index & 0x07; //��&����ʵ�ֵ� 8 ����
beats--; //�ܽ�����-1
}
else //������Ϊ 0 ��رյ�����е���
{
P1 = P1 | 0x0F;
}
}