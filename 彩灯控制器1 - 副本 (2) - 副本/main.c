#include <reg52.h>
#include <intrins.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;

sbit ADDR3=P1^3;
sbit ENLED=P1^4;
sbit BUZZ = P1^6; //��������������
bit flag300ms = 1;       //300ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

bit flagUart=0;
bit d=0;
static unsigned int cnt=0;
unsigned int t=0;
unsigned int t0=0;
static unsigned char m=1;

static unsigned char c=0;
static unsigned char v=0;

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
extern void Led7();
extern void Led8();
extern void Led9();
extern void Led0();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
void UsartConfiguration();
unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = { //�����+���� LED ��ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


unsigned char code L1[7]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB};
    //��ʵ���Ƕ�����һ������״̬��,���м䵽���ߣ��������ߵ��м�,Ȼ��P0ѭ��������
    //1110 0111  ,  1101 1011 , 1011 1101  ,0111 1110, 1011 1101, 1101 1011, 1110 0111
unsigned char code L2[30]={0xE7,0xDB,0xBD,0x7E,0xBD,0xDB,
                         0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,
						 0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xEF}; //22
unsigned char code L3[50]={0xEF,0xDF,0xBF,0x7F, 0xF7,0xFB,0xFD,0xFE,
                         0xEF,0xDF,0xBF,0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,
                         0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xBF,0xDF};  //28	
unsigned char ledd3[20]={0xFC,0xF9,0xE7,0xCF,0x9F,0x3F,0x7F,0x3F,0x9F,0xCF,0xE7,0xF9,0xFC,0xFE}; //14 
unsigned char code ledd4[25]={0x7F,0x3F,0x1F,0x0F,0x87,0xC3,0xE1,
                         0xF0,0xF8,0xFC,0xFE,0xFC,0xF8,0xF0,0xE1,0xC3,
						 0x87,0x0F,0x1F,0x3F,0x7F}; //   21  	
unsigned char code ledd5[20]={0xFA,0xFD,0xF5,0xF4,0xEB,0xF7,0xD7,0xEF,0xAF,0xDF,0x5F,0xBF}; 
//unsigned char val;

unsigned char code image[11][8] = {
    {0xC3, 0x81, 0x99, 0x99, 0x99, 0x99, 0x81, 0xC3},  //����0
    {0xEF, 0xE7, 0xE3, 0xE7, 0xE7, 0xE7, 0xE7, 0xC3},  //����1
    {0xC3, 0x81, 0x9D, 0x87, 0xC3, 0xF9, 0xC1, 0x81},  //����2
    {0xC3, 0x81, 0x9D, 0xC7, 0xC7, 0x9D, 0x81, 0xC3},  //����3
    {0xCF, 0xC7, 0xC3, 0xC9, 0xC9, 0x81, 0xCF, 0xCF},  //����4
    {0x81, 0xC1, 0xF9, 0xC3, 0x87, 0x9D, 0x81, 0xC3},  //����5
    {0xC3, 0x81, 0xF9, 0xC1, 0x81, 0x99, 0x81, 0xC3},  //����6
    {0x81, 0x81, 0x9F, 0xCF, 0xCF, 0xE7, 0xE7, 0xE7},  //����7
    {0xC3, 0x81, 0x99, 0xC3, 0xC3, 0x99, 0x81, 0xC3},  //����8
    {0xC3, 0x81, 0x99, 0x81, 0x83, 0x9F, 0x83, 0xC1},  //����9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //ȫ��
};
static char str[10]={0,0,0,0,0,0,0,0};

unsigned int code NoteFrequ[] = { //���� 1-7 �͸��� 1-7 ��ӦƵ���б�
523, 587, 659, 698, 784, 880, 988, //���� 1-7
1047, 1175, 1319, 1397, 1568, 1760, 1976 //���� 1-7
};
unsigned int code NoteReload[] = { //���� 1-7 �͸��� 1-7 ��Ӧ�Ķ�ʱ������ֵ
65536 - (11059200/12) / (523*2), //���� 1
65536 - (11059200/12) / (587*2), //2
65536 - (11059200/12) / (659*2), //3
65536 - (11059200/12) / (698*2), //4
65536 - (11059200/12) / (784*2), //5
65536 - (11059200/12) / (880*2), //6
65536 - (11059200/12) / (988*2), //7
65536 - (11059200/12) / (1047*2), //���� 1
65536 - (11059200/12) / (1175*2), //2
65536 - (11059200/12) / (1319*2), //3
65536 - (11059200/12) / (1397*2), //4
65536 - (11059200/12) / (1568*2), //5
65536 - (11059200/12) / (1760*2), //6
65536 - (11059200/12) / (1976*2), //7
0,
};
bit flagBuzz = 0; //����������ʹ�ܱ�־
bit tmrflag = 0; //��ʱ���ж���ɱ�־
void PlayTwoTiger();

void main()
{
    unsigned int i;
    unsigned char val;
    EA = 1;            //�����ж�
	ENLED=0;
	
    ConfigTimer0(10);  //����T0��ʱ10ms
	ConfigUART(9600);   //���ò�����Ϊ9600
    InitLcd1602();     //��ʼ��Һ��
	//led1();  
    while (1)
    {
	switch(m)
			{
			     case 1:	LcdShowStr(5, 0,"1");c=0;break;

				 case 2:    LcdShowStr(5, 0,"2");c=0;break;
				 case 3:    LcdShowStr(5, 0,"3");Led3();c=0;break;
				 case 4:    LcdShowStr(5, 0,"4");Led4();c=0;break;
				 case 5:    LcdShowStr(5, 0,"5");Led5();c=0;break;
				 case 6:    LcdShowStr(5, 0,"6");Led6();c=0;break;
				 case 7:    LcdShowStr(5, 0,"7");Led7();c=0;break;
				 case 8:    LcdShowStr(5, 0,"8");Led8();c=0;break;
				 case 9:    LcdShowStr(5, 0,"9");Led9();c=0;break;
				 case 10:   LcdShowStr(5, 0,"*");c=0;break;
				 case 0:    LcdShowStr(5, 0,"0");Led0();c=0;PlayTwoTiger();Led9();break;
				 default:
				 P0=0xFF;break;
		     }
	 ENLED=1;	  //�ر�LEDʹ�ܶˣ���ֹ��˸
	 KeyDriver();  //���ð�������
	 UartDriver();  //���ô�������  
        //if (flag300ms)
        {
            flag300ms = 0;
			//��ʾͨ��0�ĵ�ѹ
			LcdShowStr(9, 0, "TJ 50");  //��ʾͨ��ָʾ
            val=GetADCValue(0);  //��ȡADCͨ��0��ת��ֵ
			LcdShowStr(0, 0, "Mode=");  //��ʾͨ��ָʾ
			ADDR2=1;
            ADDR1=1;
            ADDR0=0;
			P0=0xFF;
            ValueToString(str, val);  //תΪ�ַ�����ʽ�ĵ�ѹֵ			 
			LcdShowStr(0, 1, "Voltage=");    //��ʾ��Һ����
            LcdShowStr(8, 1, str);    //��ʾ��Һ��
			TI=1;
			if(flagUart)
			{
			      UartWrite(str,8);
				  //UartWrite('\n',1);

			}
		}	
      }
      ENLED=0;	  //��LEDʹ�ܶ�
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
	t=val*6;
	t0=val/100;
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
	unsigned char code cmd3[] = "4";  //�ַ�����ʾ����
	unsigned char code cmd4[] = "5";  //�ַ�����ʾ����
	unsigned char code cmd5[] = "6";  //�ַ�����ʾ����
	unsigned char code cmd6[] = "7";  //�ַ�����ʾ����
	unsigned char code cmd7[] = "8";  //�ַ�����ʾ����
	unsigned char code cmd8[] = "9";  //�ַ�����ʾ����
	unsigned char code cmd9[] = "0";  //�ַ�����ʾ����
    unsigned char code cmdLen[] = {          //����Ȼ��ܱ�
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,	 sizeof(cmd3)-1, sizeof(cmd4)-1, sizeof(cmd5)-1,
		sizeof(cmd6)-1,sizeof(cmd7)-1,sizeof(cmd8)-1,sizeof(cmd8)-1,
    };
    unsigned char code *cmdPtr[] = {         //����ָ����ܱ�
        &cmd0[0],  &cmd1[0],  &cmd2[0],	&cmd3[0],&cmd4[0],&cmd5[0],&cmd6[0],&cmd7[0],&cmd8[0],&cmd9[0],
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
		    m=1;
            break;
        case 1:
            //flagBuzzOn = 0; //�رշ�����
			m=2;
            break;
        case 2:
            m=3;
            break;
		case 3:
            m=4;
            break;
		case 4:
            m=5;
            break;
		case 5:
            m=6;
            break;
		case 6:
            m=7;
            break;
		case 7:
            m=8;
            break;
		case 8:
            m=9;
            break;
		   case 9:
            m=0;
            break;
        default:   //δ�ҵ��������ʱ�����ϻ����͡������������ʾ
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r';  //��Ч���ִ�к���ԭ����֮֡�����
    buf[len++] = '\n';  //�س����з��󷵻ظ���λ������ʾ��ִ��
    UartWrite(buf, len);
}

/*void Led1()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  //while(1)
  {
	 for(cnt=0;cnt<8;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x01<<cnt);		//�߼�����,ȡ����0000 0011����1111 1100
		for(i=0;i<t;i++);	  //��ʱ����������ִ�п�ѭ��
		if(c==1)
		break;
     }
	 P0=0xFF;
  }
}	 */

void Led1()
{
     static unsigned char i = 0;  //��̬ɨ�������
	 switch (i)
    {
        //����LED����
        case 0:  i++; P0=0xE7; break;
        case 1:   i++; P0=0xDB; break;
        case 2:  i++; P0=0xBD; break;
        case 3:  i++; P0=0x7E; break;
        case 4:  i++; P0=0xBD; break;
		case 5:i++; P0=0xBD; break;
        case 6:i++; P0=0xE7; break;
        case 7: i=0; P0=0x7E; break; 
	}
}


/*void Led2()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  P0=0xFF;
  //while(1)
  {
	 for(cnt=0;cnt<8;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0x80>>cnt);		//�߼�����,ȡ����1100 0000����0011 1111
		if(c==1)
		break;
		for(i=0;i<t;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}  */

void Led2()
{
     static unsigned char i = 0;  //��̬ɨ�������
	 switch (i)
    {
        //����LED����
        case 0:  i++; P0=0xFE; break;
        case 1:   i++; P0=0xDB; break;
        case 2:  i++; P0=0xBD; break;
        case 3:  i++; P0=0x7E; break;
        case 4:  i++; P0=0xBD; break;
		case 5:i++; P0=0xBD; break;
        case 6:i++; P0=0xE7; break;
        case 7: i=0; P0=0x7E; break; 
	}
}
void Led3()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;
  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  //while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x01<<cnt);		//�߼�����,ȡ����0000 0001����1111 1110
		for(i=0;i<t;i++);	  //��ʱ����������ִ�п�ѭ��
		if(c==1)
		break;
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0x80>>cnt);		//�߼�����,ȡ����1000 0000����0111 1111
		if(c==1)
		break;
		for(i=0;i<t;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}
void Led4()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<6;cnt++)
		{
            P0=L1[cnt];
			for(i=0;i<t;i++);
		}
  }
}
void Led5()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<22;cnt++)
		{
            P0=L2[cnt];
			if(c==1)
		    break;
			for(i=0;i<t;i++);
		}
  }
}
void Led6()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<8;cnt++)
		{
            P0=L3[cnt];
			if(c==1)
		    break;
			for(i=0;i<t;i++);
		}
  }
}	 
void Led7()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<14;cnt++)
		{
            P0=ledd3[cnt];
			if(c==1)
		    break;
			for(i=0;i<t;i++);
		}
  }
}

void Led8()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<11;cnt++)
		{
            P0=ledd5[cnt];
			if(c==1)
		    break;
			for(i=0;i<t;i++);
		}
  }
}
void Led9()
{
    unsigned int i=0;
	unsigned int cnt=0;
	ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=1;
    ADDR0=0;
   
    //while(1)
    {
        for(cnt=0;cnt<11;cnt++)
		{
            P0=ledd4[cnt];
			if(c==1)
		    break;
			for(i=0;i<t;i++);
		}
  }
}
void Led0()
{
   unsigned int i=0;
   unsigned int cnt=0;
    ENLED=0;
    ADDR3=1;

    ADDR2=1;
    ADDR1=0;
    ADDR0=1;
    P0=0xC0;
}

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    static unsigned char u=0;
    if  (keycode == 0x31)   // 1
    {
	    
		//LcdShowStr(5, 0,"1");
	    
		
        //Led1();
		//c=1;
		m=1;
		
    }
	 else if (keycode == 0x32)  //2
    {
	    //LcdShowStr(5, 0,"2");

        //Led2();
		//c=2;
		m=2;
    }
	 else if (keycode == 0x33)  //3
    {
	   	//LcdShowStr(5, 0,"3");
        //Led3();
		m=3;
    }
	else if (keycode == 0x34)  //4
    {
	   	//LcdShowStr(5, 0,"4");
        //Led4();
		m=4;
    }
	else if (keycode == 0x35)  // 5
    {
	   	//LcdShowStr(5, 0,"5");
        //Led5();
		m=5;
    }
	else if (keycode == 0x36)  //6
    {
	   	//LcdShowStr(5, 0,"6");
       // Led6();
	   m=6;
    }
	else if (keycode == 0x37)  //7
    {
	   	//LcdShowStr(5, 0,"7");
        //Led7();
		m=7;
    }
	else if (keycode == 0x38)  //8
    {
	   	//LcdShowStr(5, 0,"8");
        //Led8();
		m=8;
    }
		else if (keycode == 0x39)  //9
    {
	   	//LcdShowStr(5, 0,"9");
        //Led9();
		m=9;
    } 
	
		else if (keycode == 0x30)  //0
    {
	   flagBuzz=1;
	   m=0;
    } 
    else if (keycode == 0x26)  
    {
        //OprtKeyAction(0);
		t=t-10000;
    }
    else if (keycode == 0x28)  
    {
       // OprtKeyAction(1);
	   t=t+10000;
    }
    else if (keycode == 0x25)  //
    {
        //OprtKeyAction(2);
    }
    else if (keycode == 0x27)  //
    {
       // OprtKeyAction(3);
    }
    else if (keycode == 0x0D)  //�س�����������
    {
	   switch(u)
			{
              case  0:flagUart=1;u++;break;
			  case  1:flagUart=0;u=0;break;
			  default:break;
			}

    }
    else if (keycode == 0x1B)  //Esc�������
    {
	    m=10;
		v=0;
		c=1;
		flagBuzz=0;
    }
}

void PlayTwoTiger()
{
unsigned char beat; //��ǰ��������
unsigned char note; //��ǰ���Ķ�Ӧ������
unsigned int time = 0; //��ǰ���ļ�ʱ
unsigned int beatTime = 0; //��ǰ������ʱ��
unsigned int soundTime = 0; //��ǰ�����跢��ʱ��
//��ֻ�ϻ�������
unsigned char code TwoTigerNote[] = {
8,9,10,8,12,     10,9,12,9,8,  6,10,   8,7   ,7,6,7,8,9,5,  8,9,10,11, 11,10,9,8,9,
8,9,10,8,12,	 10,9,12,9,8,  6,6,7,  8,5,   5,6,7,8,9,5,  8,9,10,11, 11,10,9,8,8,
10,11,12,12,12,   12,12,13,12,11,  10,10,10,10,10,  11,10,9,8,8, 8,7,6,7,7,8,9,  9,10,9,10,9,
10,11,12,12,12,   12,12,13,12,11, 10,10,10,10,11,10,9,  8,7,6,7,6,7,8,9,5, 8,9,10,9,9,9,8,8,15,

8,9,10,8,12,     10,9,12,9,8,  6,10,   8,7   ,7,6,7,8,9,5,  8,9,10,11, 11,10,9,8,9,
8,9,10,8,12,	 10,9,12,9,8,  6,6,7,  8,5,   5,6,7,8,9,5,  8,9,10,11, 11,10,9,8,8

};
//��ֻ�ϻ����ı�4 ��ʾһ�ģ�1 ���� 1/4 �ģ�8 ���� 2 ��
unsigned char code TwoTigerBeat[] = {
2,2,2,2,6 ,2,4,4,4,  2,2,6,2,8,   4,4,4,  2,2,4,4,2,2,  4,2,2,2,2,8,
2,2,2,2,6 ,2,4,4,4,  2,2,4,2,2,   6,   2,4,4,2,2,4,4,  2,2,4,2,2,2,2,8,

2,2,4,4,4,   4,2,2,2,2,4,4,4,4,  2,2,2,2,4,  4,2,2,4,4, 2,2,4,2,2,2,2,8,
2,2,4,4,4,4,   2,2,2,2,4,4,4,  2,2,2,2,2,2,4,  2,2,2,2,4,4,2,2,6,2,2,2,16,

2,2,2,2,6 ,2,4,4,4,  2,2,6,2,8,   4,4,4,  2,2,4,4,2,2,  4,2,2,2,2,8,
2,2,2,2,6 ,2,4,4,4,  2,2,4,2,2,   6,   2,4,4,2,2,4,4,  2,2,4,2,2,2,2,8

};
for (beat=0; beat<sizeof(TwoTigerNote); ) //�ý���������Ϊѭ������
{
while (!tmrflag); //ÿ�ζ�ʱ���ж���ɺ󣬼�Ⲣ�������
tmrflag = 0;
if (time == 0) //��ǰ���Ĳ���������һ���½���
{
note = TwoTigerNote[beat] - 1;
T0RH = NoteReload[note] >> 8;
T0RL = NoteReload[note];
//���������ʱ�䣬���� 2 λ�൱�ڳ� 4����λ����������Լӿ�ִ���ٶ�
beatTime = (TwoTigerBeat[beat] * NoteFrequ[note]) >> 2;
//���㷢��ʱ�䣬Ϊ��ʱ��� 0.75����λԭ��ͬ��
soundTime = beatTime - (beatTime >> 2);
flagBuzz = 1; //ָʾ��������ʼ����
time++;
}
else //��ǰ����δ��������ǰ����
{
if (time >= beatTime) //��ǰ����ʱ�䵽�������ʱ��ʱ���㣬
{ //������������������׼�������½���
time = 0;
beat++;
}
else //��ǰ����ʱ��δ�ﵽ��ʱ��ʱ��
{
time++; //�ۼ�ʱ�����
if (time == soundTime) //���﷢��ʱ���ָʾ�رշ�������
{ //���� 0.25*��ʱ��ľ��������
flagBuzz = 0; //����������������������
}
}
}
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

/* T0�жϷ�������ִ��300ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
	 unsigned int i=0;
	unsigned int cnt=0;
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	tmrflag = 1;	
    if (flagBuzz) //ʹ��ʱ��ת���������Ƶ�ƽ
	{
    BUZZ = ~BUZZ;
	ADDR3 = 1; ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=0x00;  
	}
    else //δʹ��ʱ�رշ�����
    BUZZ = 1;
	KeyScan();   //����ɨ��
	//LedScan();
	UartRxMonitor(1);  //���ڽ��ռ��
    tmr300ms++;
    if (tmr300ms >= t0)  //��ʱ300ms
    {
		 ENLED=0;
         ADDR3=1;

        ADDR2=1;
        ADDR1=1;
        ADDR0=0;
		//P0=0xFF;
	    switch(m)
		{
		case 1:Led1();break;
		case 2:Led2();break;
		case 3:Led3();break;
		case 4:Led4();break;
	    }
        tmr300ms = 0;
        flag300ms = 1;
    }
}