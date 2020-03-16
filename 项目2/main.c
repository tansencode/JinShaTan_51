#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

sbit KEY_IN_1 = P2^4;
sbit KEY_IN_2 = P2^5;
sbit KEY_IN_3 = P2^6;
sbit KEY_IN_4 = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

sbit BUZZ = P1^6;  //��������������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

//void OpenBuzz(unsigned int frequ);
//void StopBuzz();

void buzz()
{
    unsigned int i;
	while(1)
	{
	    
	   BUZZ =0; //��ת���������Ƶ�ƽ
       for(i=0;i<50;i++);
	   BUZZ=1;
	   for(i=0;i<50;i++);
	}
}
	

/* ����������������frequ-����Ƶ�� */
/*void OpenBuzz(unsigned int frequ)
{
    unsigned int reload;    //��������Ķ�ʱ������ֵ
    
    reload = 65536 - (11059200/12)/(frequ*2);  //�ɸ���Ƶ�ʼ��㶨ʱ������ֵ
    T0RH = (unsigned char)(reload >> 8);  //16λ����ֵ�ֽ�Ϊ�ߵ������ֽ�
    T0RL = (unsigned char)reload;
    TH0  = 0xFF;  //�趨һ���ӽ�����ĳ�ֵ����ʹ��ʱ������Ͷ�빤��
    TL0  = 0xFE;
    ET0  = 1;     //ʹ��T0�ж�
    TR0  = 1;     //����T0
}

void StopBuzz()
{
    ET0 = 0;   //����T0�ж�
    TR0 = 0;   //ֹͣT0
} */

void liushuideng1()
{
   unsigned int i=0;
   unsigned char cnt=0;
   ENLED=0;
   ADDR3=1;
   ADDR2=1;
   ADDR1=1;
   ADDR0=0;
   while(1)
   {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x01<<cnt);		//�߼�����,ȡ����0000 0001����1111 1110
		for(i=0;i<20000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0x80>>cnt);		//�߼�����,ȡ����1000 0000����0111 1111
		for(i=0;i<20000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}

void liushuideng2()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x03<<cnt);		//�߼�����,ȡ����0000 0011����1111 1100
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0xC0>>cnt);		//�߼�����,ȡ����1100 0000����0011 1111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}

void liushuideng3()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt++)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x0F<<cnt);		//�߼�����,ȡ����0000 1111����1111 0000
		for(i=0;i<20000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt++)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0xF0>>cnt);		//�߼�����,ȡ����1111 0000����0000 1111
		for(i=0;i<20000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}

void liushuideng4()
{
  unsigned int i=0;
  unsigned char cnt=0;
  ENLED=0;
  ADDR3=1;

  ADDR2=1;
  ADDR1=1;
  ADDR0=0;
  while(1)
  {
	 for(cnt=0;cnt<7;cnt=cnt+2)    //��λ�жϣ�������7λ����һ����������λ
     {
        P0=~(0x08<<cnt);		//�߼�����,ȡ����0000 1000����1111 0111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
     }
	 for(cnt=0;cnt<7;cnt=cnt+2)	    //��λ�жϣ�������7λ����һ����������λ
	 {
	    P0=~(0x10>>cnt);		//�߼�����,ȡ����0001 0000����1110 1111
		for(i=0;i<30000;i++);	  //��ʱ����������ִ�п�ѭ��
	  }
  }
}

unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
    { 0x31, 0x32, 0x33, 0x26 }, //���ּ� 1�����ּ� 2�����ּ� 3�����ϼ�
    { 0x34, 0x35, 0x36, 0x25 }, //���ּ� 4�����ּ� 5�����ּ� 6�������
    { 0x37, 0x38, 0x39, 0x28 }, //���ּ� 7�����ּ� 8�����ּ� 9�����¼�
    { 0x30, 0x1B, 0x0D, 0x27 } //���ּ� 0��ESC ���� �س����� ���Ҽ�
};
unsigned char KeySta[4][4] = { //ȫ�����󰴼��ĵ�ǰ״̬
     {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};

void KeyDriver();

void main()
{
   EA = 1; //ʹ�����ж�
   
   TMOD = 0x01; //���� T0 Ϊģʽ 1
   TH0 = 0xFC; //Ϊ T0 ����ֵ 0xFC67����ʱ 1ms
   TL0 = 0x67;
   ET0 = 1; //ʹ�� T0 �ж�
   TR0 = 1; //���� T0
   while (1)
   {
      KeyDriver(); //���ð�����������
   }
}

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
       if ((keycode>=0x30) && (keycode<=0x39)) //���� 0-9 ������
       {
          ;
          }

          else if (keycode == 0x26) //��ˮ��1
          {
              liushuideng1();
          }

          else if (keycode == 0x28) //��ˮ��2
          {
             liushuideng2();
          }

          else if (keycode == 0x25) //��ˮ��3
          {
              liushuideng3();
          }
          else if (keycode == 0x27) //��ˮ��4
          {
               liushuideng4();
           }
         else if(keycode==0x0D)	//��������
         {
		    buzz();
	      }
	      
 	
          else if (keycode == 0x1B) //Esc ������ͣ���˹�����ʱ��δʵ�֣����ø�λ������
         {
		    BUZZ=1;
      	    EA=0;
			ENLED=0;
			ET0=1;
			TR0=0;   
          }
}

/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char backup[4][4] = { //����ֵ���ݣ�����ǰһ�ε�ֵ
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
    };
    for (i=0; i<4; i++) //ѭ����� 4*4 �ľ��󰴼�
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j]) //��ⰴ������
            {
                if (backup[i][j] != 0) //��������ʱִ�ж���
                {
                     KeyAction(KeyCodeMap[i][j]); //���ð�����������
                }
                backup[i][j] = KeySta[i][j]; //ˢ��ǰһ�εı���ֵ
             }
          }
      }
}

/* ����ɨ�躯�������ڶ�ʱ�ж��е��ã��Ƽ����ü�� 1ms */
void KeyScan()
{
     unsigned char i;
     static unsigned char keyout = 0; //���󰴼�ɨ���������
     static unsigned char keybuf[4][4] = { //���󰴼�ɨ�軺����
     {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
     {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}
     };	 //��һ�е� 4 ������ֵ���뻺����
      
     keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
     keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
     keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
     keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
     //��������°���״̬
     for (i=0; i<4; i++) //ÿ�� 4 ������������ѭ�� 4 ��
     {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        { //���� 4 ��ɨ��ֵΪ 0���� 4*4ms �ڶ��ǰ���״̬ʱ������Ϊ�������ȶ��İ���
             KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        { //���� 4 ��ɨ��ֵΪ 1���� 4*4ms �ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
            KeySta[keyout][i] = 1;
        }
      }		  //ִ����һ�ε�ɨ�����
       
      keyout++; //�����������
      keyout = keyout & 0x03; //����ֵ�ӵ� 4 ������
      switch (keyout) //�����������ͷŵ�ǰ������ţ������´ε��������
      {
          case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
          case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
          case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
          case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
          default: break;
       }
}


/* T0 �жϷ����������ڰ���ɨ�� */

void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;   //���¼�������ֵ
    TL0 = T0RL;
    KeyScan(); //���ð���ɨ�躯��
}

   