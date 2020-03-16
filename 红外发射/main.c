#include <reg52.h>  
  
sbit  P3_4 = P3^4;  
  
static bit OP;        //���ⷢ��ܵ�����  
  
static unsigned int ncount;       //��ʱ������  
  
static unsigned int endncount; //��ֹ��ʱ����  
  
static unsigned char flag;      //���ⷢ�ͱ�־  
  
char iraddr1;  //ʮ��λ��ַ�ĵ�һ���ֽ�  
  
char iraddr2;  //ʮ��λ��ַ�ĵڶ����ֽ�  
  
void SendIRdata(char p_irdata);  
  
void delay();  
  
void main(void)  
  
{  
  
    ncount = 0;  
  
    flag = 0;  
  
    OP = 0;  
  
    P3_4 = 0;  
  
    EA = 1; //����CPU�ж�  
  
    TMOD = 0x11; //�趨ʱ��0��1Ϊ16λģʽ1  
  
    ET0 = 1; //��ʱ��0�ж�����  
  
    TH0 = 0xFF;  
  
    TL0 = 0xE8; //�趨ʱֵ0Ϊ38K Ҳ����ÿ��26us�ж�һ��   
  
    TR0 = 1;//��ʼ����  
  
    iraddr1=3;  
  
    iraddr2=252;  
  
    do{  
  
        //unsigned char i;  
  
        delay();  
  
        SendIRdata(12);  
  
    }while(1);  
  
}  
  
//��ʱ��0�жϴ���  
  
void timeint(void) interrupt 1  
  
{  
  
    TH0=0xFF;  
  
    TL0=0xE8; //�趨ʱֵΪ38K Ҳ����ÿ��26us�ж�һ��  
  
    ncount++;  
  
    if (flag==1)  
  
    {  
  
        OP=~OP;  
  
    }  
  
    else  
  
    {  
  
        OP = 0;  
  
    }  
  
    P3_4 = OP;  
  
}  
  
void SendIRdata(char p_irdata)  
  
{  
  
    int i;  
  
    char irdata=p_irdata;  
  
    //����9ms����ʼ��  
  
    endncount=223;  
  
    flag=1;  
  
    ncount=0;  
  
    do{}while(ncount<endncount);  
  
    //����4.5ms�Ľ����  
  
    endncount=117;  
  
    flag=0;  
  
    ncount=0;  
  
    do{}while(ncount<endncount);  
  
    //����ʮ��λ��ַ��ǰ��λ  
  
    irdata=iraddr1;  
  
    for(i=0;i<8;i++)  
  
    {  
  
        //�ȷ���0.56ms��38KHZ���Ⲩ����������0.56ms�ĵ͵�ƽ��  
  
        endncount=10;  
  
        flag=1;  
  
        ncount=0;  
  
        do{}while(ncount<endncount);  
  
        //ֹͣ���ͺ����źţ��������еĸߵ�ƽ��  
  
        if(irdata-(irdata/2)*2)  //�ж϶���������λΪ1����0  
  
        {  
  
            endncount=41;  //1Ϊ��ĸߵ�ƽ  
  
        }  
        else  
        {  
            endncount=15;   //0Ϊխ�ĸߵ�ƽ  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    //����ʮ��λ��ַ�ĺ��λ  
    irdata=iraddr2;  
    for(i=0;i<8;i++)  
    {  
        endncount=10;  
        flag=1;  
        ncount=0;  
        do{}while(ncount<endncount);  
        if(irdata-(irdata/2)*2)  
        {  
            endncount=41;  
        }  
        else  
        {  
            endncount=15;  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    //���Ͱ�λ����  
    irdata=p_irdata;  
    for(i=0;i<8;i++)  
    {  
        endncount=10;  
        flag=1;  
        ncount=0;  
        do{}while(ncount<endncount);  
        if(irdata-(irdata/2)*2)  
        {  
            endncount=41;  
        }  
        else  
        {  
            endncount=15;  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    //���Ͱ�λ���ݵķ���  
    irdata=~p_irdata;  
    for(i=0;i<8;i++)  
    {  
        endncount=10;  
        flag=1;  
        ncount=0;  
        do{}while(ncount<endncount);  
        if(irdata-(irdata/2)*2)  
        {  
            endncount=41;  
        }  
        else  
        {  
            endncount=15;  
        }  
        flag=0;  
        ncount=0;  
        do{}while(ncount<endncount);  
        irdata=irdata>>1;  
    }  
    endncount=10;  
    flag=1;  
    ncount=0;  
    do{}while(ncount<endncount);  
    flag=0;  
}  
  
void delay()  
  
{  
  
    int i,j;  
  
    for(i = 0; i < 400; i++){  
  
        for(j = 0; j < 200; j++){  
  
        }  
  
    }  
  
}  