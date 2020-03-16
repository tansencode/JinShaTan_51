/*****************ZY08-C����ѭ����ң�ء����ϡ�Ѱ�������*****************
//=====================================================================//
                   ZY08-Cѭ����������ϡ�ң���ۺϳ�����ʾ                         //
//  ��������Ҫ��ѭ�����򡢺�����ϳ���ң���������ۺ���һ��ͨ�����尴���л�
����ɫС�������ұ߰���S4�л�������ͻ��������ɫ������ ����ô����������ߵİ���S1
�л����������ʾ1������ѭ��ģʽ����һ�°����������ʾ2�����Ǳ���ģʽ����һ�°���
�������ʾ3�����Ǻ���ң�س����ܶ���ʼ�л�ģʽ��
//---------------------------------------------------------------------//

*  ƽ̨��ZY08-C����ѭ����ң�ء����ϡ�Ѱ������� + Keil U4 + STC89C52
*  �������ƣ�5��ZY08-Cѭ����������ϡ�ң���ۺϳ���
*  ��˾����������ƽ��豸���޹�˾
*  ��д�����˾�з�һ��
*  ����:���ܳ�QQ:1018973181 
*  ����:11.0592MHZ
*  �ۺ���˵������ѿ�Դ����֧�ֿͻ��Լ��޸ĳ������������������ṩ�ҹ�˾����˵��
*  ��Ƶ�̳̣���С������ѧϰC������ϸ��Ƶ�̳̣�����ͳһ��������
******************************************************************/  

#include <reg52.h> //����51��Ƭ����ص�ͷ�ļ�
#include <intrins.h>
sbit LeftLed=P2^0;                        //����ǰ�����ָʾ�ƶ˿�
sbit RightLed=P0^7;                       //����ǰ���Ҳ�ָʾ�ƶ˿�
sbit FontLled=P1^7;

sbit LeftIR=P3^5;                         //����ǰ��������̽ͷ�˿�
sbit RightIR=P3^6;                        //����ǰ���Ҳ����̽ͷ�˿�
sbit FontIR=P3^7;                         //����ǰ����ǰ������̽ͷ�˿�

sbit M1A=P0^0;                            //�������������A��
sbit M1B=P0^1;                            //�������������B��

sbit M2A=P0^2;                            //�����Ҳ�������A��
sbit M2B=P0^3;                            //�����Ҳ�������B��

sbit B1=P0^4;                             //��������ʶʶ�𴫸����˿�
sbit SB1=P0^6;                            //����������˿�
sbit IRIN=P3^3;                                       //���������ն˿�

unsigned char code  LedShowData[]={0x03,0x9F,0x25,0x0D,0x99,  //�����������ʾ����
                                   0x49,0x41,0x1F,0x01,0x19};//0,1,2,3,4,5,6,7,8,9
unsigned char code  RecvData[]={0x19,0x46,0x15,0x43,0x44,0x40,0x0D,0x0E,0x00,0x0F};
unsigned char IRCOM[7];
static unsigned int LedFlash;                         //��������Ƶ�ʼ�������
bit EnableLight=0;                                    //����ָʾ��ʹ��λ
#define ShowPort P2                                   //�����������ʾ�˿�
unsigned char temp = 1;
void tingzhi()
{
   M1A=0;                                   //��M1���A�˳�ʼ��Ϊ0
   M1B=0;                                   //��M1���B�˳�ʼ��Ϊ0
   M2A=0;                                   //��M2���A�˳�ʼ��Ϊ0
   M2B=0;

}

void qianjin()
{
   M1A=1;                                   
   M1B=0;                                   
   M2A=1;                                   
   M2B=0;
}

void houtui()
{
   M1A=0;                                   
   M1B=1;                                   
   M2A=0;                                   
   M2B=1;
}

void zuozhuan()
{
   M1A=0;                                   
   M1B=1;                                   
   M2A=1;                                   
   M2B=0;
}

void youzhuan()
{
   M1A=1;                                   
   M1B=0;                                   
   M2A=0;                                   
   M2B=1;
}

void Delay1ms(unsigned int i) 
{ 
unsigned char j,k; 
do{ 
  j = 10; 
  do{ 
   k = 50; 
   do{ 
    _nop_(); 
   }while(--k);     
  }while(--j); 
}while(--i); 

} 

void delay_nus(unsigned int i)  //��ʱ:i>=12 ,i����С��ʱ��12 us
{ 
  i=i/10;
  while(--i);
}   
void delay_nms(unsigned int n)  //��ʱn ms
{ 
  n=n+1;
  while(--n)  
  delay_nus(900);         //��ʱ 1ms,ͬʱ���в���
  
}  

void delayms(unsigned char x)                         //0.14mS��ʱ����
{
  unsigned char i;                                    //������ʱ����
  while(x--)                                          //��ʱʱ��ѭ��
  {
    for (i = 0; i<13; i++) {}                         //14mS��ʱ
  }
}

void Delay()                                          //������ʱ�ӳ���
{ 
  unsigned int DelayTime=30000;                       //������ʱʱ�����
  while(DelayTime--);                                 //��ʼ������ʱѭ��
  return;                                             //�ӳ��򷵻�
}
void ControlCar(unsigned char ConType)    //�����������ӳ���
{
 
  tingzhi();
 switch(ConType)                          //�ж��û��趨�����ʽ
 {
  case 1:  //ǰ��                         //�ж��û��Ƿ�ѡ����ʽ1
  { 
    //tingzhi();                                                      //����ǰ��֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
        //Delay1ms(50);
        LeftLed = 0 ;
        qianjin();
    break;
  }
  case 2: //����                          //�ж��û��Ƿ�ѡ����ʽ2
  { 
   // tingzhi();                                                              //�������֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
   // Delay1ms(50);
    LeftLed = 1 ;          
        houtui();                                //M2�����ת
    break;
  }
  case 3: //��ת                          //�ж��û��Ƿ�ѡ����ʽ3
  { 
     //tingzhi();                                                                  //������ת֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
        // Delay1ms(50); 
         zuozhuan();                               //M2�����ת
         break;
  }
  case 4: //��ת                          //�ж��û��Ƿ�ѡ����ʽ4
  { 
   // tingzhi();                                                                  //������ת֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
//        Delay1ms(50);
        youzhuan();                                //M1�����ת
                                          //M2�����ת
        break;
  }
  case 8: //ֹͣ                          //�ж��û��Ƿ�ѡ����ʽ8
  {
    tingzhi();
        break;                                //�˳���ǰѡ��
  }
 }
}

void ControlCar_yaokong(unsigned char ConType)    //�����������ӳ���        (����ң�ص�������һ�� switch  case  ���  )
{
 
  tingzhi();
 switch(ConType)                          //�ж��û��趨�����ʽ
 {
  case 1:  //ǰ��                         //�ж��û��Ƿ�ѡ����ʽ1
  { 
    tingzhi();                                                      //����ǰ��֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
        Delay1ms(150);
        LeftLed = 0 ;
        qianjin();
    break;
  }
  case 2: //����                          //�ж��û��Ƿ�ѡ����ʽ2
  { 
    tingzhi();                                                              //�������֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
    Delay1ms(150);
    LeftLed = 1 ;          
        houtui();                                //M2�����ת
    break;
  }
  case 3: //��ת                          //�ж��û��Ƿ�ѡ����ʽ3
  { 
     tingzhi();                                                                  //������ת֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
         Delay1ms(150); 
         zuozhuan();                               //M2�����ת
         break;
  }
  case 4: //��ת                          //�ж��û��Ƿ�ѡ����ʽ4
  { 
     tingzhi();                                                                  //������ת֮ǰ ��ֹͣһ��ʱ��  ��ֹ��������ѹ������� ����ϵͳ��λ
         Delay1ms(150);
     youzhuan();                                //M1�����ת  //M2�����ת
          break;
  }
  case 8: //ֹͣ                          //�ж��û��Ƿ�ѡ����ʽ8
  {
    tingzhi();
        break;                                //�˳���ǰѡ��
  }
 }
}
void Robot_Avoidance()                   //�����˱����ӳ���
{

   LeftLed=LeftIR;                        //ǰ�����ָʾ��ָʾ��ǰ��������̽ͷ״̬
   RightLed=RightIR;                      //ǰ���Ҳ�ָʾ��ָʾ��ǰ���Ҳ����̽ͷ״̬
   FontLled= FontIR; 
   SB1=FontIR;

    if(FontIR == 0)                          //���ǰ����ϴ�������⵽�ϰ���
   {
      ControlCar(8);                       //ֹͣ
      delay_nms (300);                                           //ֹͣ300MS   ��ֹ��������ѹ���  ����ϵͳ��λ
          ControlCar(2);                                           //����  
          delay_nms (1000);                                           //����1500MS  
          ControlCar(3);                                           //
          delay_nms (1800);        
  }
   else
   {
      ControlCar(1);                      //�Ҳ�û���ź�ʱ,��ʼ����תһ���ĽǶ�
      delay_nms (10);
   }
    ControlCar(1);
}
//������ѭ���ӳ���
void Robot_Traction()                     //������ѭ���ӳ���
{
   LeftLed=LeftIR;                        //ǰ�����ָʾ��ָʾ��ǰ��������̽ͷ״̬
   RightLed=RightIR;                      //ǰ���Ҳ�ָʾ��ָʾ��ǰ���Ҳ����̽ͷ״̬
   FontLled= FontIR; 
   SB1=LeftIR;
   if(LeftIR == 0 && RightIR == 0)    //���������⵽���ߣ���ǰ��
   {
      ControlCar(1);                      //���û���ź�ʱ����ʼ����תһ���ĽǶ�
      delay_nms (10);
   }
   
   else if(LeftIR == 0 && RightIR == 1)
   {
      ControlCar(3);                      //�Ҳ�û���ź�ʱ,��ʼ����תһ���ĽǶ�
      delay_nms (10);
   }

   else if(LeftIR == 1 && RightIR == 0)
   {
      ControlCar(4);                      //�Ҳ�û���ź�ʱ,��ʼ����תһ���ĽǶ�
      delay_nms (10);
   }
    ControlCar(8);
}

void main()                               //���������
{
  
  bit RunFlag=0;                          //����С�����б�־λ
  LedFlash=3000;                                       //���������ݽ��г�ʼ��
  //EX1=1;                                               //ͬ�⿪���ⲿ�ж�1
  IT1=1;                                               //�趨�ⲿ�ж�1Ϊ�ͱ�Ե��������
  EA=1;                                                //���жϿ���
  ControlCar(8);                          //��ʼ��С������״̬
  while(1)                                //������ѭ��
  {
             if(P3^2 == 0)
          {
           delay_nms(10);
           if(P3^2 == 0)
           {
                     temp++;
                  while(!P3^2);
           }
          }
          if(temp > 3)
          {
          temp = 1;
          }
          switch(temp)
          {
                   case 1:        ShowPort = LedShowData[1];Robot_Traction();EX1 = 0;break;
                case 2: ShowPort = LedShowData[2];Robot_Avoidance();EX1 = 0;break;
                case 3: ShowPort = LedShowData[3];EX1 = 1;break;
          }
  }
}
//----------����ң��-------------------------------------------------------------
void IR_IN() interrupt 2 using 0                      //����INT2�ⲿ�жϺ���
{
  unsigned char j,k,N=0;                              //������ʱ���ձ���
   
  EX1 = 0;                                            //�ر��ⲿ�ж�,��ֹ�����źŵ���   
  delayms(15);                                        //��ʱʱ�䣬���к�������
  if (IRIN==1)                                        //�жϺ����ź��Ƿ���ʧ
  {  
     EX1 =1;                                          //�ⲿ�жϿ�
         return;                                          //����
  } 
                           
  while (!IRIN)                                       //��IR��Ϊ�ߵ�ƽ������9ms��ǰ���͵�ƽ�źš�
  {
      delayms(1);                                     //��ʱ�ȴ�
  }

  for (j=0;j<4;j++)                                   //�ɼ�����ң��������
  { 
    for (k=0;k<8;k++)                                 //�ִβɼ�8λ����
    {
       while (IRIN)                                   //�� IR ��Ϊ�͵�ƽ������4.5ms��ǰ���ߵ�ƽ�źš�
       {
         delayms(1);                                  //��ʱ�ȴ�
       }
       
       while (!IRIN)                                  //�� IR ��Ϊ�ߵ�ƽ
       {
         delayms(1);                                  //��ʱ�ȴ�
       }
   
       while (IRIN)                                   //����IR�ߵ�ƽʱ��
       {
         delayms(1);                                  //��ʱ�ȴ�
         N++;                                         //�������Ӽ�
         if (N>=30)                                   //�жϼ������ۼ�ֵ
             { 
           EX1=1;                                     //���ⲿ�жϹ���
               return;                                    //����
         }                   
       }
                                       
      IRCOM[j]=IRCOM[j] >> 1;                         //��������λ�Ʋ������Զ�����
     
      if (N>=8)                                       //�ж����ݳ��� 
      {
         IRCOM[j] = IRCOM[j] | 0x80;                  //�������λ��1
      } 
      N=0;                                            //����λ����¼��
    }
  }
   
  if (IRCOM[2]!=~IRCOM[3])                            //�жϵ�ַ���Ƿ���ͬ
  { 
     EX1=1;                                           //���ⲿ�ж�
     return;                                          //����
  }

  for(j=0;j<10;j++)                                   //ѭ�����м������
   {
      if(IRCOM[2]==RecvData[j])                       //���м�λ��Ӧ
      {
       // ControlCar(j);
                ControlCar_yaokong(j);                               //�������ʾ��Ӧ����
      }
   } 
   EX1 = 1;                                           //�ⲿ�жϿ� 
} 
//------------------------------------------------------------------------------------------------------- 