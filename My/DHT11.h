#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "uart.h"
#include "interrupt.h"
#include "pinmux.h"
#include "utils.h"
#include "prcm.h"
#include "gpio.h"
#include "gpio_if.h"



//IO∑ΩœÚ…Ë÷√
#define Date_Clk_Init() PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK)
#define Date_In()  GPIODirModeSet(GPIOA0_BASE, 0x20, GPIO_DIR_MODE_IN)
#define Date_Out() GPIODirModeSet(GPIOA0_BASE, 0x20, GPIO_DIR_MODE_OUT)//60
#define Date_Read()  ReadGPIO(GPIOA0_BASE,0x20)
#define Date_Send_H()   GPIO_IF_Set(5, GPIOA0_BASE,0x20, 1) 	//SDA	
#define Date_Send_L()   GPIO_IF_Set(5, GPIOA0_BASE,0x20, 0)





extern void Delay_10us(int  nus);
extern void Delay_Ms(int nms);
extern void COM(void);
extern int RH(void);

