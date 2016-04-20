/*****************************************************************************
*
*  cloud.c - cloud cloud communications.
*  Copyright (C) 2012 cloud LLC
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
#include <stdio.h>
#include "includes.h"
#include "cloud_pal.h"
#include "cloud.h"
#include "../oslib/osi.h"
#include "../common/common.h"
#include "../common/gpio_if.h"
#include "../simplelink/include/wlan.h"
#include "cloud_connect.h"


extern unsigned int g_uiDeviceModeConfig;
unsigned char g_ucLEDStatus = LED_OFF;



static const char STR_TIMESTAMP_URL[] = "GET /timestamp ";
static const char STR_CIK_HEADER[] = "X-cloud-CIK: ";
static const char STR_CONTENT_LENGTH[] = "Content-Length: ";
static const char STR_READ_URL[] = "GET /download/download_data.php?";
static const char STR_WRITE_URL[] = "POST /update/update_data.php ";
static const char STR_ACTIVATE_URL[] = "POST /provision/activate.php ";
static const char STR_RPC_URL[] = "POST /onep:v1/rpc/process ";
static const char STR_HTTP[] = "HTTP/1.1 ";
static const char STR_HOST[] = "Host: cc3200airkissphp.applinzi.com ";
static const char STR_ACCEPT[] = "Accept: application/x-www-form-urlencoded; charset=utf-8";
static const char STR_ACCEPT_JSON[] = "Accept: application/json; charset=utf-8";
static const char STR_CONTENT[] = "Content-Type: application/x-www-form-urlencoded; charset=utf-8";
static const char STR_CONTENT_JSON[] = "Content-Type: application/json; charset=utf-8";
static const char STR_CRLF[] = "\r\n";
static const char TEMPERATURE_MSG[]  = "Temperature"; 
static const char TEMPERATURE_TYPE[] =  "C" ;
static const char HUMIDITY_MSG[]     = "Humidity";
static const char HUMIDITY_TYPE[]     = "%RH";
// local functions
static uint8_t cloud_connect();
static uint8_t cloud_disconnect();
static uint8_t cloud_checkResponse(char * response, const char * code);


#define STR_VENDOR  "vendor"
#define STR_MODEL   "&model"
#define STR_SN      "&sn"
static char cikBuffer[CIK_LENGTH];
static char vendorBuffer[MAX_VENDOR_LENGTH];
static char modelBuffer[MAX_MODEL_LENGTH];
static char uuidBuffer[MAX_UUID_LENGTH];


/*!
 * Used to determine if there is currently an open socket.  This value is 1 if
 * we have an open socket, else 0.
 */
static uint8_t isSocketOpen = 0;


static CLOUD_STATE initState = CLOUD_STATE_NOT_COMPLETE;

int32_t cloud_getBody(char *response, char **bodyStart, uint16_t *bodyLength);


/*!
 * \brief Reset the cik to ""
 *
 * The following code would reset the contents of the cik to be an empty string.
   \code{.c}
   cloud_resetCik();
   \endcode
 *
 * \return Returns 0 if successful, else error code
 * \sa
 * \note
 * \warning
 */
uint8_t cloud_resetCik()
{

    cloudPal_setCik("");
    cikBuffer[0] = '\0';
    return 0;
}

/*!
 * \brief  Initializes the cloud libraries and attempts to activate the
 *          with cloud
 *
 * This **MUST** be called before any other cloud library calls are called.
 *
 * Assumes that the modem is setup and ready to make a socket connection.
 * This will fail if activation fails.  After initialization, this function
 * calls cloud_activate
 *
 * \param[in] vendor Pointer to string containing vendor name
 * \param[in] model Pointer to string containing model name
 *
 * \return Device Activation status
 */
CLOUD_STATE cloud_init(const char * vendor, const char *model)
{
    uint8_t retStatus = 0;
    // reset state
    initState = CLOUD_STATE_NOT_COMPLETE;
    

    cloudPal_init();

    // get cik and uuid and any other nvm stored data, into ram.
    cloudPal_getCik(cikBuffer);
    retStatus = cloudPal_getUuid(uuidBuffer);
    cloudPal_memcpy(vendorBuffer, vendor, MAX_VENDOR_LENGTH);
    cloudPal_memcpy(modelBuffer, model, MAX_MODEL_LENGTH);
    // create activation request
    if (retStatus)
    {
        initState = CLOUD_STATE_INIT_ERROR;
        return initState;
    }
    initState = cloud_activate();
    if (initState == CLOUD_STATE_VALID_CIK)
    {
        initState = CLOUD_STATE_INIT_COMPLETE;
    }
    return initState;
}




/*!
 * \brief  Makes a provisioning request to cloud.
 *
 *
 *
 * \return The devices activation status
 */
CLOUD_STATE cloud_activate()
{
    // assume content length will never be over 9999 bytes
    char contentLengthStr[5];
    uint8_t len_of_contentLengthStr;
    CLOUD_STATE retVal;
    uint16_t responseLen;
    char Body_Buff[256];

    // Try and activate device with cloud, four possible cases:
    // * We don't have a stored CIK and receive a 200 response with a CIK
    //    * Means device was enabled and this was our first connection
    // * We don't have a stored CIK and receive a 409 response
    //    * The device is not enabled.
    // * We have a stored CIK and receive a 409 response.
    //     *  Device has already been activated and has a valid CIK
    // * We have a stored CIK and receive a 401 response
    //    * R/W error
    sprintf(Body_Buff,"%s=%s%s=%s%s=%s",STR_VENDOR,vendorBuffer,STR_MODEL,modelBuffer,
            STR_SN,uuidBuffer);

    uint16_t bodyLength=strlen(Body_Buff);
    
    len_of_contentLengthStr = cloudPal_itoa((int)bodyLength, contentLengthStr, 5);


    cloud_connect();


    // send request
    cloudPal_socketWrite(STR_ACTIVATE_URL, sizeof(STR_ACTIVATE_URL) - 1);
    cloudPal_socketWrite(STR_HTTP, sizeof(STR_HTTP) - 1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF) - 1);

    // send Host header
    cloudPal_socketWrite(STR_HOST, sizeof(STR_HOST) - 1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF) - 1);

    // send content type header
    cloudPal_socketWrite(STR_CONTENT, sizeof(STR_CONTENT) - 1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF) - 1);

    // send content length header
    cloudPal_socketWrite(STR_CONTENT_LENGTH, sizeof(STR_CONTENT_LENGTH) - 1);
    cloudPal_socketWrite(contentLengthStr, len_of_contentLengthStr);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF) - 1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF) - 1);

    // send body
    
    cloudPal_socketWrite(Body_Buff,sizeof(Body_Buff)-1);
    cloudPal_sendingComplete();

    retVal = CLOUD_STATE_CONNECTION_ERROR;


    

    cloudPal_socketRead( cloudPal_rxBuffer, RX_BUFFER_SIZE, &responseLen);

    cloud_disconnect();

    
    if (responseLen == 0)
    {
        // if we didn't receive any data from the modem
        retVal = CLOUD_STATE_NO_RESPONSE;
    }
    else if (cloud_checkResponse(cloudPal_rxBuffer, "200"))
    {
        // we received a CIK.
        char * bodyStart;
        uint16_t bodyLength = 0;
        int32_t response;
        response = cloud_getBody(cloudPal_rxBuffer, &bodyStart, &bodyLength);
        if (response < 0)
        {
            return CLOUD_STATE_CONNECTION_ERROR;
        }
        if ((cloud_isCIKValid(bodyStart)) && (bodyLength == CIK_LENGTH))
        {
            // got a valid cik in the response
            cloudPal_setCik(bodyStart);
            cloudPal_memcpy(cikBuffer, bodyStart, CIK_LENGTH);
            retVal = CLOUD_STATE_VALID_CIK;
        }
    }
    else if (cloud_checkResponse(cloudPal_rxBuffer, "409"))
    {
        cloudPal_getCik(cikBuffer);

        if (cloud_isCIKValid(cikBuffer))
        {
            // If we receive a 409 and we do have a valid CIK, we will
            // assume we are good to go.
            retVal = CLOUD_STATE_VALID_CIK;

        }
        else
        {
            // if we don't have a CIK in nvm and we receive a 409
            // The device isn't enabled in the dashboard
            retVal = CLOUD_STATE_DEVICE_NOT_ENABLED;

        }
    }
    else if (cloud_checkResponse(cloudPal_rxBuffer, "404"))
    {
		// platform doesn't know about this device
		retVal = CLOUD_STATE_DEVICE_NOT_ENABLED;
    }
    else if (cloud_checkResponse(cloudPal_rxBuffer, "401"))
    {
        // RW error
        retVal = CLOUD_STATE_R_W_ERROR;
    }

    return retVal;

}


/*!
 * @brief  Sets bodyStart to point to the start of the http body
 *
 * @param response [in] Full http response with headers
 * @param bodyStart [out] Will be updated to point at the start of the http body
 * @param bodyLength [out] Length of the body
 * 
 * @return int32_t 0 if successful, else negative
 */
int32_t cloud_getBody(char *response, char **bodyStart, uint16_t *bodyLength)
{
    // find content length
    char* strStart;
    char* charAfterContentLengthValue = 0;
    char cr[] = "\r";
    char httpBodyToken[] = "\r\n\r\n";
    // find start of content length header
    strStart = cloudPal_strstr(response, STR_CONTENT_LENGTH);
    if (strStart <= 0)
    {
        return -1;
    }
    strStart = strStart + sizeof(STR_CONTENT_LENGTH) - 1;
    // get \r and set to '\0' for atoi
    charAfterContentLengthValue = cloudPal_strstr(strStart, cr);
    if (charAfterContentLengthValue <= 0)
    {
        return -2;
    }
    if (charAfterContentLengthValue != 0)
    {
        // temporarily null terminate the content length
        *charAfterContentLengthValue = '\0';
        *bodyLength = cloudPal_atoi(strStart);
        *charAfterContentLengthValue = '\r';
        
        
        strStart = cloudPal_strstr(strStart, httpBodyToken);
        if (strStart <= 0)
        {
            return -3;
        }
        *bodyStart = (char *)(strStart + sizeof(httpBodyToken)-1);
    }
    return 0;
}


/*!
 * \brief Checks if the given cik is valid
 *
 * Checks that the first 40 chars of `cik` are valid, lowercase hexadecimal bytes.
 *
 *
 *
 * \param[in] cik array of CIK_LENGTH bytes
 *
 * \return Returns 1 if successful, else 0
 * \sa
 * \note
 * \warning
 */
uint8_t cloud_isCIKValid(char cik[CIK_LENGTH])
{
    uint8_t i;

    for (i = 0; i < CIK_LENGTH; i++)
    {
        if (!(cik[i] >= 'a' && cik[i] <= 'f' || cik[i] >= '0' && cik[i] <= '9'))
        {
            return 0;
        }
    }

    return 1;
}



/*!
 *  \brief  Programs a new CIK to flash / non volatile
 *
 * \param[in] pCIK Pointer to CIK
 *
 */
void cloud_setCIK(char * pCIK)
{
    cloudPal_setCik((char *)pCIK);
    cloudPal_memcpy(cikBuffer, pCIK, CIK_LENGTH);
    return;
}



/*!
 *  \brief  Retrieves a the CIK from NVM and places it in to the string pointed
 *			at by \a cik
 *
 * \param[out] cik Pointer to CIK
 *
 */
void cloud_getCIK(char * cik)
{
    cloudPal_getCik((char *)cik);

    return;
}


/*!
 *  \brief  Writes data to cloud
 *
 * Writes the data in writeData to cloud.  The data is written as the body of
 * a POST request to cloud's `/onep:v1/stack/alias request`.
 *
 * Below is how you would write a value of `5` to the `myAlias` alias.
 * \code{.c}
 * cloud_write("myAlias=5", sizeof("myAlias=5"));
 * \endcode
 *
 * \param[in] writeData Pointer to buffer of data to write to cloud
 * \param[in] length length of data in buffer
 *
 * \return Error code if fails, else 0
 *
 */
int32_t cloud_write(const char * writeData, uint16_t length)
{
    // assume content length won't be greater than 9999.
    char contentLengthStr[5];
    uint8_t len_of_contentLengthStr;
    uint16_t responseLength;
    uint8_t connection_status;
    int32_t results = 0;
    if(!cloud_isCIKValid(cikBuffer))
    {
        // tried to write without a valid CIK
        return -99;
    }
    
    // connect to cloud
    connection_status = cloud_connect();

    // return error message if connect failed.
    if (connection_status != 0)
    {
        return connection_status;
    }


    len_of_contentLengthStr = cloudPal_itoa((int)length, contentLengthStr, 5);

    // send request
    results |= cloudPal_socketWrite(STR_WRITE_URL, sizeof(STR_WRITE_URL)-1);
    results |= cloudPal_socketWrite(STR_HTTP, sizeof(STR_HTTP)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send Host header
    results |= cloudPal_socketWrite(STR_HOST, sizeof(STR_HOST)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send cik header
    results |= cloudPal_socketWrite(STR_CIK_HEADER, sizeof(STR_CIK_HEADER)-1);
    results |= cloudPal_socketWrite(cikBuffer, sizeof(cikBuffer));
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send content type header
    results |= cloudPal_socketWrite(STR_CONTENT, sizeof(STR_CONTENT)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send content length header
    results |= cloudPal_socketWrite(STR_CONTENT_LENGTH, sizeof(STR_CONTENT_LENGTH)-1);
    results |= cloudPal_socketWrite(contentLengthStr, len_of_contentLengthStr);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send body
    results |= cloudPal_socketWrite(writeData, length);

    results |= cloudPal_sendingComplete();
    
    if (results != 0)
    {
        cloud_disconnect();
        return results;
    }

    responseLength = 0;
    // get response
    cloudPal_socketRead(cloudPal_rxBuffer, RX_BUFFER_SIZE, &responseLength);


    cloud_disconnect();

    // 204 "No content"
    if (cloud_checkResponse(cloudPal_rxBuffer, "204"))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}



/*!
 *  \brief  Reads data from cloud
 *
 * Allows the reading of for reading data from the cloud platform.  The alias
 * variable can include one, or multiple alias names.  For example, if you
 * included they must be separated by a '&'.
 *
 * For example, if you want to read from a single alias, you would set the alias
 * parameter to `"myAliasName"`.  If you wanted to read from multiple alias', you
 * would set the alias parameter to `"myAliasName&myOtherAliasName"`.
 *
 * If the read is successful, the value returned in readResponse will be the
 * body of the HTTP response, and will be in this format
 * `"myAliasName=someValue&myOtherAliasName=23"`.
 *
   \code{.c}
   cloud_read("myAlias", readBuffer, lenOfReadBuffer, retLen);
   // After this call, the readBuffer would look something like "myAlias=3". The
   // length variable would be updated with the length of the response string, in this
   // case, 9.
   \endcode
 *
 * \param[in] alias Name/s of data source/s alias to read from
 * \param[out] readResponse buffer to place read response in
 * \param[in] buflen length of buffer
 * \param[out] length Length of data placed into readResponse
 *
 * \return Error code if fail, else 0
 *
 */
int32_t cloud_read(const char * alias, char * readResponse, uint16_t buflen, uint16_t * length)
{
    uint16_t responseLength;
    int i,j;
    uint8_t connection_status;
    int32_t results = 0;
    
    if(!cloud_isCIKValid(cikBuffer))
    {
        // tried to write without a valid CIK
        return -99;
    }
    
    // connect to cloud
    connection_status = cloud_connect();

    // return error message if connect failed.
    if (connection_status != 0)
    {
        return connection_status;
    }

    // send request
    results |= cloudPal_socketWrite(STR_READ_URL, sizeof(STR_READ_URL)-1);
    results |= cloudPal_socketWrite(alias, cloudPal_strlen(alias));
    results |= cloudPal_socketWrite(" ", cloudPal_strlen(" "));
    results |= cloudPal_socketWrite(STR_HTTP, sizeof(STR_HTTP)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send Host header
    results |= cloudPal_socketWrite(STR_HOST, sizeof(STR_HOST)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send cik header
    results |= cloudPal_socketWrite(STR_CIK_HEADER, sizeof(STR_CIK_HEADER)-1);
    results |= cloudPal_socketWrite(cikBuffer, sizeof(cikBuffer));
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send content type header
    results |= cloudPal_socketWrite(STR_CONTENT, sizeof(STR_CONTENT)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send accept header
    results |= cloudPal_socketWrite(STR_ACCEPT, sizeof(STR_ACCEPT)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    results |= cloudPal_sendingComplete();

    if (results != 0)
    {
        cloud_disconnect();
        return results;
    }

    responseLength = 0;
    // get response
    cloudPal_socketRead(cloudPal_rxBuffer, RX_BUFFER_SIZE, &responseLength);


    cloud_disconnect();
    
    // 204 "No content"
    if (cloud_checkResponse(cloudPal_rxBuffer, "200"))
    {
        //find first '\n' char from end of response
        for (i = responseLength - 1; i > 0; i--)
        {

            // if we found the '\n' before we hit the beginning of the buffer
            if (cloudPal_rxBuffer[i] == '\n')
            {
                // copy http body into readResponse buffer
                for (j = i; j < (responseLength - 1); j++)
                {
                    readResponse[j-i] = cloudPal_rxBuffer[j + 1];
                }

                // add null teminator at end of string
                readResponse[j-i] = '\0';
                // exit out
                i = 0;
            }
        }
    }
    else
    {
      return 1;
    }

    return 0;
}



/*!
 *  \brief  Reads data from cloud
 *
 * Reads a single alias from the cloud One Platform.  The alias
 * variable can only include one alias name.
 *
 * For example, if you want to read from the `myAlias` alias, you would set the
 * alias parameter to `"myAliasName"`.
 *
 * If the read is successful, the value returned in readResponse will be the
 * value of your alias.  These will always be strings.  Even if your data source
 * contains a numeric value, you must convert it to an integer before using it.
 *
   \code{.c}
   cloud_readSingle("myAlias", readBuffer, lenOfReadBuffer, retLen);
   // After this call, the readBuffer would look something like this: "3". The
   // length variable would be updated with the length of the response string, in this
   // case, 1.
   \endcode
 *
 * \param[in] alias Name/s of data source/s alias to read from
 * \param[out] readResponse buffer to place read response in
 * \param[in] buflen length of buffer
 * \param[out] length data placed into readResponse
 *
 * \return Error code if fail, else 0
 *
 */
int32_t cloud_readSingle(const char * alias, char * readResponse, uint16_t buflen, uint16_t * length)
{
    uint16_t responseLength;
    int16_t i;
    uint16_t j;
    uint16_t k;
    uint8_t connection_status;
    int32_t results = 0;
    if(!cloud_isCIKValid(cikBuffer))
    {
        // tried to write without a valid CIK
        return -99;
    }
    
    // connect to cloud
    connection_status = cloud_connect();

    // return error message if connect failed.
    if (connection_status != 0)
    {
        return connection_status;
    }

    // send request
    results |= cloudPal_socketWrite(STR_READ_URL, sizeof(STR_READ_URL)-1);
    results |= cloudPal_socketWrite(alias, cloudPal_strlen(alias));
    results |= cloudPal_socketWrite(" ", cloudPal_strlen(" "));
    results |= cloudPal_socketWrite(STR_HTTP, sizeof(STR_HTTP)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send Host header
    results |= cloudPal_socketWrite(STR_HOST, sizeof(STR_HOST)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send cik header
    results |= cloudPal_socketWrite(STR_CIK_HEADER, sizeof(STR_CIK_HEADER)-1);
    results |= cloudPal_socketWrite(cikBuffer, sizeof(cikBuffer));
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send content type header
    results |= cloudPal_socketWrite(STR_CONTENT, sizeof(STR_CONTENT)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send accept header
    results |= cloudPal_socketWrite(STR_ACCEPT, sizeof(STR_ACCEPT)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    results |= cloudPal_sendingComplete();
    
    if (results != 0)
    {
        cloud_disconnect();
        return results;
    }
    
    responseLength = 0;
    // get response
    cloudPal_socketRead(cloudPal_rxBuffer, RX_BUFFER_SIZE, &responseLength);

    cloud_disconnect();
    
    // if we received a 200
    if (cloud_checkResponse(cloudPal_rxBuffer, "200"))
    {
        //find first '\n' char from end of response
        for (i = responseLength; i > 0; i--)
        {
            if (cloudPal_rxBuffer[i] == '\n')
            {
                // '\n' found
                uint8_t charNotMatch = 0;
                for ( j = 1; (j <= i) && i > 0; j++)
                {
                    // If we're at the end of the inputted string?
                    if (alias[j-1] == '\0')
                    {
                        // if all chars of our requested alias match, we found the key
                        if (!charNotMatch)
                        {
                            // move j passed the '='
                            j++;

                            for (k = 0;
                                 (k <= buflen) && ((i + j + k) <= responseLength);
                                 k++)
                            {
                                // copy remaining data into buffer
                                readResponse[k] = cloudPal_rxBuffer[i+j+k];
                                *length = k;
                            }
                            i = 0;
                        }
                        else
                        {
                            // match not found, exit
                            i = 0;
                            *length = 0;
                        }
                    }

                    // confirm letter by letter
                    charNotMatch |= (cloudPal_rxBuffer[i+j] != alias[j-1]);
                }
            }
        }
    }

    return 0;
}

/*!
 * @brief  Retrieves the timestamp from m2.cloud.com/timestamp
 *
 * @param timestamp Timestamp retrieved from cloud
 * 
 * @return int8_t Returns negative error code if failed, else returns 0
 */
int8_t cloud_getTimestamp(int32_t * timestamp)
{
    char * bodyStart;
    uint16_t responseLength = 0;
    int32_t status;
    uint8_t connection_status;
    char temp;
    connection_status = cloud_connect();
    cloudPal_socketWrite(STR_TIMESTAMP_URL, sizeof(STR_TIMESTAMP_URL)-1);
    cloudPal_socketWrite(STR_HTTP, sizeof(STR_HTTP)-1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send Host header
    cloudPal_socketWrite(STR_HOST, sizeof(STR_HOST)-1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);
    cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);
    
    cloudPal_sendingComplete();
    
    cloudPal_socketRead(cloudPal_rxBuffer, RX_BUFFER_SIZE, &responseLength);
    cloud_disconnect();
    
    status = cloud_getBody(cloudPal_rxBuffer, &bodyStart, &responseLength);
    
    if (status < 0 )
    {
        return -1;
    }
    
    temp = bodyStart[responseLength];
    bodyStart[responseLength] = '\0';
    
    *timestamp = cloudPal_atoi(bodyStart);
    bodyStart[responseLength] = temp;
    
    return 0;
    
}


/*!
 * @brief  Makes a request to the cloud RPC API
 *
 *
 * @param requestBody Full json rpc request string
 * @param requestLength Length of json request string
 * 
 * @return int32_t 
 */
int32_t cloud_rawRpcRequest(const char * requestBody, uint16_t requestLength, char * responseBuffer, int32_t responseBufferLength)
{
    char contentLengthStr[5];
    uint16_t responseLength = 0;
    uint8_t connection_status;
    uint8_t len_of_contentLengthStr;
    int32_t results = 0;
    
    if(!cloud_isCIKValid(cikBuffer))
    {
        // tried to write without a valid CIK
        return -99;
    }
    
    // connect to cloud
    connection_status = cloud_connect();
    
    // assume content length will never be over 9999 bytes
    
    len_of_contentLengthStr = cloudPal_itoa((int)requestLength, contentLengthStr, 5);
    // return error message if connect failed.
    if (connection_status != 0)
    {
        return connection_status;
    }

    // send request
    results |= cloudPal_socketWrite(STR_RPC_URL, sizeof(STR_RPC_URL)-1);
    results |= cloudPal_socketWrite(STR_HTTP, sizeof(STR_HTTP)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send Host header
    results |= cloudPal_socketWrite(STR_HOST, sizeof(STR_HOST)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send content type header
    results |= cloudPal_socketWrite(STR_CONTENT_JSON, sizeof(STR_CONTENT_JSON)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send accept header
    results |= cloudPal_socketWrite(STR_ACCEPT_JSON, sizeof(STR_ACCEPT_JSON)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send content length header
    results |= cloudPal_socketWrite(STR_CONTENT_LENGTH, sizeof(STR_CONTENT_LENGTH)-1);
    results |= cloudPal_socketWrite(contentLengthStr, len_of_contentLengthStr);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);
    results |= cloudPal_socketWrite(STR_CRLF, sizeof(STR_CRLF)-1);

    // send body
    results |= cloudPal_socketWrite(requestBody, requestLength);

    if (results != 0)
    {
        cloud_disconnect();
        return results;
    }


    cloudPal_sendingComplete();
    
    // get response
    cloudPal_socketRead(responseBuffer, responseBufferLength, &responseLength);

    cloud_disconnect();
    return responseLength;

}



/*!
 * \brief Connects to cloud
 *
 * This would typically be a call to open a socket
 *
 * \return Returns a 0 if socket was successfully opened, else returns the
 *          error code
 * \sa
 * \note
 * \warning
 */
uint8_t cloud_connect(void)
{
    // open socket to cloud

    return cloudPal_tcpSocketOpen();
}

/*!
 * \brief Connects to cloud
 *
 * This would typically be a call to close a socket
 *
 * \return Returns a 0 if socket was successfully close, else returns the
 *          error code
 * \sa
 * \note
 * \warning
 */
uint8_t cloud_disconnect(void)
{
    // Close socket to cloud
    return cloudPal_tcpSocketClose();
}


/*!
 * \brief  determines if response matches code
 *
 * \param[in] code Pointer to expected HTTP response code
 * \param[in] response an HTTP response string
 *
 * \return 1 if match, 0 if no match
 *
 */
uint8_t cloud_checkResponse(char * response, const char * code)
{
    // get start second ' ' separate column
    // assumes will always be before the 15th char
    // assumes first char isn't a ' '
    uint8_t spaceFound = 0;
    int16_t i;
    for (i = 1; (i < 15) && (spaceFound == 0); i++)
    {
        if (response[i] == ' ')
        {
            spaceFound = i + 1;
        }
    }
    if (spaceFound > 0)
    {
        // If we found a ' ', try to match the code
        if (code[0] == response[spaceFound] && code[1] == response[spaceFound + 1] && code[2] == response[spaceFound + 2])
        {
            return 1;
        }
    }

    return 0;
}
void CloudsiteTask(void *pvParameters)
{
    long   ret = -1;
    int tmp=0;
    unsigned long long retry_delay = 100;
    unsigned long long counter = 0;
    uint16_t temperature = 0;
    uint16_t humidity = 0;
    char buf[1024];
    uint16_t retLen =0;
    Date_Clk_Init();
    PinTypeGPIO(PIN_60, PIN_MODE_0, false);
    CLOUD_STATE state = CLOUD_STATE_NOT_COMPLETE;
    //Connect to Network
    ret = ConnectToNetwork();
    if(ret < 0)
    {
        ERR_PRINT(ret);
        LOOP_FOREVER();
    }
    // Wait here if we're in AP Mode
    while(g_uiDeviceModeConfig != ROLE_STA){
        osi_Sleep(1000);
    }
    //
    // Initialize I2C Interface
    //   
    I2C_IF_Open(I2C_MASTER_MODE_STD);
    Start_BH1750();
    osi_Sleep(200);
    Read_BH1750();
    cloudPal_GetHostIP();
    //Handle Async Events
    while(1)
    {
    	//TODO: Wrap all Cloud calls in a check to see if we're connected.
        while(state != CLOUD_STATE_INIT_COMPLETE){
    		UART_PRINT("[CLOUD] Cloud Init\r\n");
    		state = cloud_init("MyCloud","CC3200lp_v1");
    		if(state == CLOUD_STATE_INIT_COMPLETE){
    			UART_PRINT("[CLOUD] Activated, Ready to Go\r\n");
    		}else if(state == CLOUD_STATE_INIT_ERROR){
    			UART_PRINT("[CLOUD] Error Activating on Cloud (Add to Account or Re-enable for Activation)\r\n");
    			retry_delay *= 2;
    	        osi_Sleep(retry_delay);
    		}else if(state == CLOUD_STATE_CONNECTION_ERROR){
    			UART_PRINT("[CLOUD] Error Connecting to Cloud, Check Your Connection\r\n");
    			//retry_delay *= 2;
    	        osi_Sleep(retry_delay);
    		}else if(state == CLOUD_STATE_NO_RESPONSE){
    			UART_PRINT("[CLOUD] We connected, but timed-out waiting for a response. Try again.\r\n");
    			retry_delay *= 2;
    	        osi_Sleep(retry_delay);
    		}else if(state == CLOUD_STATE_DEVICE_NOT_ENABLED){
    			UART_PRINT("[CLOUD] Couldn't Activate on Cloud (Add to Account or Re-enable for Activation)\r\n");
    			retry_delay *= 2;
    	        osi_Sleep(retry_delay);
    		}else if(state == CLOUD_STATE_R_W_ERROR){
    			UART_PRINT("[CLOUD] Read/Write Error\r\n");
    			retry_delay *= 2;
    	        osi_Sleep(retry_delay);
    		}else{
    			UART_PRINT("[CLOUD] Unknown State in Init");
    			return;
    		}
        }
        tmp = RH();
        temperature=tmp&0x00FF;
//        UART_PRINT("%d%s\n",temperature,TEMPERATURE_TYPE);
        humidity = tmp>>8;
//        UART_PRINT("%d%s\n",humidity,HUMIDITY_TYPE);
        UART_PRINT("[CLOUD] Writing Values...");
        sprintf(buf,"&MsgId=%s&FromUserName=%s&MsgType=%s&Content=%d",uuidBuffer,TEMPERATURE_MSG,TEMPERATURE_TYPE,temperature);
        ret = cloud_write(buf,strlen(buf));
        sprintf(buf,"&MsgId=%s&FromUserName=%s&MsgType=%s&Content=%d",uuidBuffer,HUMIDITY_MSG,HUMIDITY_TYPE,humidity);
        ret = cloud_write(buf,strlen(buf));
        if(ret == 0){
        	UART_PRINT("OK\r\n");
        	retry_delay = 1000;
        }else{
        	UART_PRINT("ERROR\r\n");
        	retry_delay *= 2;
        	state = CLOUD_STATE_NOT_COMPLETE;
        	ERR_PRINT(ret);
        }
        cloud_read("Temperature",buf,1024,&retLen);
//        UART_PRINT("%s\n",buf);
        
        //LED Actions for OOB Code
        if(g_ucLEDStatus == LED_ON)
        {
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        }
        if(g_ucLEDStatus == LED_OFF)
        {
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }
        if(g_ucLEDStatus==LED_BLINK)
        {
        	GPIO_IF_LedToggle(MCU_RED_LED_GPIO);
        }

        // Limit Retry Delay to 10 Seconds During Development
        if(retry_delay > 10000)
        	retry_delay = 10000;

        osi_Sleep(retry_delay);

        counter++;
    }
}
