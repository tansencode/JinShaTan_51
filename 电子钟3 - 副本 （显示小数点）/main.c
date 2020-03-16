#include <reg52.h>
//#define  _LED_BUZZER_C
#define _TIME_C
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
/*unsigned char LedBuff1[] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};	*/
unsigned char Leddp[]={
0x7F,0x7F,0x7F,0x7F,0x7F,0x7F};

//struct sLedBuff ledBuff;
struct sTime CurTime;
struct sTime {  //����ʱ��ṹ�嶨��
    unsigned int  year;
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char week;
};

bit flag200ms = 1;  //200ms��ʱ��־
struct sTime bufTime;  //����ʱ�仺����
unsigned char setIndex = 0;  //ʱ����������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned int index;
unsigned int num;
unsigned int point;

void ConfigTimer0(unsigned int ms);
void RefreshTimeShow();
void RefreshTime();
extern void InitDS1302();
extern void GetRealTime(struct sTime *time);
extern void SetRealTime(struct sTime *time);
extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdSetCursor(unsigned char x, unsigned char y);
extern void LcdOpenCursor();
extern void LcdCloseCursor();


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
    unsigned char psec=0xAA;  //�뱸�ݣ���ֵAAȷ���״ζ�ȡʱ����ˢ����ʾ
    EA = 1;           //�����ж�
	ENLED = 0; //ѡ������ܽ�����ʾ
    ADDR3 = 1;
	ET0 = 1; //ʹ�� T0 �ж�
	TR0 = 1; //���� T0
    ConfigTimer0(1);  //T0��ʱ1ms
	P0=0xFF;
    InitDS1302();     //��ʼ��ʵʱʱ��
    InitLcd1602();    //��ʼ��Һ��
	 RefreshTime();
    
    //��ʼ����Ļ�Ϲ̶����������
    //LcdShowStr(3, 0, "20  -  -  ");
    
    while (1)
    {
        KeyDriver();  //���ð�������
        if (flag200ms && (setIndex == 0))
        {                             //ÿ��200ms��δ��������״̬ʱ��
            flag200ms = 0;
			
			RefreshTime();
			P0=0xFF;
            GetRealTime(&bufTime);    //��ȡ��ǰʱ��
            if (psec != bufTime.sec)  //��⵽ʱ���б仯ʱˢ����ʾ
            {
               // RefreshTimeShow();
				RefreshTime();
                psec = bufTime.sec;   //�õ�ǰֵ�����ϴ�����
			
            }
        }
    }
}

/* ��һ��BCD���ֽ���ʾ����Ļ�ϣ�(x,y)-��Ļ��ʼ���꣬bcd-����ʾBCD�� */
/*void ShowBcdByte(unsigned char x, unsigned char y, unsigned char bcd)
{
    unsigned char str[4];
    
    str[0] = (bcd >> 4) ;
    str[1] = (bcd&0x0F);
    ShowLedNumber(index,str,0);
	RefreshTime();

}	 */

/* ��ȡ��ǰ����ʱ�䣬��ˢ��ʱ������ڵ���ʾ */
void RefreshTime()
{
    GetRealTime(&bufTime);                  //��ȡ��ǰ����ʱ��
    ShowLedNumber(5,bufTime.hour>>4, 0);   //ʱ
    ShowLedNumber(4, bufTime.hour&0xF,1);
    ShowLedNumber(3, bufTime.min>>4,  0);   //��
    ShowLedNumber(2, bufTime.min&0xF, 1);
    ShowLedNumber(1, bufTime.sec>>4,  0);   //��
    ShowLedNumber(0, bufTime.sec&0xF, 0);
} 

void RefreshTime0()
{
   // GetRealTime(&bufTime);                  //��ȡ��ǰ����ʱ��
    ShowLedNumber(5,bufTime.hour>>4, 0);   //ʱ
    ShowLedNumber(4, bufTime.hour&0xF,0);
    ShowLedNumber(3, bufTime.min>>4,  0);   //��
    ShowLedNumber(2, bufTime.min&0xF, 0);
    ShowLedNumber(1, bufTime.sec>>4,  0);   //��
    ShowLedNumber(0, bufTime.sec&0xF, 0);
} 
/* ����ˢ�º�����ops-ˢ��ѡ�Ϊ0ʱֻ�����ڱ仯��ˢ�£���0������ˢ�� */
/*void RefreshDate(uint8 ops)
{
    uint8 pdata str[12];
    static uint8 backup = 0;
    
    if ((backup!=CurTime.day) || (ops!=0))
    {
        str[0] = ((CurTime.year>>12) & 0xF) + '0';  //4λ�����
        str[1] = ((CurTime.year>>8) & 0xF) + '0';
        str[2] = ((CurTime.year>>4) & 0xF) + '0';
        str[3] = (CurTime.year & 0xF) + '0';
        str[4] = '-';                        //�ָ���
        str[5] = (CurTime.mon >> 4) + '0';   //�·�
        str[6] = (CurTime.mon & 0xF) + '0';
        str[7] = '-';                        //�ָ���
        str[8] = (CurTime.day >> 4) + '0';   //����
        str[9] = (CurTime.day & 0xF) + '0';
        str[10] = '\0';         //�ַ���������
        LcdShowStr(0, 0, str);  //��ʾ��Һ����
        backup = CurTime.day;   //ˢ���ϴ�����ֵ
    }
} */
/* ˢ������ʱ�����ʾ */
/*void RefreshTimeShow()
{
    ShowBcdByte(5,  0, bufTime.year);
    ShowBcdByte(8,  0, bufTime.mon);
    ShowBcdByte(11, 0, bufTime.day);
    ShowBcdByte(4,  1, bufTime.hour);
    ShowBcdByte(7,  1, bufTime.min);
    ShowBcdByte(10, 1, bufTime.sec);
} */
/*void RefreshTimeShow()
{
    ShowBcdByte(5,  0, bufTime.year);
    ShowBcdByte(8,  0, bufTime.mon);
    ShowBcdByte(11, 0, bufTime.day);
    ShowBcdByte(4,  1, bufTime.hour);
    ShowBcdByte(7,  1, bufTime.min);
    ShowBcdByte(10, 1, bufTime.sec);
} */  

/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
   static unsigned char i = 0; //��̬ɨ�������
   P0 = 0xFF; //��ʾ����
   P1=(P1&0xF8)|i;		 //�ص㣬P1�ڵ���λ�Ƕ���ADDR0~R2�ģ�����λѡ
   P0=LedBuff[i];
   if(i<5)
     i++;
   else
     i=0;
}


/* ˢ�µ�ǰ����λ�Ĺ��ָʾ */
/*void RefreshSetShow()
{
    switch (setIndex)
    {
        case 1:  LcdSetCursor(5,  0); break;
        case 2:  LcdSetCursor(6,  0); break;
        case 3:  LcdSetCursor(8,  0); break;
        case 4:  LcdSetCursor(9,  0); break;
        case 5:  LcdSetCursor(11, 0); break;
        case 6:  LcdSetCursor(12, 0); break;
        case 7:  LcdSetCursor(4,  1); break;
        case 8:  LcdSetCursor(5,  1); break;
		case 9:  LcdSetCursor(7,  1); break;
		case 10: LcdSetCursor(8,  1); break;
		case 11: LcdSetCursor(10,  1); break;
		case 12: LcdSetCursor(11,  1); break;
        default:  break;
    }
}  */
/* ����һ��BCD��ĸ�λ */
unsigned char IncBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) < 0x90)
        bcd += 0x10;
    else
        bcd &= 0x0F;
    
    return bcd;
}
/* ����һ��BCD��ĵ�λ */
unsigned char IncBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) < 0x09)
        bcd += 0x01;
    else
        bcd &= 0xF0;
    
    return bcd;
}
/* �ݼ�һ��BCD��ĸ�λ */
unsigned char DecBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) > 0x00)
        bcd -= 0x10;
    else
        bcd |= 0x90;
    
    return bcd;
}
/* �ݼ�һ��BCD��ĵ�λ */
unsigned char DecBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) > 0x00)
        bcd -= 0x01;
    else
        bcd |= 0x09;
    
    return bcd;
}
/* ����ʱ�䵱ǰ����λ��ֵ */
void IncSetTime()
{
    switch (setIndex)
    {
        //case 1:  bufTime.year = IncBcdHigh(bufTime.year); break;
        //case 2:  bufTime.year = IncBcdLow(bufTime.year);  break;
        //case 3:  bufTime.mon  = IncBcdHigh(bufTime.mon);  break;
        //case 4:  bufTime.mon  = IncBcdLow(bufTime.mon);   break;
        //case 5:  bufTime.day  = IncBcdHigh(bufTime.day);  break;
        //case 6:  bufTime.day  = IncBcdLow(bufTime.day);   break;
        case 1:  bufTime.hour = IncBcdHigh(bufTime.hour); ShowLedNumber(5,bufTime.hour>>4, 1);  break;
        case 2:  bufTime.hour = IncBcdLow(bufTime.hour); ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3:  bufTime.min  = IncBcdHigh(bufTime.min); ShowLedNumber(3, bufTime.min>>4,  1);  break;
        case 4:  bufTime.min  = IncBcdLow(bufTime.min); ShowLedNumber(2, bufTime.min&0xF, 1);  break;
        case 5:  bufTime.sec  = IncBcdHigh(bufTime.sec);ShowLedNumber(1, bufTime.sec>>4,  1);   break;
        case 6:  bufTime.sec  = IncBcdLow(bufTime.sec); ShowLedNumber(0, bufTime.sec&0xF, 1);  break;
        default:  break;
    }
    //RefreshTimeShow();
	//RefreshTimeShow();
    // RefreshTime();
}
/* �ݼ�ʱ�䵱ǰ����λ��ֵ */
void DecSetTime()
{
    switch (setIndex)
    {
        //case 1:  bufTime.year = DecBcdHigh(bufTime.year); break;
        //case 2:  bufTime.year = DecBcdLow(bufTime.year);  break;
        //case 3:  bufTime.mon  = DecBcdHigh(bufTime.mon);  break;
        //case 4:  bufTime.mon  = DecBcdLow(bufTime.mon);   break;
        //case 5:  bufTime.day  = DecBcdHigh(bufTime.day);  break;
        //case 6:  bufTime.day  = DecBcdLow(bufTime.day);   break;
        case 1:  bufTime.hour = DecBcdHigh(bufTime.hour);ShowLedNumber(5,bufTime.hour>>4, 1); break;
        case 2:  bufTime.hour = DecBcdLow(bufTime.hour); ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3:  bufTime.min  = DecBcdHigh(bufTime.min); ShowLedNumber(3, bufTime.min>>4,  1); break;
        case 4:  bufTime.min  = DecBcdLow(bufTime.min);   ShowLedNumber(2, bufTime.min&0xF, 1); break;
        case 5:  bufTime.sec  = DecBcdHigh(bufTime.sec); ShowLedNumber(1, bufTime.sec>>4,  1);  break;
        case 6:  bufTime.sec  = DecBcdLow(bufTime.sec);   ShowLedNumber(0, bufTime.sec&0xF, 1); break;
        default:  break;
    }
    //RefreshTimeShow();
	//RefreshTimeShow();
   //	RefreshTime();
}
/* ����ʱ������λ */
void RightShiftTimeSet()
{
        unsigned char *p;
    	if(setIndex!=0)
		  {
        if (setIndex <6)
         setIndex++;
		  
        else
            setIndex = 1;
		}
			switch(setIndex)
		{ 
		case 1: RefreshTime0(); ShowLedNumber(5,bufTime.hour>>4, 1); break;
        case 2: RefreshTime0();ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3: RefreshTime0();ShowLedNumber(3, bufTime.min>>4,  1); break;
        case 4: RefreshTime0(); ShowLedNumber(2, bufTime.min&0xF, 1); break;
        case 5: RefreshTime0();ShowLedNumber(1, bufTime.sec>>4,  1);  break;
        case 6: RefreshTime0();ShowLedNumber(0, bufTime.sec&0xF, 1);break;
           // setIndex--;
		  }
        //RefreshSetShow();
		//RefreshTime();
}

/* ����ʱ������λ */
void LeftShiftTimeSet()
{
    	if(setIndex!=0)
		{
        if (setIndex >1)
		{
	
		setIndex--;	
	
		  }
        else
            setIndex = 6;
		  	switch(setIndex)
		{ 
		case 1:  RefreshTime0();ShowLedNumber(5,bufTime.hour>>4, 1); break;
        case 2:  RefreshTime0();ShowLedNumber(4, bufTime.hour&0xF,1); break;
        case 3:  RefreshTime0();ShowLedNumber(3, bufTime.min>>4,  1); break;
        case 4:  RefreshTime0(); ShowLedNumber(2, bufTime.min&0xF, 1); break;
        case 5:  RefreshTime0();ShowLedNumber(1, bufTime.sec>>4,  1);  break;
        case 6:  RefreshTime0(); ShowLedNumber(0, bufTime.sec&0xF, 1);break;
           // setIndex--;
		  }
		}
       // RefreshSetShow();
	   //RefreshTime();
}
/* ����ʱ������״̬ */
void EnterTimeSet()
{
    //static unsigned char i = 5; //��̬ɨ�������
    setIndex = 1;       //��������������Ϊ5�����ɽ�������״̬
	ShowLedNumber(4, bufTime.hour&0xF,0);
	ShowLedNumber(2, bufTime.min&0xF, 0);
	ShowLedNumber(5,bufTime.hour>>4, 1);   //ʱ
	//ShowLedNumber(5,bufTime.hour>>4, 1);
	//ShowLedNumber(5,bufTime.hour>>4, 1);   //ʱ
    //RightShiftTimeSet(); //�������ֳɵ����Ʋ����Ƶ�λ��1�������ʾˢ��
    //LcdOpenCursor();    //�򿪹����˸Ч��
}
/* �˳�ʱ������״̬��save-�Ƿ񱣴浱ǰ���õ�ʱ��ֵ */
void ExitTimeSet(bit save)
{
    setIndex = 0;     //��������������Ϊ0�������˳�����״̬
    if (save)         //�豣��ʱ���ѵ�ǰ����ʱ��д��DS1302
    {
        SetRealTime(&bufTime);
    }
    //LcdCloseCursor(); //�رչ����ʾ
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    static unsigned char i = 0; //��̬ɨ�������
    if  ((keycode>='0') && (keycode<='9'))  //�����в���Ӧ�ַ���
    {
    }
    else if (keycode == 0x26)  //���ϼ���������ǰ����λ��ֵ
    {
        IncSetTime();
		//RefreshTime();
    }
    else if (keycode == 0x28)  //���¼����ݼ���ǰ����λ��ֵ
    {
        DecSetTime();
		//RefreshTime();
    }
    else if (keycode == 0x25)  //������������л�����λ
    {
        LeftShiftTimeSet();
    }
    else if (keycode == 0x27)  //���Ҽ��������л�����λ
    {
	    
        RightShiftTimeSet();
    }
    else if (keycode == 0x0D)  //�س�������������ģʽ/���õ�ǰ����ֵ
    {
        if (setIndex == 0)  //����������״̬ʱ����������״̬
        {
            EnterTimeSet();
        }
        else                //�Ѵ�������״̬ʱ������ʱ�䲢�˳�����״̬
        {
            ExitTimeSet(1);
        }
    }
    else if (keycode == 0x1B)  //Esc����ȡ����ǰ����
    {
        ExitTimeSet(0);
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
/* T0�жϷ�������ִ�а���ɨ���200ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
	static int i=0;

    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
/*	if(ENLED==0)
	{
	  P0=0xff;
	  P1=(P1&0xF0)|i;
	  P0=*((int data*)&LedBuff+i);
	  if(i<(sizeof(LedBuff)-1))
	  i++;
	  else
	  i=0;
	}
	*/
	KeyScan();   //����ɨ��
	LedScan(); //�����������ʾɨ�躯��
    tmr200ms++;
    if (tmr200ms >= 200)  //��ʱ200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }

}

/* T2�жϷ�������LED��̬ɨ�衢���������� */
/*void InterruptTimer2() interrupt 5
{
    static i = 0;  //LEDλѡ����
    
    TF2 = 0;  //����T2�жϱ�־
    //ȫ��LED��̬ɨ����ʾ
    if (ENLED == 0)  //LEDʹ��ʱ�Ž��ж�̬ɨ��
    {
        P0 = 0xFF;                       //�ر����ж�ѡλ����ʾ����
        P1 = (P1 & 0xF0) | i;            //λѡ����ֵ��ֵ��P1�ڵ�4λ
        P0 = *((data*)&ledBuff+i); //������������λ�õ������͵�P0��
        if (i < (sizeof(ledBuff)-1))     //��������ѭ������������������
            i++;
        else
            i = 0;
    }
}  */