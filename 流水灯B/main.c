#include<reg52.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;

/*void main()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x0F<<cnt);		//�߼�����,ȡ����0000 1111����1111 0000
		for(i=0;i<20000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0xF0>>cnt);		//�߼�����,ȡ����1111 0000����0000 1111
		for(i=0;i<20000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}*/
void main()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt=cnt+2)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x08<<cnt);		//�߼�����,ȡ����0000 1000����1111 0111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt=cnt+2)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0x10>>cnt);		//�߼�����,ȡ����0001 0000����1110 1111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}  

