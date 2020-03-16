#include <reg52.h>

bit flag300ms = 1;       //300ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char RxdByte = 0;  //���ڽ��յ����ֽ�

void ConfigTimer0(unsigned int ms);
unsigned char GetADCValue(unsigned char chn);
void ValueToString(unsigned char *str, unsigned long val);
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
void UsartConfiguration();
unsigned char CNCHAR[10] = "V  ";
void ConfigUART(unsigned int baud);

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
            val=GetADCValue(0);  //��ȡADCͨ��0��ת��ֵ
			LcdShowStr(0, 0, "50  TJ");  //��ʾͨ��ָʾ
            ValueToString(str, val);  //תΪ�ַ�����ʽ�ĵ�ѹֵ			 
			LcdShowStr(0, 1, "Voltage=");    //��ʾ��Һ����
            LcdShowStr(8, 1, str);    //��ʾ��Һ��
			TI=1;
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

/* ADCת��ֵתΪʵ�ʵ�ѹֵ���ַ�����ʽ��str-�ַ���ָ�룬val-ADת��ֵ */
/*void ValueToString(unsigned char *str, unsigned long val)
{
    unsigned char i;
	//unsigned char str1[]= {0, 0, 0, 0, 0, 0}; 
    //��ѹֵ=ת�����*2.5V/255��ʽ�е�25������һλʮ����С��
    val = (val*2500) / 255;
    str[0] = (val/1000)%10 + '0';  //����λ�ַ�
	//SBUF = str[0];//�����յ������ݷ��뵽���ͼĴ���
	//while (!TI);			         //�ȴ������������
	//TI = 0;

    str[1] = '.';             //С����
	//SBUF = '.';//�����յ������ݷ��뵽���ͼĴ���
//	while (!TI);			         //�ȴ������������
//	TI = 0;

    str[2] = (val/100)%10 + '0';  //С��λ�ַ�
	//SBUF = str[2];//�����յ������ݷ��뵽���ͼĴ���
	//while (!TI);			         //�ȴ������������
	//TI = 0;
	str[3] = (val/10)%10 +'0';
	//SBUF = str[3];//�����յ������ݷ��뵽���ͼĴ���
//	while (!TI);			         //�ȴ������������
//	TI = 0;
	str[4] = (val%10) + '0';
	//SBUF = str[4];//�����յ������ݷ��뵽���ͼĴ���
	//while (!TI);			         //�ȴ������������
	//TI = 0;
    str[5] = 'V';             //��ѹ��λ
	//SBUF = str[5];//�����յ������ݷ��뵽���ͼĴ���
	//while (!TI);			         //�ȴ������������
	//TI = 0;
    str[6] = '\0';            //������
//	TI=1;
	
		 
	/*SBUF = str[0];//�����յ������ݷ��뵽���ͼĴ���
	while (!TI);			         //�ȴ������������
	TI = 0;
	SBUF = '.';//�����յ������ݷ��뵽���ͼĴ���
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
	TI = 0;	 */
//	for(i=0; i<6; i++)
//	{
//	 	SBUF = CNCHAR[i];//�����յ������ݷ��뵽���ͼĴ���
//		while (!TI);			         //�ȴ������������
//		TI = 0;
//	} 

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
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
} 

/* T0�жϷ�������ִ��300ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr300ms++;
    if (tmr300ms >= 30)  //��ʱ300ms
    {
        tmr300ms = 0;
        flag300ms = 1;
    }
}

void InterruptUART() interrupt 4
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
}