#include<reg52.h> 
#define uint unsigned int //�궨��
sbit SPK=P1^6;            //�������ȶ˿�
sbit key=P2^4;            //�������϶�Ӧ����s18����

void delay(uint z)
{
uint x,y;
for(x=z;x>0;x--)
for(y=110;y>0;y--);
}

void main()
{
    while(1)
    { 
if(key==0)
{
       delay(100); 
   SPK=0;
delay(100);
       SPK=1;
}
else
{
SPK=1;
}
    }
}
