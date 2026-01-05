/**
  ******************************************************************************
  * File Name          : mbedtls.c
  * Description        : This file provides code for the configuration
  *                      of the mbedtls instances.
  ******************************************************************************
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

/* Includes ------------------------------------------------------------------*/
#include "mbedtls.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"
#else
#include "cmsis_os.h"
#endif

/* Global variables ---------------------------------------------------------*/

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
static unsigned char memory_buf[4096];

void exit (int i)
{
  for (;;);
}

#else
static void *platform_calloc( size_t n, size_t size )
{
  ((void) size);
  return pvPortMalloc(n);
}

static void platform_free( void *ptr )
{
  vPortFree(ptr);
}
#endif

/* MBEDTLS init function */
void MX_MBEDTLS_Init(void)
{
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
  mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));
#else
  mbedtls_platform_set_calloc_free(platform_calloc, platform_free);
#endif
}
