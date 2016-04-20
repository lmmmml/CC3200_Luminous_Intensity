/*
 * PutChar.c
 *
 *  Created on: 2015年8月9日
 *      Author: Mars
 */
#include "includes.h" /*单片机寄存器头文件*/
#include "ctype.h"       /*isdigit函数需要该头文件*/
#include "LCD.h" /*LCD函数库头文件*/
#include "stdio.h"
#include "string.h"
//#include 'PutChar.h'
char  FirstChrFlag=1,x_flag=0,y_flag=0;  //第一个字符标志位

//int printf(const char *_format, ...);
//int putchar(int ch);
/****************************************************************************
* 名    称：putchar()
* 功    能：向LCD显示屏输出一个ASCII字符。
* 入口参数：ch: 待发送的字符  
* 出口参数：发出的字符
* 说    明: printf函数会调用该函数作为底层输出。该函数将字符输出到LCD上
            因此printf的结果将显示在LCD上。
****************************************************************************/

 int putchar(int ch)
{

if(FirstChrFlag){clear_screen(); x_flag=0;y_flag=0;}//第一个字符到来的时候清除上一屏显示内容
FirstChrFlag=0;
if(ch=='\f')     {clear_screen(); x_flag=0;y_flag=0;}//'\f'表示走纸翻页，相当于清除显示
if(isdigit(ch))
{
	switch(ch)
	 {
	 case '0' : display_string_5x8(y_flag,x_flag,'0');x_flag+=6;break;
	 case '1' : display_string_5x8(y_flag,x_flag,'1');x_flag+=6;break;
	 case '2' : display_string_5x8(y_flag,x_flag,'2');x_flag+=6;break;
	 case '3' : display_string_5x8(y_flag,x_flag,'3');x_flag+=6;break;
	 case '4' : display_string_5x8(y_flag,x_flag,'4');x_flag+=6;break;
	 case '5' : display_string_5x8(y_flag,x_flag,'5');x_flag+=6;break;
	 case '6' : display_string_5x8(y_flag,x_flag,'6');x_flag+=6;break;
	 case '7' : display_string_5x8(y_flag,x_flag,'7');x_flag+=6;break;
	 case '8' : display_string_5x8(y_flag,x_flag,'8');x_flag+=6;break;
	 case '9' : display_string_5x8(y_flag,x_flag,'9');x_flag+=6;break;
	 }
}
//数字和对应ASCII字母之间差0x30   '1'=0x31 '2'=0x32... isdigit也是C语言标准函数
else             //否则，不是数字，是字母
{
   switch(ch)    //根据字母选择程序分支
   {
		case 'A'		:	display_string_5x8(y_flag,x_flag,'A');x_flag+=6;break;
		case 'B'		:	display_string_5x8(y_flag,x_flag,'B');x_flag+=6;break;
		case 'C'		:	display_string_5x8(y_flag,x_flag,'C');x_flag+=6;break;
		case 'D'		:	display_string_5x8(y_flag,x_flag,'D');x_flag+=6;break;
		case 'E'		:	display_string_5x8(y_flag,x_flag,'E');x_flag+=6;break;
		case 'F'		:	display_string_5x8(y_flag,x_flag,'F');x_flag+=6;break;
		case 'G'		:	display_string_5x8(y_flag,x_flag,'G');x_flag+=6;break;
		case 'H'		:	display_string_5x8(y_flag,x_flag,'H');x_flag+=6;break;
		case 'I'		:	display_string_5x8(y_flag,x_flag,'I');x_flag+=6;break;
		case 'J'		:	display_string_5x8(y_flag,x_flag,'J');x_flag+=6;break;
		case 'K'		:	display_string_5x8(y_flag,x_flag,'K');x_flag+=6;break;
		case 'L'		:	display_string_5x8(y_flag,x_flag,'L');x_flag+=6;break;
		case 'M'		:	display_string_5x8(y_flag,x_flag,'M');x_flag+=6;break;
		case 'N'		:	display_string_5x8(y_flag,x_flag,'N');x_flag+=6;break;
		case 'O'		:	display_string_5x8(y_flag,x_flag,'O');x_flag+=6;break;
		case 'P'		:	display_string_5x8(y_flag,x_flag,'P');x_flag+=6;break;
		case 'Q'		:	display_string_5x8(y_flag,x_flag,'Q');x_flag+=6;break;
		case 'R'		:	display_string_5x8(y_flag,x_flag,'R');x_flag+=6;break;
		case 'S'		:	display_string_5x8(y_flag,x_flag,'S');x_flag+=6;break;
		case 'T'		:	display_string_5x8(y_flag,x_flag,'T');x_flag+=6;break;
		case 'U'		:	display_string_5x8(y_flag,x_flag,'U');x_flag+=6;break;
		case 'V'		:	display_string_5x8(y_flag,x_flag,'V');x_flag+=6;break;
		case 'W'		:	display_string_5x8(y_flag,x_flag,'W');x_flag+=6;break;
		case 'X'		:	display_string_5x8(y_flag,x_flag,'X');x_flag+=6;break;
		case 'Y'		:	display_string_5x8(y_flag,x_flag,'Y');x_flag+=6;break;
		case 'Z'		:	display_string_5x8(y_flag,x_flag,'Z');x_flag+=6;break;
		case 'a'		:	display_string_5x8(y_flag,x_flag,'a');x_flag+=6;break;
		case 'b'		:	display_string_5x8(y_flag,x_flag,'b');x_flag+=6;break;
		case 'c'		:	display_string_5x8(y_flag,x_flag,'c');x_flag+=6;break;
		case 'd'		:	display_string_5x8(y_flag,x_flag,'d');x_flag+=6;break;
		case 'e'		:	display_string_5x8(y_flag,x_flag,'e');x_flag+=6;break;
		case 'f'		:	display_string_5x8(y_flag,x_flag,'f');x_flag+=6;break;
		case 'g'		:	display_string_5x8(y_flag,x_flag,'g');x_flag+=6;break;
		case 'h'		:	display_string_5x8(y_flag,x_flag,'h');x_flag+=6;break;
		case 'i'		:	display_string_5x8(y_flag,x_flag,'i');x_flag+=6;break;
		case 'j'		:	display_string_5x8(y_flag,x_flag,'j');x_flag+=6;break;
		case 'k'		:	display_string_5x8(y_flag,x_flag,'k');x_flag+=6;break;
		case 'l'		:	display_string_5x8(y_flag,x_flag,'l');x_flag+=6;break;
		case 'm'		:	display_string_5x8(y_flag,x_flag,'m');x_flag+=6;break;
		case 'n'		:	display_string_5x8(y_flag,x_flag,'n');x_flag+=6;break;
		case 'o'		:	display_string_5x8(y_flag,x_flag,'o');x_flag+=6;break;
		case 'p'		:	display_string_5x8(y_flag,x_flag,'p');x_flag+=6;break;
		case 'q'		:	display_string_5x8(y_flag,x_flag,'q');x_flag+=6;break;
		case 'r'		:	display_string_5x8(y_flag,x_flag,'r');x_flag+=6;break;
		case 's'		:	display_string_5x8(y_flag,x_flag,'s');x_flag+=6;break;
		case 't'		:	display_string_5x8(y_flag,x_flag,'t');x_flag+=6;break;
		case 'u'		:	display_string_5x8(y_flag,x_flag,'u');x_flag+=6;break;
		case 'v'		:	display_string_5x8(y_flag,x_flag,'v');x_flag+=6;break;
		case 'w'		:	display_string_5x8(y_flag,x_flag,'w');x_flag+=6;break;
		case 'x'		:	display_string_5x8(y_flag,x_flag,'x');x_flag+=6;break;
		case 'y'		:	display_string_5x8(y_flag,x_flag,'y');x_flag+=6;break;
		case 'z'		:	display_string_5x8(y_flag,x_flag,'z');x_flag+=6;break;
                case '-':           display_string_5x8(y_flag,x_flag,'-');x_flag+=6;break;  //字符-
                case '`':            display_string_5x8(y_flag,x_flag,'`');x_flag+=6;break;  //字符`
                case ' ':            display_string_5x8(y_flag,x_flag,' ');x_flag+=6;break;  //空格
                case '.':            display_string_5x8(y_flag,x_flag,'.'); x_flag+=6; break;  //小数点，直接显示在右下角
                case '*':            display_string_5x8(y_flag,x_flag,'*'); x_flag+=6; break;  //小数点，直接显示在右下角
                case '&':            display_string_5x8(y_flag,x_flag,'&'); x_flag+=6; break;  //小数点，直接显示在右下角
                case '!':            display_string_5x8(y_flag,x_flag,'!'); x_flag+=6; break;  //小数点，直接显示在右下角
               case '\n': case '\r':  y_flag++,x_flag=0;  break;  //换行符的下一个字母将清屏
               default :            display_string_5x8(y_flag,x_flag,' ');x_flag+=6;break;//显示不出来的字母用空格替代
     }
 }
if(x_flag>123)
{
	 x_flag=0;
	 y_flag++;

}
if(y_flag>7)
{
	 clear_screen(); x_flag=0;y_flag=0;
	 FirstChrFlag=1;
}
	return (ch);  //返回显示的字符(putchar函数标准格式要求返回显示字符)
}
int fputc(int ch, FILE *f)
{
//串口发送函数
	putchar(ch);
  return ch;
}
int fputs(const char *_ptr, register FILE *_fp)
{
  unsigned int i, len;

  len = strlen(_ptr);

  for(i=0 ; i<len ; i++)
  {
	  putchar(_ptr[i]);
  }
  return len;
}
/****************************************************************************
* 名    称：putchar()
* 功    能：向标准终端设备发送一字节数据(1个ASCII字符)
* 入口参数：ch: 待发送的字符  
* 出口参数：发出的字符
* 说    明: UART.c内的putchar函数printf函数，这里从串口输出字符到PC机的超
            级终端软件上，printf的结果将打印到超级终端上。供对比。
****************************************************************************/
/*
int putchar(int ch)
{
  if (ch == '\n')        //  '\n'(回车)扩展成 '\n''\r' (回车+换行) 
  {
    UART_PutChar(0x0d) ; //'\r'
  }
  UART_PutChar(ch);      //从串口发出数据  
  return (ch);
}
*/
