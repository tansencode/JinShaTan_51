#include<REGX52.h>
sbit clk_out=P1^0;

/*void pinlv()	//1 00	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0x94;
   RCAP2L=0x00;
   TR2=1;
} */

/*void pinlv()	//1 000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xf5;
   RCAP2L=0x33;
   TR2=1;
}	*/

/*void pinlv()	//1 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xfe;
   RCAP2L=0xeb;
   TR2=1;
}  */

/*void pinlv()	//10 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xff;
   RCAP2L=0xe4;
   TR2=1;
}	 */

/*void pinlv()	//20 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xff;
   RCAP2L=0xf2;
   TR2=1;
}  */

/*void pinlv()	//30 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xff;
   RCAP2L=0xf7;
   TR2=1;
} */



/*void pinlv()	//1 00	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0x8a;
   RCAP2L=0xd0;
   TR2=1;
}  */

/*void pinlv()	//1 000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xf4;
   RCAP2L=0x48;
   TR2=1;
} */

/*void pinlv()	//1 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xfe;
   RCAP2L=0xd4;
   TR2=1;
} */
/*void pinlv()	//10 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xff;
   RCAP2L=0xe2;
   TR2=1;
} 	*/

/*void pinlv()	//20 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xff;
   RCAP2L=0xf1;
   TR2=1;
} */
void pinlv()	//30 0000	 
{
   C_T2=0;
   T2MOD=0x02;
   RCAP2H=0xff;
   RCAP2L=0xf6;
   TR2=1;
}  
void main()
{
   pinlv();
   while(1);
}
