/*
*********************************************************************************************************
* HART-IP Application Layer.h
*********************************************************************************************************
*/
#ifndef _HIP_APP_H
#define _HIP_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hip_svr.h"

/* Definitions */
#define DEV_REV_NUM  1
#define SW_REV_NUM   1
#define HW_REV_NUM   1

// Device simulation collection
typedef struct
{
  uint8_t     Mode;                         // Simulation mode: 0->off; 1->on
  uint8_t     Unit;                         // Simulation unit code
  float       Value;                        // Simulation value
  uint8_t     Status;                       // Simulation status
} _DEV_SIMU_STRUCT;

// Burst message collection
#define BURST_MSG_NUM  3
typedef struct
{
  uint16_t    CmdNum;                       // Burst Command Number
  uint8_t     CtrlCode;                     // Burst Control Code
  uint8_t     TrigMode;                     // Burst Trigger Mode
  uint8_t     DVCode[8];                    // Device Variable Code assigned to Slot 0~7
  uint32_t    UpdatePeriod_ms;              // Update Period
  uint32_t    MaxUpPeriod_ms;               // Maximum Update Period
  float       TrigLevel;                    // Burst Trigger Level
} _BURST_MESSAGE_STRUCT;

// NVM data collection
typedef struct
{
  uint16_t    CfgChgCnt;                    // Configuration Change Counter
  uint8_t     CmdCfgChgFlg;                 // Command execute config change flag
  uint8_t     PollingAddr;                  // Polling Address
  uint8_t     LongTag[32];                  // Long tag (Latin-1)
  uint8_t     UnitTag[32];                  // Process Unit tag (Latin-1)
  uint8_t     Tag[6];                       // Tag (8 characters)
  uint8_t     Descriptor[12];               // Descriptor (16 characters)
  uint8_t     Message[24];                  // Message (32 characters)
  uint8_t     Date[3];                      // Date
  uint8_t     FinalAsmbNum[3];              // Final assembly number

  _BURST_MESSAGE_STRUCT BurstMsg[BURST_MSG_NUM]; // Burst Messages

  uint16_t    UdpPort;                      // UDP port Number
  uint16_t    TcpPort;                      // TCP port Number

  _CLIENT_REC_STRUCT CliRec[CLIENT_SLOTS_MAX]; // Client Records
  _SYSLOG_SVR_STRUCT SyslogSrv;             // Syslog Server
	
	uint8_t     DscpCode;                     // DSCP Value Code
	uint8_t     enDHCP;                       // DHCP enabled
} _NVM_DATA_STRUCT;


#define ByteArrayLsbToUInt16(arr2, uint16) { (uint16) = (uint16_t)((arr2)[0] | ((arr2)[1] << 8)); }
#define ByteArrayMsbToUInt16(arr2, uint16) { (uint16) = (uint16_t)(((arr2)[0] << 8) | (arr2)[1]); }
#define UInt16ToByteArrayLsb(uint16, arr2) { (arr2)[0] = (uint8_t)(uint16); \
                                             (arr2)[1] = (uint8_t)((uint16) >> 8); }
#define UInt16ToByteArrayMsb(uint16, arr2) { (arr2)[1] = (uint8_t)(uint16); \
                                             (arr2)[0] = (uint8_t)((uint16) >> 8); }

#define ByteArrayLsbToUInt32(arr4, uint32) { (uint32) = (uint32_t)((arr4)[0] | ((arr4)[1] << 8) | ((arr4)[2] << 16) | ((arr4)[3] << 24)); }
#define ByteArrayMsbToUInt32(arr4, uint32) { (uint32) = (uint32_t)(((arr4)[0] << 24) | ((arr4)[1] << 16) | ((arr4)[2] << 8) | (arr4)[3]); }
#define UInt32ToByteArrayLsb(uint32, arr4) { (arr4)[0] = (uint8_t)(uint32); \
                                             (arr4)[1] = (uint8_t)((uint32) >> 8); \
                                             (arr4)[2] = (uint8_t)((uint32) >> 16); \
                                             (arr4)[3] = (uint8_t)((uint32) >> 24); }
#define UInt32ToByteArrayMsb(uint32, arr4) { (arr4)[3] = (uint8_t)(uint32); \
                                             (arr4)[2] = (uint8_t)((uint32) >> 8); \
                                             (arr4)[1] = (uint8_t)((uint32) >> 16); \
                                             (arr4)[0] = (uint8_t)((uint32) >> 24); }


/* Variables */
extern _NVM_DATA_STRUCT NvmData;

extern uint8_t DevTypeCode[2];
extern uint8_t DevUniqueID[3];
extern uint8_t ManuID[2];

extern uint8_t DeviceStatus; // Device Status
#define MAL_FUNCTION_BIT          0x80
#define CONFIG_CHANGED_BIT        0x40
#define COLD_START_BIT            0x20
#define MORE_STATUS_BIT           0x10
#define CURRENT_FIXED_BIT         0x08
#define CURRENT_SATUR_BIT         0x04
#define NONPV_OUT_OF_LIMIT_BIT    0x02
#define PV_OUT_OF_LIMIT_BIT       0x01

extern uint8_t ColdStartFlg; // Cold Start Flag
extern uint8_t MoreStatusFlg; // More Status Flag

#define CMD48_BYTES_LEN 9
extern uint8_t Cmd48Bytes_Pre[CMD48_BYTES_LEN], Cmd48Bytes_Now[CMD48_BYTES_LEN]; // Command 48
#define DEV_SPEC_STATUS0          Cmd48Bytes_Now[0]
#define DEV_SPEC_STATUS1          Cmd48Bytes_Now[1]
#define DEV_SPEC_STATUS2          Cmd48Bytes_Now[2]
#define DEV_SPEC_STATUS3          Cmd48Bytes_Now[3]
#define DEV_SPEC_STATUS4          Cmd48Bytes_Now[4]
#define DEV_SPEC_STATUS5          Cmd48Bytes_Now[5]
#define EXT_DEV_STATUS            Cmd48Bytes_Now[6]
#define DEV_OPA_MODE              Cmd48Bytes_Now[7]
#define STANDARD_STATUS0          Cmd48Bytes_Now[8]

extern uint8_t AppDate_Now[3];
#define  DAY_NOW                  AppDate_Now[0]
#define  MONTH_NOW                AppDate_Now[1]
#define  YEAR_NOW                 AppDate_Now[2]
extern uint32_t AppTime_Now; // Unit in 1/32 ms
#define  MS_TO_TIME(x)            (x * 32)

extern float fPv, fmA, fPercent;
#define PV_UPPER_LIMIT  100.0f
#define PV_LOWER_LIMIT  -40.0f

extern _DEV_SIMU_STRUCT PV_Simu;

/* Prototypes */
void hip_app_init(void);

void hip_app_route_1s(void);

void CfgChg_Ind(void);
uint8_t Get_DevStatus (void);

void Float2Bytes(float fIn, uint8_t* pBytes);
float Bytes2Float(uint8_t* pBytes);
void SetFloatNaN(float* pf);

void String2Bytes(char* pChar, uint8_t* pBytes, uint8_t byte_len);
void Bytes2String(uint8_t* pBytes, char* pChar, uint8_t byte_len);

uint32_t HTimeArrayToMs(uint8_t* pArr);
void MsToHTimeArray(uint32_t timer_ms, uint8_t* pArr);

int8_t find_cli_slot (char* pIDChar, uint8_t len);
/* return -1 if found no client record; otherwise return the client slot number */

#ifdef __cplusplus
}
#endif

#endif /* _HIP_APP_H */
