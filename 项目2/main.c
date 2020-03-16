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

sbit BUZZ = P1^6;  //蜂鸣器控制引脚
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

//void OpenBuzz(unsigned int frequ);
//void StopBuzz();

void buzz()
{
    unsigned int i;
	while(1)
	{
	    
	   BUZZ =0; //反转蜂鸣器控制电平
       for(i=0;i<50;i++);
	   BUZZ=1;
	   for(i=0;i<50;i++);
	}
}
	

/* 蜂鸣器启动函数，frequ-工作频率 */
/*void OpenBuzz(unsigned int frequ)
{
    unsigned int reload;    //计算所需的定时器重载值
    
    reload = 65536 - (11059200/12)/(frequ*2);  //由给定频率计算定时器重载值
    T0RH = (unsigned char)(reload >> 8);  //16位重载值分解为高低两个字节
    T0RL = (unsigned char)reload;
    TH0  = 0xFF;  //设定一个接近溢出的初值，以使定时器马上投入工作
    TL0  = 0xFE;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
}

void StopBuzz()
{
    ET0 = 0;   //禁用T0中断
    TR0 = 0;   //停止T0
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
	 for(cnt=0;cnt<7;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x01<<cnt);		//逻辑左移,取反，0000 0001——1111 1110
		for(i=0;i<20000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0x80>>cnt);		//逻辑右移,取反，1000 0000——0111 1111
		for(i=0;i<20000;i++);	  //延时函数，不断执行空循环
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
	 for(cnt=0;cnt<7;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x03<<cnt);		//逻辑左移,取反，0000 0011——1111 1100
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0xC0>>cnt);		//逻辑右移,取反，1100 0000——0011 1111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
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
	 for(cnt=0;cnt<7;cnt++)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x0F<<cnt);		//逻辑左移,取反，0000 1111——1111 0000
		for(i=0;i<20000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt++)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0xF0>>cnt);		//逻辑右移,取反，1111 0000——0000 1111
		for(i=0;i<20000;i++);	  //延时函数，不断执行空循环
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
	 for(cnt=0;cnt<7;cnt=cnt+2)    //移位判断，向左移7位，第一个不记做移位
     {
        P0=~(0x08<<cnt);		//逻辑左移,取反，0000 1000——1111 0111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
     }
	 for(cnt=0;cnt<7;cnt=cnt+2)	    //移位判断，向右移7位，第一个不记做移位
	 {
	    P0=~(0x10>>cnt);		//逻辑右移,取反，0001 0000——1110 1111
		for(i=0;i<30000;i++);	  //延时函数，不断执行空循环
	  }
  }
}

unsigned char code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
    { 0x31, 0x32, 0x33, 0x26 }, //数字键 1、数字键 2、数字键 3、向上键
    { 0x34, 0x35, 0x36, 0x25 }, //数字键 4、数字键 5、数字键 6、向左键
    { 0x37, 0x38, 0x39, 0x28 }, //数字键 7、数字键 8、数字键 9、向下键
    { 0x30, 0x1B, 0x0D, 0x27 } //数字键 0、ESC 键、 回车键、 向右键
};
unsigned char KeySta[4][4] = { //全部矩阵按键的当前状态
     {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
};

void KeyDriver();

void main()
{
   EA = 1; //使能总中断
   
   TMOD = 0x01; //设置 T0 为模式 1
   TH0 = 0xFC; //为 T0 赋初值 0xFC67，定时 1ms
   TL0 = 0x67;
   ET0 = 1; //使能 T0 中断
   TR0 = 1; //启动 T0
   while (1)
   {
      KeyDriver(); //调用按键驱动函数
   }
}

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
       if ((keycode>=0x30) && (keycode<=0x39)) //输入 0-9 的数字
       {
          ;
          }

          else if (keycode == 0x26) //流水灯1
          {
              liushuideng1();
          }

          else if (keycode == 0x28) //流水灯2
          {
             liushuideng2();
          }

          else if (keycode == 0x25) //流水灯3
          {
              liushuideng3();
          }
          else if (keycode == 0x27) //流水灯4
          {
               liushuideng4();
           }
         else if(keycode==0x0D)	//蜂鸣器响
         {
		    buzz();
	      }
	      
 	
          else if (keycode == 0x1B) //Esc 键，暂停，此功能暂时还未实现，可用复位键代替
         {
		    BUZZ=1;
      	    EA=0;
			ENLED=0;
			ET0=1;
			TR0=0;   
          }
}

/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char backup[4][4] = { //按键值备份，保存前一次的值
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
    };
    for (i=0; i<4; i++) //循环检测 4*4 的矩阵按键
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j]) //检测按键动作
            {
                if (backup[i][j] != 0) //按键按下时执行动作
                {
                     KeyAction(KeyCodeMap[i][j]); //调用按键动作函数
                }
                backup[i][j] = KeySta[i][j]; //刷新前一次的备份值
             }
          }
      }
}

/* 按键扫描函数，需在定时中断中调用，推荐调用间隔 1ms */
void KeyScan()
{
     unsigned char i;
     static unsigned char keyout = 0; //矩阵按键扫描输出索引
     static unsigned char keybuf[4][4] = { //矩阵按键扫描缓冲区
     {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
     {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}
     };	 //将一行的 4 个按键值移入缓冲区
      
     keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
     keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
     keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
     keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
     //消抖后更新按键状态
     for (i=0; i<4; i++) //每行 4 个按键，所以循环 4 次
     {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        { //连续 4 次扫描值为 0，即 4*4ms 内都是按下状态时，可认为按键已稳定的按下
             KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        { //连续 4 次扫描值为 1，即 4*4ms 内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[keyout][i] = 1;
        }
      }		  //执行下一次的扫描输出
       
      keyout++; //输出索引递增
      keyout = keyout & 0x03; //索引值加到 4 即归零
      switch (keyout) //根据索引，释放当前输出引脚，拉低下次的输出引脚
      {
          case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
          case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
          case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
          case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
          default: break;
       }
}


/* T0 中断服务函数，用于按键扫描 */

void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;   //重新加载重载值
    TL0 = T0RL;
    KeyScan(); //调用按键扫描函数
}

   