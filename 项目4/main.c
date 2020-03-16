#include <reg52.h>
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit BUZZ = P1^6; //��������������
bit flagBuzzOn = 0; //������������־
unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

bit flag1s = 0;          //1s��ʱ��־	 
//bit flag200ms = 1;  //200ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
extern void DelayX10us(unsigned char t);
//extern void InitLcd1602();
//extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
//void ShowNumber(unsigned long num);

/* ���������ʾһλ���֣�index-�����λ����(���ҵ����Ӧ0��5)��
**     num-����ʾ�����֣�point-�����Ƿ���ʾ��λ�ϵ�С���� */
void ShowLedNumber(unsigned int index, unsigned int num,unsigned int point)
{
   P0=0xFF;
   LedBuff[index] = LedChar[num];  //��������ת��Ϊ������ַ�0��F
    if (point != 0)
    {
       LedBuff[index] &= 0x7F;  //point��Ϊ0ʱ������ǰλ��С����
    }
}

void main()
{
    
    bit res;
    int temp;        //��ȡ���ĵ�ǰ�¶�ֵ
    int intT, decT;  //�¶�ֵ��������С������
    unsigned char len;
    unsigned char str[12];

    EA = 1;            //�����ж�
	EA = 1; //ʹ�����ж�
    ENLED = 0; //ѡ������ܽ�����ʾ
    ADDR3 = 1;
	TMOD = 0x01; //���� T0 Ϊģʽ 1
    TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
    TL0 = 0x67;
    ET0 = 1; //ʹ�� T0 �ж�
    TR0 = 1; //���� T0
    ConfigTimer0(1);  //T0��ʱ10ms
    Start18B20();      //����DS18B20

	DelayX10us(100000);
    //InitLcd1602();     //��ʼ��Һ��
    while (1)
    {
        if (flag1s)  //ÿ�����һ���¶�
        {
            flag1s = 0;
			P0=0xFF;
		//	DelayX10us(100000);
            res = Get18B20Temp(&temp);  //��ȡ��ǰ�¶�
			
            if (res)                    //��ȡ�ɹ�ʱ��ˢ�µ�ǰ�¶���ʾ
            {
                intT = temp >> 4;             //������¶�ֵ��������
                decT = temp & 0xF;            //������¶�ֵС������
               // len = IntToString(str, intT); //��������ת��Ϊ�ַ���
               // str[len++] = '.';             //���С����
                decT = (decT*10) / 16;        //�����Ƶ�С������ת��Ϊ1λʮ����λ
                //str[len++] = decT + '0';      //ʮ����С��λ��ת��ΪASCII�ַ�
                //while (len < 6)               //�ÿո��뵽6���ַ�����
               // {
                    //str[len++] = ' ';
               // }
                //str[len] = '\0';              //����ַ���������
               // LcdShowStr(0, 0, str);        //��ʾ��Һ������
			    ShowLedNumber(5,intT/10,0);
				ShowLedNumber(4,(intT-(intT/10)*10),1);
				ShowLedNumber(3,decT,0);
				//ShowLedNumber(2,decT,0);
				//ShowLedNumber(1,decT,0);
				if(intT>29)
				flagBuzzOn = 1; //����������
				else
				flagBuzzOn = 0; //����������
            }
            else                        //��ȡʧ��ʱ����ʾ������Ϣ
            {
              //  LcdShowStr(0, 0, "error!");
			  ShowLedNumber(5,0,0);
			  ShowLedNumber(4,0,1);
			  ShowLedNumber(3,0,0);
            }
            Start18B20();               //����������һ��ת��
			DelayX10us(100000);
        }
    }
}


/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
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
}
/* ������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
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
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ����������1�붨ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr1s = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	LedScan(); //�����������ʾɨ�躯��
    tmr1s++;
    if (tmr1s++>=1)  //��ʱ1ms
    {
        tmr1s = 0;
        flag1s = 1;
    }
	if (flagBuzzOn) //ִ�з��������л�ر�
    BUZZ = ~BUZZ;
    else
    BUZZ = 1;
}
