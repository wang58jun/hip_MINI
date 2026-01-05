/*
    Module       : tcp_server.c
    Description  : TCP/UDP server middleware based on W5500 chip
    Date         : Jan 2025
    Version      : v1.00
    Changelog    : v1.00 Initial
*/

#include <string.h>
#include "tcp_server.h"
#include "spi.h"
#include "w5500.h"
#include "socket.h"
#include "dhcp.h"

/* If you want to display debug & processing message, Define _TCP_DEBUG_ in tcp_server.h */
#ifdef _TCP_DEBUG_
   #include <stdio.h>
#endif

#if TLS_ENABLE
#include "mbedtls.h"
#include "mbedtls/net_sockets.h"

const uint16_t cipherSuites[] = {MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256,
                                MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,
                                MBEDTLS_TLS_PSK_WITH_AES_128_CCM};
#endif

/*
 * Definitions
 */
#if TLS_ENABLE
typedef struct
{
	mbedtls_ssl_context TlsCtx;      // TLS context
	mbedtls_ssl_config TlsCfg;       // TLS configuration
	mbedtls_ctr_drbg_context CtrDrbg;
	mbedtls_entropy_context entropy;
} _TLS_STRUCT;
#endif

/*
 * Variables
 */
// Default Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x03, 0x04, 0x05},
                            .ip = {192, 168, 0, 42},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 0, 1},
                            .dns = {8,8,8,8},
                            .dhcp = NETINFO_STATIC };
static uint8_t dhcp_sock_buff[TCP_BUF_SIZE];
static uint16_t port[TCP_SOCKS_CNT+UDP_SOCKS_CNT] = {5094, 5094, 5094, 5094, 5094, 5094, 5094};
static uint8_t rmt_udp_ip[UDP_SOCKS_CNT][4];
static uint16_t rmt_udp_port[UDP_SOCKS_CNT];

#if TLS_ENABLE
static _TLS_STRUCT tls[TCP_SOCKS_CNT + UDP_SOCKS_CNT];
#endif

/*
 * local functions
 */
// Register SPI callback functions for accessing WIZCHIP
static void register_wizchip_cbfunc()
{
	/* Critical section callback */
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   
	/* Chip selection call back */
	reg_wizchip_cs_cbfunc(SPI_ETH_CS_Select, SPI_ETH_CS_Deselect);
	
	/* SPI Read & Write callback function */
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);  
}

// Reset WIZCHIP
static void reset_wizchip()
{
	/* Chip RST pin set */
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	/* WIZCHIP SOCKET Buffer initialize */
	uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2} };
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1) {
#ifdef _TCP_DEBUG_
		printf("WIZCHIP Initialized fail.\r\n");
#endif
		while(1);
	}

	/* PHY link status check */
	uint8_t tmp;
	do{
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1) {
#ifdef _TCP_DEBUG_
			printf("Unknown PHY Link stauts.\r\n");
#endif
		}
	}while(tmp == PHY_LINK_OFF);
}

// Network parameters initial
static void network_init(void)
{
	wiz_NetTimeout gWIZNETTIME = {.retry_cnt = 8, .time_100us = 2000};
	ctlnetwork(CN_SET_TIMEOUT, (void*)&gWIZNETTIME);
	ctlnetwork(CN_GET_TIMEOUT, (void*)&gWIZNETTIME);

	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	
	// Display Network Information
#ifdef _TCP_DEBUG_
  uint8_t tmpstr[6];
	ctlwizchip(CW_GET_ID, (void*)tmpstr);
	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
    gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("=================\r\n");
#endif
}

static void my_ip_assign(void)
{
   getIPfromDHCP(gWIZNETINFO.ip);
   getGWfromDHCP(gWIZNETINFO.gw);
   getSNfromDHCP(gWIZNETINFO.sn);
   getDNSfromDHCP(gWIZNETINFO.dns);
   gWIZNETINFO.dhcp = NETINFO_DHCP;
   /* Network initialization */
   network_init(); // apply from dhcp
#ifdef _TCP_DEBUG_
   printf("DHCP LEASED TIME : %d Sec.\r\n", getDHCPLeasetime());
#endif
}

static void my_ip_conflict(void)
{
#ifdef _TCP_DEBUG_
	printf("CONFLICT IP from DHCP\r\n");
#endif
	//halt or reset or any...
	while(1); // this example is halt.
}

static uint8_t dhcp_start(void)
{
	setSHAR(gWIZNETINFO.mac);
	DHCP_init(DHCP_SOCK_NUM, dhcp_sock_buff);
	reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);

	return dhcp_poll();
}

/*
 * Prototypes
 */
uint8_t tcp_server_init (uint8_t dhcp_en)
{
  register_wizchip_cbfunc(); // Wiz chip call-back functions registration
  reset_wizchip(); // Reset Wiz chip

#if TLS_ENABLE
  MX_MBEDTLS_Init();
  for (uint8_t i=0; i<(TCP_SOCKS_CNT+UDP_SOCKS_CNT); i++) {
    mbedtls_ssl_init(&tls[i].TlsCtx);
    mbedtls_ssl_config_init(&tls[i].TlsCfg);
  }
#endif

  if (dhcp_en) { // start DHCP
		return dhcp_start();
	}
	else{
		network_init(); // Static IP for newwork initial
		return TCP_IP_ASSIGNED;
	}
}

/* Poll DHCP */
uint8_t dhcp_poll (void)
{
	return (DHCP_run() == DHCP_IP_LEASED) ? TCP_IP_ASSIGNED : TCP_IP_NULL;
}

// Set tcp/udp port number
void set_port_number (uint8_t sn, uint16_t port_num)
{
	port[sn] = port_num;
}

// tcp/udp socket package pull
uint16_t tcp_server_pull (uint8_t sn, uint8_t* tcpPdu)
{
	uint16_t len = 0;
	switch(getSn_SR(sn))									      // Get current socket status
	{
	case SOCK_CLOSED:														// Socket closed
//		if (sn < TCP_SOCKS_CNT) {
			socket(sn, Sn_MR_TCP, port[sn], (SF_TCP_NODELAY|SF_IO_NONBLOCK));	// Open Socket
//		} else {
//			socket(sn, Sn_MR_UDP, port[sn], 0); // UDP
//		}
#ifdef _TCP_DEBUG_
		printf("Socket-%d: openned.\r\n", sn);
#endif
		break;

	case SOCK_INIT:															// Socket initialized
		listen(sn);     													// Start to listen
		break;

	case SOCK_ESTABLISHED:											// Socket established
		if(getSn_IR(sn) & Sn_IR_CON) {
			setSn_IR(sn, Sn_IR_CON);						    // Notify connected
#ifdef _TCP_DEBUG_
			printf("Socket-%d: connected.\r\n", sn);
#endif
		}

		// Check received
		len = getSn_RX_RSR(sn);
		if(len) {
			memset(tcpPdu, 0, len+1);
#if TLS_ENABLE
      if (tls[sn].TlsCtx.private_state != MBEDTLS_SSL_HELLO_REQUEST) {
        mbedtls_ssl_read(&tls[sn].TlsCtx, (void*)tcpPdu, (size_t)len);
      } else
#endif
      {
        recv(sn, tcpPdu, len);
      }
#ifdef _TCP_DEBUG_
			// loop back
			printf("Socket-%d: %s\r\n", sn, tcpPdu);
			send(sn, tcpPdu, len);
#endif
		}
		break;

	case SOCK_CLOSE_WAIT:												// Socket waiting for close
		disconnect(sn);
		break;

	case SOCK_UDP:
		HAL_Delay(10);
		if(getSn_IR(sn) & Sn_IR_RECV) {
			setSn_IR(sn, Sn_IR_RECV);		// Set Sn_IR RECV bit
		}

		len = getSn_RX_RSR(0);
		if(len) {
			memset(tcpPdu, 0, len+1);
			recvfrom(sn, tcpPdu, len, rmt_udp_ip[sn], &rmt_udp_port[sn]);
#ifdef _TCP_DEBUG_
			printf("Socket-%d: %s\r\n", sn, tcpPdu);
			sendto(sn, tcpPdu, len, rmt_udp_ip[sn], &rmt_udp_port[sn]);
#endif
		}
		break;
	}

	return len;
}

// tcp/udp socket package push
int32_t tcp_server_push (uint8_t sn, uint8_t* tcpPdu, uint16_t len)
{
	if (sn < TCP_SOCKS_CNT) {
#if TLS_ENABLE
    if (tls[sn].TlsCtx.private_state != MBEDTLS_SSL_HELLO_REQUEST) {
      return mbedtls_ssl_write(&tls[sn].TlsCtx, (void*)tcpPdu, (size_t)len);
    } else
#endif
    {
      return send(sn, tcpPdu, len);
    }
	} else {
		return sendto(sn, tcpPdu, len, rmt_udp_ip[sn], rmt_udp_port[sn]);
	}
}

// tcp/udp socket close
int8_t tcp_server_close (uint8_t sn)
{
#if TLS_ENABLE
  // Release TLS context
  if (tls[sn].TlsCtx.private_state != MBEDTLS_SSL_HELLO_REQUEST) {
    mbedtls_ssl_free(&tls[sn].TlsCtx);
  }
#endif

  return disconnect(sn);
}


#if TLS_ENABLE
/**
 * @brief Open secure connection
 * @param[in] session number
 * @return Error code
 **/

static int mbedtls_status_is_ssl_in_progress(int ret) {
    return ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
           ret == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS;
}

int tls_connect(uint8_t sn, uint8_t* psk, char* client)
{
  int ret;

  //Init TLS context
  mbedtls_ssl_init(&tls[sn].TlsCtx);
  mbedtls_ssl_config_init(&tls[sn].TlsCfg);
  mbedtls_ctr_drbg_init(&tls[sn].CtrDrbg);
  mbedtls_entropy_init(&tls[sn].entropy);

  do {
    // Seed the random number generator
    if ((ret = mbedtls_ctr_drbg_seed(&tls[sn].CtrDrbg, mbedtls_entropy_func, &tls[sn].entropy, NULL, 0)) != 0) {
#ifdef _TCP_DEBUG_
      printf("Failed to seed RNG: -0x%x\n", -ret);
#endif
      break;
    }

    // Configure SSL
    if ((ret = mbedtls_ssl_config_defaults(&tls[sn].TlsCfg, MBEDTLS_SSL_IS_SERVER,
                                            MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
#ifdef _TCP_DEBUG_
      printf("Failed to configure SSL: -0x%x\n", -ret);
#endif
      break;
    }
    mbedtls_ssl_conf_rng(&tls[sn].TlsCfg, mbedtls_ctr_drbg_random, &tls[sn].CtrDrbg);
    mbedtls_ssl_conf_renegotiation(&tls[sn].TlsCfg, MBEDTLS_SSL_RENEGOTIATION_DISABLED);
    mbedtls_ssl_conf_ciphersuites(&tls[sn].TlsCfg, (const int *)cipherSuites);
#if (1)
    ret = mbedtls_ssl_conf_psk(&tls[sn].TlsCfg, psk, 16, (const uint8_t*)client, strlen(client));
#else
    /* BUG: HART-IP Portable Client v2 is using a string type for the PSK key */
    unsigned char str[] = "7777772e68617274636f6d6d2e6f7267 ";
    str[32] = '\0';
    ret = mbedtls_ssl_conf_psk(&tls[sn].TlsCfg, str, 33, (const uint8_t*)client, strlen(client));
#endif
    if (ret != 0) {
#ifdef _TCP_DEBUG_
      printf("Failed to set up PSK: -0x%x\n", -ret);
#endif
      break;
    }
    // Setup SSL
    if ((ret = mbedtls_ssl_setup(&tls[sn].TlsCtx, &tls[sn].TlsCfg)) != 0) {
#ifdef _TCP_DEBUG_
      printf("Failed to setup SSL: -0x%x\n", -ret);
#endif
      break;
    }
  } while(0);

  if (ret == 0) {
    // Setup BIO
    mbedtls_ssl_set_bio(&tls[sn].TlsCtx, &sn, mbedtls_net_send, mbedtls_net_recv, NULL);

    // Start handshake
    while ((ret = mbedtls_ssl_handshake(&tls[sn].TlsCtx)) != 0) {
      if (!mbedtls_status_is_ssl_in_progress(ret)) {
#ifdef _TCP_DEBUG_
        printf("Failed to perform SSL handshake: -0x%x\n", -ret);
#endif
        break;
      }
    }
#ifdef _TCP_DEBUG_
    if (ret == 0) {
      printf("SSL handshake ok\n    [ Protocol is %s ]\n    [ Ciphersuite is %s ]\n",
                mbedtls_ssl_get_version(&tls[sn].TlsCtx), mbedtls_ssl_get_ciphersuite(&tls[sn].TlsCtx));
    }
#endif
  } else {
    mbedtls_ssl_free(&tls[sn].TlsCtx);
    mbedtls_ssl_config_free(&tls[sn].TlsCfg);
    mbedtls_ctr_drbg_free(&tls[sn].CtrDrbg);
    mbedtls_entropy_free(&tls[sn].entropy);
  }

  return ret;
}
#endif
