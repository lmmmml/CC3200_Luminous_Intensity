#ifndef _STAT_COMMUNICATE_H_
#define _STAT_COMMUNICATE_H_

#include "includes.h"
#define LIBRARY_COUNT   0
//
// Values for below macros shall be modified as per access-point(AP) properties
// SimpleLink device will connect to following AP when application is executed
//
#define SSID_NAME           "WR842N"    /* AP SSID */
#define SECURITY_TYPE       SL_SEC_TYPE_WPA/* Security type (OPEN or WEP or WPA*/
#define SECURITY_KEY        "denglegejian"              /* Password of the secured AP */
#define SSID_LEN_MAX        (32)
#define BSSID_LEN_MAX       (6)

#define SUBNETMASK      SL_IPV4_VAL(255,255,255,0)      //������
#define GATEWAY         SL_IPV4_VAL(10,10,10,254)        //����
#define DNSSERVER       SL_IPV4_VAL(192,168,111,8)      //Dns�����


#define SENSOR_NUM       (4)                     //������������
#define PORT_NUM         (2)                     //ÿ���������Ķ˿�����
#define MY_NUM  1                               //���������
#define MY_IP   (Sensor[MY_NUM].IP)
#define HOST_IP SL_IPV4_VAL(10,10,10,158)      //����IP
#define MY_PORTBASE     (5001)                  //�˿ڻ���ַ
#define HOSTPORT        (5010)
typedef struct
{
  unsigned long int RecCnt;                     //���Ӽ���
  unsigned long int ConnectedMobileNUM;         //���ӵ��ֻ���
  unsigned long int FreePortNUM;                //���еĶ˿���
  unsigned char PortStatus[SENSOR_NUM];          //����CC3200�Ķ˿�״̬
  unsigned long int Sensor_Status[SENSOR_NUM];    //���е�״̬
  unsigned long int SendIP;                     //�����ߵ�IP
  unsigned char  SensorNum;                      //
  short int  err[SENSOR_NUM];                    //ÿ���������Ĵ�������
  unsigned long int NumLirary;                  //ͼ��ݵ�����
  unsigned char SensorRdyTbl[SENSOR_NUM+1];       //�������ľ�����
  unsigned char SensorRdyGrp;                    //�������ľ�����
  unsigned char SensorHighRdy;                   //���е���߾�����
}NetMessage,SensorDataStore;                     //CC3200��Ϣ���ṹ��
typedef struct
{
  unsigned long int RecCnt;
  unsigned long int Port[PORT_NUM];
  unsigned char PortStatus;
  unsigned long int Sensor_Status;
  unsigned long int NumLirary;                   //ͼ��ݵ�����
}SensorStatus;                                   //������״̬�ṹ��
typedef struct
{
  unsigned long int IP;
  unsigned long int Port[PORT_NUM];
}SensorInfo;                                     //��������Ϣ�ṹ��
// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,       
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;
void InterMessageInit();
void MessageRrefresh(NetMessage* msg);
void AcquireDataRefresh(NetMessage* msg);
int SensorServer(int uPort);
int HostServer(int uPort);
int HostClient(int uPort,unsigned long int SensorIP,_u8 SensorNum);
void DataProcess(NetMessage* msg,_u8 SensorNum);
void WlanStationMode_Server_1( void *pvParameters );
void WlanStationMode_Server_2( void *pvParameters );
void WlanStationMode_Host_1( void *pvParameters );
void WlanStationMode_Host_2( void *pvParameters );
void Seat_Error( void *pvParameters );
#ifdef  LIBRARY_COUNT 
void Library_Count( void *pvParameters );
#endif
void InitializeAppVariables();
long ConfigureSimpleLinkToDefaultState();
void WlanStationMode_Stat( void *pvParameters );
void WifiConnectInit(unsigned long int ip);
long WlanConnect();

#endif