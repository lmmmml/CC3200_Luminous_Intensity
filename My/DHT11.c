#include "DHT11.h"
#include "includes.h"
 
unsigned char  Flag;
unsigned char  Temp;
unsigned char  T_data_H,T_data_L,RH_data_H,RH_data_L,checkdata;
unsigned char  T_data_H_temp,T_data_L_temp,RH_data_H_temp,RH_data_L_temp,checkdata_temp;
unsigned char  Comdata;


void Delay_10us(int  nus)
{
  int i=nus;
  for(i=nus;i>0;i--)
    UtilsDelay(118);
}

void Delay_Ms(int nms)
{    
  int i=nms;
  for(i=nms;i>0;i--)
  Delay_10us(110);
}


void COM(void)
{   
  unsigned char i,I;         
  for(i=0;i<8;i++)	   
  {       
    Flag=2;
    Date_In();
    I = Date_Read();
    while(!I)
      I = Date_Read();
    Delay_10us(3);
    Temp=0;
    I = Date_Read();
    if(I)
       Temp=1;
    Flag=2;
    I = Date_Read();
    while(I)
      I = Date_Read();;
    if(Flag==1)
      break;	        
    Comdata<<=1;
    Comdata|=Temp;     
  }
}



int RH(void)
{
  unsigned char I;
  int Rec=0;
  Date_Out();
  Date_Send_L();
  osi_Sleep(18);
  Date_Send_H();
  Delay_10us(4);
//  Date_Send_H();
  Date_In();
  I = Date_Read();
  if(!Date_Read())		  
  {
    Flag=2; 
    I = Date_Read();
    while(!I)
      I = Date_Read();
    Flag=2;
    I = Date_Read();
    while(I)
      I = Date_Read();
    COM();
    RH_data_H_temp=Comdata;
    COM();
    RH_data_L_temp=Comdata;
    COM();
    T_data_H_temp=Comdata;
    COM();
    T_data_L_temp=Comdata;
    COM();
    checkdata_temp=Comdata;
    Date_Out();
    Date_Send_H();
    Temp=(T_data_H_temp+T_data_L_temp+RH_data_H_temp+RH_data_L_temp);
    if(Temp==checkdata_temp)
    {
      RH_data_H=RH_data_H_temp;
      RH_data_L=RH_data_L_temp;
      T_data_H=T_data_H_temp;
      T_data_L=T_data_L_temp;
      checkdata=checkdata_temp;
      Rec = ((Rec|RH_data_H)<<8)|T_data_H;
      return Rec;
    }
  }
  return ERROR;
}
