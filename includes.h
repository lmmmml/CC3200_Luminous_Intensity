#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#undef _DLIB_FILE_DESCRIPTOR
#define _DLIB_FILE_DESCRIPTOR 1

// Standard includes
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Driverlib includes
#include "utils.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_adc.h"
#include "hw_ints.h"
#include "hw_gprcm.h"
#include "hw_gpio.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "uart.h"
#include "i2c.h"
#include "i2c_if.h"
#include "pinmux.h"
#include "pin.h"
#include "adc.h"
#include "gpio.h"
#include "netcfg.h"
#include "smartconfig.h"
#include "gpio_if.h"
#include "adc_userinput.h"
#include "uart_if.h"
#include "common.h"
#include "Stat_Communicate.h"
#include "LCD.h"

#include "cloud_pal.h"
#include "device_status.h"
#include "cloud_connect.h"
#include "cloud.h"

#include "DHT11.h"
#include "GY30.h"
#endif