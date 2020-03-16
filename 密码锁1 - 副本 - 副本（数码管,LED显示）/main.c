/*
*******************************************************************************
* 1����0��9�����ּ��������룬������������λ����PSW_MAX_LEN�궨�����
* 2���û��趨��������EEPROM���棬�����á��ۼӺ͡�У������֤���ݵ���Ч��
* 3�������״�����ʱEEPROM�е�������Ч����ʱ����ʾ���������롱
* 4����������ʱ��Ҫ�û�����������ͬ�����룬�Ա�֤���óɹ������ú������Զ�����
* 5�������������벻��ͬʱ����������������ʧ�ܣ����س�����������
* 6���������óɹ���ÿ�ο�������Ҫ���û��������룬�û���������󣬰��س�����
     ϵͳ�������������EEPROM�б�����������Աȣ���ͬ��ϵͳ�����������������
* 7�������������ϵͳ����ʱ�����س����������¿�ʼ��������
* 8������������ȷϵͳ����ʱ�����س������Խ����޸�����Ĳ���
* 9������������ʱ����ͨ���޸�PSW_MAX_LEN��PSW_SAVE_ADDR�궨���ֵ����������
*******************************************************************************
*/

#include <reg52.h>

#define PSW_MAX_LEN    9    //�ú궨���������󳤶ȣ���󲻳���16��
#define PSW_SAVE_ADDR  0x20  //�ú궨���������EEPROM�еı����ַ

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

sbit MC0 = P3^0;
sbit MC1 = P3^1;
sbit MC2 = P3^2;
sbit MC3 = P3^3;

unsigned char code LedChar[] = { //�������ʾ�ַ�ת����
0x89, 0x89, 0x89, 0x89, 0x89,0x89,0x89, 0x89,
0x89, 0x89, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char LedBuff[6] = { //�������ʾ������
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

signed long beats = 0; //���ת����������
unsigned char status;  //ϵͳ״̬�֣�0-�������룬1-�������룬2-��������ȷ�ϣ�
                       //3-��������ʧ�ܣ�4-�����������ȷ��5-������������
static unsigned long num=0; //���ڱ����������
unsigned char PswCnt;  //��ǰ��������λ���ļ�����
unsigned char pdata PswSet[PSW_MAX_LEN];   //���趨����������
unsigned char pdata PswBuf1[PSW_MAX_LEN];  //�������뻺����1
unsigned char pdata PswBuf2[PSW_MAX_LEN];  //�������뻺����2

unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

static unsigned char j ; //��̬ɨ�������
void ledon();
extern void KeyScan();
extern void KeyDriver();
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);

void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len);
void ConfigTimer0(unsigned int ms);
void ShowNumber(unsigned long num);
bit  ReadPassword();

void main()
{
    EA = 1;           //�����ж�
	ENLED = 0; //ѡ������ܽ�����ʾ
    ADDR3 = 1;
		
	TMOD = 0x01; //���� T0 Ϊģʽ 1
    TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
    TL0 = 0x67;
    ET0 = 1; //ʹ�� T0 �ж�
    TR0 = 1; //���� T0
    ConfigTimer0(1);  //����T0��ʱ1ms
	LedBuff[0] = LedChar[10]; //�ϵ�����ʾ���ȴ���������
    InitLcd1602();    //��ʼ��Һ��
    if (ReadPassword())  //��ȡ���벢�ж��Ƿ���Ч
    {
	    j=2;
        status = 0;	
    }
    else
    {
	    j=0;
        status = 1;
    }
    FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
    FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
    PswCnt = 0;
    while (1)
    {
        KeyDriver();
    }
}

/* �����������������angle-��ת���ĽǶ� */
void StartMotor(signed long angle)
{
  //�ڼ���ǰ�ر��жϣ���ɺ��ٴ򿪣��Ա����жϴ�ϼ�����̶���ɴ���
  EA = 0;
  beats = (angle * 4076)/360 ; //ʵ��Ϊ 4076 ��ת��һȦ
  EA = 1;
}
/* �������ֹͣ���� */
void StopMotor()
{
   EA = 0;
   beats = 0;
   EA = 1;
}


/* ��һ���ڴ����ݿ�������һ����dest-Ŀ�ĵ�ַ��src-Դ��ַ��len-���ݳ��� */
void CopyMemory(unsigned char *dest, unsigned char *src, unsigned char len)
{
    while (len--)
    {
        *dest++ = *src++;
    }
}
/* ��һ��ָ���ֽ����һ���ڴ棬dest-Ŀ�ĵ�ַ��byt-������ֽڣ�len-���ݳ��� */
void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len)
{
    while (len--)
    {
        *dest++ = byt;
    }
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
/* ��һ�����ݰ��ֽ��ۼӣ����ػ�õ��ۼӺͣ�dat-����ָ�룬len-���ݳ��� */
unsigned char GetChkSum(unsigned char *dat, unsigned char len)
{
    unsigned char sum = 0;
    
    while (len--)
    {
        sum += *dat++;
    }
    return sum;
}
/* ��EEPROM�ж�ȡ������������ݣ�����������С��ۼӺ͡�У�飬
   ����ֵ-У��ͨ������1�����򷵻�0 */
bit ReadPassword()
{
    unsigned char sum;
    unsigned char buf[PSW_MAX_LEN+1];
    
    E2Read(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //��ȡ�������ݺ�У���ֽ�
    sum = GetChkSum(buf, PSW_MAX_LEN); //�����������ݵ��ۼӺ�
    if (buf[PSW_MAX_LEN] == ~sum)      //У��ͨ������������Чʱ����������
    {
        CopyMemory(PswSet, buf, PSW_MAX_LEN);
        return 1;
    }
    else  //����������Чʱ��������Ĭ�ϳ�ȫ0
    {
        FillMemory(PswSet, '\0', PSW_MAX_LEN);
        return 0;
    }
}
/* ����ǰ���������ݱ��浽EEPROM�У�ͬʱ���У���ֽ� */
void SavePassword()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    CopyMemory(buf, PswSet, PSW_MAX_LEN);  //�����������ݵ���������
    buf[PSW_MAX_LEN] = ~GetChkSum(buf, PSW_MAX_LEN);  //��������ۼӺ�У���ֽ�
    E2Write(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //���������ݱ��浽EEPROM��
}
/* ��Һ������ʾ�뵱ǰ��������λ����ͬ��*����ָʾ��ǰ���������λ�� */
void ShowPswCnt()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    FillMemory(buf, '*', PswCnt);
    FillMemory(buf+PswCnt, '\0', sizeof(buf)-PswCnt);
  //  LcdShowStr(0, 1, buf);
}
/* ���ּ�����������keycode-���ְ�����ASCIIֵ */
void KeyNumAction(unsigned char keycode)
{
    switch (status)
    {
        case 0: //��������״̬ʱ���������ַ���䵽PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 1: //��������״̬ʱ���������ַ���䵽PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 2: //��������ȷ��״̬ʱ���������ַ���䵽PswBuf2
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf2[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        default: //����״̬������Ӧ
            break;
    }
}

/* ��һ���޷��ų����͵�������ʾ��������ϣ�num-����ʾ���� */
void ShowNumber(unsigned long num)
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

void LedScan1()	 //led��ʾ��������
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
	case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
	case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0x7F; break;
	default: break;
  }
}

void LedScan2()			 //led��ʾ�ٴ�������֤����
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
		 case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
		 case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xBF; break;
		 default: break;
     }
}
void LedScan3()	  //led��ʾ��������
{
     static unsigned char i = 0; //��̬ɨ�������
     static unsigned int n; //��̬ɨ�������
     P0 = 0xFF; //��ʾ����
     switch (i)
     {
         case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
         case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
         case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
         case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
         case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
         case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
         case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xCF; break;
         default: break;
     }
}

void LedScan4()	  //����������ȷ��ʾ
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
       case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
       case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xDF; break;
       default: break;
    }
}
void LedScan5()					 //�������������ʾ
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
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0xEF; break;
        default: break;
    }
}

void ShowNumber1(unsigned long num)	//���������ʾ�ȴ���������
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
       LedBuff[i] = 0xFF;
	}
    for ( ; i>=0; i--) //ʣ���λ����ʵת��Ϊ�������ʾ�ַ�
    {
        LedBuff[i] =0xFF; 
     } 
}
/* �س����������� */
void KeyEnterAction()
{
    
    switch (status)
    {
        case 0: //�������������Ƿ���ȷ
            LcdClearScreen();
            if (CmpMemory(PswSet, PswBuf1, PSW_MAX_LEN))
            {   //��ȷʱ��ʾ��������ʾ
				StartMotor(90);
				j=3;
                status = 4;  //�������״̬
            }
            else
            {   //����ʱ��ʾ����������ʾ
				StartMotor(-90);
				j=4;
                status = 5;  //��������״̬
            }
            break;
        case 1: //�����������õ�ȷ�ϲ��裬��������һ������
		    num=0;
		    ShowNumber1(num);
		    j=1;
            PswCnt = 0;
            status = 2;
            break;
        case 2: //�����������������Ƿ�һ��
            if (CmpMemory(PswBuf1, PswBuf2, PSW_MAX_LEN))
            {   //һ��ʱ�������õ�����
				num=0;
		        ShowNumber1(num);
                CopyMemory(PswSet, PswBuf1, PSW_MAX_LEN);
                SavePassword();
                FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
				j=2;
                PswCnt = 0;
                status = 0;  //������������״̬
            }
            else
            {   //��һ��ʱ��ʾ��������ʧ��
               	num=0;
		        ShowNumber1(num);
			    j=1;
                status = 3;  //������������ʧ��״̬
            }
            break;
        case 3: //������ʾ�������룬������������״̬
		    num=0;
		    ShowNumber1(num);
		    j=0;
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        case 4: //�������ü��޸�����
		    num=0;
		    ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
			j=0;
            PswCnt = 0;
            status = 1;
            break;
        case 5: //������������
		   num=0;
		   ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
			j=2;
            PswCnt = 0;
            status = 0;
            break;
        default: //������Ӧ
            break;
    }
}
/* Esc���������� */
void KeyEscAction()
{
     static unsigned char i = 0; //��̬ɨ�������
    switch (status)
    {
        case 0: //��������״̬ʱ����յ�ǰ���������¿�ʼ
		    num=0;
			ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
			j=2;
            PswCnt = 0;
            break;
        case 1: //��������״̬ʱ����յ�ǰ���������¿�ʼ
           	num=0;
			ShowNumber1(num);
		    j=0;
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            break;
        case 2: //��������ȷ��״̬ʱ��������벢������������
		    num=0;
			ShowNumber1(num);
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
			j=0;
            PswCnt = 0;
            status = 1;
            break;
        default: //����״̬������Ӧ
            break;
    }
}

/* ���ת�����ƺ��� */
void TurnMotor()
{
unsigned char tmp; //��ʱ����
static unsigned char index = 0; //�����������
unsigned char code BeatCode[8] = { //����������Ķ�Ӧ�� IO ���ƴ���
0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};
if (beats != 0) //��������Ϊ 0 �����һ����������
{
if (beats > 0) //���������� 0 ʱ��ת
{
index++; //��תʱ���������������
index = index & 0x07; //��&����ʵ�ֵ� 8 ����
beats--; //��תʱ���ļ����ݼ�
}
else //������С�� 0 ʱ��ת
{
index--; //��תʱ������������ݼ�
index = index & 0x07; //��&����ͬ������ʵ�ֵ�-1 ʱ�� 7
beats++; //��תʱ���ļ�������
}
tmp = P3; //�� tmp �� P1 �ڵ�ǰֵ�ݴ�
tmp = tmp & 0xF0; //��&��������� 4 λ
tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д���� 4 λ
P3= tmp; //�ѵ� 4 λ�Ľ��Ĵ���͸� 4 λ��ԭֵ�ͻ� P1
}
else //������Ϊ 0 ��رյ�����е���
{
P3 = P3 | 0x0F;
}
}


/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    
	static unsigned int i=0;
    if  ((keycode>='0') && (keycode<='9'))  //�����ַ�
    {
        KeyNumAction(keycode);
		num = (num*10)+(keycode-0x30); //����ʮ�������ƣ������ֽ����λ
		ShowNumber(num); //��������ʾ�������
    }
    else if (keycode == 0x0D)  //�س���
    {
	    
        KeyEnterAction();
    }
    else if (keycode == 0x1B)  //Esc��
    {
        KeyEscAction();
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
/* T0�жϷ�������ִ�а���ɨ�� */
void InterruptTimer0() interrupt 1
{
    static bit div=0;
	static int n=90000;
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
	LedScan(); //�����������ʾɨ�躯��
    KeyScan();   //����ɨ��
	switch(j)
	{
	   case 0:
       
	   LedScan1();
	break;
      case 1:
    
	   LedScan2();
		break;
       case 2:
	  LedScan3();
		break;
		case 3:
	   LedScan4();
		break;
	case 4:
	   LedScan5();
		break;
		default:break;
		}
	div = ~div;
   if (div == 1)
   {
      TurnMotor();
   }
}
