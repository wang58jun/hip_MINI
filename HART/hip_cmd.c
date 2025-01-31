/*
    Module       : hip_cmd.c
    Description  : HART-IP Application Layer Commands
    Date         : Jan 2025
    Version      : v1.00
    Changelog    : v1.00 Initial
*/

#include <string.h>
#include "hip_svr.h"
#include "hip_app.h"
#include "hip_cmd.h"

/*
 * Definitions
 */

/*
 * Variables
 */
/* Table Of CRC Values for high-order byte */
const uint8_t auchCRCHi[256] = {
                                0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
                                0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
                                0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
                                0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
                                0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
                                0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
                                0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
                                0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
                                0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
                                0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
                                0x40
};

 /* Table of CRC values for low-order byte    */
const uint8_t auchCRCLo[256] = {
                                0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
                                0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
                                0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
                                0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
                                0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
                                0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
                                0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
                                0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
                                0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
                                0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
                                0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
                                0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
                                0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
                                0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
                                0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
                                0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
                                0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
                                0x40
};


/*
 * local functions
 */
/*
 * HART Commands Table
 * Parameters:
 *  cmd: 16-bits command number
 *  req_bc: Request byte count
 *  *pIn: Request data
 *  *pRC: Response code
 *  *pOut: Response data (not include response code)
 *  return: response byte count (not include response code)
 */
// Universal commands
static int16_t Cmd0Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd1Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd2Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd3Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd6Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd7Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd8Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd9Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd11Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd12Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd13Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd14Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd15Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd16Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd17Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd18Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd19Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd20Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd21Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd22Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd38Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd48Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
// Common practice commands
static int16_t Cmd41Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd50Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd54Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd72Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd78Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd79Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd90Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd103Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd104Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd105Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd107Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd108Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd109Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd520Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd521Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd532Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd533Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd534Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd538Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd539Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd540Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd541Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd542Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd543Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd544Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd545Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd546Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd547Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd552Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);
static int16_t Cmd553Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut);

/* Commands swtich */
static int16_t Cmd_Table_Switch(uint16_t cmd, uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  switch(cmd)
  { /* Commands table */
  // Universal Commands
  case 0: return Cmd0Routine(req_bc, pIn, pRC, pOut);
  case 1: return Cmd1Routine(req_bc, pIn, pRC, pOut);
  case 2: return Cmd2Routine(req_bc, pIn, pRC, pOut);
  case 3: return Cmd3Routine(req_bc, pIn, pRC, pOut);
  case 6: return Cmd6Routine(req_bc, pIn, pRC, pOut);
  case 7: return Cmd7Routine(req_bc, pIn, pRC, pOut);
  case 8: return Cmd8Routine(req_bc, pIn, pRC, pOut);
  case 9: return Cmd9Routine(req_bc, pIn, pRC, pOut);
  case 11: return Cmd11Routine(req_bc, pIn, pRC, pOut);
  case 12: return Cmd12Routine(req_bc, pIn, pRC, pOut);
  case 13: return Cmd13Routine(req_bc, pIn, pRC, pOut);
  case 14: return Cmd14Routine(req_bc, pIn, pRC, pOut);
  case 15: return Cmd15Routine(req_bc, pIn, pRC, pOut);
  case 16: return Cmd16Routine(req_bc, pIn, pRC, pOut);
  case 17: return Cmd17Routine(req_bc, pIn, pRC, pOut);
  case 18: return Cmd18Routine(req_bc, pIn, pRC, pOut);
  case 19: return Cmd19Routine(req_bc, pIn, pRC, pOut);
  case 20: return Cmd20Routine(req_bc, pIn, pRC, pOut);
  case 21: return Cmd21Routine(req_bc, pIn, pRC, pOut);
  case 22: return Cmd22Routine(req_bc, pIn, pRC, pOut);
  case 38: return Cmd38Routine(req_bc, pIn, pRC, pOut);
  case 48: return Cmd48Routine(req_bc, pIn, pRC, pOut);
  // Common practice commands
  case 41: return Cmd41Routine(req_bc, pIn, pRC, pOut);
  case 50: return Cmd50Routine(req_bc, pIn, pRC, pOut);
  case 54: return Cmd54Routine(req_bc, pIn, pRC, pOut);
  case 72: return Cmd72Routine(req_bc, pIn, pRC, pOut);
  case 78: return Cmd78Routine(req_bc, pIn, pRC, pOut);
  case 79: return Cmd79Routine(req_bc, pIn, pRC, pOut);
  case 90: return Cmd90Routine(req_bc, pIn, pRC, pOut);
  case 103: return Cmd103Routine(req_bc, pIn, pRC, pOut);
  case 104: return Cmd104Routine(req_bc, pIn, pRC, pOut);
  case 105: return Cmd105Routine(req_bc, pIn, pRC, pOut);
  case 107: return Cmd107Routine(req_bc, pIn, pRC, pOut);
  case 108: return Cmd108Routine(req_bc, pIn, pRC, pOut);
  case 109: return Cmd109Routine(req_bc, pIn, pRC, pOut);
  case 520: return Cmd520Routine(req_bc, pIn, pRC, pOut);
  case 521: return Cmd521Routine(req_bc, pIn, pRC, pOut);
  case 532: return Cmd532Routine(req_bc, pIn, pRC, pOut);
  case 533: return Cmd533Routine(req_bc, pIn, pRC, pOut);
  case 534: return Cmd534Routine(req_bc, pIn, pRC, pOut);
  case 538: return Cmd538Routine(req_bc, pIn, pRC, pOut);
  case 539: return Cmd539Routine(req_bc, pIn, pRC, pOut);
  case 540: return Cmd540Routine(req_bc, pIn, pRC, pOut);
  case 541: return Cmd541Routine(req_bc, pIn, pRC, pOut);
  case 542: return Cmd542Routine(req_bc, pIn, pRC, pOut);
  case 543: return Cmd543Routine(req_bc, pIn, pRC, pOut);
  case 544: return Cmd544Routine(req_bc, pIn, pRC, pOut);
  case 545: return Cmd545Routine(req_bc, pIn, pRC, pOut);
  case 546: return Cmd546Routine(req_bc, pIn, pRC, pOut);
  case 547: return Cmd547Routine(req_bc, pIn, pRC, pOut);
  case 552: return Cmd552Routine(req_bc, pIn, pRC, pOut);
  case 553: return Cmd553Routine(req_bc, pIn, pRC, pOut);
  // Others
  default:
    *pRC = 64; // Command not implemented
    return 0;
  }
}

/* CRC16: CRC-16-CCITT-FALSE (CRC-16/IBM3240) polynomial: x16 + x12 + x5 + 1 */
static uint16_t CRC16_CCITT(uint8_t *puchMsg, uint8_t usDataLen)
{
	uint8_t uchCRCHi = 0xff;
	uint8_t uchCRCLo = 0xff;
	uint8_t uindex;

	while(usDataLen--) {
		uindex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uindex];
		uchCRCLo = auchCRCLo[uindex];
	}
	return (uint16_t)((uchCRCHi<<8) | uchCRCLo);
}

/* String Characters Verify */
/* Shall only consist of Letters (a-z; A-Z), Digits (0-9), and Hyphens (LDH) */
static inline uint8_t String_Verify (char* str, uint8_t len)
{
	char c;
	uint8_t i, n = len;

	for (i=0; i<len; i++) {
		c = str[i];
		if (c == 0) {
			// Found the first null character
			n = i;
			break;
		}
		if ( ((c < '0') && (c != '-')) || ((c > '9') && (c < 'A')) || ((c > 'Z') && (c < 'a')) || (c > 'z') ) {
			return 0;
		}
	}
	if (n == 0) { return 0; }
	/* Check the rest characters */
	for (;i<len;i++) {
		if (str[i] != 0) {
			return 0;
		}
	}

	return n; // return the actual string length
}

/*
 * Prototypes
 */
/* Called by HART network layer stack to execute Token-passing command
 * appRcvPdu: bytecount + request data
 * appRspPdu: bytecount + response data (include Response_Code & Device_Status)
 */
uint8_t TPCmd_Exe(uint8_t cmd, uint8_t *appRcvPdu, uint8_t *appXmtPdu)
{
  int16_t rtn;

  if (cmd != 31) {
    rtn = Cmd_Table_Switch(cmd, appRcvPdu[0], appRcvPdu+1, appXmtPdu+1, appXmtPdu+3);
  } else {
    if (appRcvPdu[0] < 2) {
      appXmtPdu[1] = 5; // Too few bytes
      rtn = 0;
    } else {
      /* Extended command */
      uint16_t ext_cmd = (appRcvPdu[1] << 8) + appRcvPdu[2];
      appXmtPdu[3] = appRcvPdu[1], appXmtPdu[4] = appRcvPdu[2];
      rtn = Cmd_Table_Switch(ext_cmd, appRcvPdu[0]-2, appRcvPdu+3, appXmtPdu+1, appXmtPdu+5);
    }
  }
 
  if (rtn >= 0) {
    /* Need respond */
    appXmtPdu[0] = rtn + 2; // Response byte count
    if ((cmd == 31) && (appRcvPdu[0] >= 2)) { appXmtPdu[0] += 2; }
    appXmtPdu[2] = Get_DevStatus();
    return 1;
  } else {
    /* No need respond */
    appXmtPdu[0] = 0;
    return 0;
  }
}

/* Called by HART network layer stack to execute direct commands
 * req_len: total length of the request payload from network layer
 * appRcvPdu: 2 device status + commands
 * appRspPdu: 2 device status + commands
 */
uint16_t DirCmd_Exe(uint16_t req_len, uint8_t *appRcvPdu, uint8_t *appXmtPdu)
{
	int16_t i, rcv_len, xmt_len;
  uint16_t cmd;
  uint8_t *pIn, *pOut;

  rcv_len = req_len - 2; // remove the first 2 bytes
  pIn = appRcvPdu + 2; // The first request command sequence
  pOut = appXmtPdu + 2;
  xmt_len = 0;

  while (rcv_len > 0) {
    cmd = pIn[0] * 256 + pIn[1]; // Command number
    i = Cmd_Table_Switch(cmd, pIn[2], pIn+3, pOut+3, pOut+4); // Command executing
    if (i < 0) { i = 0; }

    // Update output buffer
    pOut[0] = pIn[0], pOut[1] = pIn[1];
    pOut[2] = i + 1; // command response byte count, should add one for Response Code
    i = 3 + pOut[2];
    pOut += i;
    xmt_len += i;

    // Update input
    i = 3 + pIn[2];
    pIn += i; // Next command sequence
    rcv_len -= i;
  }

  xmt_len += 2;
  appXmtPdu[0] = Get_DevStatus();
  appXmtPdu[1] = EXT_DEV_STATUS;

  return (uint16_t)xmt_len;
}

/* Called by HART application layer to execute burst command, and push into hip server with direct PDU mode
 * idx: Index of burst messages
 */
void BTCmd_Exe(uint8_t idx)
{
	int16_t n;
	uint8_t Req[8] = {0, 250, 250, 250, 250, 250, 250, 250};
	uint8_t Rsp[96];
	_BURST_MESSAGE_STRUCT *pBTmsg = NvmData.BurstMsg + idx;
	uint16_t cmd = pBTmsg->CmdNum;

	if ((cmd == 9) || (cmd == 33)) {
		for (n=0; n<8; n++) {
			if (pBTmsg->DVCode[n] >= 250) {
				break;
			}
			Req[n] = pBTmsg->DVCode[n];
		}
	} else {
		n = 0;
	}
	if ((cmd == 33) && (n > 4)) { n = 4; }

	n = Cmd_Table_Switch(cmd, (uint8_t)n, Req, Rsp+3, Rsp+4);
	if (n > 0) {
		Rsp[0] = (cmd >> 8) & 0xff;
		Rsp[1] = (cmd) & 0xff;
		Rsp[2] = (uint8_t)n;
		hip_burst_push(Rsp);
	}
}


/*
 * local functions
 */
#define CMD_RSP_CODE *pRC
// Universal commands
/* Command 0 Read Unique Identifier */
static int16_t Cmd0Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  pOut[0] = 0xfe;
  pOut[1] = DevTypeCode[0];
  pOut[2] = DevTypeCode[1];
  pOut[3] = 5; // Minimum number of Preambles Master to the Slave
  pOut[4] = 7; // HART Revision
  pOut[5] = DEV_REV_NUM;
  pOut[6] = SW_REV_NUM;
  pOut[7] = HW_REV_NUM << 3;
  pOut[8] = 0; // Flags
  pOut[9] = DevUniqueID[0];
  pOut[10] = DevUniqueID[1];
  pOut[11] = DevUniqueID[2];
  pOut[12] = 5; // Number of preambles Slave to the Master
  pOut[13] = 0; // Last Device Varialbe Code
  pOut[14] = (NvmData.CfgChgCnt >> 8) & 0xff;
  pOut[15] = NvmData.CfgChgCnt & 0xff;
  pOut[16] = EXT_DEV_STATUS; // Extended Device Status
  pOut[17] = ManuID[0];
  pOut[18] = ManuID[1];
  pOut[19] = ManuID[0];
  pOut[20] = ManuID[1];
  pOut[21] = 1;

  CMD_RSP_CODE = 0;
	return 22;
}

/* Command 1 Read Primary Variable */
static int16_t Cmd1Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  pOut[0] = 32; // Unit degC
  Float2Bytes(fPv, &pOut[1]);

  CMD_RSP_CODE = 0;
  return 5;
}

/* Command 2 Read Loop Current And Percent Of Range */
static int16_t Cmd2Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  Float2Bytes(fmA, &pOut[0]);
  Float2Bytes(fPercent, &pOut[4]);

  CMD_RSP_CODE = 0;
  return 8;
}

/* Command 3 Read Dynamic Variables And Loop Current */
static int16_t Cmd3Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  Float2Bytes(fmA, &pOut[0]);

  pOut[4] = 32; // Unit degC
  Float2Bytes(fPv, &pOut[5]);

  CMD_RSP_CODE = 0;
  return 9;
}

/* Command 6 Write Polling Address */
static int16_t Cmd6Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  if (req_bc < 1) {
    CMD_RSP_CODE = 5; // Too few bytes
    return 0;
  }

  if (pIn[0] > 63) {
    CMD_RSP_CODE = 2; // Invalid selection
    return 0;
  }

  if (req_bc >= 2) {
    /* HART 6 & 7 */
    if (pIn[1] != 0) { // Devices with no DAQ only support "Disabled" as Loop Current Mode
      CMD_RSP_CODE = 2; // Invalid selection
      return 0;
    }
  }

  NvmData.PollingAddr = pIn[0];
//  if (NvmData.PollingAddr != 0) {
//    fmA = 4.0f;
//  }

  CfgChg_Ind();                       /* Config Changed            */

  pOut[0] = NvmData.PollingAddr;
  pOut[1] = 0; // Loop current mode

  CMD_RSP_CODE = 0;
  return 2;
}

/* Command 7 Read Loop Configuration */
static int16_t Cmd7Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  pOut[0] = NvmData.PollingAddr;
  pOut[1] = 0;

  CMD_RSP_CODE = 0;
  return 2;
}

/* Command 8 Read Dynamic Variable Classifications */
static int16_t Cmd8Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  pOut[0] = 64; // Classification: Temperature
  pOut[1] = 250;
  pOut[2] = 250;
  pOut[3] = 250;

  CMD_RSP_CODE = 0;
  return 4;
}

/* Command 9 Read Device Variables with Status */
static int16_t Cmd9Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  uint8_t i, DVCode;
  uint8_t bc = req_bc;

  if (bc < 1) {
    CMD_RSP_CODE = 5; // Too few bytes
    return 0;
  }
  if (bc > 8) {
    bc = 8;
  }

  pOut[0] = EXT_DEV_STATUS; // Extended Device Status

  for(i = 0; i < bc; i++) {
    DVCode = pOut[1 + i * 8] = pIn[i];
    if (DVCode > 250) {
      CMD_RSP_CODE = 2; // Invalid selection
      return 0;
    } else if ((DVCode == 0x00) || (DVCode == 246)) {
      /* PV */
      pOut[1 + i * 8 + 1] = 64; // Classification: Temperature
      pOut[1 + i * 8 + 2] = 32; // Unit degC
      Float2Bytes(fPv, &pOut[1 + i * 8 + 3]);
      pOut[1 + i * 8 + 7] = 0xC0; // Device Variable Status: 0
    } else if (DVCode == 244) {
      /* %Range Percent */
      pOut[1 + i * 8 + 1] = 0;
      pOut[1 + i * 8 + 2] = 57;
      Float2Bytes(fPercent, &pOut[1 + i * 8 + 3]);
      pOut[1 + i * 8 + 7] = (PV_Simu.Mode) ? 0xB0 : 0xC0;
    } else if (DVCode == 245) {
      /* Loop Current */
      pOut[1 + i * 8 + 1] = 0;
      pOut[1 + i * 8 + 2] = 39;
      Float2Bytes(fmA, &pOut[1 + i * 8 + 3]);
      pOut[1 + i * 8 + 7] = (PV_Simu.Mode) ? 0xB0 : 0xC0;
    } else {
      pOut[1 + i * 8 + 1] = 0;
      pOut[1 + i * 8 + 2] = 250;
      pOut[1 + i * 8 + 3] = 0x7F;
      pOut[1 + i * 8 + 4] = 0xA0;
      pOut[1 + i * 8 + 5] = 0;
      pOut[1 + i * 8 + 6] = 0;
      pOut[1 + i * 8 + 7] = 0x30;
    }
  }

  pOut[1 + i * 8 + 0] = (AppTime_Now >> 24) & 0xff;
  pOut[1 + i * 8 + 1] = (AppTime_Now >> 16) & 0xff;
  pOut[1 + i * 8 + 2] = (AppTime_Now >> 8) & 0xff;
  pOut[1 + i * 8 + 3] = AppTime_Now & 0xff;

  CMD_RSP_CODE = 0;
  return 1 + 8*bc + 4;
}

/* Command 11 Read Unique Identifier Associated With Tag */
static int16_t Cmd11Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  if (req_bc >= 6) {
    if ((pIn[0] == NvmData.Tag[0]) &&
        (pIn[1] == NvmData.Tag[1]) &&
        (pIn[2] == NvmData.Tag[2]) &&
        (pIn[3] == NvmData.Tag[3]) &&
        (pIn[4] == NvmData.Tag[4]) &&
        (pIn[5] == NvmData.Tag[5]))
    { /* Tag matched               */
      return Cmd0Routine(req_bc, pIn, pRC, pOut);
    }
  }

  return -1; // no response
}

/* Command 12 Read Message */
static int16_t Cmd12Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  memcpy(pOut, NvmData.Message, 24);

  CMD_RSP_CODE = 0;
  return 24;
}

/* Command 13 Read Tag, Descriptor, Date */
static int16_t Cmd13Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  memcpy(pOut, NvmData.Tag, 6);
  memcpy(pOut+6, NvmData.Descriptor, 12);
  memcpy(pOut+18, NvmData.Date, 3);

  CMD_RSP_CODE = 0;
  return 6 + 12 + 3;
}

/* Command 14 Read Primary Variable Transducer Information */
static int16_t Cmd14Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  pOut[0] = pOut[1] = pOut[2] = 0x00; // Transducer Serial Number

  pOut[3] = 32; // Unit degC
  Float2Bytes(PV_UPPER_LIMIT, &pOut[4]); // USL
  Float2Bytes(PV_LOWER_LIMIT, &pOut[8]); // LSL
  Float2Bytes(0.1f, &pOut[12]); // Min Span

  CMD_RSP_CODE = 0;
  return 16;
}

/* Command 15 Read Device Information */
static int16_t Cmd15Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  pOut[0] = 250; // Alarm Code: Not Used
  pOut[1] = 0x00; // PV Transfer Function Code

  pOut[2] = 32; // Unit degC
  Float2Bytes(PV_UPPER_LIMIT, &pOut[3]); // URV
  Float2Bytes(PV_LOWER_LIMIT, &pOut[7]); // LRV
  Float2Bytes(1.0f, &pOut[11]); // Damp Value

  pOut[15] = 251; // Write Protect: None
  pOut[16] = 250;
  pOut[17] = 0;

  CMD_RSP_CODE = 0;
  return 18;
}

/* Command 16 Read Final Assembly Number */
static int16_t Cmd16Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  memcpy(pOut, NvmData.FinalAsmbNum, 3);

  CMD_RSP_CODE = 0;
  return 3;
}

/* Command 17 Write Message */
static int16_t Cmd17Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  if (req_bc < 24) {
    CMD_RSP_CODE = 5;
    return 0;
  }

  for (uint8_t i=0; i<24; i++) {
    pOut[i] = NvmData.Message[i] = pIn[i];
  }

  CfgChg_Ind();

  CMD_RSP_CODE = 0;
  return 24;
}

/* Command 18 Write Tag, Descriptor, Date */
static int16_t Cmd18Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  uint8_t i;

  if (req_bc < 21) {
    CMD_RSP_CODE = 5;
    return 0;
  }

  for (i=0; i<6; i++) {
    pOut[i] = NvmData.Tag[i] = pIn[i];
  }
  for (i=0; i<12; i++) {
    pOut[i+6] = NvmData.Descriptor[i] = pIn[i+6];
  }
  for (i=0; i<3; i++) {
    pOut[i+18] = NvmData.Date[i] = pIn[i+18];
  }

  CfgChg_Ind();

  CMD_RSP_CODE = 0;
  return 6 + 12 + 3;
}

/* Command 19 Write Final Assembly Number */
static int16_t Cmd19Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  if (req_bc < 3) {
    CMD_RSP_CODE = 5;
    return 0;
  }

  for (uint8_t i=0; i<3; i++) {
    pOut[i] = NvmData.FinalAsmbNum[i] = pIn[i];
  }

  CfgChg_Ind();

  CMD_RSP_CODE = 0;
  return 3;
}

/* Command 20 Read Long Tag */
static int16_t Cmd20Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  memcpy(pOut, NvmData.LongTag, 32);

  CMD_RSP_CODE = 0;
  return 32;
}

/* Command 21 Read Unique Identifier Associated With Long Tag */
static int16_t Cmd21Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  if (req_bc >= 32) {
    for (uint8_t i=0; i<32; i++) {
      if (NvmData.LongTag[i] != pIn[i]) {
        return -1;
      }
    }
    return Cmd0Routine(req_bc, pIn, pRC, pOut);
  }

  return -1;
}

/* Command 22 Write Long Tag */
static int16_t Cmd22_521(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut, uint8_t *pTag)
{
  uint8_t i;

  if (req_bc < 32)
  {
    CMD_RSP_CODE = 5;
    return 0;
  }

	if ((pIn[0] == 0x00) && (pTag == NvmData.UnitTag)) {
		/* Process unit tag can be null */
		for (i=0; i<32; i++) {
			if (pIn[i] != 0x00) {
				CMD_RSP_CODE = 6; // Device-Specific Command Error
				return 0;
			}
		}
	} else {
		if (!String_Verify((char*)pIn, 32)) {
			CMD_RSP_CODE = 6; // Device-Specific Command Error
			return 0;
		}
	}
  for (i=0; i<32; i++) {
    pTag[i] = pOut[i] = pIn[i];
  }

  CfgChg_Ind();

  CMD_RSP_CODE = 0;
  return 32;
}

static int16_t Cmd22Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  return Cmd22_521(req_bc, pIn, pRC, pOut, NvmData.LongTag);
}

/* Command 38 Reset Configuration Changed Flag */
static int16_t Cmd38Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  if (req_bc == 0) {
    /* HART 5 */
    pOut[0] = (NvmData.CfgChgCnt >> 8) & 0xff;
    pOut[1] = NvmData.CfgChgCnt & 0xff;
  } else {
    if (req_bc < 2) {
      CMD_RSP_CODE = 5;
      return 0;
    }
    if (NvmData.CfgChgCnt != (pIn[0] * 256 + pIn[1])) {
      CMD_RSP_CODE = 9;
      return 0;
    }
    pOut[0] = pIn[0];
    pOut[1] = pIn[1];
  }

  NvmData.CmdCfgChgFlg = 0; // Clear Config Change Flag

  CMD_RSP_CODE = 0;
  return 2;
}

/* Command 48 Read Additional Device Status */
static int16_t Cmd48Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
  CMD_RSP_CODE = 0;

  if (req_bc != 0) {
    /* HART 7 */
    if (req_bc < CMD48_BYTES_LEN) {
      CMD_RSP_CODE = 5;
      return 0;
    }

    /* Check data */
    for (uint8_t i=0; i<CMD48_BYTES_LEN; i++) {
      if (Cmd48Bytes_Now[i] != pIn[i]) {
        CMD_RSP_CODE = 14; // Status bytes mismatch
        break;
      }
    }
    if ((CMD_RSP_CODE == 0) && MoreStatusFlg) {
      memcpy (Cmd48Bytes_Pre, Cmd48Bytes_Now, CMD48_BYTES_LEN);
      MoreStatusFlg = 0; // Clear More Status Flag
    }
  }

  memcpy(pOut, Cmd48Bytes_Now, CMD48_BYTES_LEN);

  return CMD48_BYTES_LEN;
}

// Common practice commands
/* Command 41 Perform Self-Test */
static int16_t Cmd41Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	CMD_RSP_CODE = 0;
	return 0;
}

/* Command 50 Read Dynamic Variable Assignments */
static int16_t Cmd50Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	pOut[0] = 0; // Device variable 0
	pOut[1] = 250;
	pOut[2] = 250;
	pOut[3] = 250;

	CMD_RSP_CODE = 0;
	return 4;
}

static int16_t Cmd54Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t DVCode;

	if (req_bc < 1) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	DVCode = pOut[0] = pIn[0];
	if (DVCode > 250) {
		CMD_RSP_CODE = 2; // Invalid selection
		return 0;
	} else if ((DVCode == 0x00) || (DVCode == 246)) {
		/* PV */
		pOut[1] = pOut[2] = pOut[3] = 0x00; // Transducer Serial Number
		pOut[4] = 32; // Unit degC
		Float2Bytes(PV_UPPER_LIMIT, &pOut[5]); // USL
		Float2Bytes(PV_LOWER_LIMIT, &pOut[9]); // LSL
		Float2Bytes(1.0f, &pOut[13]); // Damp Value
		Float2Bytes(0.1f, &pOut[17]); // Min Span
		pOut[21] = 64; // Classification: Temperature
		pOut[22] = 4; // Family: Temperature
		pOut[23] = 0x00;
		pOut[24] = 0x00;
		pOut[25] = 0x04;
		pOut[26] = 0xE2; // Acquisition Period: 10s
		pOut[27] = (PV_Simu.Mode) ? 0x80 : 0x00; // Device Variable Properties
	} else if (DVCode == 244) {
		/* %Range Percent */
		pOut[1] = pOut[2] = pOut[3] = 0x00; // Transducer Serial Number
		pOut[4] = 57; // Unit degC
		Float2Bytes(1.125f, &pOut[5]); // USL
		Float2Bytes(-0.0125f, &pOut[9]); // LSL
		Float2Bytes(1.0f, &pOut[13]); // Damp Value
		Float2Bytes(0.1f, &pOut[17]); // Min Span
		pOut[21] = 0; // Classification: Not classified
		pOut[22] = 250; // Family: Not used
		pOut[23] = 0x00;
		pOut[24] = 0x00;
		pOut[25] = 0x04;
		pOut[26] = 0xE2; // Acquisition Period: 10s
		pOut[27] = 0x00; // Device Variable Properties
	} else if (DVCode == 245) {
		/* Loop Current */
		pOut[1] = pOut[2] = pOut[3] = 0x00; // Transducer Serial Number
		pOut[4] = 39; // Unit mA
		Float2Bytes(22.0f, &pOut[5]); // USL
		Float2Bytes(3.8f, &pOut[9]); // LSL
		Float2Bytes(1.0f, &pOut[13]); // Damp Value
		Float2Bytes(0.1f, &pOut[17]); // Min Span
		pOut[21] = 0; // Classification: Not classified
		pOut[22] = 250; // Family: Not used
		pOut[23] = 0x00;
		pOut[24] = 0x00;
		pOut[25] = 0x04;
		pOut[26] = 0xE2; // Acquisition Period: 10s
		pOut[27] = 0x00; // Device Variable Properties
	} else {
		pOut[1] = pOut[2] = pOut[3] = 0x00; // Transducer Serial Number
		pOut[4] = 250; // Unit not used
		pOut[5] = pOut[9] = pOut[13]= pOut[17] = 0x7F;
		pOut[6] = pOut[10] = pOut[14]= pOut[18] = 0xA0;
		pOut[7] = pOut[11] = pOut[15]= pOut[19] = 0x00;
		pOut[8] = pOut[12] = pOut[16]= pOut[20] = 0x00;
		pOut[21] = 0; // Classification: Not classified
		pOut[22] = 250; // Family: Not used
		pOut[23] = 0xFF;
		pOut[24] = 0xFF;
		pOut[25] = 0xFF;
		pOut[26] = 0xFF; // Acquisition Period: 10s
		pOut[27] = 0x00; // Device Variable Properties
	}

  CMD_RSP_CODE = 0;
  return 28;
}

/* Command 72 Squawk */
static int16_t Cmd72Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t u;
	if (req_bc < 1) {
		u = 2; // Squawk Once
	} else {
		u = pIn[0];
		if ((u != 0) && (u != 1)) {
			CMD_RSP_CODE = 2; // Invalid Selection
			return 0;
		}
	}

	pOut[0] = u;
	CMD_RSP_CODE = 0;
	return 1;
}

/* Command 78 Read Aggregated Commands */
static int16_t Cmd78Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	int8_t i, n;
	int16_t xmt_len;
	uint16_t cmd;
	uint8_t *pReq, *pRsp;

	if (req_bc < 1) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	n = pIn[0]; // Number of commands requested
	if (n <= 0) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	}

  pReq = pIn + 1; // The first request command sequence
  pRsp = pOut + 2;
	xmt_len = 0;

	while (n > 0) {
		cmd = pReq[0] * 256 + pReq[1]; // Command number
		if (cmd == 78) {
			CMD_RSP_CODE = 9; // Invalid Command requested
			return 0;
		}
		i = Cmd_Table_Switch(cmd, pReq[2], pReq+3, pRsp+3, pRsp+4); // Command executing
		if (i < 0) { i = 0; }

		// Update output buffer
		pRsp[0] = pReq[0], pRsp[1] = pReq[1];
		pRsp[2] = i + 1; // command response byte count, should add one for Response Code
		i = 3 + pRsp[2];
		pOut += i;
		xmt_len += i;

		// Update input
		i = 3 + pReq[2];
		pReq += i; // Next command sequence

		n--;
  }

	xmt_len += 2;
	pOut[0] = 0x00; // Extended device status
	pOut[1] = pIn[0];

	CMD_RSP_CODE = 0;
	return xmt_len;
}

/* Command 79 Write Device Variable */
static int16_t Cmd79Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t DVCode, u;

	if (req_bc < 8) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	DVCode = pIn[0];
	if (DVCode > 250) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	} else if ((DVCode == 0x00) || (DVCode == 246)) {
		/* PV */
		u = pIn[1];
		if ((u != 0) && (u != 1)) {
			CMD_RSP_CODE = 10; // Invalid Write Device Variable Command Code
			return 0;
		}
		if (pIn[2] != 32) {
			CMD_RSP_CODE = 18; // Invalid Units Code
			return 0;
		}
		PV_Simu.Mode = u;
		PV_Simu.Unit = pIn[2];
		PV_Simu.Value = Bytes2Float(pIn+3);
		PV_Simu.Status = pIn[7];
		memcpy(pOut, pIn, 7);
	} else if ((DVCode == 244) || (DVCode == 245)) {
		/* mA & PV% */
		CMD_RSP_CODE = 19; // Device Variable index not allowed for this command
		return 0;
	} else {
		CMD_RSP_CODE = 17; // Invalid Device Variable Index
		return 0;
	}

	CMD_RSP_CODE = 0;
	return 8;
}

/* Command 90 Read Real-Time Clock */
static int16_t Cmd90Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	/* Current date & time */
	pOut[0] = AppDate_Now[0];
	pOut[1] = AppDate_Now[1];
	pOut[2] = AppDate_Now[2];
	pOut[3] = (AppTime_Now >> 24) & 0xff;
	pOut[4] = (AppTime_Now >> 16) & 0xff;
	pOut[5] = (AppTime_Now >> 8) & 0xff;
	pOut[6] = AppTime_Now & 0xff;
	
	/* Last set date & time */
	pOut[7] = 1;
	pOut[8] = 1;
	pOut[9] = 0;
	pOut[10] = pOut[11] = pOut[12] = pOut[13] = 0x00;
	
	pOut[14] = 0x02; // The clock is volatile and power was removed from and restored to the device.
	
	CMD_RSP_CODE = 0;
	return 15;
}

/* Command 103 Write Burst Period */
static uint8_t BT_UpdateT_Adjust (uint32_t *pBTUpdateT)
{
	uint8_t u = 0;
	if (*pBTUpdateT < 50) { *pBTUpdateT = 50; u = 1; }
	else if ((50 < *pBTUpdateT) && (*pBTUpdateT < 100)) { *pBTUpdateT = 100; u = 1; }
	else if ((100 < *pBTUpdateT) && (*pBTUpdateT < 250)) { *pBTUpdateT = 250; u = 1; }
	else if ((250 < *pBTUpdateT) && (*pBTUpdateT < 500)) { *pBTUpdateT = 500; u = 1; }
	else if ((500 < *pBTUpdateT) && (*pBTUpdateT < 1000)) { *pBTUpdateT = 1000; u = 1; }
	else if ((1000 < *pBTUpdateT) && (*pBTUpdateT < 2000)) { *pBTUpdateT = 2000; u = 1; }
	else if ((2000 < *pBTUpdateT) && (*pBTUpdateT < 4000)) { *pBTUpdateT = 4000; u = 1; }
	else if ((4000 < *pBTUpdateT) && (*pBTUpdateT < 8000)) { *pBTUpdateT = 8000; u = 1; }
	else if ((8000 < *pBTUpdateT) && (*pBTUpdateT < 16000)) { *pBTUpdateT = 16000; u = 1; }
	else if ((16000 < *pBTUpdateT) && (*pBTUpdateT < 32000)) { *pBTUpdateT = 32000; u = 1; }
	else if ((32000 < *pBTUpdateT) && (*pBTUpdateT < 60000)) { *pBTUpdateT = 60000; u = 1; }
	else if (*pBTUpdateT > 3600000) { *pBTUpdateT = 3600000; u = 1; }

	return u;
}

static int16_t Cmd103Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t idx;
	uint32_t t, t_max;

	CMD_RSP_CODE = 0;

	if (req_bc < 9) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}
	
	idx = pOut[0] = pIn[0]; // Burst Message
	if (idx >= BURST_MSG_NUM) {
		CMD_RSP_CODE = 9; // Invalid Burst Message
		return 0;
	}
	
	/* Update Period */
	t = HTimeArrayToMs(&pIn[1]);
	t_max = HTimeArrayToMs(&pIn[5]);
	if (BT_UpdateT_Adjust(&t)) {
		CMD_RSP_CODE = 8; //Update Times Adjusted
	}
	if (BT_UpdateT_Adjust(&t_max)) {
		CMD_RSP_CODE = 8;
	}
	if (t_max < t) {
		t_max = t;
		CMD_RSP_CODE = 8;
	}
	
	MsToHTimeArray(t, &pOut[1]);
	MsToHTimeArray(t_max, &pOut[5]);
	
	NvmData.BurstMsg[idx].UpdatePeriod_ms = t;
	NvmData.BurstMsg[idx].MaxUpPeriod_ms = t_max;
	CfgChg_Ind();
	
	return 9;
}

/* Command 104 Write Burst Trigger */
static int16_t Cmd104Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t idx, n;
	uint8_t *pf;

	CMD_RSP_CODE = 0;

	if (req_bc < 8) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}
	
	idx = pOut[0] = pIn[0]; // Burst Message
	if (idx >= BURST_MSG_NUM) {
		CMD_RSP_CODE = 9; // Invalid Burst Message
		return 0;
	}
	n = pOut[1] = pIn[1];
	if (n > 4) {
		CMD_RSP_CODE = 13; // Invalid Burst Trigger Mode Selection
		return 0;
	}
	n = pOut[2] = pIn[2];
	if (n != 64) {
		CMD_RSP_CODE = 11; // Invalid Device Variable Classification
		return 0;
	}
	n = pOut[3] = pIn[3];
	if (n != 32) {
		CMD_RSP_CODE = 12; // Invalid Units Code
		return 0;
	}
	
	NvmData.BurstMsg[idx].TrigMode = pOut[1];
	pf = (uint8_t*)(&NvmData.BurstMsg[idx].TrigLevel);
	pf[3] = pOut[4] = pIn[4];
	pf[2] = pOut[5] = pIn[5];
	pf[1] = pOut[6] = pIn[6];
	pf[0] = pOut[7] = pIn[7];
	
	CfgChg_Ind();	
	
	return 8;
}

/* Command 105 Read Burst Mode Configuration */
static int16_t Cmd105Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t idx;

	if (req_bc == 0) {
		idx = 0;
		pOut[1] = NvmData.BurstMsg[idx].CmdNum;
	} else {
		idx = pIn[0];
		pOut[1] = 0x1F;
	}
	if (idx >= BURST_MSG_NUM) {
		CMD_RSP_CODE = 9; // Invalid Burst Message
		return 0;
	}

	pOut[0] = NvmData.BurstMsg[idx].CtrlCode;
	pOut[2] = NvmData.BurstMsg[idx].DVCode[0];
	pOut[3] = NvmData.BurstMsg[idx].DVCode[1];
	pOut[4] = NvmData.BurstMsg[idx].DVCode[2];
	pOut[5] = NvmData.BurstMsg[idx].DVCode[3];
	pOut[6] = NvmData.BurstMsg[idx].DVCode[4];
	pOut[7] = NvmData.BurstMsg[idx].DVCode[5];
	pOut[8] = NvmData.BurstMsg[idx].DVCode[6];
	pOut[9] = NvmData.BurstMsg[idx].DVCode[7];
	pOut[10] = idx;
	pOut[11] = BURST_MSG_NUM;
	pOut[12] = (NvmData.BurstMsg[idx].CmdNum >> 8) & 0xff;
	pOut[13] = (NvmData.BurstMsg[idx].CmdNum) & 0xff;
	MsToHTimeArray(NvmData.BurstMsg[idx].UpdatePeriod_ms, &pOut[14]);
	MsToHTimeArray(NvmData.BurstMsg[idx].MaxUpPeriod_ms, &pOut[18]);
	pOut[22] = NvmData.BurstMsg[idx].TrigMode;
	pOut[23] = 64;
	pOut[24] = 32;
	Float2Bytes(NvmData.BurstMsg[idx].TrigLevel, &pOut[25]);

	CMD_RSP_CODE = 0;
	return 29;
}

/* Command 107 Write Burst Device Variables */
static int16_t Cmd107Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t idx;
	uint8_t buf[8] = {250, 250, 250, 250, 250, 250, 250, 250};

	if (req_bc == 0) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	} else if (req_bc < 9) {
		idx = pOut[8] = 0;
	} else {
		idx = pOut[8] = pIn[8];
	}
	if (idx >= BURST_MSG_NUM) {
		CMD_RSP_CODE = 9; // Invalid Burst Message
		return 0;
	}

	for (uint8_t i=0; i<8; i++) {
		if (i < req_bc) {
			buf[i] = pIn[i];
		}
		pOut[i] = buf[i];
	}
	if (buf[0] != 0x00) {
		CMD_RSP_CODE = 2; // Invalid Device Variable Code
		return 0;
	}

	memcpy(NvmData.BurstMsg[idx].DVCode, buf, 8);
	CfgChg_Ind();

	CMD_RSP_CODE = 0;
	return 9;
}

/* Command 108 Write Burst Mode Command Number */
static int16_t Cmd108Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t idx, rsp_bc;
	uint16_t cmd;

	if ((req_bc == 0) || (req_bc == 2)) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	} else if (req_bc == 1) {
		/* HART 5 & 6 */
		idx = 0;
		cmd = pOut[0] = pIn[0];
		rsp_bc = 1;
	} else {
		cmd = (pIn[0] << 8) + pIn[1];
		pOut[0] = pIn[0], pOut[1] = pIn[1];
		idx = pOut[2] = pIn[2];
		rsp_bc = 3;
	}
	if (idx >= BURST_MSG_NUM) {
		CMD_RSP_CODE = 9; // Invalid Burst Message
		return 0;
	}

	if ((cmd != 1) && (cmd != 2) && (cmd != 3) && (cmd != 9) && (cmd != 38) && (cmd != 48)) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	}

	NvmData.BurstMsg[idx].CmdNum = cmd;
	CfgChg_Ind();

	CMD_RSP_CODE = 0;
	return rsp_bc;
}

/* Command 109 Burst Mode Control */
static int16_t Cmd109Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t idx, n;

	if (req_bc == 0) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	} else {
		n = pOut[0] = pIn[0];
		if (req_bc == 1) {
			idx = pOut[1] = 0; // HART 5 & 6
		} else {
			idx = pOut[1] = pIn[1];
		}
	}
	if (idx >= BURST_MSG_NUM) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	}

	if ((n != 0) && (n != 4)) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	}

	NvmData.BurstMsg[idx].CtrlCode = n;
	CfgChg_Ind();

	CMD_RSP_CODE = 0;
	return 2;
}

/* Command 520 Read Process Unit Tag */
static int16_t Cmd520Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	memcpy(pOut, NvmData.UnitTag, 32);

	CMD_RSP_CODE = 0;
	return 32;
}

/* Command 521 Write Process Unit Tag */
static int16_t Cmd521Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	return Cmd22_521(req_bc, pIn, pRC, pOut, NvmData.UnitTag);
}

/* Command 532 Read Client Subscription Summary */
static int16_t Cmd532Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 5) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	uint8_t n = get_ongoing_sn();
	if (n >= CLIENT_SLOTS_MAX) { n = 0; }

	/* Check field device uID */
	for (uint8_t i=0; i<5; i++) {
		uint8_t b = pIn[i];
//		if (i == 0) { b &= 0x3F; }
		if (Cli_Sub[n].BC_addr) {
			if (b != 0x00) {
				CMD_RSP_CODE = 9; // Target Unique ID Must be Broadcast Address
				return 0;
			}
		} else {
			if ((i == 0) && (b != (DevTypeCode[0]&0x3F))) {
				CMD_RSP_CODE = 65; // Unknown Unique ID
				return 0;
			}
			if ((i == 1) && (b != DevTypeCode[1])) {
				CMD_RSP_CODE = 65; // Unknown Unique ID
				return 0;
			}
			if (b != DevUniqueID[i-2]) {
				CMD_RSP_CODE = 65; // Unknown Unique ID
				return 0;
			}
		}
		pOut[i] = b;
	}

	pOut[5] = (Cli_Sub[n].cli_sub_flag >> 8) & 0xff;
	pOut[6] = (Cli_Sub[n].cli_sub_flag) & 0xff;	

	CMD_RSP_CODE = 0;
	return 7;
}

/* Command 533 Write Client Subscription Flags */
static int16_t Cmd533Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 7) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	uint8_t n = get_ongoing_sn();
	if (n >= CLIENT_SLOTS_MAX) { n = 0; }
	
	/* Check field device uID */
	uint8_t a = TRUE;
	for (uint8_t i=0; i<5; i++) {
		uint8_t b = pIn[i];
//		if (i == 0) { b &= 0x3F; }
		if ((a == FALSE) || (b != 0x00)) {
			/* A specific target field device */
			if (i == 0) {
				/* Check if in Broadcast address now */
				if ((Cli_Sub[n].BC_addr) && (Cli_Sub[n].cli_sub_flag != 0x0000)) {
					CMD_RSP_CODE = 9; // Individual Subscription Not Allowed
					return 0;
				}
				if (b != (DevTypeCode[0]&0x3F)) {
					CMD_RSP_CODE = 65; // Unknown Unique ID
					return 0;
				}
			}
			if ((i == 1) && (b != DevTypeCode[1])) {
				CMD_RSP_CODE = 65; // Unknown Unique ID
				return 0;
			}
			if (b != DevUniqueID[i-2]) {
				CMD_RSP_CODE = 65; // Unknown Unique ID
				return 0;
			}
			a = FALSE;
		}
		pOut[i] = b;
	}
	Cli_Sub[n].BC_addr = a;

	CMD_RSP_CODE = 0;
	if (pIn[5] != 0x00) {
		pOut[5] = 0x00, CMD_RSP_CODE = 8; // Set to Nearest Value
	} else {
		pOut[5] = pIn[5];
	}
	if (pIn[6] & 0x02) {
		pOut[6] = pIn[6] & 0x0D, CMD_RSP_CODE = 8; // Do not support event notification
	} else {
		pOut[6] = pIn[6];
	}
	Cli_Sub[n].cli_sub_flag = (pOut[5] << 8) + pOut[6];

	return 7;
}

/* Command 534 Read Device Variable Command Code */
static int16_t Cmd534Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	uint8_t DVCode;

	if (req_bc < 1) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	DVCode = pOut[0] = pIn[0];
	if (DVCode > 250) {
		CMD_RSP_CODE = 2; // Invalid selection
		return 0;
	} else if ((DVCode == 0x00) || (DVCode == 246)) {
		/* PV */
		if (PV_Simu.Mode == 0) {
			/* Simu Off */
			pOut[1] = 0;
			pOut[2] = 250;
			pOut[3] = 0x7F;
			pOut[4] = 0xA0;
			pOut[5] = 0x00;
			pOut[6] = 0x00;
			pOut[7] = 0x00;
		} else {
			pOut[1] = 1;
			pOut[2] = PV_Simu.Unit;
			Float2Bytes(PV_Simu.Value, &pOut[3]);
			pOut[7] = PV_Simu.Status;
		}
	} else if ((DVCode == 244) || (DVCode == 245)) {
		/* mA & PV% */
		pOut[1] = 0;
		pOut[2] = 250;
		pOut[3] = 0x7F;
		pOut[4] = 0xA0;
		pOut[5] = 0x00;
		pOut[6] = 0x00;
		pOut[7] = 0x00;
	} else {
		CMD_RSP_CODE = 17; // Invalid Device Variable Index
		return 0;
	}

	CMD_RSP_CODE = 0;
	return 8;
}

/* Command 538 Read HART-IP Server Ports */
static int16_t Cmd538Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	pOut[0] = (NvmData.UdpPort >> 8) & 0xff;
	pOut[1] = NvmData.UdpPort & 0xff;
	
	pOut[2] = (NvmData.TcpPort >> 8) & 0xff;
	pOut[3] = NvmData.TcpPort & 0xff;
	
	CMD_RSP_CODE = 0;
	return 4;
}

static int16_t Cmd539_540(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut, uint16_t *pPort)
{
	if (req_bc < 2) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	pOut[0] = pIn[0], pOut[1] = pIn[1];
	*pPort = (pIn[0] << 8) + pIn[1];
	hip_port_update();
	CfgChg_Ind();

	CMD_RSP_CODE = 0;
	return 2;
}

/* Command 539 Write HART-IP UDP Port */
static int16_t Cmd539Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	return Cmd539_540(req_bc, pIn, pRC, pOut, &NvmData.UdpPort);
}

/* Command 540 Write HART-IP TCP Port */
static int16_t Cmd540Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	return Cmd539_540(req_bc, pIn, pRC, pOut, &NvmData.TcpPort);
}

/* Command 541 Write Client PAKE Password */
static int16_t Cmd541Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 194) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	/* Check if it's from client slot 0 */
	if (get_ongoing_sn()) {
		CMD_RSP_CODE = 16; // Access Restricted
		return 0;
	}
	
	uint8_t n, i;

	n = pIn[0] & 0x0f;
	if (n >= CLIENT_SLOTS_MAX) {
		CMD_RSP_CODE = 12; // Invalid Client record (slot number too big)
		return 0;
	}

	/* password check */
	for (i=0; i<64; i++) {
		if (pIn[i+2] == 0x00) {
			break;
		}
	}
	if (i < 12) {
		CMD_RSP_CODE = 10; // Password too short
		return 0;
	}

	/* client_ID check */
	for (i=0; i<128; i++) {
		if (pIn[i+66] == 0x00) {
			break;
		}
	}
	if (i < 8) {
		CMD_RSP_CODE = 67; // Client Identifier too short
		return 0;
	} else if (i > 127) {
		i = 127;
	}
	int8_t r = find_cli_slot((char*)(pIn+66), i);
	if ((r >= 0) && (r != n)) {
		CMD_RSP_CODE = 13; // Duplicate Client ID
		return 0;
	}
	NvmData.CliRec[n].ID_len = i;

	NvmData.CliRec[n].cli_opt = (pIn[0] & 0xf0) >> 4;
	pOut[0] = pIn[0];
	pOut[1] = 0;
	for (i=0; i<64; i++) {
		NvmData.CliRec[n].passwd[i] = pOut[i+2] = pIn[i+2];
	}
	for (i=0; i<128; i++) {
		NvmData.CliRec[n].cli_ID[i] = pOut[i+66] = pIn[i+66];
	}

	CMD_RSP_CODE = 0;
	return 194;
}

/* Command 542 Write Client Pre-Shared Key */
static int16_t Cmd542Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 197) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	/* Check if it's from client slot 0 */
	if (get_ongoing_sn()) {
		CMD_RSP_CODE = 16; // Access Restricted
		return 0;
	}
	
	uint8_t n, i;

	n = pIn[0] & 0x0f;
	if (n >= CLIENT_SLOTS_MAX) {
		CMD_RSP_CODE = 12; // Invalid Client record (slot number too big)
		return 0;
	}

	/* key check */
	i = pIn[2];
	if (i != 18) { // 16-bytes PSK + 2-bytes CRC
		CMD_RSP_CODE = 10; // Unsupported Key Length
		return 0;
	}
	if (CRC16_CCITT(pIn+3, i)) {
		CMD_RSP_CODE = 65; // Key change failed
		return 0;
	}

	/* client_ID check */
	for (i=0; i<128; i++) {
		if (pIn[i+69] == 0x00) {
			break;
		}
	}
	if (i < 8) {
		CMD_RSP_CODE = 67; // Client Identifier too short
		return 0;
	} else if (i > 127) {
		i = 127;
	}
	int8_t r = find_cli_slot((char*)(pIn+69), i);
	if ((r >= 0) && (r != n)) {
		CMD_RSP_CODE = 13; // Duplicate Client ID
		return 0;
	}

	NvmData.CliRec[n].ID_len = i;
	NvmData.CliRec[n].cli_opt = (pIn[0] & 0xf0) >> 4;
	pOut[0] = pIn[0];
	pOut[1] = 0;
	memcpy(pOut+2, pIn+2, 67); // 1-byte key length + 66-bytes key values (PSK + CRC16)
	memcpy(NvmData.CliRec[n].PSK, pIn+3, 16);
	for (i=0; i<128; i++) {
		NvmData.CliRec[n].cli_ID[i] = pOut[i+69] = pIn[i+69];
	}

	CMD_RSP_CODE = 0;
	return 197;
}

/* Command 543 Read syslog Server HOSTNAME and Port */
static int16_t Cmd543Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	pOut[0] = (NvmData.SyslogSrv.port >> 8) & 0xff;
	pOut[1] = (NvmData.SyslogSrv.port) & 0xff;
	
	memcpy(pOut+2, (uint8_t*)NvmData.SyslogSrv.hostname, 64);
	
	CMD_RSP_CODE = 0;
	return 66;
}

/* Command 544 Write syslog Port */
static int16_t Cmd544Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	
	pOut[0] = pIn[0], pOut[1] = pIn[1];
	NvmData.SyslogSrv.port = (pIn[0] << 8) + pIn[1];
	CfgChg_Ind();

	CMD_RSP_CODE = 0;
	return 2;
}

/* Command 545 Write syslog Server HOSTNAME */
static int16_t Cmd545Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 64) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	/* Name string check */
	if (!String_Verify((char*)pIn, 64)) {
		CMD_RSP_CODE = 9; // Invalid HOSTNAME
		return 0;
	}

	for (uint8_t i=0; i<64; i++) {
		NvmData.SyslogSrv.hostname[i] = pOut[i] = pIn[i];
	}
	CfgChg_Ind();
	
	CMD_RSP_CODE = 0;
	return 64;
}

/* Command 546 Write syslog Server Pre-Shared Key */
static int16_t Cmd546Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 1) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}

	uint8_t keyL = pIn[0];
	if (req_bc < (keyL+1)) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	} else if (keyL != 18) { // 16-bytes PSK + 2-bytes CRC
		CMD_RSP_CODE = 10; // Unsupported Key Length
		return 0;
	}

	if (CRC16_CCITT(pIn+1, keyL)) {
		CMD_RSP_CODE = 65; // Key change failed
		return 0;		
	}

	memcpy(pOut, pIn, req_bc);
	memcpy(NvmData.SyslogSrv.PSK, pIn+1, 16);
	CfgChg_Ind();
	
	CMD_RSP_CODE = 0;
	return req_bc;
}

/* Command 547 Write syslog Server PAKE Password */
static int16_t Cmd547Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 64) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}
	
	uint8_t i;

	/* password check */
	for (i=0; i<64; i++) {
		if (pIn[i+2] == 0x00) {
			break;
		}
	}
	if (i < 12) {
		CMD_RSP_CODE = 10; // Password too short
		return 0;
	}
	for (i=0; i<64; i++) {
		NvmData.SyslogSrv.passwd[i] = pOut[i] = pIn[i];
	}
	CfgChg_Ind();
	
	CMD_RSP_CODE = 0;
	return 64;
}

/* Command 552 Read DiffServ DSCP Value */
static int16_t Cmd552Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	pOut[0] = NvmData.DscpCode;
	CMD_RSP_CODE = 0;
	return 1;
}

/* Command 553 Write DiffServ DSCP Value */
static int16_t Cmd553Routine(uint8_t req_bc, uint8_t *pIn, uint8_t *pRC, uint8_t *pOut)
{
	if (req_bc < 1) {
		CMD_RSP_CODE = 5; // Too few bytes
		return 0;
	}
	
	uint8_t u = pIn[0];
	if ((u & 0x01) || (u == 42) ||(u == 44)) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	} else if ((u != 0) && (u < 8) && (u > 48)) {
		CMD_RSP_CODE = 2; // Invalid Selection
		return 0;
	}
	
	/*
		TODO: setsockopt(fd, IPPROTO_IP, IP_TOS, &dscp_val, sizeof dscp_val)
	*/
	
	NvmData.DscpCode = pOut[0] = u;
	CfgChg_Ind();
	
	CMD_RSP_CODE = 0;
	return 1;
}
