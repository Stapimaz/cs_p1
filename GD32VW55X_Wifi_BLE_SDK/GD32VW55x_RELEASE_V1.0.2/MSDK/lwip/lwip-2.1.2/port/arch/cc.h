/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * Copyright (c) 2024, GigaDevice Semiconductor Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

// #include <sys/time.h> // for struct timeval used by socket API
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"

#include "gd32vw55x_fwdgt.h"

// #include "trng.h"

#if 0
/* Define generic types used in lwIP */
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

typedef size_t mem_ptr_t;
typedef u32_t sys_prot_t;
#endif
typedef unsigned long  sys_prot_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "02x"
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"
#define SZT_F U32_F

/* Compiler hints for packing structures */
#define PACK_STRUCT_STRUCT __attribute__( (packed) )

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x) \
do {\
    printf x;\
} while(0);

#undef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) do { \
    printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    /* Enable write access to the watchdog */ \
    fwdgt_write_enable(); \
    /* Set the watchdog timeout to its maximum value (to ensure reset) */ \
    fwdgt_config(0x0FFF, FWDGT_PSC_DIV64); \
    /* Start the watchdog timer */ \
    fwdgt_enable(); \
    /* Wait for reset (this will not return) */ \
    while(1){}; \
} while(0)


#define LWIP_RAND() ((u32_t)rand()) // ((u32_t)trng_get())

#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) uint8_t variable_name[size] __ALIGN4 __SHAREDRAM

#endif /* __ARCH_CC_H__ */
