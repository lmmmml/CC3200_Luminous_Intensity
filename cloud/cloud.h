/*****************************************************************************
*
*  cloud.h - Exosite library interface header
*  Copyright (C) 2012 Exosite LLC
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#ifndef CLOUD_H
#define CLOUD_H


#include <stdint.h>


// DEFINES
#define CIK_LENGTH                              40
#define MAX_UUID_LENGTH                         40
#define MAX_VENDOR_LENGTH                       20
#define MAX_MODEL_LENGTH                        20

/*!< This defines the maximum size that a string can be for sending data
   to Exosite.  It is used to prevent cloud_strlen from overrunning.
   If you are have a need to increase string length, you can freely adjust
   this number up to uint16_t*/




// ENUMS
/*!
 * Tracks the current state of the device on the Exosite Platform.
 */
typedef enum CLOUD_DEVICE_STATE_tag
{
    CLOUD_STATE_CONNECTION_ERROR,       /*!< Error in connecting to Exosite */
    CLOUD_STATE_VALID_CIK,              /*!< CIK is valid  */
    CLOUD_STATE_DEVICE_NOT_ENABLED,     /*!< Device has not been enabled */
    CLOUD_STATE_R_W_ERROR,              /*!< R/W error */
    CLOUD_STATE_UNABLE_TO_MAKE_REQUEST, /*!< string was too long */
    CLOUD_STATE_INIT_COMPLETE,          /*!< Initialization successfully completed. */
    CLOUD_STATE_NOT_COMPLETE,           /*!< Device has not yet completed initialization. */
    CLOUD_STATE_INIT_ERROR,             /*!< An error occured in cloud_init(). */
    CLOUD_STATE_NO_RESPONSE             /*!< No response was received from modem */
}CLOUD_STATE;

typedef enum
{
  LED_OFF = 0,
  LED_ON,
  LED_BLINK
}eLEDStatus;


// PUBLIC FUNCTIONS
CLOUD_STATE cloud_activate();
CLOUD_STATE cloud_init(const char *vendor, const char *model);

int32_t cloud_write(const char * writeData, uint16_t length);
int32_t cloud_read(const char * alias, char * readResponse, uint16_t buflen, uint16_t * responseSize);
int32_t cloud_readSingle(const char * alias, char * readResponse, uint16_t buflen, uint16_t * length);

int32_t cloud_rawRpcRequest(const char * requestBody, uint16_t requestLength, char * responseBuffer, int32_t responseBufferLength);

int8_t cloud_getTimestamp(int32_t * timestamp);
int32_t cloud_getBody(char *response, char **bodyStart, uint16_t *bodyLength);

uint8_t cloud_isCIKValid(char cik[CIK_LENGTH]);
void cloud_setCIK(char * pCIK);

uint8_t cloud_resetCik();
void CloudsiteTask(void *pvParameters);
void cloud_getCIK(char * pCIK);
#endif



