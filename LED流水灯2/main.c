#include<reg52.h>
#include<intrins.h>
sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;

void main()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;
  ADDR2=1;
  ADDR1=1;
  ADDR0=0; 
  P0=~0x01;		   //�����ⲿ���壬�ƶ�����ֻ�����ƶ�������ֱ�ӵ�����һ��С��
  for(i=0;i<40000;i++);			  //��һ������λ
  while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
	   
        P0=_crol_(P0,1);		// //ѭ�����ƣ�i=0��ʼ�ƶ�������i<7
		for(i=0;i<40000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {	 
	 
        P0=_cror_(P0,1);	//ѭ�����ƣ�i=0��ʼ�ƶ�������i<7
		for(i=0;i<40000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}