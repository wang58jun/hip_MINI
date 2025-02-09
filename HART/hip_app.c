/*
    Module       : hip_app.c
    Description  : HART-IP Application Layer Implementation
    Date         : Jan 2025
    Version      : v1.00
    Changelog    : v1.00 Initial
*/

#include <string.h>
#include "adc.h"
#include "hip_svr.h"
#include "hip_app.h"
#include "hip_cmd.h"

#ifdef _HIP_DEBUG_
	#include "usbd_cdc_if.h"
#endif

/*
 * Definitions
 */


/*
 * Variables
 */
_NVM_DATA_STRUCT NvmData; // NVM data

uint8_t DevTypeCode[2] = {0xf9, 0xf5};
uint8_t DevUniqueID[3] = {0x00, 0x00, 0x00};
uint8_t ManuID[2] = {0xfe, 0xf9};

uint8_t DeviceStatus; // Device Status
uint8_t ColdStartFlg; // Cold Start Flag
uint8_t MoreStatusFlg; // More Status Flag
uint8_t Cmd48Bytes_Pre[CMD48_BYTES_LEN], Cmd48Bytes_Now[CMD48_BYTES_LEN]; // Command 48

uint8_t AppDate_Now[3];
uint32_t AppTime_Now; // Unit in 1/32 ms

float fPv, fmA, fPercent;
_DEV_SIMU_STRUCT PV_Simu;

static uint32_t BT_tick[BURST_MSG_NUM];


/*
 * local functions
 */
static void Factory_Recovery (void)
{
  char str[32];
	uint8_t i;

	NvmData.CfgChgCnt = 0x0000; // <TODO> Should not be reset
	NvmData.CmdCfgChgFlg = 0x00; // <TODO> Should not be reset
	NvmData.PollingAddr = 0x00;	// Polling Address

	sprintf(str, "00-%02X-%02X-%02X-%02X-%02X", ManuID[0], ManuID[1], DevUniqueID[0], DevUniqueID[1], DevUniqueID[2]);
	memcpy(NvmData.LongTag, (uint8_t*)str, 17);
	memset(NvmData.LongTag+17, 0, 32-17); // Long tag (Latin-1)
  memset(NvmData.UnitTag, 0, 32); // Process Unit tag (Latin-1)

	memset(NvmData.Tag, 0xaa, 6); // Tag (8 characters)
	memset(NvmData.Descriptor, 0xaa, 12); // Descriptor (16 characters)
	memset(NvmData.Message, 0xaa, 12); // Descriptor (16 characters)

	NvmData.Date[0] = NvmData.Date[1] = 1; // Day & Month
	NvmData.Date[2] = 0; // Year - 1900

	memset(NvmData.FinalAsmbNum, 0, 3); // Final assembly number

	// Burst Messages
	for (i=0; i<BURST_MSG_NUM; i++) {
		NvmData.BurstMsg[i].CtrlCode = 0; // Off
		NvmData.BurstMsg[i].CmdNum = 0x0001; // Command 1
		NvmData.BurstMsg[i].DVCode[0] = 0; // Device variable 0
		memset(NvmData.BurstMsg[i].DVCode+1, 0xFA, 7);
		NvmData.BurstMsg[i].UpdatePeriod_ms = 1000; // 1s
		NvmData.BurstMsg[i].MaxUpPeriod_ms= 16000; // 16s
		NvmData.BurstMsg[i].TrigMode = 0; // Continual
		NvmData.BurstMsg[i].TrigLevel = PV_UPPER_LIMIT;
	}

	NvmData.UdpPort = NvmData.TcpPort = 5094; // UDP & TCP port Number	

	// Client records
	for (i=0; i<CLIENT_SLOTS_MAX; i++) {
		NvmData.CliRec[i].cli_opt = 0;
		NvmData.CliRec[i].ID_len = 0;
		memset((uint8_t*)NvmData.CliRec[i].cli_ID, 0, 128); // null
		memset((uint8_t*)NvmData.CliRec[i].passwd, 0, 64); // null
		memset(NvmData.CliRec[i].PSK, 0, 64); // null
	}
	sprintf(str, "HART-IPClient"); NvmData.CliRec[0].ID_len = 13;
	memcpy((uint8_t*)NvmData.CliRec[0].cli_ID, (uint8_t*)str, 13);
	sprintf(str, "!1HARTIPhighway");
	memcpy((uint8_t*)NvmData.CliRec[0].passwd, (uint8_t*)str, 15);
	NvmData.CliRec[0].PSK[0] = 0x77; NvmData.CliRec[0].PSK[1] = 0x77;
	NvmData.CliRec[0].PSK[2] = 0x77; NvmData.CliRec[0].PSK[3] = 0x2E;
	NvmData.CliRec[0].PSK[4] = 0x68; NvmData.CliRec[0].PSK[5] = 0x61;
	NvmData.CliRec[0].PSK[6] = 0x72; NvmData.CliRec[0].PSK[7] = 0x74;
	NvmData.CliRec[0].PSK[8] = 0x63; NvmData.CliRec[0].PSK[9] = 0x6F;
	NvmData.CliRec[0].PSK[10] = 0x6D; NvmData.CliRec[0].PSK[11] = 0x6D;
	NvmData.CliRec[0].PSK[12] = 0x2E; NvmData.CliRec[0].PSK[13] = 0x6F;
	NvmData.CliRec[0].PSK[14] = 0x72; NvmData.CliRec[0].PSK[15] = 0x67;

	// Syslog Server
	NvmData.SyslogSrv.port = 514;
	for (i=0; i<64; i++) {
		NvmData.SyslogSrv.hostname[i] = NvmData.SyslogSrv.passwd[i] = 0; // null
	}
	sprintf(str, "syslog"); memcpy((uint8_t*)NvmData.SyslogSrv.hostname, (uint8_t*)str, 6);
	sprintf(str, "!1HARTIPhighway");
	memcpy((uint8_t*)NvmData.SyslogSrv.passwd, (uint8_t*)str, 15);
	NvmData.SyslogSrv.PSK[0] = 0x77; NvmData.SyslogSrv.PSK[1] = 0x77;
	NvmData.SyslogSrv.PSK[2] = 0x77; NvmData.SyslogSrv.PSK[3] = 0x2E;
	NvmData.SyslogSrv.PSK[4] = 0x68; NvmData.SyslogSrv.PSK[5] = 0x61;
	NvmData.SyslogSrv.PSK[6] = 0x72; NvmData.SyslogSrv.PSK[7] = 0x74;
	NvmData.SyslogSrv.PSK[8] = 0x63; NvmData.SyslogSrv.PSK[9] = 0x6F;
	NvmData.SyslogSrv.PSK[10] = 0x6D; NvmData.SyslogSrv.PSK[11] = 0x6D;
	NvmData.SyslogSrv.PSK[12] = 0x2E; NvmData.SyslogSrv.PSK[13] = 0x6F;
	NvmData.SyslogSrv.PSK[14] = 0x72; NvmData.SyslogSrv.PSK[15] = 0x67;
	
	NvmData.DscpCode = 10; // AF11. High-throughput data. Assured Forwarding Class 1. Low Drop Probability (Cisco "Gold")
	NvmData.enDHCP = FALSE; // Disable DHCP
}

static void DevStatus_Update (void)
{
	uint8_t i;
	
	if (PV_Simu.Mode) {
		STANDARD_STATUS0 |= 0x01; // Device Variable Simulation Active
	} else {
		STANDARD_STATUS0 &= ~0x01;
	}
	
	if (MoreStatusFlg) {
		/* MSA bit was set */
		for (i=0; i<CMD48_BYTES_LEN; i++) {
			if (Cmd48Bytes_Pre[i] != Cmd48Bytes_Now[i]) {
				break;
			}
		}
		if (i >= CMD48_BYTES_LEN) {
			/* All status recovery */
			MoreStatusFlg = 0;
		}
	} else {
		for (i=0; i<CMD48_BYTES_LEN; i++) {
			if (Cmd48Bytes_Pre[i] != Cmd48Bytes_Now[i]) {
				MoreStatusFlg = 1;
				break;
			}
		}
	}
}

static void DateTime_Update (void)
{
	/* Update Application Layer Timer */
	AppTime_Now += MS_TO_TIME(1) * 1000;
	
	if (AppTime_Now >= (uint32_t)(0xA4CB8000)) { // 24h * 60m * 60s * 32
		AppTime_Now = 0;
		/* Adjust date */
		uint8_t  bDayOverFlow = FALSE;
		DAY_NOW ++;
		if ( (DAY_NOW > 28) && (MONTH_NOW == 2) ) {
			bDayOverFlow = TRUE;
		} else if ( (DAY_NOW > 30) && ((MONTH_NOW == 4) || (MONTH_NOW == 6) || (MONTH_NOW == 9) || (MONTH_NOW == 11)) ) {
			bDayOverFlow = TRUE;
		} else if (DAY_NOW > 31) {
			bDayOverFlow = TRUE;
		}
		if (bDayOverFlow == TRUE) {
			DAY_NOW = 1;
			MONTH_NOW ++;
			if (MONTH_NOW > 12) {
				MONTH_NOW = 1; YEAR_NOW ++;
			}
		}
		/* End of date adjustment */
	}
}

static void BT_msg_refresh(void)
{
	for (uint8_t i=0; i<BURST_MSG_NUM; i++) {
		if (NvmData.BurstMsg[i].CtrlCode == 4) {
			if (BT_tick[i] <= 1000) {
				BTCmd_Exe(i); // Execute the burst command and push to hip server
				BT_tick[i] = NvmData.BurstMsg[i].UpdatePeriod_ms;
			} else {
				BT_tick[i] -= 1000;
			}
		} else {
			BT_tick[i] = NvmData.BurstMsg[i].UpdatePeriod_ms;
		}
	}
}

#if FLASH_ACCESS
inline static void NvmUpdate(void)
{

}
#endif


/*
 * Prototypes
 */
void hip_app_init(void)
{
	uint32_t i;
	uint8_t *pb = (uint8_t*)&i;

	// Use chip ID to fill the device UID
	i = HAL_GetUIDw0(); DevUniqueID[0] = pb[0] + pb[1] + pb[2] + pb[3];
	i = HAL_GetUIDw1(); DevUniqueID[1] = pb[0] + pb[1] + pb[2] + pb[3];
	i = HAL_GetUIDw2(); DevUniqueID[2] = pb[0] + pb[1] + pb[2] + pb[3];

	/* Initial NVM data */
	Factory_Recovery();

	// ADC calibration
  HAL_ADCEx_Calibration_Start(&hadc1);

	DeviceStatus = 0;
	ColdStartFlg = 1;
	MoreStatusFlg = 0;

	for (i=0; i<CMD48_BYTES_LEN; i++) {
		Cmd48Bytes_Pre[i] = Cmd48Bytes_Now[i] = 0;
	}

  AppDate_Now[0] = 1; // Day
  AppDate_Now[1] = 1; // Month
  AppDate_Now[2] = 2025-1900; // Year: 2025
	AppTime_Now = MS_TO_TIME(HAL_GetTick());

	for (i=0; i<BURST_MSG_NUM; i++) {
		BT_tick[i] = NvmData.BurstMsg[i].UpdatePeriod_ms;
	}
	
  fPv = single_chipT_conv();
  fPercent = (fPv - PV_LOWER_LIMIT) / (PV_UPPER_LIMIT - PV_LOWER_LIMIT);
  fmA = fPercent * 16.0f + 4.0f;
	
	PV_Simu.Mode = 0; // Simulation is off
	PV_Simu.Unit = 250;
	SetFloatNaN(&PV_Simu.Value);
	PV_Simu.Value = 0x00;
}

/* Application layer sub_function, called by network layer thread every 1s */
void hip_app_route_1s(void)
{
	float f;
	
	/* Toggle the LED */
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	
	/* Chip temperature sub-task */
	fPv = single_chipT_conv();
	f = (PV_Simu.Mode) ? PV_Simu.Value : fPv;
	fPercent = (f - PV_LOWER_LIMIT) / (PV_UPPER_LIMIT - PV_LOWER_LIMIT);
	fmA = fPercent * 16.0f + 4.0f;
#ifdef _HIP_DEBUG_
	usb_printf("Chip Temp: %.2f\r\n", fPv);
#endif
	
	/* Update device status bytes */
	DevStatus_Update();
	
	/* Update date & time */
	DateTime_Update();
	
	/* Refresh Burst messages */
	BT_msg_refresh();
}

void CfgChg_Ind(void)
{
  NvmData.CfgChgCnt ++; // Config Change Counter
  NvmData.CmdCfgChgFlg = 1; // Config Change Flags
#if FLASH_ACCESS
  NvmUpdate();
#endif
}

uint8_t Get_DevStatus (void)
{
	uint8_t s = DeviceStatus;

	// Config Changed bit
	if (NvmData.CmdCfgChgFlg) {
		s |= CONFIG_CHANGED_BIT;
	} else {
		s &= ~CONFIG_CHANGED_BIT;
	}
	// Cold start bit
	if (ColdStartFlg) {
		s |= COLD_START_BIT;
		ColdStartFlg = 0; // Clear Cold_Start_Bits
	} else {
		s &= ~COLD_START_BIT;
	}
	// More status bit  
	if (MoreStatusFlg) {
		s |= MORE_STATUS_BIT;
	} else {
		s &= ~MORE_STATUS_BIT;
	}

  return s;
}

void Float2Bytes(float fIn, uint8_t* pBytes)
{
	uint8_t* pdata = (uint8_t*)(&fIn);

	*(pBytes+3) = *(pdata++);
	*(pBytes+2) = *(pdata++);
	*(pBytes+1) = *(pdata++);
	*(pBytes) = *(pdata);
}

float Bytes2Float(uint8_t* pBytes)
{
	float f;
	uint8_t* pdata = (uint8_t*)(&f);

	*(pdata++) = *(pBytes+3) ;
	*(pdata++) = *(pBytes+2);
	*(pdata++) = *(pBytes+1);
	*(pdata) = *(pBytes);

	return f;
}

void SetFloatNaN(float* pf)
{
	uint8_t* pb = (uint8_t*)pf;

	pb[3] = 0x7F;
	pb[2] = 0xA0;
	pb[1] = 0x00;
	pb[0] = 0x00;
}

void String2Bytes(char* pChar, uint8_t* pBytes, uint8_t byte_len)
{
	for (uint8_t i=0; i<byte_len; i++) {
		pBytes[i] = atoi(pChar+i*2) * 16 + atoi(pChar+i*2+1);
	}
}

void Bytes2String(uint8_t* pBytes, char* pChar, uint8_t byte_len)
{
	for (uint8_t i=0; i<byte_len; i++) {
		sprintf(pChar+i*2, "%02X", pBytes[i]);
	}
}

uint32_t HTimeArrayToMs(uint8_t* pArr)
{
	uint32_t u32;
	ByteArrayMsbToUInt32(pArr, u32);
	u32 /= 32;
	return u32;
}

void MsToHTimeArray(uint32_t timer_ms, uint8_t* pArr)
{
	uint32_t u32 = timer_ms * 32;
	UInt32ToByteArrayMsb(u32, pArr);
}

/* return -1 if found no client record; otherwise return the client slot number */
int8_t find_cli_slot (char* pIDChar, uint8_t len)
{
	for (int8_t i=0; i<CLIENT_SLOTS_MAX; i++) {
		if (len != NvmData.CliRec[i].ID_len) {
			continue;
		} else if (!strcmp(pIDChar, NvmData.CliRec[i].cli_ID)) {
			// string matched
			return i;
		}
	}
	
	return -1;
}
