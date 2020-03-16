#include <reg52.h>

bit flag300ms = 1;       //300ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char RxdByte = 0;  //���ڽ��յ����ֽ�

void ConfigTimer0(unsigned int ms);
unsigned char GetADCValue(unsigned char chn);
void ValueToString(unsigned char *str, unsigned long val);
extern void KeyScan();
extern void KeyDriver();

extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);

extern void Led1();
extern void Led2();
extern void Led3();
extern void Led4();
extern void Led5();
extern void Led6();
//extern void Led7();
//extern void Led8();
//extern void Led9();

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
void UsartConfiguration();
//unsigned char CNCHAR[10] = "V  ";
//void ConfigUART(unsigned int baud);

//unsigned char val;
static char str[10]={0,0,0,0,0,0,0,0};
void main()
{
    unsigned char val;
    //unsigned char str[10];
    
    EA = 1;            //�����ж�
    ConfigTimer0(10);  //����T0��ʱ10ms
	ConfigUART(9600);   //���ò�����Ϊ9600
    InitLcd1602();     //��ʼ��Һ��  
    while (1)
    {
        if (flag300ms)
        {
            flag300ms = 0;
			//��ʾͨ��0�ĵ�ѹ
			LcdShowStr(9, 0, "TJ 50");  //��ʾͨ��ָʾ
            val=GetADCValue(0);  //��ȡADCͨ��0��ת��ֵ
			LcdShowStr(0, 0, "Mode=");  //��ʾͨ��ָʾ
            ValueToString(str, val);  //תΪ�ַ�����ʽ�ĵ�ѹֵ			 
			LcdShowStr(0, 1, "Voltage=");    //��ʾ��Һ����
            LcdShowStr(8, 1, str);    //��ʾ��Һ��
			TI=1;

			KeyDriver();  //���ð�������
			UartDriver();  //���ô�������
			//ValueToString();

            //��ʾͨ��1�ĵ�ѹ
            //val = GetADCValue(1);
            //ValueToString(str, val);
            //LcdShowStr(6, 1, str);
            //��ʾͨ��3�ĵ�ѹ
           // val = GetADCValue(3);
            //ValueToString(str, val);
            //LcdShowStr(12, 1, str);
        }
    }
}
/* ��ȡ��ǰ��ADCת��ֵ��chn-ADCͨ����0~3 */
unsigned char GetADCValue(unsigned char chn)
{
    signed long val;
    
    I2CStart();
    if (!I2CWrite(0x48<<1))  //ѰַPCF8591����δӦ����ֹͣ����������0
    {
        I2CStop();
        return 0;
    }
    I2CWrite(0x40|chn);        //д������ֽڣ�ѡ��ת��ͨ��
    I2CStart();
    I2CWrite((0x48<<1)|0x01);  //ѰַPCF8591��ָ������Ϊ������    
    I2CReadACK();              //�ȿն�һ���ֽڣ��ṩ����ת��ʱ��
    val = I2CReadNAK();        //��ȡ�ո�ת�����ֵ
    I2CStop();
    
    return val;
}
/* ADCת��ֵתΪʵ�ʵ�ѹֵ���ַ�����ʽ��str-�ַ���ָ�룬val-ADת��ֵ */
void ValueToString(unsigned char *str, unsigned long val)
{
    //��ѹֵ=ת�����*2.5V/255��ʽ�е�25������һλʮ����С��
    val = (val*2500) / 255;
    str[0] = (val/1000)%10 + '0';  //����λ�ַ�

    str[1] = '.';             //С����

    str[2] = (val/100)%10 + '0';  //С��λ�ַ�
	
	str[3] = (val/10)%10 +'0';
	
	str[4] = (val%10) + '0';

    str[5] = 'V';             //��ѹ��λ

    str[6] = '\0';            //������
}

/* �ڴ�ȽϺ������Ƚ�����ָ����ָ����ڴ������Ƿ���ͬ��
   ptr1-���Ƚ�ָ��1��ptr2-���Ƚ�ָ��2��len-���Ƚϳ���
   ����ֵ-�����ڴ�������ȫ��ͬʱ����1����ͬ����0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //�������������ʱ���̷���0
        {
            return 0;
        }
    }
    return 1;  //�Ƚ���ȫ���������ݶ�����򷵻�1
}
/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
   buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char code cmd0[] = "1";   //������������
    unsigned char code cmd1[] = "2";  //�ط���������
    unsigned char code cmd2[] = "3";  //�ַ�����ʾ����
    unsigned char code cmdLen[] = {          //����Ȼ��ܱ�
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {         //����ָ����ܱ�
        &cmd0[0],  &cmd1[0],  &cmd2[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  //���������б�������ͬ����
    {
        if (len >= cmdLen[i])  //���Ƚ��յ������ݳ���Ҫ��С�������
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  //�Ƚ���ͬʱ�˳�ѭ��
            {
                break;
            }
        }
    }
    switch (i)  //ѭ���˳�ʱi��ֵ���ǵ�ǰ���������ֵ
    {
        case 0:
            //flagBuzzOn = 1; //����������
			Led1();
            break;
        case 1:
            //flagBuzzOn = 0; //�رշ�����
			Led2();
            break;
        case 2:
           // buf[len] = '\0';  //Ϊ���յ����ַ�����ӽ�����
           // LcdShowStr(0, 0, buf+cmdLen[2]);  //��ʾ�������ַ���
           // i = len - cmdLen[2];              //������Ч�ַ�����
           // if (i < 16)  //��Ч�ַ�����16ʱ�����Һ���ϵĺ����ַ�λ
           // {
           //     LcdAreaClear(i, 0, 16-i);
           // }
		   Led3();
            break;
        default:   //δ�ҵ��������ʱ�����ϻ����͡������������ʾ
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r';  //��Ч���ִ�к���ԭ����֮֡�����
    buf[len++] = '\n';  //�س����з��󷵻ظ���λ������ʾ��ִ��
    UartWrite(buf, len);
}

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    if  (keycode == 0x31)   //�����ַ�
    {
	    
		LcdShowStr(5, 0,"1");
        Led1();
    }
	 else if (keycode == 0x32)  //���ϼ���+
    {
	    LcdShowStr(5, 0,"2");
        Led2();
    }
	 else if (keycode == 0x33)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"3");
        Led3();
    }
	else if (keycode == 0x34)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"4");
        Led4();
    }
	else if (keycode == 0x35)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"5");
        Led5();
    }
	else if (keycode == 0x36)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"6");
        Led6();
    }
	/*else if (keycode == 0x37)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"7");
        Led7();
    }
	else if (keycode == 0x38)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"8");
        Led8();
    }
		else if (keycode == 0x39)  //���ϼ���+
    {
	   	LcdShowStr(5, 0,"9");
        Led9();
    } */
    else if (keycode == 0x26)  //���ϼ���+
    {
        //OprtKeyAction(0);
    }
    else if (keycode == 0x28)  //���¼���-
    {
       // OprtKeyAction(1);
    }
    else if (keycode == 0x25)  //�������*
    {
        //OprtKeyAction(2);
    }
    else if (keycode == 0x27)  //���Ҽ�����
    {
       // OprtKeyAction(3);
    }
    else if (keycode == 0x0D)  //�س�����������
    {
        //GetResult();
    }
    else if (keycode == 0x1B)  //Esc�������
    {
        P0=0xFF;
    }
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

/* �������ú�����baud-ͨ�Ų����� */
/*void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
}  */

/* T0�жϷ�������ִ��300ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	KeyScan();   //����ɨ��
	UartRxMonitor(1);  //���ڽ��ռ��
    tmr300ms++;
    if (tmr300ms >= 30)  //��ʱ300ms
    {
        tmr300ms = 0;
        flag300ms = 1;
    }
}

/*void InterruptUART() interrupt 4
{
   	 //unsigned char i;
	 SBUF = ' ' ;//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = ' ' ;//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = str[0];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
   	 SBUF =str[1];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = str[2];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = str[3];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = str[4];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = str[5];//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;
	 SBUF = ' ';//�����յ������ݷ��뵽���ͼĴ���
	 while (!TI);			         //�ȴ������������
	 TI = 0;		 
	 //for(i=0; i<6; i++)
	 //{
	 //	SBUF = CNCHAR[i];//�����յ������ݷ��뵽���ͼĴ���
	//	while (!TI);			         //�ȴ������������
	//	TI = 0;
	// } 
    //SBUF ='0';  //���յ���������ֱ�ӷ��أ�����-"echo" 
	//SBUF = str;//�����յ������ݷ��뵽���ͼĴ���                 
    //if (TI)  //�ֽڷ������
    //{
       // TI = 0;  //�ֶ����㷢���жϱ�־λ
    //}
} */