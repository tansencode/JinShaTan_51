#include <reg52.h>

sbit ADDR0 = P1^0;	//����ledʹ�ܶ˼�λѡ��
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

sbit Buzz=P1^6;	  //��������������
bit flagBuzzon=0;
sbit KEY_IN_1  = P2^4;  //������������
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;


bit flag300ms = 1;  //200ms��ʱ��־

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;
unsigned char i=0;
unsigned int j=0;
static unsigned int cnt=0;
unsigned char T0RH=0;
unsigned char T0RL=0;
static unsigned char keyout = 0;   //���󰴼�ɨ���������

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartRead(unsigned char *buf, unsigned char len);
extern void UartWrite(unsigned char *buf, unsigned char len);
unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
    { 0x31, 0x32, 0x33, 0x26 }, //���ּ�1�����ּ�2�����ּ�3�����ϼ�
    { 0x34, 0x35, 0x36, 0x25 }, //���ּ�4�����ּ�5�����ּ�6�������
    { 0x37, 0x38, 0x39, 0x28 }, //���ּ�7�����ּ�8�����ּ�9�����¼�
    { 0x30, 0x1B, 0x0D, 0x27 }  //���ּ�0��ESC����  �س����� ���Ҽ�
};
unsigned char KeySta[4][4] = {  //ȫ�����󰴼��ĵ�ǰ״̬
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

unsigned long pdata KeyDownTime[4][4] = {  //ÿ���������µĳ���ʱ�䣬��λms
    {0, 0, 0, 0},  {0, 0, 0, 0},  {0, 0, 0, 0},  {0, 0, 0, 0}
};
//unsigned char i, j;
void KeyDriver();

void main()
{
    unsigned char psec=0xAA;  
    EA = 1;       //ʹ�����ж�
	TMOD = 0x01; //���� T0 Ϊģʽ 1
    TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
    TL0 = 0x67;
    ET0 = 1; //ʹ�� T0 �ж�
    TR0 = 1; //���� T0
	ConfigTimer0(1); //���� T0 ��ʱ 1ms
    ConfigUART(9600); //���ò�����Ϊ 9600
	InitDS1302();     //��ʼ��ʵʱʱ��
    while (1)
    {
        KeyDriver();   //���ð�����������
		//UartDriver(); //���ô�������
		UartWrite(KeyDownTime,10);
    }
}

void ledon()		//LED��������ƺ���
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    
    ADDR3 = 1;
	ADDR2=1;
	ADDR1=1;
	ADDR0=0;
	switch(i)
	{
	     case 0:P0=0xfe;i++;break;
		 case 1:P0=0xff;i=0;break;
		 default:break;
	}

}


void delay(u16 j)
{
	while(j--);	
}

void buzz()	  //������������ֹͣ���ƺ���
{
    unsigned int cnt=0;
	switch(cnt)
	{
       case 0:
	   flagBuzzon=1;  //����������
		 ++cnt;
		   break;
		case 1:
		  flagBuzzon=0;	   //�������ر�
		   cnt=0;
		   break;
		 default:break;	       
	}
}

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    // unsigned int cnt=0;
     if (keycode == 0x31 )  //K1�������Ʒ�����������ֹͣ
    {  
	   buzz();
    }
    else if (keycode == 0x32 )  //K2��������led1������
    {
		ledon();
    }
}
/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
   // unsigned char i, j;
    static unsigned char pdata backup[4][4] = {  //����ֵ���ݣ�����ǰһ�ε�ֵ
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    static unsigned long pdata TimeThr[4][4] = {  //��������ִ�е�ʱ����ֵ
        {1000, 1000, 1000, 1000},  {1000, 1000, 1000, 1000},
        {1000, 1000, 1000, 1000},  {1000, 1000, 1000, 1000}
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
            if (KeyDownTime[i][j] > 0)            //���ִ�п�������
            {
                if (KeyDownTime[i][j] >= TimeThr[i][j])
                {                                 //�ﵽ��ֵʱִ��һ�ζ���
                    KeyAction(KeyCodeMap[i][j]);  //���ð�����������
                    TimeThr[i][j] += 200; //ʱ����ֵ����200ms����׼���´�ִ��
                }
            }
            else   //��������ʱ��λ��ֵʱ��
            {
                TimeThr[i][j] = 1000;  //�ָ�1s�ĳ�ʼ��ֵʱ��
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
            KeyDownTime[keyout][i] += 4;  //���µĳ���ʱ���ۼ�
			//UartWrite(KeyDownTime[keyout][i],10);
		//	SBUF =KeyDownTime[keyout][i] ;//�����յ������ݷ��뵽���ͼĴ���
	      //  while (!TI);			         //�ȴ������������
	     //   TI = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //����4��ɨ��ֵΪ1����4*4ms�ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
            KeySta[keyout][i] = 1;
            KeyDownTime[keyout][i] = 0;   //���µĳ���ʱ������
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

/* ���ò����� T0��ms-T0 ��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp; //��ʱ����
    tmp = 11059200 / 12; //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000; //��������ļ���ֵ
    tmp = 65536 - tmp; //���㶨ʱ������ֵ
    tmp = tmp + 33; //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8); //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0; //���� T0 �Ŀ���λ
    TMOD |= 0x01; //���� T0 Ϊģʽ 1
    TH0 = T0RH; //���� T0 ����ֵ
    TL0 = T0RL;
    ET0 = 1; //ʹ�� T0 �ж�
    TR0 = 1; //���� T0
}


/* T0�жϷ������������������ʾɨ���밴��ɨ�� */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmrls=0;
    TH0 = 0xFC; 
    TL0 = 0x67;		
	tmrls++;
	if(	tmrls>=300)
	{
	   flag300ms=1;
	}

    KeyScan();   //���ð���ɨ�躯��
	if(flagBuzzon)    //ִ�з��������л�ر�
    	Buzz=~Buzz;
	else
	    Buzz=1;
	UartRxMonitor(1);


}
/*void InterruptUART() interrupt 4
{
   	 unsigned char i;
	 SBUF = ' ' ;//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = ' ' ;//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF =KeyDownTime[keyout][i];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
}*/	 