#include<reg51.h>
#include<intrins.h>

    /***********�궨��*******************/
    #define uint unsigned int   
    #define uchar unsigned char 
    #define long_time0 40    //��ʱ��0��ʱʱ�䣬ÿ��50ms

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;
    /************����ȫ�ֱ���************/
    uint n=0;                //ѭ����λ����

    /************�Ӻ�������**************/
    void init_time0(void);   //��ʱ��0��ʼ���Ӻ���

    /*************������*****************/
    void main(void)
    {
	    	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   	//P0=0xFF;
        init_time0();

    }

    /*********��ʱ��0��ʼ������**********/
    void init_time0(void)
    {
            EA=1;                   //�����ж�
            ET0=1;                  //����ʱ��0�ж�
            TMOD=0X01;              //ѡ��ʽ1
            TH0=(65536-50000)/256;  //ÿ�ζ�ʱ50ms
            TL0=(65526-50000)%256;
            TR0=1;                  //������ʱ��
            while(1);               //�ȴ���ʱ��0�ж�
    }
    /************��ʱ���жϺ���**********/
    void time0(void) interrupt 1 using 0
    {
        static uint i=0;
        i++;
        if(long_time0==i)
        {
            P0=~(0x01<<n);          //P1=~(0xfe<<n);
            if(++n==8)
			
			n=0;
            i=0;                    //��ʱʱ�䵽����
			
        }
        TH0=(65536-50000)/256;      //����װ���ֵ
        TL0=(65526-50000)%256;
    }