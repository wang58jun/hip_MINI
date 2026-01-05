/**
  ******************************************************************************
  * @file    hardware_rng.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    14-April-2017
  * @brief   mbedtls alternate entropy data function.
  *          the mbedtls_hardware_poll() is customized to use the STM32 RNG
  *          to generate random data, required for TLS encryption algorithms.
  *
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
#include "mbedtls_config.h"

#ifdef MBEDTLS_ENTROPY_HARDWARE_ALT

#include "main.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "mbedtls/entropy.h"

int mbedtls_hardware_poll( void *Data, unsigned char *Output, size_t Len, size_t *oLen )
{
  unsigned long randomValue = ((rand() * 20) + 1000); // Generate the random value
  ((void) Data);
  *oLen = 0;

  if (Len < sizeof(unsigned long)) { return 0; }

  memcpy( Output, &randomValue, sizeof(unsigned long) );
  *oLen = sizeof(unsigned long);

  return 0;
}

#endif /*MBEDTLS_ENTROPY_HARDWARE_ALT*/
