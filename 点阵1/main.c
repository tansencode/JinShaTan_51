#include <reg52.h>

sbit LED = P0^0;
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
void main()
{
ENLED = 0; //U3��U4 ��Ƭ 74HC138 ��ʹ��
ADDR3 = 0; //ʹ�� U4 ʹ֮�������

ADDR2 = 0; //�� U4 �� Y0 ������������� Q10
ADDR1 = 0;
ADDR0 = 0;

LED = 0; //�� P0.0 д�� 0 ���������Ͻǵ�һ����
while(1); //����ֹͣ������
}