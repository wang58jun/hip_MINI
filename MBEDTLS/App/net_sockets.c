/**
  ******************************************************************************
  * File Name       : App/net_sockets.c.h
  * Description     : TCP/IP or UDP/IP networking functions implementation based
                    see the file "mbedTLS/library/net_socket_template.c"
                    for the standard implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/*
 * This is a template implementation of the net_socket.c
 *
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <string.h>
#include <stdint.h>
#if defined(MBEDTLS_NET_C)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif

#include "mbedtls/net_sockets.h"
#include "socket.h"


static int net_would_block( const mbedtls_net_context *ctx );

/*
 * Initialize IP stack
 */
void mbedtls_net_init( mbedtls_net_context *ctx )
{
//  MX_LWIP_Init();
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
  int ret = 0;

  return( ret );
}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind( mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto )
{
  int ret;

  return( ret );
}

/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 *
 * Note: on a blocking socket this function always returns 0!
 */
static int net_would_block( const mbedtls_net_context *ctx )
{
  return( 0 );
}

/*
 * Accept a connection from a remote client
 */
int mbedtls_net_accept( mbedtls_net_context *bind_ctx,
                        mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len )
{
  return( 0 );
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
  /* LwIP doesn't currently support it  */
  return( 1 );
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
//    return( fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL, 0 ) | O_NONBLOCK ) );
  return( 0 );
}

/*
 * Check if data is available on the socket
 */

int mbedtls_net_poll( mbedtls_net_context *ctx, uint32_t rw, uint32_t timeout )
{
  int ret = 0;

  return( ret );
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep( unsigned long usec )
{
/*
  struct timeval tv;

  tv.tv_sec  = usec / 1000000;
  tv.tv_usec = usec % 1000000;

  select( 0, NULL, NULL, NULL, &tv );
*/
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
  return recv((uint8_t)(&ctx), buf, len);
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf,
                              size_t len, uint32_t timeout )
{
  /* This call will not block */
  return( mbedtls_net_recv( ctx, buf, len ) );
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
  return send((uint8_t)(&ctx), (uint8_t*)buf, len);
}

/*
 * Gracefully close the connection
 */
void mbedtls_net_free( mbedtls_net_context *ctx )
{
  if( ctx->fd == -1 )
    return;

//  shutdown( ctx->fd, 2 );
//  close( ctx->fd );

  ctx->fd = -1;
 }

#endif /* MBEDTLS_NET_C */
