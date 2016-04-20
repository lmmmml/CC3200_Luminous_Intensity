#ifndef _CLOUD_CONNECT_
#define _CLOUD_CONNECT_

#include "includes.h"
#define AUTO_CONNECTION_TIMEOUT_COUNT   50      /* 5 Sec */

int ConfigureMode(int iMode);
long ConnectToNetwork();


#endif