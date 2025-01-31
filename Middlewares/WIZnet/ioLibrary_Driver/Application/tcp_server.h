#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wizchip_conf.h"

// Definitions
#define TCP_IP_NULL     0
#define TCP_IP_ASSIGNED 1

#define TCP_SOCKS_CNT   5
#define UDP_SOCKS_CNT   2
#define DHCP_SOCK_NUM   7
#define TCP_BUF_SIZE  512

// Variables
extern wiz_NetInfo gWIZNETINFO;

// Prototypes
uint8_t tcp_server_init (uint8_t dhcp_en);

uint8_t dhcp_poll (void);

void set_port_number (uint8_t sn, uint16_t port_num);

uint16_t tcp_server_pull (uint8_t sn, uint8_t* tcpPdu);

int32_t tcp_server_push (uint8_t sn, uint8_t* tcpPdu, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* _TCP_SERVER_H_ */
