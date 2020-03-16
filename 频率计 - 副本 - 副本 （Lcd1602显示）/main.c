#include <reg52.h>
#define uchar unsigned char 
#define uint unsigned int

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned long frequency=0;
bit start=0;   //��ʱ��������ģʽ��������
unsigned char Pulse = 0;
unsigned char T0RH=0;
unsigned char T0RL=0;

unsigned char len;
unsigned char str[12];
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);

/*unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};	*/

/* ��һ���޷��ų����͵�������ʾ��������ϣ�num-����ʾ���� */
/*void ShowNumber(unsigned long num)
{
     signed char i;
     unsigned char buf[6];
     for (i=0; i<6; i++) //�ѳ�������ת��Ϊ 6 λʮ���Ƶ�����
     {
         buf[i] = num % 10;
         num = num / 10;
      }
      for (i=5; i>=1; i--) //�����λ������ 0 ת��Ϊ�ո������� 0 ���˳�ѭ��
      {
          if (buf[i] == 0)
          LedBuff[i] = 0xFF;
          else
         break;
      }
      for ( ; i>=0; i--) //ʣ���λ����ʵת��Ϊ�������ʾ�ַ�
      {
          LedBuff[i] = LedChar[buf[i]];
       }
}		*/

/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
/*void LedScan()
{
     static unsigned char i = 0; //��̬ɨ�������
     P0 = 0xFF; //��ʾ����
     switch (i)
     {
          case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
          case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
          case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
          case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
          case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
          case 5: ADDR2=1; ADDR1=0; ADDR0=1; i=0; P0=LedBuff[5]; break;
          default: break;
      }
}		   */
/* ������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char IntToString(unsigned char *str, long dat)	 //�ص㣬frequenceΪlong�ͣ�����int��
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[12];
    
    if (dat < 0)  //���Ϊ����������ȡ����ֵ������ָ������Ӹ���
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
}
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;
	tmp=11059200/12;
	tmp=(tmp*ms)/1000;
	tmp=65536-tmp;
	tmp=tmp+36;	     //��Ƶ36��100�� 1000�� 10000��  ��Ƶ24��10 0000��20 0000�� ��Ƶ58��30 0000��
	T0RH=(unsigned char)(tmp>>8);
	T0RL=(unsigned char)tmp;
	TMOD&=0xF0;
	TMOD|=0x01;
	TH0=T0RH;
	TL0=T0RL;
	ET0=1; 
	TR0=1;
}
void main()
{
	P3 = 0xFF;
    EA = 1; //ʹ�����ж�
    //ENLED = 0; //ѡ������ܽ�����ʾ
    //ADDR3 = 1;
    ConfigTimer0(1); //T0��ʱ1ms
	TMOD = 0x51; //���� T0��ʱ��T1������Ϊģʽ 1
	TH1=0x00;
    TL1=0x00;
	ET1=1;
	TR1=1;
    //LedBuff[0] = LedChar[0]; //�ϵ���ʾ0
	 InitLcd1602();     //��ʼ��Һ��
    while(1)
    {
     if(start==1)
       {
           TR1=1;  //����������
		   TR0 = 1;
           start=0;  //�ر���������λ ��֤1��ʱ��
           Pulse = 0;
         }
		 len = IntToString(str,frequency); //��������ת��Ϊ�ַ���
              // str[len] = "Hz";      //ʮ����С��λ��ת��ΪASCII�ַ�
               // while (len < 6)               //�ÿո��뵽6���ַ�����
               // {
                   // str[len++] = ' ';
                //}
                str[len] = 'H';              //���ݺ�����ַ�H,��0��ʼ������len��ʾ��β��һ���ַ�
				str[len+1] = 'z';              //���ݺ�����ַ�z
                LcdShowStr(0, 1, str);        //��ʾ��Һ������
				//LcdShowStr(10, 1, "Hz");        //��ʾ��Һ������	
		        LcdShowStr(0,0,"50  TJ");
         //ShowNumber(frequency);	
     }
}
void time0() interrupt 1	   //T0��ʱ
{
    static unsigned int i=0;
    TH0=T0RH;
    TL0=T0RL;
    //TH0 = 0xFC; //���¼��س�ֵ,��ʱ1ms
    //TL0 = 0x67;
	 i++;
   // LedScan(); //�����������ʾɨ�躯��
    if(i>=1000)
    {
      i=0;
      TR1=0;    //ֹͣ����
      TR0=0;          //ֹͣ��ʱ
      frequency = ((Pulse*65535)+(TH1*256)+TL1);	 //���Ƶ��ֵ   ����1�����������
	  Pulse=0;
      TH1=0;          //����ֵ����
      TL1=0;
	  TH0=T0RH;
      TL0=T0RL;
	  //TH0 = 0xFC; //���¼��س�ֵ,��ʱ1ms
      //TL0 = 0x67;
      start=1;               //������ʱ����������
	  //TR1=1;
 }
}
void Time1() interrupt 3	//T1����
{
   TH1=0;
   TL1=0;
   TR1=1;
   Pulse++;
}
 