#include "msp430g2553.h"
#define uchar unsigned char
#define RS1 P1OUT |=BIT5 
#define RS0 P1OUT &=~BIT5 
#define RW1 P1OUT |=BIT6 
#define RW0 P1OUT &=~BIT6
#define EN1 P1OUT |=BIT7
#define EN0 P1OUT &=~BIT7
uchar an1[]={0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
             0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
long int cha,start,end;
unsigned int overflow,tt;
float time,juli,yy;
char b[];
int ist,iend,data,i,j;
void sendbit();
float jisuan(unsigned int x);
void display(float juli);
void write_com(char com);
void write_data(char data_bit);
void initial_lcd(void);
void delay_s(int i); 
void LCD_Display(char x,char y,char *str); 
void write_CGRAM(unsigned char *p);
void DisplayOneChar(unsigned char x,unsigned char y,unsigned char Data);
void main( )
{
  // Stop watchdog timer to prevent time out reset
  
  WDTCTL = WDTPW + WDTHOLD;
    if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)                                     
  {  
    while(1);                 // If calibration constants erased
                            // do not load, trap CPU!!
  } 

  P1DIR|=BIT0;
  P1DIR &= ~BIT2;         // P1.2 = (InputDirection + TA0.1) = CCIxA.1 --> CCR1
  P1SEL |= BIT2;
  TACTL = TASSEL_2  + MC_2 + TAIE;   //SMCLK,  /8 UCCR0
  CCTL1 = CM_3 + SCS + CAP + CCIE+CCIS_0;
  write_CGRAM(an1);
    while(1)
 {
   sendbit();
 _EINT(); //开总中断 
while((!(ist==1))|(!(iend==1)));
 ist=0;
 iend=0;
 cha=overflow*65536+end - start;
  time = (float)cha/1000000;
  juli =time*170;  
  display(juli);
 }
}
void sendbit()
{
  unsigned char i;
  P1OUT &= ~BIT0;
  for(i=10;i>0;i--)
    _NOP();
  P1OUT |= BIT0;
  for(i=10;i>0;i--)
    _NOP();
  P1OUT &= ~BIT0;
}


#pragma vector= TIMER0_A1_VECTOR               //定时器A中断处理
__interrupt void timer_a(void)
{
    
    switch(TAIV)                               //向量查询
       { case 2:                                 //捕获中断
            if(TACCTL1 & CCI)                        //上升沿
              {
                       //更变设置为下降沿触发
                start=TAR;                       //记录初始时间
                overflow=0; //溢出计数变量复位
                ist=1;
              }
            else                   //下降沿
             {     
               if(ist==1)
               {end=TAR;//用start,end,overflow计算脉冲宽度
                iend=1;
            TACTL|=TACLR;
               }
               }
            CCTL1&=~CCIFG;
            break;
         case 10:                                //定时器溢出中断
         {
           overflow++;
         } break;                               //溢出计数加1
         default:break;
       } 
  }
 
/************************延时函数*****************************/ 
void delay(int i) 
{ 
 int s=570; 
 for(;i>0;i--) 
 for(;s>0;s--) ;
} 

void display(float juli)
{
    P2SEL=0x00;       //设置P2为通用IO 
    P2DIR |= 0xff;                         // Set P2 to output direction 
    P1DIR |= 0xfa;      //  p1.5 1.6 1.7 作为1602控制
    P1OUT|=BIT1+BIT3+BIT4;
    initial_lcd();      //LCD初始化 
    delay(1);
    juli=juli*1000;
    b[0]=((long)juli/1000+0x30);
    b[1]=0x2e; 
    b[2]=((long)juli%1000/100+0x30); 
    b[3]=((long)juli%1000%100/10+0x30);

    b[4]=((long)juli%10+0x30);


    b[5]=0x6d;
    b[6]='\0';
    for(j = 0;j <= 10;j++)     //扫描延时稳定显示 
              { 
   LCD_Display(0,0,b); 
   if(juli<=1000)
           {
            DisplayOneChar(1,0,0x00);
            DisplayOneChar(1,1,0x01);
            DisplayOneChar(1,2,0x02);
            DisplayOneChar(1,3,0x03);
            DisplayOneChar(1,4,0x04);
            DisplayOneChar(1,5,0x05);
            DisplayOneChar(1,6,0x06);
            DisplayOneChar(1,7,0x07);
            //LCD_Display(1,0,);
           //  LCD_Display(1,0,h);
            // LCD_Display(1,0,l);
             //LCD_Display(1,0,m);
             P1OUT&=~BIT1;                 //共阳极
           }
    else if((1000<juli)&&(juli<=3000))
           {
           // LCD_Display(1,0,e);
            // LCD_Display(1,0,f);
            // LCD_Display(1,0,l);
             //LCD_Display(1,0,m);
             P1OUT&=~BIT3;                 //共阳极
           }
    else 
           {
           // LCD_Display(1,0,c);
             //LCD_Display(1,0,d);
             //LCD_Display(1,0,l);
             //LCD_Display(1,0,m);
             P1OUT&=~BIT4;                 //共阳极
           }
 } 
}
 /************************lcd写指令*****************************/ 
void write_com(char com) 
{ 
  RS0;//RS=0,输入指令 
  RW0;//RW=0向lcd写入指令或数据 
  EN0;//EN=0始终执行指令 
  delay(8); 
  P2OUT=com; 
  delay(8); 
  EN1;//EN=1读取信息 
  delay(8); 
  EN0;//EN=0执行指令 
} 
/************************lcd写数据*****************************/ 
void write_data(char data_bit) 
{ 
 RS1;//RS=1输入数据 
 RW0;//RW=0向lcd写入指令或数据 
 EN0;//EN=0执行指令 
 delay(8); 
 P2OUT=data_bit; 
 delay(8); 
 EN1;//EN=1,读取信息 
 delay(8); 
 EN0;//EN=0执行指令 
} 
/************************lcd初始化*****************************/ 
void initial_lcd(void) 
{ 
 delay(20); 
 write_com(0x38); 
 delay(15); 
 write_com(0x0c); 
 delay(15); 
 write_com(0x06); 
 delay(15); 
 write_com(0x01) ; 
} 
/************************lcd显示*****************************/
void LCD_Display(char x,char y,char *str) 
{ 
 char xtemp; 
switch(x) 
    { 
case 0:xtemp=0x80+y; break; 
case 1:xtemp=0xc0+y; break; 
default:break; 
    } 
write_com(xtemp); 
while(*str!=0) 
    { 
write_data(*str); 
str++; 
    } 
} 


void delay_s(int i) 
{ 
 int s=1140; 
 for(;i>0;i--) 
 for(;s>0;s--) ;
} 


void write_CGRAM(unsigned char *p)
{
unsigned char i,j,kk;
unsigned char tmp=0x40; //操作CGRAM的命令码电
kk=0;
for(j=0;j<8;j++)        //64 字节存储空间,可以生成 8 个自定义字符点阵
{
    for(i=0;i<8;i++)    // 8 个字节生成 1 个字符点阵
    {
        write_com(tmp+i);    //操作CGRAM的命令码+写入CGRAM地址
        write_data(p[kk]);    //写入数据

    }
    tmp += 8;
}
}

void DisplayOneChar(unsigned char x,unsigned char y,unsigned char Data)
{
if (x==0)
 y|=0x80;//当要显示第一行时地址码+0x80;
else 
 y|=0xC0;//在第二行显示是地址码+0xC0;
write_com(y);//发送地址码
write_data(Data);//发送要显示的字符编码
}