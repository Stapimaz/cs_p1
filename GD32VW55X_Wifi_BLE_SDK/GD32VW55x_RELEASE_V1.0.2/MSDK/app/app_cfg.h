/*!
    \file    app_cfg.h
    \brief   application configuration for GD32VW55x SDK

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#include "platform_def.h"

#define CONFIG_DEBUG_PRINT_ENABLE

// #define PLATFORM_ASSERT_ENABLE

#define CONFIG_OTA_DEMO_SUPPORT
#ifdef CONFIG_OTA_DEMO_SUPPORT
#define OTA_DEMO_STACK_SIZE         512
#define OTA_DEMO_TASK_PRIO          1
#endif

// #define CONFIG_ALICLOUD_SUPPORT
#ifdef CONFIG_ALICLOUD_SUPPORT
#define LIVING_STACK_SIZE           1024
#define LIVING_TASK_PRIO            2//3
/*
    MBEDTLS_SSL_MAX_CONTENT_LEN should be set to 16384
        in ROM-EXPORT/mbedtls-2.17.0-rom/include/mbedtls/config.h
     configTIMER_TASK_STACK_DEPTH should be set to 512(in words)
        in FreeRTOSConfig.h using FreeRTOS os
    TX_TIMER_THREAD_STACK_SIZE shoud be set to 2048(in bytes)
        in tx_port.h using ThreadX os
 */
#endif

//#define CONFIG_IPERF_TEST
//#define CONFIG_IPERF3_TEST

#ifdef CONFIG_IPERF3_TEST
#define IPERF_TASK_MAX              2
#endif

#define CONFIG_BASECMD
// #define CONFIG_ATCMD
// #define CONFIG_INTERNAL_DEBUG

#define CONFIG_MQTT
// #define CONFIG_COAP

// #define CONFIG_FATFS_SUPPORT

#define CONFIG_FAST_RECONNECT

// #define CONFIG_SSL_TEST

// #define CONFIG_LWIP_SOCKETS_TEST

// #define CONFIG_IPV6_SUPPORT

//#define CONFIG_AZURE_F527_DEMO_SUPPORT

#ifdef CONFIG_AZURE_F527_DEMO_SUPPORT
#ifndef CONFIG_AZURE_IOT_SUPPORT
#error "CONFIG_AZURE_IOT_SUPPORT must be defined"
#endif

#if (CONFIG_BOARD != PLATFORM_BOARD_32VW55X_F527)
#error "PLATFORM_BOARD_32VW55X_F527 must be selected"
#endif

#undef CONFIG_ATCMD
// #define CONFIG_ATCMD
#endif /* CONFIG_AZURE_F527_DEMO_SUPPORT */

#ifdef CONFIG_AZURE_IOT_SUPPORT
#define CONFIG_NEED_RSA_4096
#endif

#ifdef CFG_MATTER
    #undef CONFIG_BASECMD
    #undef CONFIG_ATCMD
    #ifndef CONFIG_IPV6_SUPPORT
        #define CONFIG_IPV6_SUPPORT
    #endif
#endif

#ifdef CFG_BLE_SUPPORT
/* If configured, BLE will be initialized when boot and stay enabled, otherwise BLE will not be
   initialized and will be enabled when ble courier is enabled by command and be disabled after
   ble courier wifi is disabled by command.
 */
#define CONFIG_BLE_ALWAYS_ENABLE
#define FEAT_SUPPORT_BLE_DATATRANS        0
#endif

#include "rftest_cfg.h"

#endif  /* _APP_CFG_H_ */
