/*!
    \file    atcmd.h
    \brief   AT command TCPIP part for GD32VW55x SDK

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

#ifndef _ATCMD_H_
#define _ATCMD_H_

#include "app_cfg.h"

#ifndef CONFIG_AZURE_F527_DEMO_SUPPORT
#define AT_UART_BAUDRATE            115200 //3000000
#else
#define AT_UART_BAUDRATE            2000000//115200//2000000
#endif

#define CONFIG_ATCMD_DEBUG
#ifdef CONFIG_ATCMD_DEBUG
#define AT_TRACE                      app_print
//#define AT_TRACE(fmt, ...)          at_print(fmt, ## __VA_ARGS__)
#else
#define AT_TRACE(fmt, ...)          do{}while(0)
#endif

#define AT_RSP_START(len)                                       \
    char *rsp_buf = NULL;                                       \
    int rsp_buf_idx = 0;                                        \
    int rsp_buf_len = len;                                      \
    DEBUG_ASSERT(len != 0);                                     \
    rsp_buf = sys_zalloc(len);                                  \
    DEBUG_ASSERT(rsp_buf != NULL);                              \

#define AT_RSP(fmt, ...)                                        \
        rsp_buf_idx += co_snprintf((rsp_buf + rsp_buf_idx), (rsp_buf_len - rsp_buf_idx), fmt, ##__VA_ARGS__); \

#define AT_RSP_DIRECT(buf, len)                                 \
        at_uart_send(buf, len)

#define AT_RSP_OK()                                             \
    do {                                                        \
        AT_RSP("OK\r\n");                                       \
        at_uart_send(rsp_buf, rsp_buf_idx);                     \
        sys_mfree(rsp_buf);                                     \
    } while(0)

#define AT_RSP_ERR()                                            \
    do {                                                        \
        AT_RSP("ERROR\r\n");                                    \
        at_uart_send(rsp_buf, rsp_buf_idx);                     \
        sys_mfree(rsp_buf);                                     \
    } while(0)

#define AT_TRACE_DATA(title, data, len)                         \
    do {                                                        \
        int i;                                                  \
        AT_TRACE("======== %s: %u ========", title, len);       \
        for (i = 0; i < len; i++) {                              \
            if (i % 16 == 0) {                                  \
                AT_TRACE("\r\n\t");                             \
            }                                                   \
            AT_TRACE("%02x ", *(data + i));                     \
        }                                                       \
        AT_TRACE("\r\n");                                       \
    } while(0)
#ifndef CONFIG_AZURE_F527_DEMO_SUPPORT
#define AT_UART_RX_BUF_SIZE     128
#else
#define AT_UART_RX_BUF_SIZE     5000
#endif
#define AT_MAX_ARGC             8

#define AT_MAX_STATION_NUM      CFG_STA_NUM
#define AT_ETH_ALEN             3

#define AT_SEPARATE             ','
#define AT_SPACE                ' '
#define AT_EQU                  '='
#define AT_QUESTION             '?'
#define AT_QUOTE                '"'
#define AT_CR                   0x0d     /* '\r' */
#define AT_LF                   0x0a     /* '\n' */
#define AT_COLON                ':'

struct atcmd_entry {
    // name of the command
    char *name;
    // process function
    void (*exec) (int, char **);
};

int atcmd_init(void);
void atcmd_deinit(void);
int at_print(const char *format, ...);
void at_uart_rx_dma_irq_hdl(uint32_t dma_channel);

#endif // _ATCMD_H_
