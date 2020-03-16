#include <reg52.h>
extern bit irflag;
extern unsigned char ircode[4];
extern void InitInfrared();
signed char i = 0;
unsigned char len = 0;
unsigned char str[16];

unsigned char code SinWave[] = {  //���Ҳ�����
    127, 152, 176, 198, 217, 233, 245, 252,
    255, 252, 245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,   2,   9,  21,  37,  56,  78, 102,
	  127,152,176,198,217,233,245,252,255,252,
	  245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,2,   9,  21,  37,  56,  78, 102,

	  127,152,176,198,217,233,245,252,255,252,
	  245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,   2,   9,  21,  37,  56,  78, 102, 
};
unsigned char code SquWave[] = {  //��������
    255, 255, 255, 255,255, 255, 255, 255,
   0, 0, 0, 0,  0,  0,  0,   0,
      255, 255, 255, 255,255, 255, 255, 255, 
	  0, 0, 0, 0,  0,  0,  0,   0,
	  255, 255, 255, 255,255, 255, 255, 255,
   0, 0, 0, 0,  0,  0,  0,   0,
      255, 255, 255, 255,255, 255, 255, 255, 
	  0, 0, 0, 0,  0,  0,  0,   0,
	  255, 255, 255, 255,255, 255, 255, 255,
   0, 0, 0, 0,  0,  0,  0,   0,
      255, 255, 255, 255,255, 255, 255, 255, 
	  0, 0, 0, 0,  0,  0,  0,   0,

};
unsigned char code TriWave[] = {  //���ǲ�����
      0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
	 0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
	 0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
};
unsigned char code SawWave[] = {  //��ݲ���
      0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
	0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
	0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
};
unsigned char fWave = 10;   //����Ƶ��
unsigned char code *pWave;  //����ָ��
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char T1RH = 1;  //T1����ֵ�ĸ��ֽ�
unsigned char T1RL = 1;  //T1����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
void SetWaveFreq(unsigned char freq);

void NumKeyAction(unsigned char n);
unsigned char LongToString(unsigned char *str, signed long dat);
extern void KeyScan();
extern void KeyDriver();
extern void I2CStart();
extern void I2CStop();
extern bit I2CWrite(unsigned char dat);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdFullClear();
extern void LcdWriteDat(unsigned char dat);
void main()
{    
    EA = 1;           //�����ж�
	InitInfrared();   //��ʼ�����⹦��
    ConfigTimer0(1);  //����T0��ʱ1ms
    pWave = SinWave;  //Ĭ�����Ҳ�
    SetWaveFreq(10);  //Ĭ��Ƶ��10Hz
	fWave=10;
    InitLcd1602();    //��ʼ��Һ��
	LcdShowStr(0,0,"SinWave");
    LcdShowStr(0, 1, "10");  //��ʼ��ʾһ������10
	LcdShowStr(2, 1, "Hz");  //��ʼ��ʾһ������10
    while (1)
    {
        KeyDriver();  //���ð�������
		if (irflag)  //���յ���������ʱˢ����ʾ
        {
            irflag = 0;
            if (ircode[2] == 0x09)  //0x09Ϊ+����
            {
                  if (fWave < 25) //�ﵽ���ֵ25֮ǰ�ɵ���Ƶ��ֵ
                  {
                        fWave++;
                         SetWaveFreq(fWave);

                   }
            }
            else if (ircode[2] == 0x15)  //0x15Ϊ-����
            {
                   if (fWave > 2) //�ﵽ��Сֵ2֮ǰ�ɵݼ�Ƶ��ֵ
                   {
                        fWave--;
                        SetWaveFreq(fWave);
						len = LongToString(str, fWave);  //����ֵת��Ϊ�ַ���
		                LcdAreaClear(0, 1, str);        //����ڶ����м���ַ�λ
                        LcdShowStr(6, 1, str);     //��ʾ��Һ���ڶ�����
                    }
            }
				if (ircode[2] == 0x16) //  0
			{
			   //num=ircode[2];
			   NumKeyAction(0x00);
			}
			 else if (ircode[2] == 0x0C) //  1
			{
			   //num=0;
			   NumKeyAction(0x01);
			}
			else if (ircode[2] == 0x18)	  //  2
			{
			   NumKeyAction(0x02);
			}
				else if (ircode[2] == 0x5E)		//  3
			{
			   NumKeyAction(0x03);
			}
				else if (ircode[2] == 0x08)		//  4
			{
			   NumKeyAction(0x04);
			}
				else if (ircode[2] == 0x1C)	   //  5
			{
			   NumKeyAction(0x05);
			}
				else if (ircode[2] == 0x5A)	   //  6
			{
			   NumKeyAction(0x06);
			}
				else if (ircode[2] == 0x42)		//  7
			{
			   NumKeyAction(0x07);
			}
				else if (ircode[2] == 0x52)	  //  8
			{
			   NumKeyAction(0x08);
			}
			else if (ircode[2] == 0x4A)		//  9
			{
			   NumKeyAction(0x09);
			}
        }
    }
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    static unsigned char i = 0;
    
    if (keycode == 0x26)  //���ϼ����л�����
    {
        //��4�ֲ��μ�ѭ���л�
        if (i == 0)
        {
            i = 1;
            pWave = SquWave;
			LcdShowStr(0,0,"SquWave");
        }
		else if (i == 1)
        {
            i = 2;
            pWave =TriWave;
			LcdShowStr(0,0,"TriWave");
        }
        else if (i == 2)
        {
            i = 3;
            pWave = SawWave;
			LcdShowStr(0,0,"SawWave");
        }
        else
        {
            i = 0;
            pWave = SinWave;
			LcdShowStr(0,0,"SinWave");
        }

    }

	 if (keycode == 0x28)  //���¼����л�����
    {
        //��4�ֲ��μ�ѭ���л�
        if (i == 0)
        {
            i = 1;
            pWave = SquWave;
			LcdShowStr(0,0,"SquWave");
        }
		else if (i == 1)
        {
            i = 2;
            pWave =TriWave;
			LcdShowStr(0,0,"TriWave");
        }
        else if (i == 2)
        {
            i = 3;
            pWave = SawWave;
			LcdShowStr(0,0,"SawWave");
        }
        else
        {
            i = 0;
            pWave = SinWave;
			LcdShowStr(0,0,"SinWave");
        }

    }
}

/* ��������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char LongToString(unsigned char *str, signed long dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[16];
    
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
void NumKeyAction(signed char n)
{
    unsigned char len;
    unsigned char str[16];
    
    
       fWave = fWave*10 + n;             //������ֵ�ۼӵ�ԭ��������

        len = LongToString(str, fWave);  //����ֵת��Ϊ�ַ���
		 LcdAreaClear(0, 1, str);        //����ڶ����м���ַ�λ
        LcdShowStr(6, 1, str);     //��ʾ��Һ���ڶ�����
  
}
/* ����DAC���ֵ��val-�趨ֵ */
void SetDACOut(unsigned char val)
{
    I2CStart();
    if (!I2CWrite(0x48<<1)) //ѰַPCF8591����δӦ����ֹͣ����������
    {
        I2CStop();
        return;
    }
    I2CWrite(0x40);         //д������ֽ�
    I2CWrite(val);          //д��DAֵ  
    I2CStop();
}
/* ����������ε�Ƶ�ʣ�freq-�趨Ƶ�� */
void SetWaveFreq(unsigned char freq)
{
    unsigned long tmp;
    
    tmp = (11059200/12) / (freq*96); //��ʱ������Ƶ�ʣ��ǲ���Ƶ�ʵ�32��
    tmp = 65536 - tmp;               //���㶨ʱ������ֵ
    tmp = tmp + 36;                  //�����ж���Ӧ��ʱ��ɵ����
    T1RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //����T1�Ŀ���λ
    TMOD |= 0x10;   //����T1Ϊģʽ1
    TH1 = T1RH;     //����T1����ֵ
    TL1 = T1RL;
    ET1 = 1;        //ʹ��T1�ж�
    PT1 = 1;        //����Ϊ�����ȼ�
    TR1 = 1;        //����T1
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 28;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�а���ɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    KeyScan();   //����ɨ��
}
/* T1�жϷ�������ִ�в������ */
void InterruptTimer1() interrupt 3
{
    static unsigned char i = 0;
    
    TH1 = T1RH;  //���¼�������ֵ
    TL1 = T1RL;
    //ѭ����������е�����
    SetDACOut(pWave[i]);
    i++;
    if (i >= 96)
    {
        i = 0;
    }
}
