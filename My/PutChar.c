/*
 * PutChar.c
 *
 *  Created on: 2015��8��9��
 *      Author: Mars
 */
#include "includes.h" /*��Ƭ���Ĵ���ͷ�ļ�*/
#include "ctype.h"       /*isdigit������Ҫ��ͷ�ļ�*/
#include "LCD.h" /*LCD������ͷ�ļ�*/
#include "stdio.h"
#include "string.h"
//#include 'PutChar.h'
char  FirstChrFlag=1,x_flag=0,y_flag=0;  //��һ���ַ���־λ

//int printf(const char *_format, ...);
//int putchar(int ch);
/****************************************************************************
* ��    �ƣ�putchar()
* ��    �ܣ���LCD��ʾ�����һ��ASCII�ַ���
* ��ڲ�����ch: �����͵��ַ�  
* ���ڲ������������ַ�
* ˵    ��: printf��������øú�����Ϊ�ײ�������ú������ַ������LCD��
            ���printf�Ľ������ʾ��LCD�ϡ�
****************************************************************************/

 int putchar(int ch)
{

if(FirstChrFlag){clear_screen(); x_flag=0;y_flag=0;}//��һ���ַ�������ʱ�������һ����ʾ����
FirstChrFlag=0;
if(ch=='\f')     {clear_screen(); x_flag=0;y_flag=0;}//'\f'��ʾ��ֽ��ҳ���൱�������ʾ
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
//���ֺͶ�ӦASCII��ĸ֮���0x30   '1'=0x31 '2'=0x32... isdigitҲ��C���Ա�׼����
else             //���򣬲������֣�����ĸ
{
   switch(ch)    //������ĸѡ������֧
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
                case '-':           display_string_5x8(y_flag,x_flag,'-');x_flag+=6;break;  //�ַ�-
                case '`':            display_string_5x8(y_flag,x_flag,'`');x_flag+=6;break;  //�ַ�`
                case ' ':            display_string_5x8(y_flag,x_flag,' ');x_flag+=6;break;  //�ո�
                case '.':            display_string_5x8(y_flag,x_flag,'.'); x_flag+=6; break;  //С���㣬ֱ����ʾ�����½�
                case '*':            display_string_5x8(y_flag,x_flag,'*'); x_flag+=6; break;  //С���㣬ֱ����ʾ�����½�
                case '&':            display_string_5x8(y_flag,x_flag,'&'); x_flag+=6; break;  //С���㣬ֱ����ʾ�����½�
                case '!':            display_string_5x8(y_flag,x_flag,'!'); x_flag+=6; break;  //С���㣬ֱ����ʾ�����½�
               case '\n': case '\r':  y_flag++,x_flag=0;  break;  //���з�����һ����ĸ������
               default :            display_string_5x8(y_flag,x_flag,' ');x_flag+=6;break;//��ʾ����������ĸ�ÿո����
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
	return (ch);  //������ʾ���ַ�(putchar������׼��ʽҪ�󷵻���ʾ�ַ�)
}
int fputc(int ch, FILE *f)
{
//���ڷ��ͺ���
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
* ��    �ƣ�putchar()
* ��    �ܣ����׼�ն��豸����һ�ֽ�����(1��ASCII�ַ�)
* ��ڲ�����ch: �����͵��ַ�  
* ���ڲ������������ַ�
* ˵    ��: UART.c�ڵ�putchar����printf����������Ӵ�������ַ���PC���ĳ�
            ���ն�����ϣ�printf�Ľ������ӡ�������ն��ϡ����Աȡ�
****************************************************************************/
/*
int putchar(int ch)
{
  if (ch == '\n')        //  '\n'(�س�)��չ�� '\n''\r' (�س�+����) 
  {
    UART_PutChar(0x0d) ; //'\r'
  }
  UART_PutChar(ch);      //�Ӵ��ڷ�������  
  return (ch);
}
*/
