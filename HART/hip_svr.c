/*
    Module       : hip_svr.c
    Description  : HART-IP Newwork Layer Server Implementation
    Date         : Jan 2025
    Version      : v1.00
    Changelog    : v1.00 Initial
*/

#include <string.h>
#include "tcp_server.h"
#include "dhcp.h"
#include "hip_svr.h"
#include "hip_app.h"
#include "hip_cmd.h"

/* If you want to display debug & processing message, Define _HIP_DEBUG_ in hip_nwk.h */
#ifdef _HIP_DEBUG_
	#include "usbd_cdc_if.h"
#endif

/* 
 * Definitions
 */

typedef struct
{
	uint8_t cli_IPv4[4]; // Client IP address IPv4 (null if not IPv4)
	uint8_t cli_IPv6[16]; // Client IP address IPv6 (null if not IPv6)
	uint16_t cli_port;  // Client IP Port Number
	uint16_t svr_port;  // Server IP Port Number
	uint8_t conn_time[8]; // Connect time/date for that client
	uint8_t dis_time[8]; // Disconnect time/date for that client
	uint16_t SSS; // Session Status Summary
	uint16_t start_CCC; // Starting Configuration Change Counter
	uint16_t end_CCC;   // Ending Configuration Change Counter
	uint32_t N_pub;     // Number of Publish (Burst) PDUs
	uint32_t N_req;     // Number of Request PDUs
	uint32_t N_rsp;     // Number of Response PDUs
} _AUDIT_LOG_STRUCT;

#define AUDIT_LOG_LEN      sizeof(_AUDIT_LOG_STRUCT)

#define AUDIT_SSS_NONE     0x0000
#define AUDIT_SSS_WT_OCUR  0x0001  // Writes Occurred
#define AUDIT_SSS_BAD_INIT 0x0002  // Bad Session Initialization
#define AUDIT_SSS_ABORT    0x0004  // Aborted Session
#define AUDIT_SSS_TIM_OUT  0x0008  // Session Timeout
#define AUDIT_SSS_SEC      0x0010  // In-Secure Session

typedef struct
{
	uint8_t actived;    // Session is actived
	uint8_t cli_ver;    // Session client version number
	uint16_t time_out;  // Session close time (1s ~ 10min, default 90s)
	uint16_t tick;      // Session tick in actived
	_AUDIT_LOG_STRUCT *pAuditLog; // Point to the audit log
} _SESSION_STRUCT;

typedef struct
{
	uint8_t version;    // Version number
	uint8_t msg_type;   // message type: 0->Req; 1->Rsp; 2->Pub; 15->NAK
	uint8_t msg_ID;     // message ID: 0->Session Initial; 1-> Session Close; 2->Keep Alive; 3->TP PDU; 4->Dir PDU; 5->Read Audit Log	
	uint8_t status;     // status code
	uint8_t seq_num[2]; // sequence number
	uint8_t len[2];     // PDU lengh, including head & payload
	uint8_t payload[376]; // Payload (at least 384 bytes including the header should be supported)
} _HIP_PDU_STRUCT;


/* 
 * Variables
 */
// Flags for thread
static uint8_t b_1sTO, b_IPSet;

// Sessions
static _SESSION_STRUCT session[TCP_SOCKS_CNT + UDP_SOCKS_CNT];
static uint8_t sn_idx;

// PDUs
static _HIP_PDU_STRUCT rcv_pdu, xmt_pdu, pub_pdu[CLIENT_SLOTS_MAX];

// Publish
_CLIENT_SUB_STRUCT Cli_Sub[CLIENT_SLOTS_MAX];
static uint16_t pub_seq_num[CLIENT_SLOTS_MAX];

// Audit logs
static uint8_t audit_pw_time[8], audit_LSC_time[8];
static uint16_t audit_svr_status;
#define AUDIT_LOG_MAX 16  // <TODO> the STM32F103 chip does not own a large RAM
static _AUDIT_LOG_STRUCT audit_logs[AUDIT_LOG_MAX];
static uint8_t audit_log_idx;

/*
 * local functions
 */
#ifdef _HIP_DEBUG_
static void printf_IP(void)
{
	usb_printf("\r\n=== NET CONF ===\r\n");
	usb_printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
		gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	usb_printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	usb_printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	usb_printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	usb_printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	usb_printf("=================\r\n");
}
#endif

/* Syslog */
static void syslog_init(void)
{
	/* TODO */
}

/* Audit log */
static void audit_time_record(uint8_t* time_t)
{
	uint64_t t = ((YEAR_NOW-1) - 70) * 366 + (MONTH_NOW-1) * 30 + (DAY_NOW-1); // date
	t *= 24 * 60 * 60; // second
	t += (AppTime_Now / 32 / 1000);

	time_t[0] = (t >> 56) & 0xff;
	time_t[1] = (t >> 48) & 0xff;
	time_t[2] = (t >> 40) & 0xff;
	time_t[3] = (t >> 32) & 0xff;
	time_t[4] = (t >> 24) & 0xff;
	time_t[5] = (t >> 16) & 0xff;
	time_t[6] = (t >> 8) & 0xff;
	time_t[7] = (t) & 0xff;
}

static inline _AUDIT_LOG_STRUCT* new_audit_log(void)
{
	_AUDIT_LOG_STRUCT* pNewLog = &audit_logs[audit_log_idx]; // Point to the audit log

	/* TODO: add Client IPv4 & port numer */
	
	pNewLog->svr_port = NvmData.TcpPort;
	audit_time_record(pNewLog->conn_time);
	pNewLog->SSS = AUDIT_SSS_NONE;
	pNewLog->start_CCC = pNewLog->end_CCC = NvmData.CfgChgCnt;
	pNewLog->N_pub = pNewLog->N_req = pNewLog->N_rsp = 0;
	
	/* Adjust the index */
	audit_log_idx++;
	if (audit_log_idx >= AUDIT_LOG_MAX) { audit_log_idx = 0; }
	
	return pNewLog;
}

/* Session close */
static inline void session_close(uint8_t sn, uint16_t audit_sss)
{
	if (session[sn].actived && (session[sn].pAuditLog != NULL)) {
		audit_time_record(session[sn].pAuditLog->dis_time);
		session[sn].pAuditLog->SSS |= audit_sss;
	}
	session[sn].actived = 0;
	session[sn].cli_ver = 1;
	session[sn].time_out = 90;
	session[sn].tick = 0;
	session[sn].pAuditLog = NULL;
}

/* Token-passing messages */
static uint8_t TP_AddrVerify(uint8_t* pdata)
{
	uint8_t rtn = FALSE;

	if (!(pdata[0] & 0x80)) {
		// Polling address mode
		if (pdata[2] == 0) { // Check if command 0
			if ((pdata[1]&0x3f) == (NvmData.PollingAddr&0x3f)) { rtn = TRUE; }
			else { rtn = FALSE; }
		} else {
			rtn = FALSE;
		}
	} else {
		// Unique Address
		if ( ((pdata[1]&0x3f) == (DevTypeCode[0]&0x3f)) && (pdata[2] == DevTypeCode[1])
					&& (pdata[3] == DevUniqueID[0]) && (pdata[4] == DevUniqueID[1]) && (pdata[5] == DevUniqueID[2]) ) {
			rtn = TRUE;
		} else {
			if ((pdata[6] == 11) || (pdata[6] == 21)) {
				// Check if broadcase address
				if ( ((pdata[1]&0x3f) == 0) && (pdata[2] ==0 ) && (pdata[3] ==0 ) && (pdata[4] ==0 ) && (pdata[5] ==0 ) ) {
					rtn = TRUE;
				} else {
					rtn = FALSE;
				}
			} else {
				// Address Error
				rtn = FALSE;
			}
		}
	}

	return rtn;
}

static inline uint8_t CheckByte(uint8_t* pdata, uint16_t bc)
{
	uint8_t cb = 0x00;
	
	for (uint16_t i=0; i<bc; i++) {
		cb ^= pdata[i];
	}
	
	return cb;
}

static uint16_t req_pdu_handler(uint16_t bc);
static uint16_t msg_ID0_Routine(uint16_t bc);
static uint16_t msg_ID1_Routine(uint16_t bc);
static uint16_t msg_ID2_Routine(uint16_t bc);
static uint16_t msg_ID3_Routine(uint16_t bc);
static uint16_t msg_ID4_Routine(uint16_t bc);
static uint16_t msg_ID5_Routine(uint16_t bc);

static int32_t tcp_transmit_rsp(_HIP_PDU_STRUCT *pTxPdu);


/*
 * Prototypes
 */
void hip_server_init(void)
{
	/* Initial application layer including the NVM data */;
	hip_app_init();

	/* Set the mac address by using the ManuID & deviceUID */
	gWIZNETINFO.mac[0] = 0x00; gWIZNETINFO.mac[1] = ManuID[0]; gWIZNETINFO.mac[2] = ManuID[1];
	gWIZNETINFO.mac[3] = DevUniqueID[0]; gWIZNETINFO.mac[4] = DevUniqueID[1]; gWIZNETINFO.mac[5] = DevUniqueID[2];
	/* Initial TCP/UDP server */
	hip_port_update();
	b_IPSet = tcp_server_init(NvmData.enDHCP);
	if (b_IPSet) {
		printf_IP();
	}

	/* local variables */
	b_1sTO = FALSE; // No 1s tick event

	/* Sessions */
	for (uint8_t i=0; i<(TCP_SOCKS_CNT+UDP_SOCKS_CNT); i++) {
		session_close(i, AUDIT_SSS_NONE);
	}
	sn_idx = 0;

	/* Publish */
	for (uint8_t i=0; i<CLIENT_SLOTS_MAX; i++) {
		Cli_Sub[i].cli_sn = i;
		Cli_Sub[i].BC_addr = TRUE;
		Cli_Sub[i].cli_sub_flag = 0x0000; // No subscriptions

		pub_seq_num[i] = 0;
		pub_pdu[i].version = session[i].cli_ver;
		pub_pdu[i].msg_type = 0x02; // Publish
#ifdef _PUB_TP_MODE_
		pub_pdu[i].msg_ID = 0x03; // TP-Passing PDU
#else
		pub_pdu[i].msg_ID = 0x04; // Direct PDU
#endif
		pub_pdu[i].status = 0x00;
		pub_pdu[i].seq_num[0] = pub_pdu[i].seq_num[1] = 0x00;
		pub_pdu[i].len[0] = pub_pdu[i].len[1] = 0x00;
	}
	
	/* Syslog */
	syslog_init();
	
	/* Audit log */
	audit_time_record(audit_pw_time);
	memset(audit_LSC_time, 0xff, 8);
	audit_svr_status = 0x0001; // Unable to locate syslog Server
	for (uint8_t i=0; i<AUDIT_LOG_MAX; i++) {
		memset(audit_logs[i].cli_IPv4, 0x00, 4);
		memset(audit_logs[i].cli_IPv6, 0x00, 16);
		audit_logs[i].cli_port = 5094;
		audit_logs[i].svr_port = 5094;
		for (uint8_t j=0; j<8; j++) {
			audit_logs[i].conn_time[j] = audit_logs[i].dis_time[j] = 0xff;
		}
		audit_logs[i].SSS = 0x0000;
		audit_logs[i].start_CCC = audit_logs[i].end_CCC = 0x0000;
		audit_logs[i].N_pub = audit_logs[i].N_req = audit_logs[i].N_rsp = 0;
	}
	audit_log_idx = 0;
}

/* hip_server_thread: main thread for hip server routine
 * 1) this thread should be polled by the main loop periodly
 * 2) this thread would not return until the received socket message was proceeded completely;
 */
void hip_server_thread(void)
{
	/* Check if 1s time out */
	if (b_1sTO) {
		/* DHCP Check */
		if (NvmData.enDHCP && !b_IPSet) {
			b_IPSet = dhcp_poll();
#ifdef _HIP_DEBUG_
			if (b_IPSet) { printf_IP(); }
			else { usb_printf("Waiting DHCP...\r\n"); }
#endif
		}

		/* Sessions timer increase */
		for (uint8_t i=0; i<(TCP_SOCKS_CNT+UDP_SOCKS_CNT); i++) {
			if (session[i].actived) { session[i].tick ++; }
		}

		/* Call app layer routine */
		hip_app_route_1s();
		
		b_1sTO = FALSE;
	}

	/* IP connected & setup */
	if (b_IPSet) {
		uint8_t* pbuf = (uint8_t*)(&rcv_pdu); // Request pdu start point
		uint16_t len = tcp_server_pull(sn_idx, pbuf); // Read from the tcp server
		if (len) {
#ifdef _HIP_DEBUG_
			/* Print out the received PDU */
			char str[512];
			uint8_t l = (len > 160) ? 160 : len; // Prevent from overflow
			for (uint8_t i=0; i<l; i++) {
				sprintf(str+i*3, "-%02X", pbuf[i]);
			}
			usb_printf("Socket-%d rcv: [%d]%s\r\n", sn_idx, len, str);
#endif

			/* Process current received HART-IP PDU */
			len = req_pdu_handler(len);
			if (len) { // Need to respond
				tcp_transmit_rsp(&xmt_pdu); // Send to tcp server
				if (session[sn_idx].actived) { session[sn_idx].tick = 0; } // Reset session tick
			}
		}

		if (session[sn_idx].actived) { // Current session is actived
			/* Check session time out */
			if (session[sn_idx].tick > session[sn_idx].time_out) {
				session_close(sn_idx, AUDIT_SSS_TIM_OUT);
			}
			
			/* Check session publish */
			if (sn_idx < CLIENT_SLOTS_MAX) {
				_HIP_PDU_STRUCT *pPdu = &pub_pdu[sn_idx];
				len = (pPdu->len[0] << 8) + pPdu->len[1];
				if (len) {
					pPdu->version = session[sn_idx].cli_ver;
					pPdu->seq_num[0] = (pub_seq_num[sn_idx] >> 8) & 0xff;
					pPdu->seq_num[1] = (pub_seq_num[sn_idx]) & 0xff;
					len += 8; // hip header length
					pPdu->len[0] = (len >> 8) & 0xff;
					pPdu->len[1] = len & 0xff;
					if (tcp_transmit_rsp(pPdu) > 0) {
						pub_seq_num[sn_idx] ++;
					}
					pPdu->len[0] = pPdu->len[1] = 0;
				}
			}

			/* Check CCC */
			if (session[sn_idx].pAuditLog->start_CCC != NvmData.CfgChgCnt) {
				session[sn_idx].pAuditLog->end_CCC = NvmData.CfgChgCnt;
				session[sn_idx].pAuditLog->SSS |= AUDIT_SSS_WT_OCUR;
			}
		}

		sn_idx++; // Next socket
		if (sn_idx >= (TCP_SOCKS_CNT+UDP_SOCKS_CNT)) { sn_idx = 0; }
	}
}

/* Call by the system 1s timer event function */
void hip_time_handler(void)
{
	DHCP_time_handler();

	b_1sTO = TRUE;
}

/* Call by the app layer to push a burst message in TP or Direct mode (2 byte command + 1 bytecount + data)*/
void hip_burst_push(uint8_t* pBtPdu)
{
	for (uint8_t i=0; i<CLIENT_SLOTS_MAX; i++) {
		uint16_t cmd = (pBtPdu[0] << 8) + pBtPdu[1];
		uint8_t b = FALSE;
		if ((Cli_Sub[i].cli_sub_flag & 0x0004) && (cmd == 48)) {
			// 0x0004 Device Status
			b = TRUE;
		} else if ((Cli_Sub[i].cli_sub_flag & 0x0008) && (cmd == 38)) {
			// 0x0008 Device Configuration
			b = TRUE;
		} else if (Cli_Sub[i].cli_sub_flag & 0x0001) {
			// 0x0001 Process Data
			b = TRUE;
		}
		if (b) {
			uint16_t len;
#ifdef _PUB_TP_MODE_
			/* Total length: 1 Delimiter + 5 Address + 1 Command + 1 Bytecount + 1 RC + 1 DevStatus + data + 1 Checkbyte */ 
			len = 1 + 5 + 1 + 1 + 1 + pBtPdu[2] + 1;
			pub_pdu[i].payload[0] = 0x81; // Delimiter: BACK
			if (Cli_Sub[i].BC_addr) {
				memset(pub_pdu[i].payload+1, 0x00, 5); // Broadcast address
			} else {
				pub_pdu[i].payload[1] = DevTypeCode[0] & 0x3f;
				pub_pdu[i].payload[2] = DevTypeCode[1];
				pub_pdu[i].payload[3] = DevUniqueID[0];
				pub_pdu[i].payload[4] = DevUniqueID[1];
				pub_pdu[i].payload[5] = DevUniqueID[2]; 
			}
			if (cmd <= 256) {
				pub_pdu[i].payload[6] = pBtPdu[1]; // Command number
				pub_pdu[i].payload[7] = pBtPdu[2] + 1; // Byte count
				if (pBtPdu[2]) { memcpy(pub_pdu[i].payload+10, pBtPdu+4, pBtPdu[2]-1); }
			} else {
				pub_pdu[i].payload[6] = 0x31;
				pub_pdu[i].payload[7] = pBtPdu[2] + 3;
				pub_pdu[i].payload[10] = pBtPdu[0];
				pub_pdu[i].payload[11] = pBtPdu[1];
				if (pBtPdu[2]) { memcpy(pub_pdu[i].payload+12, pBtPdu+4, pBtPdu[2]-1); }
				len += 2;
			}
			pub_pdu[i].payload[8] = pBtPdu[3]; // Response code
			pub_pdu[i].payload[9] = Get_DevStatus(); // Device Status
			pub_pdu[i].payload[len-1] = CheckByte(pub_pdu[i].payload, len-1);
#else
			len = (pub_pdu[i].len[0] << 8) + pub_pdu[i].len[1];
			if (len == 0) {
				pub_pdu[i].payload[0] = Get_DevStatus();
				pub_pdu[i].payload[1] = EXT_DEV_STATUS;
				len = 2;
			}
			uint8_t n = pBtPdu[2] + 3;
			memcpy(pub_pdu[i].payload+len, pBtPdu, n);
			len += n;
#endif
			pub_pdu[i].len[0] = (len >> 8) & 0xff;
			pub_pdu[i].len[1] = (len) & 0xff;
		}
	}
}

/* Call by the app layer to update the tcp/udp port numbers */
void hip_port_update(void)
{
	for (uint8_t i=0; i<(TCP_SOCKS_CNT+UDP_SOCKS_CNT); i++) {
		if (i < (TCP_SOCKS_CNT-2)) {
			set_port_number(i, NvmData.TcpPort);
		} else if ((i > (TCP_SOCKS_CNT-1)) && (i < (TCP_SOCKS_CNT+UDP_SOCKS_CNT-1))) {
			set_port_number(i, NvmData.UdpPort);
		} else {
			set_port_number(i, 5094); // Keep others at 5094
		}
	}
}

/* Call by the app layer to get the current session number */
uint8_t get_ongoing_sn(void)
{
	return sn_idx;
}

/* Call by the app layer to indicate the security setup changed */
void sec_chg_notify(void)
{
	audit_time_record(audit_LSC_time);
}

/*
 * local functions
 */
/*
 * HART-IP request PDU processing sub-function
 * Return the byte length for transmitting (0 means no response is necessary)
*/
static uint16_t req_pdu_handler(uint16_t bc)
{
	// Check if need to decrypt
	if ((session[sn_idx].actived) && (session[sn_idx].cli_ver >= 2)) {
		/* TODO */

	}

	// Check the length
	if (bc != (rcv_pdu.len[0] * 256 + rcv_pdu.len[1])) {
		session_close(sn_idx, AUDIT_SSS_ABORT);
		return 0;
	}

	// Check HART-IP version
	if (rcv_pdu.version == 0){
		return 0;
	} else if (rcv_pdu.version > 2) {
		xmt_pdu.version = rcv_pdu.version = 2;
		xmt_pdu.status = 14; // HART-IP Protocol Version not supported
	} else {
		xmt_pdu.version = rcv_pdu.version;
		xmt_pdu.status = 0;
	}
	if ((session[sn_idx].actived) && (rcv_pdu.version != session[sn_idx].cli_ver)) {
		return 0;
	}

	// Check message type
  if ((rcv_pdu.msg_type != 0) && (rcv_pdu.msg_type != 2) && !rcv_pdu.msg_ID) {
		// Wrong message type
		session_close(sn_idx, AUDIT_SSS_ABORT); // Session close
		return 0;
	}
	if (rcv_pdu.msg_type == 2) {
		// Client publish
		return 0; // Discard the publish from Client, but not close the session
	}

	// Check message ID
	if (rcv_pdu.msg_ID > 5) {
		// Wrong message ID
		session_close(sn_idx, AUDIT_SSS_ABORT); // Session close
		return 0;
	}

  uint16_t xmt_len = 8;
	xmt_pdu.msg_type = 0x01; // Message type: Resposne
	xmt_pdu.msg_ID = rcv_pdu.msg_ID;
	xmt_pdu.status = 0;
	xmt_pdu.seq_num[0] = rcv_pdu.seq_num[0];
	xmt_pdu.seq_num[1] = rcv_pdu.seq_num[1];
	switch (rcv_pdu.msg_ID) {
	case 0:	xmt_len += msg_ID0_Routine(bc-8); break;
	case 1:	xmt_len += msg_ID1_Routine(bc-8); break;
	case 2:	xmt_len += msg_ID2_Routine(bc-8); break;
	case 3:	xmt_len += msg_ID3_Routine(bc-8); break;
	case 4:	xmt_len += msg_ID4_Routine(bc-8); break;
	case 5:	xmt_len += msg_ID5_Routine(bc-8); break;	
	}
	xmt_pdu.len[0] = (xmt_len>>8) & 0xff;
	xmt_pdu.len[1] = (xmt_len) & 0xff;

	if (session[sn_idx].pAuditLog != NULL) {
		session[sn_idx].pAuditLog->N_req ++;
	}
	
	return xmt_len;
}

/* Session Initiate (Message ID = 0) */
static uint16_t msg_ID0_Routine(uint16_t bc)
{
	if (bc < 5) {
		xmt_pdu.status = 5; // Too Few Data Bytes Received
		return 0;
	}
	if (session[sn_idx].actived) {
		xmt_pdu.status = 16; // Session already established
		return 0;
	}

	xmt_pdu.payload[0] = 1; // Primary master

	uint8_t* pdata = rcv_pdu.payload;
	uint32_t to = (pdata[1] << 24) + (pdata[2] << 16) + (pdata[3] << 8) + pdata[4];
	if (to > 600*1000) { to = 600*1000; xmt_pdu.status = 8; }
	else if (to < 1000) { to = 1000; xmt_pdu.status = 8; }
	xmt_pdu.payload[1] = (to >> 24) & 0xff;
	xmt_pdu.payload[2] = (to >> 16) & 0xff;
	xmt_pdu.payload[3] = (to >> 8) & 0xff;
	xmt_pdu.payload[4] = (to) & 0xff;
	session[sn_idx].time_out = (uint16_t)(to / 1000);

	session[sn_idx].actived = 1;
	session[sn_idx].cli_ver = rcv_pdu.version;
	session[sn_idx].pAuditLog = new_audit_log(); // Create new audit log
	if ((session[sn_idx].pAuditLog != NULL) && (rcv_pdu.version >= 2)) {
		session[sn_idx].pAuditLog->SSS |= AUDIT_SSS_SEC;
	}

	return 5;
}

/* Session Close (Message ID = 1) */
static uint16_t msg_ID1_Routine(uint16_t bc)
{
	session_close(sn_idx, AUDIT_SSS_NONE);
	return 0;
}

/* Keep Alive (Message ID = 2) */
static uint16_t msg_ID2_Routine(uint16_t bc)
{
	/* Do nothing */
	return 0;
}

/* HART Token-Passing Data-Link Layer PDU (Message ID = 3) */
static uint16_t msg_ID3_Routine(uint16_t bc)
{
	if (!session[sn_idx].actived) {
		xmt_pdu.status = 16; // Access Restricted
		return 0;
	}

	uint8_t* pTPreq = rcv_pdu.payload;
	uint8_t* pTPrsp = xmt_pdu.payload;

	uint8_t uniq_add_mode = (pTPreq[0] & 0x80);

	/* Check Delimiter */
	if ((pTPreq[0]&0x07) != 0x02) { return 0; } // Wrong frame type
	/* Check expand byte */
	if (pTPreq[0] & 0x60) { return 0; } // Expand byte should be 0
	/* Check Address */
	if (!TP_AddrVerify(pTPreq)) { return 0; } // Wrong address
	/* Check Byte */
	if (CheckByte(pTPreq, bc)) { return 0; } // Check byte error

	uint16_t xmt_len;
	if (!uniq_add_mode) {
		pTPrsp[0] = 0x06; // Delimiter
		pTPrsp[1] = (pTPreq[1] & 0x3F) + 0x80; // Address (Primary master always)
		pTPrsp[2] = pTPreq[2]; // Command
		xmt_len = 3;
	} else {
		pTPrsp[0] = 0x86;
		pTPrsp[1] = (pTPreq[1] & 0x3F) + 0x80;
		pTPrsp[2] = pTPreq[2], pTPrsp[3] = pTPreq[3], pTPrsp[4] = pTPreq[4], pTPrsp[5] = pTPreq[5];
		pTPrsp[6] = pTPreq[6];
		xmt_len = 7;
	}
	pTPreq += xmt_len; // point to byte-count byte
	pTPrsp += xmt_len;
	/* Application layer command executing */
	if (!TPCmd_Exe(rcv_pdu.payload[xmt_len-1], pTPreq, pTPrsp)) { return 0; } // Error
	xmt_len = xmt_len + 1 + pTPrsp[0]; // Byte count + data
	xmt_pdu.payload[xmt_len] = CheckByte(xmt_pdu.payload, xmt_len);
	xmt_len++;

	return xmt_len;
}

/* HART Direct PDU (Message ID = 4) */
static uint16_t msg_ID4_Routine(uint16_t bc)
{
	if (!session[sn_idx].actived) {
		xmt_pdu.status = 16; // Access Restricted
		return 0;
	}

	return DirCmd_Exe(bc, rcv_pdu.payload, xmt_pdu.payload);
}

/* Read Audit Log (Message ID = 5) */
static uint16_t msg_ID5_Routine(uint16_t bc)
{
	uint8_t start_idx, num_rec;
	uint8_t* pReq = rcv_pdu.payload;
	uint8_t* pRsp = xmt_pdu.payload;

	if (bc < 2) {
		start_idx = (bc == 0) ? 0 : pReq[0];
		num_rec = 1;
		xmt_pdu.status = 8; // Set to Nearest Possible Value
	} else {
		start_idx = pReq[0];
		num_rec = pReq[1];
		if (start_idx > AUDIT_LOG_MAX) { start_idx = 0; xmt_pdu.status = 8; }
		if (num_rec == 0) { num_rec = 1; xmt_pdu.status = 8; }
		if (num_rec > 4) { num_rec = 4; xmt_pdu.status = 8; }
		if ((num_rec+start_idx) > AUDIT_LOG_MAX) { num_rec = AUDIT_LOG_MAX - start_idx; xmt_pdu.status = 8; }
	}

	pRsp[0] = start_idx;
	pRsp[1] = num_rec;
	memcpy(pRsp+2, audit_pw_time, 8); 
	memcpy(pRsp+10, audit_LSC_time, 8);
	pRsp[18] = (audit_svr_status >> 8) & 0xff;
	pRsp[19] = (audit_svr_status) & 0xff;
	uint16_t recode_size = AUDIT_LOG_LEN * AUDIT_LOG_MAX;
	pRsp[20] = (recode_size >> 8) & 0xff;
	pRsp[21] = (recode_size) & 0xff;

	pRsp += 22; // Point to the start the first record
	for (uint8_t i=0; i<num_rec; i++) {
		memcpy(pRsp, (uint8_t*)&audit_logs[start_idx+i], AUDIT_LOG_LEN);	
		pRsp += AUDIT_LOG_LEN;
	}

	return (22 + num_rec*AUDIT_LOG_LEN);
}

static int32_t tcp_transmit_rsp(_HIP_PDU_STRUCT *pTxPdu)
{
	if ((session[sn_idx].actived) || (pTxPdu->msg_ID == 0x01)) {
		uint16_t len = (pTxPdu->len[0] << 8) + pTxPdu->len[1];
		uint8_t* pbuf = (uint8_t*)pTxPdu;

		// Check if need to crypt
		if (session[sn_idx].cli_ver >= 2) {
			/* TODO */
		}

		if (session[sn_idx].pAuditLog != NULL) {
			if (pTxPdu == &xmt_pdu) {
				session[sn_idx].pAuditLog->N_rsp ++;
			} else {
				session[sn_idx].pAuditLog->N_pub ++;
			}
		}
#ifdef _HIP_DEBUG_
		/* Print out the transmitted PDU */
		char str[512];
		uint8_t l = (len > 160) ? 160 : len; // Prevent from overflow
		for (uint8_t i=0; i<l; i++) {
			sprintf(str+i*3, "-%02X", pbuf[i]);
		}
		usb_printf("Socket-%d xmt: [%d]%s\r\n", sn_idx, len, str);
#endif
		return tcp_server_push(sn_idx, pbuf, len); // Send out response
	}
	
	return 0;
}
