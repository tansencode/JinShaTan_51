#include <reg52.h>
sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;

unsigned char ledd[7]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB};
    //��ʵ���Ƕ�����һ������״̬��,���м䵽���ߣ��������ߵ��м�,Ȼ��P0ѭ��������
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111
 
void main()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    while(1)
    {
        for(cnt=0;cnt<6;cnt++)
		{
            P0=ledd[cnt];
			for(i=0;i<30000;i++);
		}
  }
}