#include<reg52.h>
#include<intrins.h>
#include<codetab.h>
#include<LQ12864.h>

//sfr T2MOD=0xC9;
sbit BUZZ  = P1^6;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //�������ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
    { 0x31, 0x32, 0x33, 0x26 }, //���ּ�1�����ּ�2�����ּ�3�����ϼ�
    { 0x34, 0x35, 0x36, 0x25 }, //���ּ�4�����ּ�5�����ּ�6�������
    { 0x37, 0x38, 0x39, 0x28 }, //���ּ�7�����ּ�8�����ּ�9�����¼�
    { 0x30, 0x1B, 0x0D, 0x27 }  //���ּ�0��ESC����  �س����� ���Ҽ�
};

unsigned char KeySta[4][4] = {  //ȫ�����󰴼��ĵ�ǰ״̬
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};
bit enBuzz = 0;     //������ʹ�ܱ�־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned int  CountDown = 0;  
static int m=0;

void ConfigTimer0(unsigned int ms);
void ShowNumber(unsigned long num);
void KeyDriver();

//���ü���������ֵ�����õ����ݶ�Ӧ��д�ɶ���������������ά��

void main(void)
{
	EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ѡ������ܺͶ���LED
    ADDR3 = 1;
    ConfigTimer0(1);  //����T0��ʱ1ms
    ShowNumber(0);    //�ϵ���ʾ0
	/*T2MOD = 0x00; //���� T2 Ϊģʽ 0
    T2CON=0x00;
    TH2 = 0xFC; //Ϊ T2 ����ֵ 0xFC67����ʱ 1ms
    TL2 = 0x67;
    RCAP2H=0xFC;
    RCAP2L=0x67;
    ET2 = 1; //ʹ�� T2 �ж�
    TR2 = 1; //���� T2	*/
	OLED_Init(); //OLED��ʼ��
	while(1)
	{
	    OLED_P8x16Str(0,0,"munu");//��һ�� -- 8x16����ʾ��Ԫ��ʾASCII��
	    OLED_P8x16Str(0,2,"counts");
	    OLED_P8x16Str(0,4,"bell");
		KeyDriver();  //���ð�����������
	}
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
/* ��һ���޷��ų����͵�������ʾ��������ϣ�num-����ʾ���� */
void ShowNumber(unsigned long num)
{
    signed char i;
    unsigned char buf[6];
    
    for (i=0; i<6; i++)   //�ѳ�������ת��Ϊ6λʮ���Ƶ�����
    {
        buf[i] = num % 10;
        num = num / 10;
    }
    for (i=5; i>=1; i--)  //�����λ������0ת��Ϊ�ո�������0���˳�ѭ��
    {
        if (buf[i] == 0)
            LedBuff[i] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)    //ʣ���λ����ʵת��Ϊ�������ʾ�ַ�
    {
        LedBuff[i] = LedChar[buf[i]];
    }
}

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)  //�����������������ݼ���ִ����Ӧ����
{
    if (keycode == 0x26)       //���ϼ�������ʱ�趨ֵ����
    {
	    m=1;
        OLED_P8x16Str(0,2,"COUNTS");
		//	delay(4000);
		enBuzz = 0;            //�رշ�����
    }
    else if (keycode == 0x28)  //���¼�������ʱ�趨ֵ�ݼ�
    {
	    m=2;
       	OLED_P8x16Str(0,4,"BELL");
     	//delay(4000);
	
    }
    else if (keycode == 0x0D)  //�س�������������ʱ
    {	 
	     switch(m)
		 {
		        case 1:	CountDown++;
                    ShowNumber(CountDown);
			        //m=0;
					break;
			    case 2:enBuzz=1;
				   // m=0;
					break;
			    default:
				break;
		   }
    }
    else if (keycode == 0x1B)  //Esc����ȡ������ʱ
    {
        enBuzz = 0;            //�رշ�����
        CountDown = 0;         //��������
        ShowNumber(CountDown);
    }
}
/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char pdata backup[4][4] = {  //����ֵ���ݣ�����ǰһ�ε�ֵ
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    for (i=0; i<4; i++)  //ѭ��ɨ��4*4�ľ��󰴼�
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])     //��ⰴ������
            {
                if (backup[i][j] != 0)            //��������ʱִ�ж���
                {
                    KeyAction(KeyCodeMap[i][j]);  //���ð�����������
                }
                backup[i][j] = KeySta[i][j];      //ˢ��ǰһ�εı���ֵ
            }
        }
    }
}
/* ����ɨ�躯�������ڶ�ʱ�ж��е��� */
void KeyScan()
{
    unsigned char i;
    static unsigned char keyout = 0;   //���󰴼�ɨ���������
    static unsigned char keybuf[4][4] = {  //���󰴼�ɨ�軺����
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
    };

    //��һ�е�4������ֵ���뻺����
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //��������°���״̬
    for (i=0; i<4; i++)  //ÿ��4������������ѭ��4��
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   //����4��ɨ��ֵΪ0����4*4ms�ڶ��ǰ���״̬ʱ������Ϊ�������ȶ��İ���
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //����4��ɨ��ֵΪ1����4*4ms�ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
            KeySta[keyout][i] = 1;
        }
    }
    //ִ����һ�ε�ɨ�����
    keyout++;        //�����������
    keyout &= 0x03;  //����ֵ�ӵ�4������
    switch (keyout)  //�����������ͷŵ�ǰ������ţ������´ε��������
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}
/* LED��̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ������
    
    P0 = 0xFF;             //�ر����ж�ѡλ����ʾ����
    P1 = (P1 & 0xF8) | i;  //λѡ����ֵ��ֵ��P1�ڵ�3λ
    P0 = LedBuff[i];       //������������λ�õ������͵�P0��
    if (i < 5)             //��������ѭ������������������
        i++;
    else
        i = 0;
}
/* T0�жϷ��������������ܡ�����ɨ�����붨ʱ */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;   //���¼�������ֵ
    TL0 = T0RL;
	LedScan();   //LEDɨ����ʾ
    KeyScan();   //����ɨ��
    if (enBuzz)   //��������������
        BUZZ = ~BUZZ;  //��������������
    else
        BUZZ = 1;      //�رշ�����	*/
    /*switch(m)
		 {
		        case 1:OLED_P8x16Str(0,2,"COUNTS");	
			        //m=0;
					break;
			    case 2:	OLED_P8x16Str(0,4,"BELL");
				   // m=0;
					break;
			    default:
				break;
		   }  */
}


/* T2 �жϷ����� */
/*void InterruptTimer2() interrupt 5
{
      TF2=0;
     // LedScan();   //LEDɨ����ʾ
     // KeyScan();   //����ɨ��
      if (enBuzz)   //��������������
        BUZZ = ~BUZZ;  //��������������
      else
        BUZZ = 1;      //�رշ�����
      switch(m)
		 {
		        case 1:OLED_P8x16Str(0,2,"COUNTS");	
			        //m=0;
					break;
			    case 2:	OLED_P8x16Str(0,4,"BELL");
				   // m=0;
					break;
			    default:
				break;
		   } 
}  */