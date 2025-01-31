/*
*********************************************************************************************************
* HART-IP Network Layer Server.h
*********************************************************************************************************
*/
#ifndef _HIP_SVR_H
#define _HIP_SVR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Definitions */
#define _HIP_DEBUG_
#define _PUB_TP_MODE_

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

// Client slot collection
#define CLIENT_SLOTS_MAX  5
typedef struct
{
  uint8_t     cli_opt;                      // Client options
  uint8_t     ID_len;                       // Client ID length (0 means not been initilized)
  char        cli_ID[128];                  // Client Identity shall be up to 127 printable ISO Latin-1 characters and must be at least 8 characters long
  char        passwd[64];                   // Password shall be 12-63 ISO-Latin-1 characters
  uint8_t     PSK[16];                      // Pre-Shared Key (shall be 128 bit length)
} _CLIENT_REC_STRUCT;

typedef struct
{
  uint8_t     cli_sn;                       // Client slot number
	uint8_t     BC_addr;                      // Broadcast address: 1 ; otherwise: 0
	uint16_t    cli_sub_flag;                 // Client Subscription Flags
} _CLIENT_SUB_STRUCT;

extern _CLIENT_SUB_STRUCT Cli_Sub[CLIENT_SLOTS_MAX];

typedef struct
{
  uint16_t    port;                      		// Port number
  char        hostname[64];                 // HOSTNAME, up to 64 ISO-Latin-1 characters
  char        passwd[64];                   // Password shall be 12-63 ISO-Latin-1 characters
  uint8_t     PSK[16];                      // Pre-Shared Key (shall be 128 bit length)
} _SYSLOG_SVR_STRUCT;


/* Prototypes */
void hip_server_init(void);

void hip_server_thread(void);

void hip_time_handler(void);

void hip_burst_push(uint8_t* pBtPdu);

void hip_port_update(void);

uint8_t get_ongoing_sn(void);

void sec_chg_notify(void);

#ifdef __cplusplus
}
#endif

#endif /* _HIP_SVR_H */
