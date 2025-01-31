/*
*********************************************************************************************************
* HART-IP Commands.h
*********************************************************************************************************
*/
#ifndef _HIP_CMD_H
#define _HIP_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Prototypes */
uint8_t TPCmd_Exe(uint8_t cmd, uint8_t *appRcvPdu, uint8_t *appXmtPdu);

uint16_t DirCmd_Exe(uint16_t req_len, uint8_t *appRcvPdu, uint8_t *appXmtPdu);

void BTCmd_Exe(uint8_t idx);

#ifdef __cplusplus
}
#endif

#endif /* _HIP_APP_H */
