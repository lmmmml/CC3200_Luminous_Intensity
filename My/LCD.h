/*
 * LCD.h
 *
 *  Created on: 2015Äê8ÔÂ9ÈÕ
 *      Author: Mars
 */
#ifndef LCD_H_
#define LCD_H_

extern void transfer_data(int data1);
extern void transfer_command(int data1);
extern unsigned char  ascii_table_5x8[95][5];
extern void Delay(int i);
extern void Delay1(int i);
extern void disp_grap(char *dp);
extern void initial_lcd();
extern void clear_screen();
extern void display_string_5x8(unsigned int page,unsigned int column,unsigned char text);
extern void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp);
extern void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char *dp);
extern void display_graphic_32x32(unsigned char page,unsigned char column,unsigned char *dp);
extern void lcd_address(unsigned char page,unsigned char column);
extern void display_graphic_16x16_string(unsigned char page,unsigned char column,unsigned char size,unsigned char *dp);
extern void LCD_PinMuxConfig(void);




#endif /* LCD_H_ */
