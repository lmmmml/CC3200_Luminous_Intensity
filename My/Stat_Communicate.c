#include "Stat_Communicate.h"
NetMessage ExternalMsg;
SensorStatus InternalMsg;
SensorInfo const Sensor[SENSOR_NUM]={{SL_IPV4_VAL(10,10,10,160),{5001,5002}},           //传感器表
                                     {SL_IPV4_VAL(10,10,10,161),{5001,5002}},
                                     {SL_IPV4_VAL(10,10,10,162),{5001,5002}},
                                     {SL_IPV4_VAL(10,10,10,163),{5001,5002}}};
SensorDataStore  SensorData;                                                      //传感器数据存储
unsigned char const SensorUnmapTbl[]={0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,                 
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,           //传感器就绪码表
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
                                     4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0};
unsigned char const reftbl[7]={0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00};             //参考码


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern unsigned long  g_ulStatus ;//SimpleLink Status
unsigned long  g_ulPingPacketsRecv = 0; //Number of Ping Packets received 
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID


//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//****************************************************************************
//
//!    \brief 该函数对内部消息进行初始化
//!
//!    \param[in]  None
//!
//!    \return     None
//
//****************************************************************************
void InterMessageInit()//
{
  for(int i=0;i<PORT_NUM;i++)
  {
    InternalMsg.Port[i]=MY_PORTBASE+i;
  }
  InternalMsg.PortStatus=0;
  InternalMsg.Sensor_Status=0;
  InternalMsg.RecCnt=0;
}
//****************************************************************************
//
//!    \brief   该函数用于从传感器在与主传感器通信时的消息更新
//!
//!    \param[in]       msg  ：网络消息指针
//!
//!    \return          None
//
//****************************************************************************
void MessageRrefresh(NetMessage* msg)
{
  InternalMsg.RecCnt++;
  msg->RecCnt=InternalMsg.RecCnt;
  UART_PRINT("Receive Count is :%d \n\r",msg->RecCnt);
  msg->Sensor_Status[MY_NUM]=InternalMsg.Sensor_Status;
  msg->PortStatus[MY_NUM]=InternalMsg.PortStatus;
  msg->SendIP=MY_IP;
  msg->SensorNum=MY_NUM;
#if  (LIBRARY_COUNT == MY_NUM)   
  msg->NumLirary=InternalMsg.NumLirary;
#endif
}
//****************************************************************************
//
//!    \brief   该函数用于主传感器作为TCP客户端与从传感器通信
//!
//!    \param[in]   - uPort   ：从传感器的端口号
//！                - SensorIP :从传感器的IP
//！                - SensorNum ：从传感器的编号
//!
//!    \return     通信是否成功
//
//****************************************************************************
int HostClient(int uPort,unsigned long int SensorIP,_u8 SensorNum)//主TCP客户端函数
{
  int   iSockID;
  int   iStatus;
  char  g_cBsdBuf[128];
  
  memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
  SlSockAddrIn_t  sAddr;
  sAddr.sin_family = SL_AF_INET;
  sAddr.sin_port = sl_Htons((unsigned short)uPort);
  sAddr.sin_addr.s_addr = sl_Htonl(SensorIP);
  
  iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
  if( iSockID < 0 )
  {
    // error
    ASSERT_ON_ERROR(iSockID);
  }
   struct SlTimeval_t timeVal;
    timeVal.tv_sec =  5;             // Seconds
    timeVal.tv_usec = 0;             // Microseconds. 10000 microseconds resolution
  sl_SetSockOpt(iSockID,SL_SOL_SOCKET,SL_SO_RCVTIMEO, (_u8 *)&timeVal, sizeof(timeVal));    // Enable receive timeout 
  iStatus = sl_Connect(iSockID, ( SlSockAddr_t *)&sAddr, sizeof(SlSockAddrIn_t));
  if( iStatus < 0 )
  {
    // error
    sl_Close(iSockID);
    return -1;
  }
  iStatus = sl_Send(iSockID, "connected", 128, 0 );
  if( iStatus < 0 )
  {
    // error
    sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus);
  }
  iStatus = sl_Recv(iSockID, g_cBsdBuf, sizeof(g_cBsdBuf), 0);//从其它从传感器获取信息
  if( iStatus < 0 )
  {
    // error
    sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus);
  }
  DataProcess((NetMessage*)g_cBsdBuf,SensorNum);//对获取的信息进行处理
  iStatus = sl_Send(iSockID,g_cBsdBuf, sizeof(g_cBsdBuf), 0 );//将处理后的信息发回给从传感器
  if( iStatus < 0 )
  {
    // error
    sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus);
  }
  iStatus = sl_Close(iSockID);
  ASSERT_ON_ERROR(iStatus);
  GPIO_IF_LedOff(MCU_ALL_LED_IND);
  return SUCCESS;
}
//****************************************************************************
//
//!    \brief   该函数用于主传感器在接收到从传感器的消息后对获得的消息进行处理
//!
//!    \param[in]       - msg  ：网络消息指针
//！                    - SensorNum ：传感器的编号
//!
//!    \return          None
//
//****************************************************************************
void DataProcess(NetMessage* msg,_u8 SensorNum)//数据处理函数
{
  int iCount=0;
  unsigned char Rdytemp=0x01|(reftbl[SENSOR_NUM-1]);  //与传感器的个数有关
  for(int i=0;i<SENSOR_NUM;i++)
  {
    Rdytemp|=((SensorData.err[i]>>7)<<(i+1));//标记传感器是否异常
  }
  SensorData.Sensor_Status[msg->SensorNum]=msg->Sensor_Status[msg->SensorNum];//从当前发出消息的从传感器中获取传感器的状态信息
  SensorData.PortStatus[msg->SensorNum]=msg->PortStatus[msg->SensorNum];//从当前发出的从传感器中获取传感器的TCP端口信息
  for(int i=0;i<SENSOR_NUM;i++)
  {
    msg->Sensor_Status[i]=SensorData.Sensor_Status[i];//将统计好的所有传感器的状态都赋发出的消息
    SensorData.SensorRdyTbl[i+1]=~(0xf9|(SensorData.PortStatus[i]<<1));
    switch( SensorData.PortStatus[i]&0x03 )
    {
      case 0x01:
      case 0x02:iCount++;break;
      case 0x03:iCount+=2;Rdytemp|=(0x01<<(i+1));break;
      default :break;
    }
  }
  SensorData.SensorRdyGrp=~Rdytemp;//计算出端口就绪的从传感器
  Rdytemp=SensorUnmapTbl[SensorData.SensorRdyGrp];
  SensorData.SensorHighRdy=(unsigned char)((Rdytemp<<1)+SensorUnmapTbl[SensorData.SensorRdyTbl[Rdytemp]]);
  if(SensorData.SensorHighRdy!=0)
  {
    SensorData.SensorHighRdy-=2;
  }
  UART_PRINT("Free Port is :%d \n\r",SensorData.SensorHighRdy);
  msg->ConnectedMobileNUM=iCount;
  msg->FreePortNUM=SENSOR_NUM*PORT_NUM-iCount;
  SensorData.ConnectedMobileNUM=msg->ConnectedMobileNUM;
  SensorData.FreePortNUM=msg->FreePortNUM;
  UART_PRINT("The Number of Free Port is :%d \n\r",msg->FreePortNUM);
  UART_PRINT("The Number of Connected Mobile is :%d \n\r",msg->ConnectedMobileNUM);
  if(msg->RecCnt>SensorData.RecCnt)
  {
    SensorData.RecCnt=msg->RecCnt;
  }
  else
  {
    msg->RecCnt=SensorData.RecCnt;
  }
  UART_PRINT("Receive Count is :%d \n\r",SensorData.RecCnt);
  if(SensorNum==LIBRARY_COUNT)
  {
    SensorData.NumLirary=msg->NumLirary;
  }
  else
  {
    msg->NumLirary=SensorData.NumLirary;
  }
}
//********************************************************************************
//
//!    \brief   该函数用于从传感器在与主传感器通信时接收到消息时对自身的消息更新
//!
//!    \param[in]       msg  ：网络消息指针
//!
//!    \return          None
//
//*********************************************************************************
void AcquireDataRefresh(NetMessage* msg)
{
  for(int i=0;i<SENSOR_NUM;i++)
  {
    SensorData.Sensor_Status[i]=msg->Sensor_Status[i];
    SensorData.PortStatus[i]=msg->PortStatus[i];
    SensorData.err[i]=msg->err[i];
  }
  SensorData.SensorHighRdy=msg->SensorHighRdy;
  SensorData.ConnectedMobileNUM=msg->ConnectedMobileNUM;
  SensorData.FreePortNUM=msg->FreePortNUM;
  SensorData.NumLirary=msg->NumLirary;
  UART_PRINT("The Number of Free Port is :%d \n\r",msg->FreePortNUM);
  UART_PRINT("The Number of Connected Mobile is :%d \n\r",msg->ConnectedMobileNUM);
}
//****************************************************************************
//
//!    \brief   该函数用于从传感器作为TCP服务端与手持设备通信
//!
//!    \param[in]       uPort   ：手持设备端口号
//!
//!    \return     通信是否成功
//
//****************************************************************************
int SensorServer(int uPort)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iAddrSize;
    int             iSockID;
    int             iStatus;
    int             iNewSockID;
    long            lNonBlocking = 1;
    char            g_cBsdBuf[128];
    
    
    memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
    
    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)uPort);
    sLocalAddr.sin_addr.s_addr = 0;
    
    
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( iSockID < 0 )
    {
        ASSERT_ON_ERROR(iSockID);
    }
    struct SlTimeval_t timeVal;
    timeVal.tv_sec =  5;             // Seconds
    timeVal.tv_usec = 0;             // Microseconds. 10000 microseconds resolution
    sl_SetSockOpt(iSockID,SL_SOL_SOCKET,SL_SO_RCVTIMEO, (_u8 *)&timeVal, sizeof(timeVal));    // Enable receive timeout 
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, sizeof(SlSockAddrIn_t));
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }

    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, 0);
    if( iStatus < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // setting socket option to make the socket as non blocking
    iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &lNonBlocking, sizeof(lNonBlocking));
    if( iStatus < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    iNewSockID = SL_EAGAIN;
    // waiting for an incoming TCP connection
    while( iNewSockID < 0 )
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
        iNewSockID = sl_Accept(iSockID, ( struct SlSockAddr_t *)&sAddr, (SlSocklen_t*)&iAddrSize);
        if( iNewSockID == SL_EAGAIN )
        {
           osi_Sleep(1);
        }
        else if( iNewSockID < 0 )
        {
            // error
            sl_Close(iNewSockID);
            sl_Close(iSockID);
            return -1;
        }
    }
    iStatus = sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus); 
//    sl_SetSockOpt(iNewSockID, SL_SOL_SOCKET, SL_SO_SECMETHOD, (_u8 *)&method, sizeof(method));
    UART_PRINT("client IP is : 0x%x\n\r",sAddr.sin_addr.s_addr);
    iStatus = sl_Recv(iNewSockID, g_cBsdBuf,sizeof(g_cBsdBuf), 0);
    if( iStatus <= 0 )
    {
      // error
      sl_Close(iNewSockID);
//      sl_Close(iSockID);
      ASSERT_ON_ERROR(iStatus);
    }
    InternalMsg.PortStatus|=(1<<(uPort-MY_PORTBASE));
    while(1)
    { 
      memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
      for(int i=0;i<SENSOR_NUM;i++)
        g_cBsdBuf[i]='0'+SensorData.Sensor_Status[i]&0xFE;
      sprintf(&g_cBsdBuf[SENSOR_NUM],"%d",SensorData.NumLirary);
      iStatus = sl_Send(iNewSockID,g_cBsdBuf, sizeof(g_cBsdBuf), 0);
      if( iStatus <= 0 )
      {
        // error
        sl_Close(iNewSockID);
        //sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
      }
      GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
    
      osi_Sleep(2000);
      GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
    } 
}
//****************************************************************************
//
//!    \brief   该函数用于从传感器作为TCP服务端与主传感器通信
//!
//!    \param[in]       uPort   ：主传感器端口号
//!
//!    \return     通信是否成功
//
//****************************************************************************
int SensorToHost(int uPort)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iAddrSize;
    int             iSockID;
    int             iStatus;
    int             iNewSockID;
    long            lNonBlocking = 1;
    char            g_cBsdBuf[128];
    
    memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
    

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)uPort);
    sLocalAddr.sin_addr.s_addr = 0;
    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( iSockID < 0 )
    {
        ASSERT_ON_ERROR(iSockID);
    }
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, sizeof(SlSockAddrIn_t));
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, 0);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // setting socket option to make the socket as non blocking
    iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &lNonBlocking, sizeof(lNonBlocking));
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    iNewSockID = SL_EAGAIN;
    // waiting for an incoming TCP connection
    while( iNewSockID < 0)
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
        iNewSockID = sl_Accept(iSockID, ( struct SlSockAddr_t *)&sAddr,(SlSocklen_t*)&iAddrSize);
        
        if( iNewSockID == SL_EAGAIN )
        {
           osi_Sleep(50);
        }
        else if( iNewSockID < 0 )
        {
            // error
            sl_Close(iNewSockID);
            sl_Close(iSockID);
            return -1;
        }
    }
    iStatus = sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus);                            
    UART_PRINT("client IP is : 0x%x\n\r",sAddr.sin_addr.s_addr);
    iStatus = sl_Recv(iNewSockID, g_cBsdBuf, sizeof(g_cBsdBuf), 0);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iNewSockID);
//        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
    MessageRrefresh((NetMessage*)g_cBsdBuf);
    iStatus = sl_Send(iNewSockID, g_cBsdBuf, sizeof(g_cBsdBuf), 0);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iNewSockID);
//        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
    iStatus = sl_Recv(iNewSockID, g_cBsdBuf, sizeof(g_cBsdBuf), 0);
    AcquireDataRefresh((NetMessage*)g_cBsdBuf);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iNewSockID);
//        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // close the connected socket after receiving from connected TCP client
    iStatus = sl_Close(iNewSockID);    
    ASSERT_ON_ERROR(iStatus);
    GPIO_IF_LedOn(MCU_ALL_LED_IND);
    osi_Sleep(300);
    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    return SUCCESS;
}
//****************************************************************************
//
//!    \brief   该函数用于主传感器作为TCP服务端与手持设备通信
//!
//!    \param[in]       uPort   ：手持设备端口号
//!
//!    \return     通信是否成功
//
//****************************************************************************
int HostServer(int uPort)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iAddrSize;
    int             iSockID;
    int             iStatus;
    int             iNewSockID;
    long            lNonBlocking = 1;
    char            g_cBsdBuf[128];
    
    memset(g_cBsdBuf,0,sizeof(g_cBsdBuf));
    

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)uPort);
    sLocalAddr.sin_addr.s_addr = 0;
    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( iSockID < 0 )
    {
        ASSERT_ON_ERROR(iSockID);
    }
    // binding the TCP socket to the TCP server address
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, sizeof(SlSockAddrIn_t));
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, 0);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // setting socket option to make the socket as non blocking
    iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &lNonBlocking, sizeof(lNonBlocking));
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    iNewSockID = SL_EAGAIN;
    // waiting for an incoming TCP connection
    while( iNewSockID < 0)
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
        iNewSockID = sl_Accept(iSockID, ( struct SlSockAddr_t *)&sAddr,(SlSocklen_t*)&iAddrSize);
        
        if( iNewSockID == SL_EAGAIN )
        {
           osi_Sleep(50);
        }
        else if( iNewSockID < 0 )
        {
            // error
            sl_Close(iNewSockID);
            sl_Close(iSockID);
            return -1;
        }
    }
    iStatus = sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus);
    UART_PRINT("client IP is : 0x%x\n\r",sAddr.sin_addr.s_addr);
    sprintf(g_cBsdBuf,"%d",SensorData.SensorHighRdy);
    iStatus = sl_Send(iNewSockID, g_cBsdBuf,sizeof(g_cBsdBuf), 0);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iNewSockID);
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    iStatus = sl_Recv(iNewSockID, g_cBsdBuf, sizeof(g_cBsdBuf), 0);
    if( iStatus < 0 )
    {
        /* error */
        sl_Close(iNewSockID);
        sl_Close(iSockID);
        ASSERT_ON_ERROR(iStatus);
    }
    // close the connected socket after receiving from connected TCP client
    iStatus = sl_Close(iNewSockID);    
    ASSERT_ON_ERROR(iStatus);
    GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
    osi_Sleep(200);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
    return SUCCESS;
}

//****************************************************************************
//
//!    \brief   该函数为从传感器与主传感器通信的进程
//!
//!    \param[in]       pvParameters
//!
//!    \return     None
//
//****************************************************************************
void WlanStationMode_Stat( void *pvParameters )
{
    WifiConnectInit(MY_IP);
    while(1)
    {
      SensorToHost(5010);
    }
    
}
//****************************************************************************
//
//!    \brief   该函数为从传感器与手持设备通信的子进程1
//!
//!    \param[in]       pvParameters
//!
//!    \return     None
//
//****************************************************************************
void WlanStationMode_Server_1( void *pvParameters )
{
    
    osi_Sleep(5000);
    while(1)
    {
      int iStatus=SensorServer(MY_PORTBASE);
      if(iStatus<0)
      {
        InternalMsg.PortStatus&=~(1<<(0));
      }
    }
}
//****************************************************************************
//
//!    \brief   该函数为从传感器与手持设备通信的子进程2
//!
//!    \param[in]       pvParameters
//!
//!    \return     None
//
//****************************************************************************
void WlanStationMode_Server_2( void *pvParameters )
{
  osi_Sleep(5000);
  while(1)
  {
    int iStatus=SensorServer(MY_PORTBASE+1);
    if(iStatus<0)
    {
      InternalMsg.PortStatus&=~(1<<(1));
    }
  }
}
#define MAX(x)  (((x)>40)?(40):(x))
//****************************************************************************
//
//!    \brief   该函数为主传感器与多个从传感器通信的进程
//!
//!    \param[in]       pvParameters
//!
//!    \return     None
//
//****************************************************************************
void WlanStationMode_Host_1( void *pvParameters )
{
  _u8 tmp=0;
  static _u8 CNT[SENSOR_NUM];
  int iStatus;
  WifiConnectInit(HOST_IP);
  while(1)
  {
    for(int i=0;i<SENSOR_NUM;i++)
    {
      
      if(SensorData.err[i]<2)
      {
        
        iStatus=HostClient(5010,Sensor[i].IP,i);
      }
      else
      {
        if(CNT[i]>MAX(20+SensorData.err[i]&(~0x80)))
        {
          SensorData.err[i]|=0x80;
          iStatus=HostClient(5010,Sensor[i].IP,i);
          CNT[i]=0;
        }
        else
        {
          CNT[i]++;
          continue;
        }
      }
      if(iStatus==SUCCESS)
      {
        SensorData.err[i]=0;
        tmp=0;
        for(int j=0;j<SENSOR_NUM;j++)
        {
          if(SensorData.err[j]&0x80)
            tmp++;
        }
        osi_Sleep(150+250*tmp);
        GPIO_IF_LedOn(MCU_ALL_LED_IND);
      }
      else
      {
        SensorData.err[i]++;
//        osi_Sleep(200);
        GPIO_IF_LedOff(MCU_ALL_LED_IND);
      }
      CNT[i]++;
    }
  } 
}
//****************************************************************************
//
//!    \brief   该函数为主传感器与手持设备通信的进程
//!
//!    \param[in]       pvParameters
//!
//!    \return     None
//
//****************************************************************************
void WlanStationMode_Host_2( void *pvParameters )
{
  osi_Sleep(5000);
  while(1)
  {
    HostServer(HOSTPORT);
  }
}
//****************************************************************************
//
//!    \brief   主传感器显示座位情况的进程
//!
//!    \param[in]       pvParameters
//!
//!    \return     None
//
//****************************************************************************
void Seat_Error( void *pvParameters )
{
  extern char x_flag,y_flag;
  char Flag[3]={'*','&','!'};
  osi_Sleep(400);
  printf("\t\t\tPlease wait for \nsystem initialization\n");
  osi_Sleep(1000);
  printf("\t\tConnecting to WIFI");
  osi_Sleep(1000);
  clear_screen();
  x_flag=0,y_flag=0;
  printf("\n\t\t\t\t\t\t\t\tHello\n");
  osi_Sleep(2000);
  printf("\n\n \t\t\t\tAdministrator");
  osi_Sleep(3000);
  clear_screen();
  while(1){
    x_flag=0,y_flag=0;    
    clear_screen();
    printf("\t\t\t\t\t\t\tLibrary\n\n");
    printf("\t\t\t\t\tSeat  Status\n\n");
    printf("\t\t\t\t1\t\t\t\t\t2\t\t\t\t\t3\n\n"); 
    printf("\t\t\t\t%c\t\t\t\t\t%c\t\t\t\t\t%c",Flag[(((SensorData.Sensor_Status[1]&0x01)+((SensorData.Sensor_Status[1]&0x02)>>1))&0x03)],
                                                Flag[(((SensorData.Sensor_Status[2]&0x01)+((SensorData.Sensor_Status[2]&0x02)>>1))&0x03)],
                                                Flag[(((SensorData.Sensor_Status[3]&0x01)+((SensorData.Sensor_Status[3]&0x02)>>1))&0x03)]);
    osi_Sleep(1000);
  }

}
//****************************************************************************
//
//!    \brief This function makes sensor connect to Wifi
//!
//!    \param[in]  None
//!
//!    \return     None
//
//****************************************************************************
void WifiConnectInit(unsigned long int ip)
{
   long lRetVal = -1;
    InitializeAppVariables();

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
        {
            UART_PRINT("Failed to configure the device in its default state\n\r");
        }

        LOOP_FOREVER();
    }

    UART_PRINT("Device is configured in default state \n\r");
    
    //
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //
    lRetVal = sl_Start(0, 0, 0);
    if (lRetVal < 0 || ROLE_STA != lRetVal)
    {
        UART_PRINT("Failed to start the device \n\r");
        LOOP_FOREVER();
    }
    UART_PRINT("Device started as STATION \n\r");
    SlNetCfgIpV4Args_t ipV4;
    ipV4.ipV4 = (_u32)ip; // _u32 IP address 
    ipV4.ipV4Mask = (_u32)SUBNETMASK; // _u32 Subnet mask for this STA/P2P
    ipV4.ipV4Gateway = (_u32)GATEWAY; // _u32 Default gateway address
    ipV4.ipV4DnsServer = (_u32)DNSSERVER; // _u32 DNS server address
    sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8 *)&ipV4); 
    sl_Stop(0);
    sl_Start(NULL,NULL,NULL);
    //
    //Connecting to WLAN AP
    //
    lRetVal = WlanConnect();
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to establish connection w/ an AP \n\r");
        LOOP_FOREVER();
    }
    UART_PRINT("Connection established w/ AP and IP is aquired \n\r");
}
//****************************************************************************
//
//!    \brief This function initializes the application variables
//!
//!    \param[in]  None
//!
//!    \return     None
//
//****************************************************************************
void InitializeAppVariables()
{
    g_ulStatus = 0;
    g_ulPingPacketsRecv = 0;
    g_ulGatewayIP = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}
//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;
    
    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);
   
    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0,1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }
     //Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);
    
    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 15;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();
    
    return lRetVal; // Success
}
//****************************************************************************
//
//! \brief Connecting to a WLAN Accesspoint
//!
//!  This function connects to the required AP (SSID_NAME) with Security
//!  parameters specified in te form of macros at the top of this file
//!
//! \param  None
//!
//! \return  None
//!
//! \warning    If the WLAN connection fails or we don't aquire an IP 
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
long WlanConnect()
{
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;
    
    lRetVal = sl_WlanConnect((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))) 
    { 
        // Toggle LEDs to Indicate Connection Progress
        GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
        GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
    }

    return SUCCESS;
   
}
#if  (LIBRARY_COUNT == MY_NUM)
void Library_Count( void *pvParameters )
{
  while(1)
  {
    osi_Sleep(500);
    InternalMsg.NumLirary++;
  }
}
#endif