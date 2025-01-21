/*!
    \file    atcmd.c
    \brief   AT command for GD32VW55x SDK

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

#include "version.h"
#include "_build_date.h"
#include "app_cfg.h"
#include "wlan_config.h"
#include "build_config.h"
#include "gd32vw55x.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "wifi_vif.h"
#include "wifi_net_ip.h"
#include "wifi_management.h"
#include "wifi_export.h"
#include "wifi_init.h"
#include "cmd_shell.h"
#include "dbg_print.h"
#include "uart.h"
#include "uart_config.h"
#include "ping.h"
#include "net_iperf.h"
#include "log_uart.h"
#include "wakelock.h"
#include "atcmd.h"
#include "atcmd_wifi.h"
#include "atcmd_tcpip.h"
#ifdef CONFIG_AZURE_IOT_SUPPORT
#include "atcmd_azure.h"
#endif

#ifdef CONFIG_ATCMD
uart_config_t at_uart_conf = {
    .usart_periph = AT_UART,
    .baudrate = AT_UART_BAUDRATE,
    .databits = USART_WL_8BIT,
    .stopbits = USART_STB_1BIT,
    .parity = USART_PM_NONE,
    .flow_ctrl = (USART_CTS_DISABLE | USART_RTS_DISABLE)
};

static char at_uart_rx_buf[AT_UART_RX_BUF_SIZE];
static volatile uint16_t at_uart_rx_buf_idx = 0;
static os_sema_t at_uart_tx_sema = NULL;
static os_sema_t at_uart_dma_sem = NULL;
static volatile uint8_t at_cmd_received = 0;
static uint8_t at_task_exit = 0;
static const struct atcmd_entry atcmd_table[];

static void at_uart_init(void);
static void at_uart_send(char *data, int size);
static void at_uart_dma_receive(uint32_t address, uint32_t num);
static void at_uart_dma_receive_config(void);
static void at_uart_irq_receive_config(void);
static void at_uart_dma_receive_start(uint32_t address, uint32_t num);
static void at_uart_dma_receive_stop(void);
static bool at_uart_rx_is_ongoing(void);

/*!
    \brief      convert command to serial port configuration parameters
    \param[in]  cmd: serial port configuration parameters included in the command
    \param[out] gd: serial port configuration parameters
    \retval     none
*/
static void uart_param_at2gd(uart_config_t *cmd, uart_config_t *gd)
{
    gd->baudrate = cmd->baudrate;
    if (cmd->databits == 8)
        gd->databits = USART_WL_8BIT;
    else
        gd->databits = USART_WL_9BIT;

    if (cmd->stopbits == 0)
        gd->stopbits = USART_STB_0_5BIT;
    else if (cmd->stopbits == 1)
        gd->stopbits = USART_STB_1BIT;
    else if (cmd->stopbits == 2)
        gd->stopbits = USART_STB_1_5BIT;
    else
        gd->stopbits = USART_STB_2BIT;

    if (cmd->parity == 0)
        gd->parity = USART_PM_NONE;
    else if (cmd->parity == 1)
        gd->parity = USART_PM_ODD;
    else
        gd->parity = USART_PM_EVEN;

    if (cmd->flow_ctrl == 0)
        gd->flow_ctrl = 0;
    else if (cmd->flow_ctrl == 1)
        gd->flow_ctrl = USART_RTS_ENABLE;
    else if (cmd->flow_ctrl == 2)
        gd->flow_ctrl = USART_CTS_ENABLE;
    else
        gd->flow_ctrl = (USART_CTS_ENABLE | USART_RTS_ENABLE);
}

/*!
    \brief      convert serial port configuration parameters to command
    \param[in]  gd: serial port configuration parameters
    \param[out] cmd: serial port configuration parameters included in the command
    \retval     none
*/
static void uart_param_gd2at(uart_config_t *gd, uart_config_t *cmd)
{
    cmd->baudrate = gd->baudrate;
    if (gd->databits == USART_WL_8BIT)
        cmd->databits = 8;
    else
        cmd->databits = 9;
    if (gd->stopbits == USART_STB_0_5BIT)
        cmd->stopbits = 0;
    else if (gd->stopbits == USART_STB_1BIT)
        cmd->stopbits = 1;
    else if (gd->stopbits == USART_STB_1_5BIT)
        cmd->stopbits = 2;
    else
        cmd->stopbits = 3;
    if (gd->parity == USART_PM_NONE)
        cmd->parity = 0;
    else if (gd->parity == USART_PM_ODD)
        cmd->parity = 1;
    else
        cmd->parity = 2;
    cmd->flow_ctrl = 0;
    if ((gd->flow_ctrl & USART_RTS_ENABLE) == USART_RTS_ENABLE)
        cmd->flow_ctrl |= BIT(0);
    if ((gd->flow_ctrl & USART_CTS_ENABLE) == USART_CTS_ENABLE)
        cmd->flow_ctrl |= BIT(1);
}

int at_print(const char *format, ...)
{
    int ret;
    va_list args;

    va_start(args, format);
    ret = print((void *)at_uart_conf.usart_periph, format, args, 0);
    va_end(args);

    return ret;
}

/*!
    \brief      parse string
    \param[in]  param: pointer to a string waiting to be parsed
    \param[out] none
    \retval     none
*/
char *at_string_parse(char *param)
{
    char *find = strchr(param, AT_QUOTE);
    char *head = NULL, *ptr, *tail = NULL;

    if (find == NULL)
        return NULL;
    head = ptr = ++find;
    while (1) {
        find = strchr(ptr, AT_QUOTE);
        if (find == NULL)
            break;
        else
            tail = find;
        ptr = find + 1;
    }
    if (tail != NULL)
        *tail = '\0';
    else
        return NULL;
    if (*head == '\0')
        head = NULL;
    return head;
}

/*!
    \brief      enter AT command mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_entry(int argc, char **argv)
{
    //cmd_mode_type_set(CMD_MODE_TYPE_AT);

    cip_info_init();

    AT_RSP_DIRECT("OK\r\n", 4);
}

/*!
    \brief      exit AT command mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_exit(int argc, char **argv)
{
    cip_info_reset();

    //cmd_mode_type_set(CMD_MODE_TYPE_NORMAL);

    AT_RSP_DIRECT("OK\r\n", 4);
}

/*!
    \brief      the AT command reset system
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_reset(int argc, char **argv)
{
    AT_RSP_DIRECT("OK\r\n", 4);
    uart_tx_idle_wait(at_uart_conf.usart_periph);
    rcu_deinit();
    SysTimer_SoftwareReset();
}

/*!
    \brief      the AT command show version of the software
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_version(int argc, char **argv)
{
    AT_RSP_START(256);
    AT_RSP("==================================\r\n");
    AT_RSP("SDK revsion: v%d.%d.%d\r\n", WIFI_VERSION_MAJ, WIFI_VERSION_MIN, WIFI_VERSION_REL);
#ifdef WIFI_GIT_REVISION
    AT_RSP("SDK git reversion: %s\r\n", WIFI_GIT_REVISION);
#endif
    AT_RSP("SDK build date: %s\r\n", SDK_BUILD_DATE);
    AT_RSP_OK();
}

/*!
    \brief      the AT command show all of the tasks
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_task(int argc, char **argv)
{
    AT_RSP_START(1024);
    sys_task_list(rsp_buf);
    rsp_buf_idx += strlen(rsp_buf);
    AT_RSP_OK();
}

/*!
    \brief      the AT command show heap usage
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_heap(int argc, char **argv)
{
    uint32_t free_sram = sys_free_heap_size();

    AT_RSP_START(256);
    AT_RSP("==================================\r\n");
    AT_RSP("Total free heap size = %d\r\n", free_sram);
    AT_RSP("Total min free heap size = %d\r\n", sys_min_free_heap_size());
    AT_RSP_OK();
}

/*!
    \brief      the AT command show system ram usage
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_sysram(int argc, char **argv)
{
    uint32_t free_sram = sys_free_heap_size();

    AT_RSP_START(256);
    AT_RSP("==================================\r\n");
    AT_RSP("Free SRAM size = %u\r\n", free_sram);
    AT_RSP_OK();
}

/*!
    \brief      the AT command show the configuration of log usart or conifgurate log usart
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_uart(int argc, char **argv)
{
    uart_config_t cmd;
    AT_RSP_START(256);
    if (argc == 1) {
        if (argv[0][strlen(argv[0]) - 1] == AT_QUESTION) {
            uart_param_gd2at(&at_uart_conf, &cmd);
            AT_RSP("+UART: %d, %d, %d, %d, %d\r\n",
                cmd.baudrate, cmd.databits, cmd.stopbits, cmd.parity, cmd.flow_ctrl);
        } else {
            goto Error;
        }
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION)
            goto Usage;
        else
            goto Error;
    } else if (argc == 6) {
        char *endptr = NULL;
        cmd.baudrate = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.databits = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.stopbits = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.parity = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        cmd.flow_ctrl = (uint32_t)strtoul((const char *)argv[5], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        if ((cmd.databits != 8 && cmd.databits != 9)
            || (cmd.stopbits > 3) || (cmd.parity > 2) || (cmd.flow_ctrl > 3))
            goto Error;

        uart_param_at2gd(&cmd, &at_uart_conf);
        at_uart_init();
    } else {
        goto Error;
    }

    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;

Usage:
    AT_RSP("+UART=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command show AT command list
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void at_help(int argc, char **argv)
{
    int i;
    AT_RSP_START(1024);
    AT_RSP("\rAT COMMAND LIST:\r\n");
    AT_RSP("==============================\r\n");

    for (i = 0; atcmd_table[i].exec != NULL; i++)
    {
        AT_RSP("    %s\r\n", atcmd_table[i].name);
    }
    AT_RSP("\r\n");
    AT_RSP_OK();
}

#include "atcmd_tcpip.c"
#include "atcmd_wifi.c"
#ifdef CONFIG_AZURE_F527_DEMO_SUPPORT
#include "atcmd_azure.c"
#endif

static const struct atcmd_entry atcmd_table[] = {
    /* ====== Base ====== */
    {"AT", at_entry},
    {"ATQ", at_exit},
    {"AT+HELP", at_help},
    {"AT+RST", at_reset},
    {"AT+GMR", at_version},
    {"AT+TASK", at_task},
    {"AT+HEAP", at_heap},
    {"AT+SYSRAM", at_sysram},
    {"AT+UART", at_uart},
    /* ====== WLAN ====== */
#ifdef CFG_WLAN_SUPPORT
    {"AT+CWMODE_CUR", at_cw_mode_cur},
    {"AT+CWJAP_CUR", at_cw_ap_cur_join},
    {"AT+CWLAP", at_cw_ap_list},
    {"AT+CWSTATUS", at_cw_status},
    {"AT+CWQAP", at_cw_ap_quit},
    {"AT+CWSAP_CUR", at_cw_ap_cur_start},
    {"AT+CWLIF", at_cw_ap_client_list},
    {"AT+CWAUTOCONN", at_cw_auto_connect},
#endif
    /* ====== TCPIP ====== */
    {"AT+PING", at_cip_ping},
    {"AT+CIPSTA", at_cip_sta_ip},
    {"AT+CIPSTART", at_cip_start},
    {"AT+CIPSEND", at_cip_send},
    {"AT+CIPSERVER", at_cip_server},
    {"AT+CIPCLOSE", at_cip_close},
    {"AT+CIPSTATUS", at_cip_status},
    {"AT+CIPMODE", at_cip_mode},
    {"AT+TRANSINTVL", at_trans_interval},
    {"AT+CIFSR", at_cip_ip_addr_get},
#ifdef CONFIG_AZURE_F527_DEMO_SUPPORT
    /* ====== Azure Cloud ====== */
    {"AT+AZCWJAP", at_azure_wifi_connect},
    {"AT+AZCOMC", at_azure_component_create},
    {"AT+AZCERT", at_azure_cert},
    {"AT+AZSYMKEY", at_azure_symkey},
    {"AT+AZEPT", at_azure_endpoint},
    {"AT+AZIDSP", at_azure_idsp},
    {"AT+AZDEVREGID", at_azure_regid},
    {"AT+AZPORT", at_azure_port},
    {"AT+AZPNPMODID", at_azure_pnp_modid},
    {"AT+AZDEVID", at_azure_devid},
    {"AT+AZHOSTNM", at_azure_host_name},
    {"AT+AZADUMANUF", at_azure_adu_manufacturer},
    {"AT+AZADUMOD", at_azure_adu_model},
    {"AT+AZADUPROV", at_azure_adu_provider},
    {"AT+AZADUPNM", at_azure_adu_updatename},
    {"AT+AZADUPVER", at_azure_adu_updatever},
    {"AT+AZCONN", at_azure_connect},
    {"AT+AZDISC", at_azure_disconnect},
    {"AT+AZTELS", at_azure_telemetry_update},
    {"AT+AZPROPS", at_azure_property_update},
    {"AT+AZPROPRSP", at_azure_property_rsp},
    {"AT+AZCMDRSP", at_azure_cmd_rsp},
    {"AT+AZSTAT", at_azure_state_get},
    //{"AT+AZOTARSP", at_azure_ota_rsp},
    {"AT+AZDEVUPT", at_azure_dev_update},  //directly update the value of slave device to azure
#endif
    {"", NULL}
};
const uint32_t AT_CMD_TABLE_SZ = (sizeof(atcmd_table) / sizeof(atcmd_table[0]));

static void at_uart_rx_irq_hdl(uint32_t usart_periph)
{
    char ch;

    usart_interrupt_disable(usart_periph, USART_INT_RBNE);
    while (1) {
        // We should have chance to check overflow error
        // Otherwise it may cause dead loop handle rx interrupt
        if (RESET != usart_flag_get(usart_periph, USART_FLAG_ORERR)) {
            usart_flag_clear(usart_periph, USART_FLAG_ORERR);
        }

        if ((RESET != usart_flag_get(usart_periph, USART_FLAG_RBNE))) {
            ch = (char)usart_data_receive(usart_periph);
        } else {
            break;
        }

        if (ch == '\0') {
            break;
        }

        if (isprint(ch)) {
            at_uart_rx_buf[at_uart_rx_buf_idx++] = ch;
            if (at_uart_rx_buf_idx >= AT_UART_RX_BUF_SIZE) {
                at_uart_rx_buf_idx = 0;
            }
            //AT_TRACE("%c", ch);
        } else if (ch == '\r') { /* putty doesn't transmit '\n' */
            at_uart_rx_buf[at_uart_rx_buf_idx] = '\0';
            //AT_TRACE("\r\n");
            if (at_uart_rx_buf_idx > 0) {
                at_cmd_received = 1;
            }
            sys_wakelock_release(LOCK_ID_USART);
        } else if (ch == '\b') { /* non-destructive backspace */
            if (at_uart_rx_buf_idx > 0) {
                at_uart_rx_buf[--at_uart_rx_buf_idx] = '\0';
            }
        }
    }

    usart_interrupt_enable(usart_periph, USART_INT_RBNE);
}

static void at_uart_init(void)
{
    memset(at_uart_rx_buf, 0, AT_UART_RX_BUF_SIZE);
    at_uart_rx_buf_idx = 0;

    if (at_uart_conf.usart_periph == USART0) {
        eclic_irq_enable(USART0_IRQn, 0xb, 0);
    } else if (at_uart_conf.usart_periph == UART1) {
        eclic_irq_enable(UART1_IRQn, 0xb, 0);
    } else if (at_uart_conf.usart_periph == UART2) {
        eclic_irq_enable(UART2_IRQn, 0xb, 0);
    }
    uart_config(at_uart_conf.usart_periph, at_uart_conf.baudrate, false, false, false);
    uart_irq_callback_register(at_uart_conf.usart_periph, at_uart_rx_irq_hdl);
}

static void at_uart_deinit(void)
{
    uart_irq_callback_unregister(at_uart_conf.usart_periph);
    usart_interrupt_disable(at_uart_conf.usart_periph, USART_INT_RBNE);
    usart_deinit(at_uart_conf.usart_periph);
    memset(at_uart_rx_buf, 0, AT_UART_RX_BUF_SIZE);
    at_uart_rx_buf_idx = 0;
}

static void at_uart_send(char *data, int size)
{
    if (size == 0) {
        return;
    }

    sys_sema_down(&at_uart_tx_sema, 0);
    while (size > 0) {
        uart_putc_noint(at_uart_conf.usart_periph, *data++);
        size--;
    }
    sys_sema_up(&at_uart_tx_sema);
}

static bool at_uart_rx_is_ongoing(void)
{
    return (at_uart_rx_buf_idx > 0) ? true : false;
}

void at_uart_rx_dma_irq_hdl(uint32_t dma_channel)
{
    if(RESET != dma_interrupt_flag_get(dma_channel, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_FTF);
        sys_sema_up_from_isr(&at_uart_dma_sem);
    }
}

static void at_uart_dma_receive(uint32_t address, uint32_t num)
{
    uint32_t dma_channel;

    uart_tx_idle_wait(at_uart_conf.usart_periph);

    uart_config(at_uart_conf.usart_periph, at_uart_conf.baudrate, false, true, false);

    switch (at_uart_conf.usart_periph) {
    case USART0:
        eclic_irq_enable(DMA_Channel2_IRQn, 8, 0);
        dma_channel = DMA_CH2;
        break;
    case UART1:
        eclic_irq_enable(DMA_Channel0_IRQn, 8, 0);
        dma_channel = DMA_CH0;
        break;
    case UART2:
    default:
        eclic_irq_enable(DMA_Channel5_IRQn, 8, 0);
        dma_channel = DMA_CH5;
        break;
    }

    dma_memory_address_config(dma_channel, DMA_MEMORY_0, address);
    dma_transfer_number_config(dma_channel, num);
    dma_channel_enable(dma_channel);

    sys_sema_down(&at_uart_dma_sem, 0);

    usart_dma_receive_config(at_uart_conf.usart_periph, USART_RECEIVE_DMA_DISABLE);

    uart_config(at_uart_conf.usart_periph, at_uart_conf.baudrate, false, false, false);
}

static void at_uart_dma_receive_config(void)
{
    uart_tx_idle_wait(at_uart_conf.usart_periph);

    uart_config(at_uart_conf.usart_periph, at_uart_conf.baudrate, false, true, false);

    switch (at_uart_conf.usart_periph) {
    case USART0:
        eclic_irq_enable(DMA_Channel2_IRQn, 8, 0);
        break;
    case UART1:
        eclic_irq_enable(DMA_Channel0_IRQn, 8, 0);
        break;
    case UART2:
    default:
        eclic_irq_enable(DMA_Channel5_IRQn, 8, 0);
        break;
    }

}

static void at_uart_irq_receive_config(void)
{
    uint32_t dma_channel;

    switch (at_uart_conf.usart_periph) {
    case USART0:
        eclic_irq_disable(DMA_Channel2_IRQn);
        dma_channel = DMA_CH2;
        break;
    case UART1:
        eclic_irq_disable(DMA_Channel0_IRQn);
        dma_channel = DMA_CH0;
        break;
    case UART2:
    default:
        eclic_irq_disable(DMA_Channel5_IRQn);
        dma_channel = DMA_CH5;
        break;
    }

    usart_dma_receive_config(at_uart_conf.usart_periph, USART_RECEIVE_DMA_DISABLE);
    uart_config(at_uart_conf.usart_periph, at_uart_conf.baudrate, false, false, false);
}

static void at_uart_dma_receive_start(uint32_t address, uint32_t num)
{
    uint32_t dma_channel;

    uart_dma_single_mode_config(at_uart_conf.usart_periph, DMA_PERIPH_TO_MEMORY);

    switch (at_uart_conf.usart_periph) {
    case USART0:
        dma_channel = DMA_CH2;
        break;
    case UART1:
        dma_channel = DMA_CH0;
        break;
    case UART2:
    default:
        dma_channel = DMA_CH5;
        break;
    }

    dma_memory_address_config(dma_channel, DMA_MEMORY_0, address);
    dma_transfer_number_config(dma_channel, num);
    dma_channel_enable(dma_channel);
}

static void at_uart_dma_receive_stop(void)
{
    uint32_t dma_channel;

    switch (at_uart_conf.usart_periph) {
    case USART0:
        dma_channel = DMA_CH2;
        break;
    case UART1:
        dma_channel = DMA_CH0;
        break;
    case UART2:
    default:
        dma_channel = DMA_CH5;
        break;
    }

    dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_FTF);
    dma_interrupt_disable(dma_channel, DMA_INT_FTF);
    dma_channel_disable(dma_channel);
}

static char *get_next_param(char **params)
{
    char *ptr = *params, *next;
    char sep = AT_SEPARATE;

    if (!ptr)
        return NULL;

    if (ptr[0] == '\'') {
        sep = ptr[0];
        ptr++;
    }

    next = strchr(ptr, sep);
    if (next) {
        *next++ = '\0';
        while (*next == AT_SPACE || *next == AT_SEPARATE)
            next++;
        if (*next == '\0')
            next = NULL;
    }
    *params = next;
    return ptr;
}

/*!
    \brief      parse AT command
    \param[in]  atcmd: the pointer of AT command
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static int atcmd_parse(char *atcmd, char **argv)
{
    int32_t i;
    int32_t argc = 0;
    char *param;
    char *buf;
    uint32_t size;

    if (atcmd == NULL) {
        return -1;
    }

    buf = atcmd;
    size = strlen(atcmd);

    /* Get command */
    argv[argc] = buf;
    for (i = 0; i < size; i++) {
        if (*buf == AT_EQU) {
            break;
        } else if ((*buf == AT_CR) || (*buf == AT_LF)) {
            goto Exit;
        } else {
            buf++;
        }
    }
    if (i == size)
        goto Exit;

    /* Get parameters */
    *buf++ = '\0';
    argc++;

    param = get_next_param(&buf);
    while (param && (argc < AT_MAX_ARGC)) {
        argv[argc] = param;
        argc++;
        param = get_next_param(&buf);
    }

    for (i = 0; i < argc; i++) {
        AT_TRACE("argv[%d] = %s\r\n", i, argv[i]);
    }
    return argc;

Exit:
    *buf = '\0';
    argc++;
    return argc;
}

static void atcmd_task(void *param)
{
    int argc = 0, i;
    char *argv[AT_MAX_ARGC];

#ifdef CFG_WLAN_SUPPORT
    wifi_wait_ready();
#endif

    for (;;) {
        while (at_cmd_received == 0) {
            sys_ms_sleep(10);
        }

        if (at_task_exit == 1)
            break;

        if (at_uart_rx_buf[0] == '\0') {
            goto cont;
        } else {
            argc = atcmd_parse(at_uart_rx_buf, argv);
            if (argc == 0)
                goto cont;
        }
        for (i = 0; i < AT_CMD_TABLE_SZ - 1; i++) {
            if ((strcmp(argv[0], atcmd_table[i].name) == 0) ||
                (strncmp(argv[0], atcmd_table[i].name, strlen(atcmd_table[i].name)) == 0
                && argv[0][strlen(argv[0]) - 1] == AT_QUESTION
                && (strlen(argv[0]) == strlen(atcmd_table[i].name) + 1))) {
                if (atcmd_table[i].exec) {
                    atcmd_table[i].exec(argc, argv);
                }
                break;
            }
        }

cont:
        AT_TRACE("# ");
        at_uart_rx_buf[0] = '\0';
        at_uart_rx_buf_idx = 0;
        at_cmd_received = 0;
    }

    sys_task_delete(NULL);
}

int atcmd_init(void)
{
    int ret = 0;

    if (LOG_UART == AT_UART) {
        AT_TRACE("AT_UART and LOG_UART select the same uart. Please seperate them.\r\n");
        ret = -1;
        goto Exit;
    }

    at_cmd_received = 0;

    at_uart_init();
    cip_info_init();

    if (sys_sema_init(&at_uart_dma_sem, 0)) {
        ret = -2;
        goto Exit;
    }

    if (sys_sema_init(&at_uart_tx_sema, 1)) {
        ret = -3;
        goto Exit;
    }

    if (sys_task_create_dynamic((const uint8_t *)"ATCMD",
            ATCMD_STACK_SIZE, ATCMD_PRIORITY, atcmd_task, NULL) == NULL) {
        ret = -4;
    }

Exit:
    if (ret) {
        AT_TRACE("%s: return %d\r\n", __func__, ret);

        if (at_uart_dma_sem) {
            sys_sema_free(&at_uart_dma_sem);
            at_uart_dma_sem = NULL;
        }

        if (at_uart_tx_sema) {
            sys_sema_free(&at_uart_tx_sema);
            at_uart_tx_sema = NULL;
        }
    }
    return ret;
}

void atcmd_deinit(void)
{
    at_task_exit = 1;

    sys_ms_sleep(100);  //wait atcmd task finished and idle task released the task tcb

    at_uart_deinit();

    cip_info_reset();

    if (at_uart_dma_sem) {
        sys_sema_free(&at_uart_dma_sem);
        at_uart_dma_sem = NULL;
    }

    if (at_uart_tx_sema) {
        sys_sema_free(&at_uart_tx_sema);
        at_uart_tx_sema = NULL;
    }
}

#endif

#if 0
static void atcmd_common_help_cb(void)
{
    AT_RSP("\t%s\n", atcmd_table[0].name);
    return;
}

static uint8_t atcmd_common_handle(void *data, void **cmd)
{
    const struct atcmd_entry *w_cmd = atcmd_table;
    char *data_tmp = (char *)data;

    if (cmd_mode_type_get() != CMD_MODE_TYPE_AT && strcmp(data_tmp, "AT"))
    {
        return CLI_UNKWN_CMD;
    }

    while (w_cmd->exec) {
        if (!strcmp(data_tmp, w_cmd->name) ||
            (!strncmp(data_tmp, w_cmd->name, strlen(w_cmd->name)) && data_tmp[strlen(w_cmd->name)] == AT_QUESTION))
        {
            *cmd = w_cmd->exec;
            break;
        }
        w_cmd++;
    }
    if (w_cmd->exec == NULL) {
        at_rsp_error();
        return CLI_UNKWN_CMD;
    }
    return CLI_SUCCESS;
}

/*!
    \brief      parse AT command
    \param[in]  atcmd: the pointer of AT command
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static int atcmd_parse(char *atcmd, char **argv)
{
    int32_t i;
    int32_t argc = 0;

    if (atcmd == NULL) {
        return -1;
    }
    char *buf = atcmd;
    uint32_t size = strlen(atcmd);

    /* Get command */
    argv[argc] = buf;
    for (i = 0; i < size; i++) {
        if (*buf == AT_EQU) {
            break;
        } else if ((*buf == AT_CR) || (*buf == AT_LF)) {
            goto Exit;
        } else {
            buf++;
        }
    }
    if (i == size)
        goto Exit;

    /* Get parameters */
    *buf++ = '\0';
    argc++;
    while((argc < AT_MAX_ARGC) && (*buf != '\0')) {
        argv[argc] = buf;
        argc++;
        buf++;

        while ((*buf != AT_SEPARATE) && (*buf != '\0')) {
            buf++;
        }
        if (*buf != '\0') {
            *buf = '\0';
            buf ++;
        }
        while (*buf == AT_SPACE) {
            *buf = '\0';
            buf ++;
        }
    }

    return argc;

Exit:
    *buf = '\0';
    argc++;
    return argc;
}

int atcmd_init(void)
{
    return cmd_module_reg(CMD_MODULE_ATCMD, NULL, atcmd_common_handle, atcmd_common_help_cb, atcmd_parse);
}
#endif
