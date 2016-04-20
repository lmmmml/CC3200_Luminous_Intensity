#include "GY30.h"
#define	  SlaveAddress           0x23 //IIC地址
#define	  PowerDown		 0x00
#define   PowerOn		 0x01
#define   Reset			 0x07
#define   One_H			 0x10
unsigned char BUF[2];


void Cmd_Write_BH1750(unsigned char cmd)
{
  unsigned char ucBuffer[1];
  I2C_IF_Write(SlaveAddress, ucBuffer,1,0x01);
  Delay_Ms(5);
}
void Start_BH1750(void)
{
  Cmd_Write_BH1750(PowerOn);	 //power on
  Cmd_Write_BH1750(Reset);	//clear
  Cmd_Write_BH1750(One_H);  //一次H分辨率模式，至少120ms，之后自动断电模式  
}
int Read_BH1750(void)
{   	
//  int Rec=0;
//  IIC_Start();                          //起始信号
//  IIC_Send_Byte(SlaveAddress+1);         //发送设备地址+读信号
//  while(IIC_Wait_Ack());
//  BUF[0]=IIC_Read_Byte(1);  //发送ACK
//  BUF[1]=IIC_Read_Byte(0);  //发送NACK
//
//  IIC_Stop();                           //停止信号
//  Delay_Ms(5);
//  Rec = ((Rec|BUF[0])<<8)|BUF[1];
//  return Rec;
    int val=0;
    unsigned char ucBuffer[2];
    I2C_IF_Read(SlaveAddress,ucBuffer,2);
    Delay_Ms(5);
    val = ((val|ucBuffer[0])<<8)|ucBuffer[1];
    return val;
}
// void Convert_BH1750(void)
// {
	
	// result=BUF[0];
	// result=(result<<8)+BUF[1];  //合成数据，即光照数据
	
	// result_lx=(float)result/1.2;

// }
























