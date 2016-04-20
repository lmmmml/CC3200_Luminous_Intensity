/*
 * LCD.c
 *
 *  Created on: 2015��8��9��
 *      Author: Mars
 */

#include "includes.h"
#include "LCD.h"

static unsigned long ulRe[]=
{
    GPIOA0_BASE,
    GPIOA1_BASE,
    GPIOA2_BASE,
    GPIOA3_BASE,
    GPIOA4_BASE
};

#define PUCGPIOPIN(ucPin)       (1 << (ucPin % 8))
#define PUIGPIOPORT(ucPin)      (ulRe[(ucPin / 8)])
#define GPIO_SET(ucPin)         HWREG(PUIGPIOPORT(ucPin) + (GPIO_O_GPIO_DATA + (PUCGPIOPIN(ucPin) << 2)))

#define         RS_PORT                 (6)    //61
#define         SCLK_PORT               (7)     //62
#define         SDA_PORT                (31)    //45
#define         CS1_PORT                (28)    //18 
#define         RESET_PORT              (30)    //53

#define         _EINT()                 IntMasterEnable()
#define         _DINT()                 IntMasterDisable()

#define    	RS_H			GPIO_SET(RS_PORT)=1 << (RS_PORT % 8)
#define		RS_L			GPIO_SET(RS_PORT)=0 << (RS_PORT % 8)
#define	        SCLK_H			GPIO_SET(SCLK_PORT)=1 << (SCLK_PORT % 8)
#define		SCLK_L			GPIO_SET(SCLK_PORT)=0 << (SCLK_PORT % 8)
#define		SDA_H			GPIO_SET(SDA_PORT)=1 << (SDA_PORT % 8)
#define         SDA_L			GPIO_SET(SDA_PORT)=0 << (SDA_PORT % 8)
#define		CS1_H			GPIO_SET(CS1_PORT)=1 << (CS1_PORT % 8)
#define         CS1_L			GPIO_SET(CS1_PORT)=0 << (CS1_PORT % 8)
#define		RESET_H			GPIO_SET(RESET_PORT)=1 << (RESET_PORT % 8)//GPIO_IF_Set(RESET_PORT, PUIGPIOPORT(RESET_PORT), PUCGPIOPIN(RESET_PORT), 1)//
#define		RESET_L			GPIO_SET(RESET_PORT)=0 << (RESET_PORT % 8)


/*******************************************
��������: initial_lcd
��    ��: ��ʼ��LCD
��    ��: ��
����ֵ  : ��
********************************************/

void LCD_PinMuxConfig(void)
{
    //
    // Enable Peripheral Clocks 
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_61 for GPIO Output
    //
    MAP_PinTypeGPIO(PIN_61, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x40, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_62 for GPIO Output
    //
    MAP_PinTypeGPIO(PIN_62, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA0_BASE, 0x80, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_53 for GPIO Input
    //
    MAP_PinTypeGPIO(PIN_53, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA3_BASE, 0x40, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_18 for GPIO Output
    //
    MAP_PinTypeGPIO(PIN_18, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA3_BASE, 0x10, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_45 for GPIO Output
    //
    MAP_PinTypeGPIO(PIN_45, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA3_BASE, 0x80, GPIO_DIR_MODE_OUT);
}

void initial_lcd()
{
                RS_H;SCLK_H;SDA_H;CS1_H;RESET_H;
		RESET_L; /*�͵�ƽ��λ*/
		MAP_UtilsDelay(1000);
		RESET_H; /*��λ���*/
		MAP_UtilsDelay(1000);
		transfer_command(0xe2); /*��λ*/
		MAP_UtilsDelay(500);
		transfer_command(0x2c); /*��ѹ���� 1*/
		MAP_UtilsDelay(500);
		transfer_command(0x2e); /*��ѹ���� 2*/
		MAP_UtilsDelay(500);
		transfer_command(0x2f); /*��ѹ���� 3*/
		MAP_UtilsDelay(500);
		transfer_command(0x24); /*�ֵ��Աȶȣ������÷�Χ 0x20��0x27*/
		transfer_command(0x81); /*΢���Աȶ�*/
		transfer_command(0x1A); /*΢���Աȶȵ�ֵ�������÷�Χ 0x00��0x3f*/
		transfer_command(0xa2); /*1/9 ƫѹ�ȣ�bias��*/
		transfer_command(0xc8); /*��ɨ��˳�򣺴��ϵ���*/
		transfer_command(0xa0); /*��ɨ��˳�򣺴�����*/
		transfer_command(0x42);
		transfer_command(0xaf); /*����ʾ*/
}



/*******************************************
��������: lcd_address
��    ��: LCD��ʾλ��ַѡ��
��    ��: page--Ҫд���ҳ
	  	  column--Ҫд�����
����ֵ  : ��
********************************************/
void lcd_address(unsigned char page,unsigned char column)
{
		CS1_L;

		column=column+4; //
		page=page+1;
		transfer_command(0xb0+page); //����ҳ��ַ��ÿҳ�� 8 �С�һ������� 64 �б��ֳ� 8 ��ҳ������ƽ����˵�ĵ� 1 ҳ���� LCD ���� IC ���ǵ� 0 ҳ�������������ȥ 1*/
		transfer_command((((column)>>4)&0x0f)+0x10); //�����е�ַ�ĸ� 4 λ
		transfer_command((column)&0x0f); //�����е�ַ�ĵ� 4 λ
}


/*******************************************
��������: display_graphic_32x32
��    ��: LCD��ʾ32x32���ַ�
��    ��: page--Ҫд���ҳ
	  	  column--Ҫд�����
	  	  dp--Ҫ��ʾ���ַ�
����ֵ  : ��
********************************************/
void display_graphic_32x32(unsigned char page,unsigned char column,unsigned char *dp)
{
		unsigned char i,j;
		CS1_L;
		for(j=0;j<4;j++)
		{
		lcd_address(page+j,column);
		for (i=0;i<32;i++)
		{
		transfer_data(*dp); /*д���ݵ� LCD,ÿд��һ�� 8 λ�����ݺ��е�ַ�Զ��� 1*/
		dp++;
		}
		}
}

/*******************************************
��������: display_graphic_16x16
��    ��: LCD��ʾ16x16���ַ�
��    ��: page--Ҫд���ҳ
	  	  column--Ҫд�����
	  	  dp--Ҫ��ʾ���ַ�
����ֵ  : ��
********************************************/
void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char *dp)
{
		unsigned char i,j;
		CS1_L;
		for(j=0;j<2;j++)
		{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{
		transfer_data(*dp); /*д���ݵ� LCD,ÿд��һ�� 8 λ�����ݺ��е�ַ�Զ��� 1*/
		dp++;
		}
		}
		CS1_H;
}
/*******************************************
��������: display_graphic_8x16
��    ��: LCD��ʾ8x16���ַ�
��    ��: page--Ҫд���ҳ
	  	  column--Ҫд�����
	  	  dp--Ҫ��ʾ���ַ�
����ֵ  : ��
********************************************/
void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp)
{
		unsigned char i,j;
		CS1_L;
		for(j=0;j<2;j++)
		{
		lcd_address(page+j,column);
		for (i=0;i<8;i++)
		{
		transfer_data(*dp); /*д���ݵ� LCD,ÿд��һ�� 8 λ�����ݺ��е�ַ�Զ��� 1*/
		dp++;
		}
		}
		CS1_H;
}


//void display_string_5x8(unsigned int page,unsigned int column,unsigned char *text)
//{
//		unsigned int i=0,j,k;
//		CS1_L;
//		while(text[i]>0x00)
//			{
//				if((text[i]>=0x20)&&(text[i]<0x7e))
//				{
//					j=text[i]-0x20;
//					lcd_address(page,column);
//					for(k=0;k<5;k++)
//				{
//					transfer_data(ascii_table_5x8[j][k]);
//				}
//				i++;
//				column+=6;
//		}
//			else
//				i++;
//		}
//}


/*******************************************
��������: display_graphic_5x8
��    ��: LCD��ʾ5x8���ַ�
��    ��: page--Ҫд���ҳ
	  	  column--Ҫд�����
	  	  dp--Ҫ��ʾ���ַ�
����ֵ  : ��
********************************************/
void display_string_5x8(unsigned int page,unsigned int column,unsigned char text)
{
		unsigned int j,k;
		CS1_L;

					j=text-0x20;
					lcd_address(page,column);
					for(k=0;k<5;k++)
				{
					transfer_data(ascii_table_5x8[j][k]);
				}
}

/*******************************************
��������: clear_screen
��    ��: LCD���ȫ��
��    ��: ��
����ֵ  : ��
********************************************/
void clear_screen()
{
		unsigned char i,j;
		CS1_L;
		for(i=0;i<8;i++)
		{
			transfer_command(0xb0+i); //set page address,
			transfer_command(0x10);
			transfer_command(0x00);
		for(j=0;j<132;j++)
		{
		transfer_data(0x00);
		}
		}
		CS1_H;
}

//void display_string_5x8(unsigned int page,unsigned int column,unsigned char *text)
//{
//		unsigned int i=0,j,k;
//		CS1_L;
//		while(text[i]>0x00)
//		{
//		if((text[i]>=0x20)&&(text[i]<0x7e))
//		{
//		j=text[i]-0x20;
//		lcd_address(page,column);
//		for(k=0;k<5;k++)
//		{
//		transfer_data(ascii_table_5x8[j][k]);/*��ʾ 5x7 �� ASCII �ֵ� LCD �ϣ�y Ϊҳ��ַ�� x Ϊ�е�ַ�����Ϊ
//		����*/
//		}
//		i++;
//		column+=6;
//		}
//		else
//		i++;
//		}
//}


/*******************************************
��������: disp_grap
��    ��: LCD��ʾ128x64���ַ�
��    ��: dp--Ҫ��ʾ���ַ�
����ֵ  : ��
********************************************/
void disp_grap(char *dp)
{
		int i,j;
		for(i=0;i<8;i++)
		{
				CS1_L;
				transfer_command(0xb0+i); //set page address,
				transfer_command(0x10);
				transfer_command(0x01);
			for(j=0;j<128;j++)
			{
				transfer_data(*dp);
				dp++;
			}
		}
}
/*******************************************
��������: transfer_command
��    ��: ��LCD��д����
��    ��: data1--Ҫд�������
����ֵ  : ��
********************************************/
void transfer_command(int data1)
{
char i;
_DINT();
CS1_L;
RS_L;
for(i=0;i<8;i++)
{
SCLK_L;
MAP_UtilsDelay(3);
if(data1&0x80)
	SDA_H;
else
	SDA_L;
MAP_UtilsDelay(3);
//Delay1(5);
SCLK_H;
//Delay1(5);
data1<<=1;
}
_EINT();
}
/*******************************************
��������: transfer_command
��    ��: ��LCD��д����
��    ��: data1--Ҫд�������
����ֵ  : ��
********************************************/
void transfer_data(int data1)
{
char i;
_DINT();
CS1_L;
RS_H;
for(i=0;i<8;i++)
{
SCLK_L;
MAP_UtilsDelay(3);
if(data1&0x80) SDA_H;
else SDA_L;
MAP_UtilsDelay(3);
SCLK_H;
data1<<=1;
}
_EINT();
}
/*******************************************
��������: Delay
��    ��: ��ʱ����
��    ��: ��ʹ�ú����������������
����ֵ  : ��
********************************************/
void Delay(int i)
{
int j,k;
for(j=0;j<i;j++)
for(k=0;k<990;k++);
}
/*******************************************
��������: Delay1
��    ��: ��ʱ����
��    ��: ��ʹ�ú����������������
����ֵ  : ��
********************************************/
void Delay1(int i)
{
int j,k;
for(j=0;j<i;j++)
for(k=0;k<10;k++);
}


/*******************************************
��������: display_graphic_16x16_string
��    ��: LCD��ʾ16x16���ַ���
��    ��:
page--Ҫд���ҳ
column--Ҫд�����
size--Ҫ��ʾ���ַ�������
����ֵ  : ��
********************************************/
void display_graphic_16x16_string(unsigned char page,unsigned char column,unsigned char size,unsigned char *dp)
{
	unsigned char i=0,j,Flog,c;
	c=column;
	CS1_L;
	for(Flog=0;Flog<size;Flog++)
	{
			column=c+Flog*16;
			for(j=0;j<2;j++)
			{
			lcd_address(page+j,column);
			for (i=0;i<16;i++)
			{
			transfer_data(*dp); /*д���ݵ� LCD,ÿд��һ�� 8 λ�����ݺ��е�ַ�Զ��� 1*/
			dp++;
			}
			}
	}
			CS1_H;
}

/*******************************************
��������: display_graphic_16x16_string_turn
��    ��: LCD��ʾ16x16���ַ������ڰ׵�ɫ��ת��
��    ��:
page--Ҫд���ҳ
column--Ҫд�����
size--Ҫ��ʾ���ַ�������
����ֵ  : ��
********************************************/
void display_graphic_16x16_string_turn(unsigned char page,unsigned char column,unsigned char size,unsigned char *dp)
{
	unsigned char i=0,j,Flog,c;
	c=column;
	CS1_L;
	for(Flog=0;Flog<size;Flog++)
	{
			column=c+Flog*16;
			for(j=0;j<2;j++)
			{
			lcd_address(page+j,column);
			for (i=0;i<16;i++)
			{
			transfer_data(~*dp); /*д���ݵ� LCD,ÿд��һ�� 8 λ�����ݺ��е�ַ�Զ��� 1*/
			dp++;
			}
			}
	}
			CS1_H;
}

/*ȫ�� ASCII �б�:5x8 ����*/
extern unsigned char  ascii_table_5x8[95][5]={

0x00,0x00,0x00,0x00,0x00,//space
0x00,0xDF,0xDF,0x00,0x00,//!
0x00,0x07,0x00,0x07,0x00,//"
0x14,0x7f,0x14,0x7f,0x14,//#
0x24,0x2a,0x7f,0x2a,0x12,//$
0x23,0x13,0x08,0x64,0x62,//%
0x78,0xFC,0xFC,0xFC,0x78,//&
0x00,0x05,0x07,0x00,0x00,//]
0x00,0x1c,0x22,0x41,0x00,//(
0x00,0x41,0x22,0x1c,0x00,//)
0x78,0x84,0x84,0x84,0x78,//*
0x08,0x08,0x3e,0x08,0x08,//+
0x00,0x50,0x30,0x00,0x00,//,
0x08,0x08,0x08,0x08,0x08,//-
0x00,0x60,0x60,0x00,0x00,//.
0x20,0x10,0x08,0x04,0x02,///
0x3e,0x51,0x49,0x45,0x3e,//0
0x00,0x42,0x7f,0x40,0x00,//1
0x42,0x61,0x51,0x49,0x46,//2
0x21,0x41,0x45,0x4b,0x31,//3
0x18,0x14,0x12,0x7f,0x10,//4
0x27,0x45,0x45,0x45,0x39,//5
0x3c,0x4a,0x49,0x49,0x30,//6
0x01,0x71,0x09,0x05,0x03,//7
0x36,0x49,0x49,0x49,0x36,//8
0x06,0x49,0x49,0x29,0x1e,//9
0x00,0x36,0x36,0x00,0x00,//:
0x00,0x56,0x36,0x00,0x00,//;
0x08,0x14,0x22,0x41,0x00,//<
0x14,0x14,0x14,0x14,0x14,//=
0x00,0x41,0x22,0x14,0x08,//>
0x02,0x01,0x51,0x09,0x06,//?
0x32,0x49,0x79,0x41,0x3e,//@
0x7e,0x11,0x11,0x11,0x7e,//A
0x7f,0x49,0x49,0x49,0x36,//B
0x3e,0x41,0x41,0x41,0x22,//C
0x7f,0x41,0x41,0x22,0x1c,//D
0x7f,0x49,0x49,0x49,0x41,//E
0x7f,0x09,0x09,0x09,0x01,//F
0x3e,0x41,0x49,0x49,0x7a,//G
0x7f,0x08,0x08,0x08,0x7f,//H
0x00,0x41,0x7f,0x41,0x00,//I
0x20,0x40,0x41,0x3f,0x01,//J
0x7f,0x08,0x14,0x22,0x41,//K
0x7f,0x40,0x40,0x40,0x40,//L
0x7f,0x02,0x0c,0x02,0x7f,//M
0x7f,0x04,0x08,0x10,0x7f,//N
0x3e,0x41,0x41,0x41,0x3e,//O
0x7f,0x09,0x09,0x09,0x06,//P
0x3e,0x41,0x51,0x21,0x5e,//Q
0x7f,0x09,0x19,0x29,0x46,//R
0x46,0x49,0x49,0x49,0x31,//S
0x01,0x01,0x7f,0x01,0x01,//T
0x3f,0x40,0x40,0x40,0x3f,//U
0x1f,0x20,0x40,0x20,0x1f,//V
0x3f,0x40,0x38,0x40,0x3f,//W
0x63,0x14,0x08,0x14,0x63,//X
0x07,0x08,0x70,0x08,0x07,//Y
0x61,0x51,0x49,0x45,0x43,//Z
0x00,0x7f,0x41,0x41,0x00,//[
0x02,0x04,0x08,0x10,0x20,//\//
0x00,0x41,0x41,0x7f,0x00,//]
0x04,0x02,0x01,0x02,0x04,//^
0x40,0x40,0x40,0x40,0x40,//_
0x01,0x02,0x04,0x00,0x00,//`
0x20,0x54,0x54,0x54,0x78,//a
0x7f,0x48,0x48,0x48,0x30,//b
0x38,0x44,0x44,0x44,0x44,//c
0x30,0x48,0x48,0x48,0x7f,//d
0x38,0x54,0x54,0x54,0x58,//e
0x00,0x08,0x7e,0x09,0x02,//f
0x48,0x54,0x54,0x54,0x3c,//g
0x7f,0x08,0x08,0x08,0x70,//h
0x00,0x00,0x7a,0x00,0x00,//i
0x20,0x40,0x40,0x3d,0x00,//j
0x7f,0x20,0x28,0x44,0x00,//k
0x00,0x41,0x7f,0x40,0x00,//l
0x7c,0x04,0x38,0x04,0x7c,//m
0x7c,0x08,0x04,0x04,0x78,//n
0x38,0x44,0x44,0x44,0x38,//o
0x7c,0x14,0x14,0x14,0x08,//p
0x08,0x14,0x14,0x14,0x7c,//q
0x7c,0x08,0x04,0x04,0x08,//r
0x48,0x54,0x54,0x54,0x24,//s
0x04,0x04,0x3f,0x44,0x24,//t
0x3c,0x40,0x40,0x40,0x3c,//u
0x1c,0x20,0x40,0x20,0x1c,//v
0x3c,0x40,0x30,0x40,0x3c,//w
0x44,0x28,0x10,0x28,0x44,//x
0x04,0x48,0x30,0x08,0x04,//y
0x44,0x64,0x54,0x4c,0x44,//z
0x08,0x36,0x41,0x41,0x00,//{
0x00,0x00,0x77,0x00,0x00,//|
0x00,0x41,0x41,0x36,0x08,//}
0x04,0x02,0x02,0x02,0x01,//~
};
