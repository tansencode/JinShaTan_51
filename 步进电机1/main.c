#include <reg52.h>
void TurnMotor(unsigned long angle);
void main()
{
TurnMotor(360*25); //360 ��*25���� 25 Ȧ
while (1);
}
/* �����ʱ��������ʱԼ 2ms */
void delay()
{
unsigned int i = 200;
while (i--);
}
/* �������ת��������angle-��ת���ĽǶ� */
void TurnMotor(unsigned long angle)
{
unsigned char tmp; //��ʱ����
unsigned char index = 0; //�����������
unsigned long beats = 0; //�����������
unsigned char code BeatCode[8] = { //����������Ķ�Ӧ�� IO ���ƴ���
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
beats = (angle*4096) / 360; //������Ҫ�Ľ���������4096 �Ķ�ӦһȦ
while (beats--) //�ж� beats ��Ϊ 0 ʱִ��ѭ����Ȼ���Լ� 1
{
tmp = P1; //�� tmp �� P1 �ڵ�ǰֵ�ݴ�
tmp = tmp & 0xF0; //��&��������� 4 λ
tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д���� 4 λ
P1 = tmp; //�ѵ� 4 λ�Ľ��Ĵ���͸� 4 λ��ԭֵ�ͻ� P1
index++; //���������������
index = index & 0x07; //��&����ʵ�ֵ� 8 ����
delay(); //��ʱ 2ms���� 2ms ִ��һ��
}
P1 = P1 | 0x0F; //�رյ�����е���
}