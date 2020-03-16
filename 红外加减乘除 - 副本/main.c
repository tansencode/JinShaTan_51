#include <reg52.h>

unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

sbit BUZZ=P1^6;
bit flagBuzzOn=0;
unsigned char step = 0;  //��������
unsigned char oprt = 0;  //��������
//static signed long num1 = 0;    //������1
//static signed long num2 = 0;    //������2
static float result2 = 0;  //������
static float result = 0.000000;  //������
static signed long res = 0;  //������
//signed char result[16];

static  float num1 = 0;    //������1
static  float num2 = 0;    //������2
//static  float result = 0;  //������

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdFullClear();
extern void LcdWriteDat(unsigned char dat);
extern bit irflag;
extern unsigned char ircode[16];
extern void InitInfrared();
void NumKeyAction1();
void ConfigTimer0(unsigned int ms);
void OprtKeyAction(unsigned char type);
void Reset();
void GetResult();
void NumKeyAction(unsigned char n);
void NumKeyAction2();
static unsigned char len;
static unsigned char str[16];
static int j=0;

void main()
{
    static signed int i=0;
    EA = 1;      //�����ж�
    InitInfrared();   //��ʼ�����⹦��
    ConfigTimer0(2);  //����T0��ʱ2ms
    InitLcd1602();    //��ʼ��Һ��
    LcdShowStr(15, 1, "0");  //��ʼ��ʾһ������0
    while (1)
    {
        if (irflag)  //���յ���������ʱˢ����ʾ
        {
            irflag = 0;
            // if ((ircode[2] >= '0')&&(ircode[2] <= '9'))   //0x09Ϊ+���룬��ת1Ȧ
		//	{
              //    NumKeyAction(ircode[2] - '0');
            //}
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
			else if (ircode[2] == 0x07)		//  -1
			{
			  	//OprtKeyAction(1);
				//NumKeyAction(-0x01);
				 NumKeyAction2();
				 //LcdWriteDat(0x2E);
				 //LcdShowStr(15, 1, ".");     //��ʾ��Һ���ڶ�����
			}
			else if(ircode[2] == 0x09) 	// '+' ���Ӻ�
            {
                 OprtKeyAction(0);
            }
            else if (ircode[2] == 0x15)  //	'-' ������
            {
                OprtKeyAction(1);
            }
			else if (ircode[2] == 0x0D)  //	'*'	���˺�
            {
                OprtKeyAction(2);
			}
			else if (ircode[2] == 0x19)  // '/'	 ������
            {
                OprtKeyAction(3);
			}

	       	else if (ircode[2] == 0x43)   //�س������ó����
            {
                GetResult();
			}
			else if(ircode[2] == 0x44)
			{
			    NumKeyAction1();
			}	 
			else if ((ircode[2] == 0x40)|| (ircode[2] == 0x45)) //Ens�����
            {
               Reset();
               LcdShowStr(15, 1, "0");
			}
		    else if (ircode[2] == 0x46)  //  ����������
            {  
			     flagBuzzOn=1;
			}
			else if (ircode[2] == 0x47)  //  �������ر�
            {  
			     flagBuzzOn=0;
			}
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

/* ��ʾ���������ʾλ��y�����������type */
void ShowOprt(unsigned char y, unsigned char type)
{
    switch (type)
    {
        case 0: LcdShowStr(0, y, "+"); break;  //0����+
        case 1: LcdShowStr(0, y, "-"); break;  //1����-
        case 2: LcdShowStr(0, y, "*"); break;  //2����*
        case 3: LcdShowStr(0, y, "/"); break;  //3����/
        default: break;
    }
}
/* ��������λ���������ֵ�������Ļ��ʾ */
void Reset()
{
    num1 = 0;
    num2 = 0;
    step = 0;
    LcdFullClear();
}
/* ���ּ�����������n-�����������ֵ */
void NumKeyAction(signed char n)
{
   // unsigned char len;
    //unsigned char str[16];
    
    if (step > 1)  //���������ɣ������¿�ʼ�µļ���
    {
        Reset();
    }
    if (step == 0)  //�����һ������
    {
	    if(j=0)
		{
        num1 = num1*(10.0) + n;             //������ֵ�ۼӵ�ԭ��������
        str[len]=num1+'0';
		len = LongToString(str, num1);  //����ֵת��Ϊ�ַ���
	
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
		}
		else if(j=1)
		{
        num1 = num1*(10.0) + n;             //������ֵ�ۼӵ�ԭ��������
		str[0]=num1/10+'0';
	    str[1]='.';
		str[2]=num1 / 10+'0';
        //len = LongToString(str, num1);  //����ֵת��Ϊ�ַ���
	
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
		j=0;
		}

	
    }
    else            //����ڶ�������
    {
        num2 = num2*(10.0) + n;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString(str, num2);  //����ֵת��Ϊ�ַ���
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
}

void NumKeyAction1()
{
    //unsigned char len;
    //unsigned char str[16];
    
    if (step > 1)  //���������ɣ������¿�ʼ�µļ���
    {
        Reset();
    }
    if (step == 0)  //�����һ������
    {
        num1 = num1/10;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString(str, num1);  //����ֵת��Ϊ�ַ���
		 LcdAreaClear(1, 1, str);        //����ڶ����м���ַ�λ
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
    else            //����ڶ�������
    {
        num2 = num2/10;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString(str, num2);  //����ֵת��Ϊ�ַ���
		 LcdAreaClear(1, 1,str);        //����ڶ����м���ַ�λ
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
}

void NumKeyAction2()
{
    //unsigned char len;
    //unsigned char str[16];
    
    if (step > 1)  //���������ɣ������¿�ʼ�µļ���
    {
        Reset();
    }
    if (step == 0)  //�����һ������
    {
        //num1 = num1/10;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString(str, num1);  //����ֵת��Ϊ�ַ���
		str[len++]='.';
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
		j=1;
		//len=len+1;
		//num1=num1*100;
    }
    else            //����ڶ�������
    {
        //num2 = num2/10;             //������ֵ�ۼӵ�ԭ��������
			str[len++]='.';
        len = LongToString(str, num2);  //����ֵת��Ϊ�ַ���
	//	str[len++]='.';
		LcdAreaClear(1, 1,str);        //����ڶ����м���ַ�λ
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
		//len=len+1;
    }
}


/*unsigned char LongToString1(unsigned char *str, signed long dat)
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
    return len;   //�����ַ�������
}		 */

/*void NumKeyAction1(signed char n)
{
    unsigned char len;
    unsigned char str[16];
    static unsigned int i=0;
    if (step > 1)  //���������ɣ������¿�ʼ�µļ���
    {
        Reset();
    }
    if (step == 0)  //�����һ������
    {
	    switch(i)
		{
		case 0:num1=n;break;
		case 1:
          num1 = num1*n;             //������ֵ�ۼӵ�ԭ��������
		  break;
		}
        len = LongToString1(str, num1);  //����ֵת��Ϊ�ַ���
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
    else            //����ڶ�������
    {
        num2 = num2*10 + n;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString1(str, num2);  //����ֵת��Ϊ�ַ���
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
}		 */

/* ����������������������������type */
void OprtKeyAction(unsigned char type)
{
    unsigned char len;
    unsigned char str[16];
    
    if (step == 0)  //�ڶ���������δ����ʱ��Ӧ������֧����������
    {
        len = LongToString(str, num1); //��һ������ת��Ϊ�ַ���
        LcdAreaClear(0, 0, 16-len);    //�����һ����ߵ��ַ�λ
        LcdShowStr(16-len, 0, str);    //�ַ���������ʾ�ڵ�һ��
        ShowOprt(1, type);             //�ڵڶ�����ʾ������
        LcdAreaClear(1, 1, 14);        //����ڶ����м���ַ�λ
        LcdShowStr(15, 1, "0");        //�ڵڶ������Ҷ���ʾ0
        oprt = type;                   //��¼��������
        step = 1;
    }
}

/* ���������� */
void GetResult()
{
    unsigned char len;
    unsigned char str[16];
    
    if (step == 1) //�ڶ�������������ʱ��ִ�м���
    {
        step = 2;
        switch (oprt)  //������������ͼ�������δ�����������
        {
            case 0: result = num1 + num2; break;
            case 1: result = num1 - num2; break;
            case 2: result = num1 * num2; break;
            case 3: result = num1 / num2; break;
            default: break;
        }
     /* result1 =result1*1000000;
      res=result1;
	  result[1]=' ';
      result[2]=' ';
	  result[3]=' ';
	  result[4]=res/10000000000+0x30;
	  result[5]=res/1000000000%10+0x30;
	  result[6]=res/100000000%10+0x30;
	  result[7]=res/10000000%10+0x30;
	  result[8]=res/1000000%10+0x30;
	  result[9]='.';		  
	  result[10]=res/100000%10+0x30;
	  result[11]=res/10000%10+0x30;
	  result[12]=res/1000%10+0x30;
	  result[13]=res/100%10+0x30;
	  result[14]=res/10%10+0x30;
	  result[15]=res%10+0x30;  */

        len = LongToString(str, num2);  //ԭ�ڶ����������������ʾ����һ��
        ShowOprt(0, oprt);
        LcdAreaClear(1, 0, 16-1-len);
        LcdShowStr(16-len, 0, str);
        len = LongToString(str, result);  //�������͵Ⱥ���ʾ�ڵڶ���
        LcdShowStr(0, 1, "=");
        LcdAreaClear(1, 1, 16-1-len);
        LcdShowStr(16-len, 1, str);
    }
}

/* �����������������angle-��ת���ĽǶ� */
/*void StartMotor(signed long angle)
{
    //�ڼ���ǰ�ر��жϣ���ɺ��ٴ򿪣��Ա����жϴ�ϼ�����̶���ɴ���
    EA = 0;
    beats = (angle * 4076) / 360; //ʵ��Ϊ4076��ת��һȦ
    EA = 1;
}	 */
/* �������ֹͣ���� */
/*void StopMotor()
{
    EA = 0;
    beats = 0;
    EA = 1;
}  */
/* ���ת�����ƺ��� */
/*void TurnMotor()
{
    unsigned char tmp;  //��ʱ����
    static unsigned char index = 0;  //�����������
    unsigned char code BeatCode[8] = {  //����������Ķ�Ӧ��IO���ƴ���
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };
    
    if (beats != 0)  //��������Ϊ0�����һ����������
    {
        if (beats > 0)  //����������0ʱ��ת
        {
            index++;               //��תʱ���������������
            index = index & 0x07;  //��&����ʵ�ֵ�8����
            beats--;               //��תʱ���ļ����ݼ�
        }
        else            //������С��0ʱ��ת
        {
            index--;               //��תʱ������������ݼ�
            index = index & 0x07;  //��&����ͬ������ʵ�ֵ�-1ʱ��7
            beats++;               //��תʱ���ļ�������
        }
        tmp = P1;                    //��tmp��P1�ڵ�ǰֵ�ݴ�
        tmp = tmp & 0xF0;            //��&���������4λ
        tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д����4λ
        P1  = tmp;                   //�ѵ�4λ�Ľ��Ĵ���͸�4λ��ԭֵ�ͻ�P1
    }
    else  //������Ϊ0��رյ�����е���
    {
        P1 = P1 | 0x0F;
    }
}	 */
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 13;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�������ɨ����ʾ */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	if(flagBuzzOn)
	   BUZZ=~BUZZ;
	else
	  BUZZ=1;
    //TurnMotor(); //ִ�е������
}
