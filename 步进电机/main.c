#include <reg52.h>

unsigned char code BeatCode[8] = { //����������Ķ�Ӧ�� IO ���ƴ���
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};

void delay();

void main()
{
unsigned char tmp; //����һ����ʱ����
unsigned char index = 0; //��������������
while (1)
{
tmp = P1; //�� tmp �� P1 �ڵ�ǰֵ�ݴ�
tmp = tmp & 0xF0; //��&��������� 4 λ
tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д���� 4 λ
P1 = tmp; //�ѵ� 4 λ�Ľ��Ĵ���͸� 4 λ��ԭֵ�ͻ� P1
index++; //���������������
index = index & 0x07; //��&����ʵ�ֵ� 8 ����
delay(); //��ʱ 2ms���� 2ms ִ��һ��
}
}

/* �����ʱ��������ʱԼ 2ms */
void delay()
{
unsigned int i = 200;
{
while(i--);
}
}