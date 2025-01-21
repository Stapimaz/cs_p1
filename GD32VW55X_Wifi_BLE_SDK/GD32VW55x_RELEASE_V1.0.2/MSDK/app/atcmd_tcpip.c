/*!
    \file    atcmd_tcpip.c
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

static cip_info_t cip_info;
static int cip_task_started = 0;
static int cip_task_terminate = 0;

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

typedef struct _passth_tx_buf {
    char *buf;
    uint32_t size;
    uint32_t writeptr;
    uint32_t readptr;
} passth_tx_buf_t;

typedef struct _cip_passth_info {
    int passth_fd_idx;
    passth_tx_buf_t passth_buf;

    os_timer_t passth_timer;
    volatile uint8_t at_tx_passth_timeout;
    volatile uint8_t terminate_send_passth;
} cip_passth_info_t;

static cip_passth_info_t cip_passth_info;

/*!
    \brief      initialize structure of tcpip information
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cip_info_init(void)
{
    int i;

    cip_task_terminate = 0;

    sys_memset(&cip_info, 0, sizeof(cip_info));
    cip_info.trans_intvl = CIP_TRANSFER_INTERVAL_DEFAULT; //ms
    cip_info.local_srv_fd = -1;
    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        cip_info.cli[i].fd = -1;
    }
    return;
}

/*!
    \brief      allocate storage space for tcpip information
    \param[in]  none
    \param[out] none
    \retval     the location of the array used to store tcpip information
*/
static int cip_info_cli_alloc(void)

{
    int i;

    if (cip_info.cli_num >= MAX_CLIENT_NUM) {
        return -1;
    }
    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd < 0)
            return i;
    }
    return -1;
}

/*!
    \brief      store tcpip information
    \param[in]  fd: the ID of the array used to store tcpip information
    \param[in]  type: the type of the client
    \param[in]  role: the role of the client
    \param[in]  remote_ip: remote ip
    \param[in]  remote_port: remote port
    \param[in]  local_port: local port
    \param[out] none
    \retval     the location of the array used to store tcpip information
*/
static int cip_info_cli_store(int fd, char *type, uint8_t role,
                            uint32_t remote_ip, uint16_t remote_port, uint16_t local_port)
{
    int idx = cip_info_cli_alloc();

    if ((idx < 0) || (fd < 0))
        return -1;
    cip_info.cli[idx].fd = fd;
    if (strncmp(type, "TCP", 3) == 0)
        cip_info.cli[idx].type = CIP_TYPE_TCP;
    else
        cip_info.cli[idx].type = CIP_TYPE_UDP;
    cip_info.cli[idx].role = role;
    cip_info.cli[idx].remote_ip = remote_ip;
    cip_info.cli[idx].remote_port = remote_port;
    cip_info.cli[idx].local_port = local_port;

    cip_info.cli_num++;

    return idx;
}

/*!
    \brief      free the specified array that stores tcpip information
    \param[in]  index: the location of the array used to store tcpip information
    \param[out] none
    \retval     none
*/
static void cip_info_cli_free(int index)
{
    if ((index >= 0) && (index < MAX_CLIENT_NUM)) {
        if (cip_info.cli[index].fd != -1) {
            sys_memset(&cip_info.cli[index], 0, sizeof(cip_info.cli[index]));
            cip_info.cli[index].fd = -1;
            cip_info.cli_num--;
        }
    }
}

/*!
    \brief      find the specified array that stores tcpip information
    \param[in]  fd: the ID of the array used to store tcpip information
    \param[out] none
    \retval     location of the array used to store tcpip information
*/
static int cip_info_cli_find(int fd)

{
    int i;

    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd == fd)
            return i;
    }
    return -1;
}

static int cip_info_valid_fd_cnt_get(void)
{
    int i, cnt = 0;

    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd >= 0)
            cnt++;
    }
    return cnt;
}

static int cip_info_valid_tcp_fd_cnt_get(void)
{
    int i, cnt = 0;

    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd >= 0 && cip_info.cli[i].type == CIP_TYPE_TCP)
            cnt++;
    }
    return cnt;
}


/*!
    \brief      reset tcpip information
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cip_info_reset(void)
{
    int i, fd;

    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd >= 0) {
            fd = cip_info.cli[i].fd;
            cip_info_cli_free(i);
            close(fd);
        }
    }
    if (cip_info.local_srv_fd >= 0) {
        fd = cip_info.local_srv_fd;
        cip_info.local_srv_fd = -1;
        cip_info.local_srv_port = 0;
        close(fd);
    }
    cip_task_terminate = 1;
}


/*!
    \brief      start a tcp client
    \param[in]  srv_ip: server IP address
    \param[in]  srv_port: server port
    \param[in]  bkeep_alive: time of keep alive
    \param[out] none
    \retval     the result of operation
*/
static int tcp_client_start(char *srv_ip, uint16_t srv_port, uint8_t bkeep_alive)
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    uint32_t nodelay = 0, keep_alive = 10; /* Seconds */
    int fd, ret, idx;
    uint32_t srv_ip_int = inet_addr(srv_ip);

    if (cip_info.trans_mode == CIP_TRANS_MODE_PASSTHROUGH &&
        (cip_info_valid_fd_cnt_get() > 0 || cip_info.local_srv_fd >= 0))
        return -1;

    sys_memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(srv_port);
    saddr.sin_addr.s_addr = srv_ip_int;

    /* creating a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        AT_TRACE("Create tcp client socket fd error!\r\n");
        return -1;
    }
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
            (const char *) &nodelay, sizeof( nodelay ) );
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE,
            (const char *) &keep_alive, sizeof( keep_alive ) );

    AT_TRACE("TCP: server IP=%s port=%d.\r\n", srv_ip, srv_port);

    /* connecting to TCP server */
    ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        AT_TRACE("Tcp client connect server error!\r\n");
        ret = -2;
        goto Exit;
    }
    /* Get local port */
    sys_memset(&saddr, 0, sizeof(saddr));
    getsockname(fd, (struct sockaddr *)&saddr, &len);
    /* save client info */
    idx = cip_info_cli_store(fd, "TCP", CIP_ROLE_CLIENT,
                            srv_ip_int, srv_port, ntohs(saddr.sin_port));
    if (idx < 0) {
        AT_TRACE("Client num reached the maximum!\r\n");
        ret = -3;
        goto Exit;
    }
    AT_TRACE("TCP: create socket %d.\r\n", fd);
    cip_passth_info.passth_fd_idx = idx;

    return 0;

Exit:
    close(fd);
    return ret;
}

/*!
    \brief      send tcp packet
    \param[in]  fd: the socket of the tcp client
    \param[in]  tx_len: length of tcp packet to be sent
    \param[out] none
    \retval     the result of operation
*/
static int at_tcp_send(int fd, uint32_t tx_len)
{
    char *tx_buf = NULL;
    int cnt = 0, idx;

    tx_buf = sys_malloc(tx_len);
    if (NULL == tx_buf) {
        AT_TRACE("Allocate client buffer failed (len = %u).\r\n", tx_len);
        return -1;
    }

    AT_RSP_DIRECT(">\r\n", 3);

#if 0
    char ch;
    usart_interrupt_disable(at_uart_conf.usart_periph, USART_INT_RBNE);
    sys_priority_set(sys_current_task_handle_get(), MGMT_TASK_PRIORITY);
    while (1)
    {
        while (RESET == usart_flag_get(at_uart_conf.usart_periph, USART_FLAG_RBNE));
        ch = usart_data_receive(at_uart_conf.usart_periph);
        if ((ch == 0x0a || ch == 0x0d) && (cnt >= tx_len))
            break;
        if (cnt < tx_len) {
            tx_buf[cnt] = ch;
            cnt++;
        } else {
            AT_TRACE("%c", ch);
        }
    }
    sys_priority_set(sys_current_task_handle_get(), CLI_PRIORITY);
    usart_interrupt_enable(at_uart_conf.usart_periph, USART_INT_RBNE);
    //buffer_dump("TX:", (uint8_t *)tx_buf, cnt);
    cnt = send(fd, tx_buf, cnt, 0);
#else

    // Block here to wait dma receive done
    at_uart_dma_receive((uint32_t)tx_buf, tx_len);

    // debug_print_dump_data("TX:", (char *)tx_buf, tx_len);

    cnt = send(fd, tx_buf, tx_len, 0);
#endif
    if (cnt < 0) {
        AT_TRACE("TCP client send data error.\r\n");
        idx = cip_info_cli_find(fd);
        cip_info_cli_free(idx);
        close(fd);
    }

    sys_mfree(tx_buf);
    return cnt;
}

static void cip_passth_tx_buf_deinit()
{
    if (cip_passth_info.passth_buf.buf)
        sys_mfree(cip_passth_info.passth_buf.buf);

    cip_passth_info.passth_buf.buf= NULL;
    cip_passth_info.passth_buf.size = 0;

    cip_passth_info.passth_buf.writeptr = 0;
    cip_passth_info.passth_buf.readptr = 0;
}

static int cip_passth_tx_buf_init()
{
    if (cip_passth_info.passth_buf.buf == NULL) {
        cip_passth_info.passth_buf.buf = (char *)sys_zalloc(PASSTH_TX_BUF_LEN);
        if (cip_passth_info.passth_buf.buf == NULL)
            return -1;
    }

    cip_passth_info.passth_buf.size = PASSTH_TX_BUF_LEN;
    cip_passth_info.passth_buf.writeptr = 0;
    cip_passth_info.passth_buf.readptr = 0;

    return 0;
}

static void cip_passth_info_deinit(void)
{
    if (cip_passth_info.passth_timer) {
        sys_timer_delete(&(cip_passth_info.passth_timer));
    }

    cip_passth_info.terminate_send_passth = 0;
    cip_passth_info.at_tx_passth_timeout = 0;

    cip_passth_tx_buf_deinit();
}

static int cip_passth_info_init(void)
{
    cip_passth_info.terminate_send_passth = 0;
    cip_passth_info.at_tx_passth_timeout = 0;

    if (cip_passth_tx_buf_init() < 0) {
        goto fail;
    }

    return 0;

fail:
    cip_passth_info_deinit();
    return -1;
}

static int at_passth_send_data(int fd, uint8_t flush, uint8_t type)
{
    passth_tx_buf_t *passth_tx_buf = &(cip_passth_info.passth_buf);
    char *start_addr = passth_tx_buf->buf + passth_tx_buf->readptr;
    int ret = 0, sent_cnt = 0, idx = 0;
    int retry_cnt = 0;
    struct sockaddr_in saddr;
    int remaining_cnt = passth_tx_buf->writeptr - passth_tx_buf->readptr;

    if (fd < 0 || ((type != CIP_TYPE_TCP) && (type != CIP_TYPE_UDP)))
        return -1;

    if (remaining_cnt == 0)
        return 0;

    if (type == CIP_TYPE_UDP) {
        idx = cip_info_cli_find(fd);
        if (idx == -1)
            return -1;
        sys_memset(&saddr, 0, sizeof(struct sockaddr_in));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(cip_info.cli[idx].remote_port);
        saddr.sin_addr.s_addr = cip_info.cli[idx].remote_ip;
    }

    if ((remaining_cnt == strlen(PASSTH_TERMINATE_STR) &&
            strncmp(passth_tx_buf->buf, PASSTH_TERMINATE_STR, strlen(PASSTH_TERMINATE_STR)) == 0)
        //|| (remaining_cnt == 5 && strncmp(passth_tx_buf->buf, PASSTH_TERMINATE_STR"\r\n", 5) == 0)
        ) {
        cip_passth_info.terminate_send_passth = 1;
        return 0;
    }

    while (remaining_cnt > 0) {
        if (remaining_cnt >= PASSTH_START_TRANSFER_LEN) {
            sent_cnt = PASSTH_START_TRANSFER_LEN;
        } else {
            if (flush == 1)
                sent_cnt = remaining_cnt;
            else
                return 0;
        }

Retry:
        if (type == CIP_TYPE_TCP)
            ret = send(fd, (void *)start_addr, sent_cnt, 0);
        else
            ret = sendto(fd, (void *)start_addr, sent_cnt, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

        if (ret <= 0) {
            if (errno == EAGAIN || errno == ENOMEM) {
                if (retry_cnt > 0) {
                    retry_cnt--;
                    goto Retry;
                }
            }
            AT_TRACE("send error:%d\r\n", errno);
            goto exit;
        }
        passth_tx_buf->readptr += sent_cnt;
        // AT_TRACE("Sendout: %d OK,w:%d,r:%d,s:%d,f=%d,t=%d\r\n", ret, passth_tx_buf->writeptr, passth_tx_buf->readptr, sent_cnt, flush, cip_passth_info.at_tx_passth_timeout);
        start_addr += sent_cnt;
        remaining_cnt = remaining_cnt - sent_cnt;
    }

    return 0;
exit:
    cip_passth_info.terminate_send_passth = 1;
    cip_passth_info.passth_fd_idx = -1;
    return -1;
}

static uint32_t at_dma_get_cur_received_num(void)
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

    return (cip_passth_info.passth_buf.size - dma_transfer_number_get(dma_channel));
}

static void at_tx_passth_timeout_cb( void *ptmr, void *p_arg )
{
    cip_passth_info.at_tx_passth_timeout = 1;
}

static int at_passth_send(int fd, uint8_t type)
{
    char *tx_buf = NULL;
    passth_tx_buf_t *passth_tx_buf;
    int passth_timeout = 0, ret;
    uint32_t last_cnt = 0, cur_cnt = 0;

    if (cip_passth_info_init()) {
        AT_RSP_DIRECT("ERROR\r\n", 7);
        return -1;
    }

    passth_tx_buf = &(cip_passth_info.passth_buf);
    AT_RSP_DIRECT(">\r\n", 3);

    if (cip_info.trans_intvl == 0)
        passth_timeout = 1;
    else
        passth_timeout = cip_info.trans_intvl;

    sys_timer_init(&(cip_passth_info.passth_timer), (const uint8_t *)("passth_intvl_timer"),
            passth_timeout, 0, at_tx_passth_timeout_cb, NULL);

    at_uart_dma_receive_config();

Loop:
    tx_buf = passth_tx_buf->buf;
    passth_tx_buf->writeptr = 0;
    passth_tx_buf->readptr = 0;

    last_cnt = 0;
    cur_cnt = 0;
    cip_passth_info.at_tx_passth_timeout = 0;

    at_uart_dma_receive_start((uint32_t)tx_buf, passth_tx_buf->size);
    sys_timer_start(&(cip_passth_info.passth_timer), 0);

    while (cip_passth_info.terminate_send_passth != 1) {
        ret = sys_sema_down(&at_uart_dma_sem, 1); // wait 1ms
        if (ret == OS_OK) {  //8192 received
            // clear timer
            sys_timer_stop(&(cip_passth_info.passth_timer), 0);
            cur_cnt = at_dma_get_cur_received_num();
            passth_tx_buf->writeptr = passth_tx_buf->size;
            //AT_TRACE("MAX, w:%d, r:%d, c=%d\r\n", passth_tx_buf->writeptr, passth_tx_buf->readptr, cur_cnt);
            at_passth_send_data(fd, 1, type);
            goto Loop;
        } else if (ret == OS_TIMEOUT && cip_passth_info.at_tx_passth_timeout != 1) {
            cur_cnt = at_dma_get_cur_received_num();
            passth_tx_buf->writeptr = cur_cnt;
            //AT_TRACE("Wait, w:%d, r:%d, c=%d\r\n", passth_tx_buf->writeptr, passth_tx_buf->readptr, cur_cnt);
            if (passth_tx_buf->writeptr - passth_tx_buf->readptr >= PASSTH_START_TRANSFER_LEN) {
                at_passth_send_data(fd, 0, type);
            }
        }

        if (cip_passth_info.at_tx_passth_timeout == 1) {
            at_uart_dma_receive_stop();

            cur_cnt = at_dma_get_cur_received_num();
            //AT_TRACE("Timeout, w:%d, r:%d, c=%d\r\n", passth_tx_buf->writeptr, passth_tx_buf->readptr, cur_cnt);
            passth_tx_buf->writeptr = cur_cnt;
            if (passth_tx_buf->writeptr > passth_tx_buf->readptr)
                at_passth_send_data(fd, 1, type);
            goto Loop;
        }

        if (cur_cnt != last_cnt) {
            sys_timer_start(&(cip_passth_info.passth_timer), 0); // Restart timer
        }

        last_cnt = cur_cnt;
    }

    //AT_TRACE("PassThrough mode exit...\r\n");
    at_uart_dma_receive_stop();
    at_uart_irq_receive_config();
    cip_passth_info_deinit();
    return 0;
}

/*!
    \brief      start a udp client
    \param[in]  srv_ip: server IP address
    \param[in]  srv_port: server port
    \param[out] none
    \retval     the result of operation
*/
static int udp_client_start(char *srv_ip, uint16_t srv_port, uint16_t local_port)
{
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    int reuse = 1;
    int fd, ret;

    if (cip_info.trans_mode == CIP_TRANS_MODE_PASSTHROUGH &&
            (cip_info_valid_fd_cnt_get() > 0 || cip_info.local_srv_fd >= 0))
        return -1;

    /* creating a UDP socket */
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        AT_TRACE("Create udp client socket fd error!\r\n");
        return -1;
    }
    setsockopt(fd , SOL_SOCKET, SO_REUSEADDR,
            (const char *)&reuse, sizeof(reuse));

    sys_memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;
    saddr.sin_len         = sizeof(saddr);

    saddr.sin_port        = htons(local_port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* binding the UDP socket to a random port */
    ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        AT_TRACE("Bind udp server socket fd error!\r\n");
        goto Exit;
    }
    /* Get local port */
    sys_memset(&saddr, 0, sizeof(saddr));
    getsockname(fd, (struct sockaddr *)&saddr, &len);
    //AT_TRACE("UDP local port %d\r\n", ntohs(saddr.sin_port));
    /* save client info */
    ret = cip_info_cli_store(fd, "UDP", CIP_ROLE_CLIENT,
                            inet_addr(srv_ip), srv_port, ntohs(saddr.sin_port));
    if (ret < 0) {
        AT_TRACE("Client num reached the maximum!\r\n");
        ret = -2;
        goto Exit;
    }
    AT_TRACE("UDP: create socket %d.\r\n", fd);

    if (local_port > 0)
        cip_passth_info.passth_fd_idx = ret; // local port is specified
    else
        cip_passth_info.passth_fd_idx = -1;

    return 0;

Exit:
    close(fd);
    return ret;
}

/*!
    \brief      send udp packet
    \param[in]  fd: the socket of the udp client
    \param[in]  tx_len: length of tcp packet to be sent
    \param[in]  srv_ip: server ip
    \param[in]  srv_port: server port
    \param[out] none
    \retval     the result of operation
*/
static int at_udp_send(int fd, uint32_t tx_len, char *srv_ip, uint16_t srv_port)
{
    char *tx_buf = NULL;
//    char ch;
    int cnt = 0, ret, idx;
    struct sockaddr_in saddr;

    tx_buf = sys_malloc(tx_len);
    if (NULL == tx_buf) {
        AT_TRACE("Allocate client buffer failed (len = %u).\r\n", tx_len);
        return -1;
    }
    AT_RSP_DIRECT(">", 1);
#if 0
    usart_interrupt_disable(at_uart_conf.usart_periph, USART_INT_RBNE);
    sys_priority_set(sys_current_task_handle_get(), MGMT_TASK_PRIORITY);
    while (1) {
        while (RESET == usart_flag_get(at_uart_conf.usart_periph, USART_FLAG_RBNE));
        ch = usart_data_receive(at_uart_conf.usart_periph);
        if ((ch == 0x0a || ch == 0x0d) && (cnt >= tx_len))
            break;
        if (cnt < tx_len) {
            tx_buf[cnt] = ch;
            cnt++;
        } else {
            AT_RSP("%c", ch);
        }
    }
    sys_priority_set(sys_current_task_handle_get(), CLI_PRIORITY);
    usart_interrupt_enable(at_uart_conf.usart_periph, USART_INT_RBNE);
    //buffer_dump("TX:", (uint8_t *)tx_buf, cnt);
#else
    // Block here to wait dma receive done
    at_uart_dma_receive((uint32_t)tx_buf, tx_len);

    // debug_print_dump_data("TX:", (char *)tx_buf, tx_len);
    cnt = send(fd, tx_buf, tx_len, 0);
#endif

    sys_memset(&saddr, 0, sizeof(struct sockaddr_in));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(srv_port);
    saddr.sin_addr.s_addr = inet_addr(srv_ip);

    //AT_TRACE("UDP: server IP=%s port=%d.\r\n", srv_ip, srv_port);
    ret = sendto(fd, tx_buf, cnt, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret <= 0) {
        /* low_level_output return ERR_MEM because wifi skb is not enough */
        AT_TRACE("UDP client send data error (ret = %d).\r\n", ret);
        idx = cip_info_cli_find(fd);
        cip_info_cli_free(idx);
        close(fd);
    }

    sys_mfree(tx_buf);
    return ret;
}

/*!
    \brief      start a tcp server
    \param[in]  srv_port: server port
    \param[out] none
    \retval     the result of operation
*/
static int tcp_server_start(uint16_t srv_port)
{
    struct sockaddr_in s_local_addr;
    int status, reuse;
    int srv_fd;

    srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_fd < 0) {
        return -1;
    }

    AT_TRACE("Create TCP server socket %d.\r\n", srv_fd);
    reuse = 1;
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR,
            (const char *) &reuse, sizeof(reuse) );
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEPORT,
            (const char *) &reuse, sizeof(reuse) );

    sys_memset((char *)&s_local_addr, 0, sizeof(s_local_addr));
    s_local_addr.sin_family      = AF_INET;
    s_local_addr.sin_len         = sizeof(s_local_addr);
    s_local_addr.sin_port        = htons(srv_port);
    s_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* binding the TCP socket to the TCP server address */
    status = bind(srv_fd, (struct sockaddr *)&s_local_addr, sizeof(s_local_addr));
    if( status < 0 ) {
        AT_TRACE("Bind tcp server socket fd error!\r\n");
        goto Exit;
    }
    AT_TRACE("Bind successfully.\r\n");

    /* putting the socket for listening to the incoming TCP connection */
    status = listen(srv_fd, 20);
    if( status != 0 ) {
        AT_TRACE("Listen tcp server socket fd error!\r\n");
        goto Exit;
    }
    cip_info.local_srv_fd = srv_fd;
    cip_info.local_srv_port = srv_port;
    AT_TRACE("TCP listen port %d\r\n", srv_port);

    return 0;

Exit:
    /* close the listening socket */
    close(srv_fd);
    return status;
}

/*!
    \brief      stop all tcp server
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void tcp_server_stop(void)
{
    int i, fd;

    if (cip_info.local_srv_fd >= 0) {
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            if ((cip_info.cli[i].fd >= 0) && (cip_info.cli[i].role == CIP_ROLE_SERVER)) {
                fd = cip_info.cli[i].fd;
                cip_info_cli_free(i);
                close(fd);
            }
        }
        fd = cip_info.local_srv_fd;
        cip_info.local_srv_fd = -1;
        cip_info.local_srv_port = 0;
        close(fd);
    }
}

/*!
    \brief      receive task
    \param[in]  param: the pointer of user parameter
    \param[out] none
    \retval     none
*/
static void cip_recv_task(void *param)
{
    struct timeval timeout;
    int max_fd_num = 0;
    int cli_fd, i, j, recv_sz;
    char *rx_buf;
    uint32_t rx_len = PASSTH_START_TRANSFER_LEN;
    struct sockaddr_in saddr;
    int addr_sz = sizeof(saddr);
    fd_set read_set, except_set;
    int status;

    rx_buf = sys_malloc(rx_len);
    if(NULL == rx_buf){
        AT_TRACE("Allocate client buffer failed (len = %u).\r\n", rx_len);
        goto Exit;
    }

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    while (1) {
        if (cip_task_terminate)
            break;

        FD_ZERO(&read_set);
        FD_ZERO(&except_set);
        if (cip_info.local_srv_fd >= 0) {
            FD_SET(cip_info.local_srv_fd, &read_set);
            FD_SET(cip_info.local_srv_fd, &except_set);
            if (cip_info.local_srv_fd > max_fd_num)
                max_fd_num = cip_info.local_srv_fd;
        }
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            //if ((cip_info.cli[i].fd >= 0) && (cip_info.cli[i].type == CIP_TYPE_TCP)) {
            if (cip_info.cli[i].fd >= 0) {
                FD_SET(cip_info.cli[i].fd, &read_set);
                FD_SET(cip_info.cli[i].fd, &except_set);
                if (cip_info.cli[i].fd > max_fd_num)
                    max_fd_num = cip_info.cli[i].fd;
            }
        }
        status = select(max_fd_num + 1, &read_set, NULL, &except_set, &timeout);
        if ((cip_info.local_srv_fd >= 0) && FD_ISSET(cip_info.local_srv_fd, &read_set)) {
            if (cip_info.cli_num >= MAX_CLIENT_NUM) {
                AT_TRACE("client full\r\n");
            } else {

                if (cip_info.trans_mode == CIP_TRANS_MODE_PASSTHROUGH &&
                        cip_info_valid_tcp_fd_cnt_get() >= 1) {
                    AT_TRACE("Only one TCP client is allowed in Passthrough mode\r\n");
                } else {
                    /* waiting for an incoming TCP connection */
                    /* accepts a connection form a TCP client, if there is any. otherwise returns SL_EAGAIN */
                    cli_fd = accept(cip_info.local_srv_fd,
                                    (struct sockaddr *)&saddr,
                                    (socklen_t*)&addr_sz);
                    if (cli_fd >= 0) {
                        AT_TRACE("new client %d\r\n", cli_fd);
                        status = cip_info_cli_store(cli_fd, "TCP", CIP_ROLE_SERVER,
                                            saddr.sin_addr.s_addr, saddr.sin_port, cip_info.local_srv_port);
                        if (status < 0) {
                            AT_TRACE("Store client info error %d!\r\n", status);
                            close(cli_fd);
                        }

                        cip_passth_info.passth_fd_idx = status;
                    }
                }
            }
        }
        /*if ((cip_info.local_srv_fd >= 0) && FD_ISSET(cip_info.local_srv_fd, &except_set)) {
            tcp_server_stop();
        }*/
        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            if ((cip_info.cli[i].fd >= 0) && FD_ISSET(cip_info.cli[i].fd, &read_set)) {
                if (cip_info.cli[i].type == CIP_TYPE_TCP) {
                    recv_sz = recv(cip_info.cli[i].fd, rx_buf, rx_len, 0);
                } else {
                    sys_memset(&saddr, 0, sizeof(saddr));
                    recv_sz = recvfrom(cip_info.cli[i].fd, rx_buf, rx_len,
                                        0, (struct sockaddr *)&saddr, (socklen_t*)&addr_sz);
                }
                AT_TRACE("RX:%d, %d\r\n", cip_info.cli[i].fd, recv_sz);
                if (recv_sz < 0) { /* Recv error */
                    AT_TRACE("rx error %d\r\n", recv_sz);
                    close(cip_info.cli[i].fd);
                    cip_info_cli_free(i);
                } else if (recv_sz == 0) {
                    AT_TRACE("remote close %d\r\n", cip_info.cli[i].fd);
                    close(cip_info.cli[i].fd);
                    cip_info_cli_free(i);
                } else {
                    if (cip_info.trans_mode == CIP_TRANS_MODE_PASSTHROUGH &&
                            cip_passth_info.passth_fd_idx == i) {
                        AT_RSP_DIRECT(rx_buf, recv_sz);
                    } else {
                        AT_RSP_START(64 + recv_sz);
                        AT_RSP("+IPD,%d,%d: ", cip_info.cli[i].fd, recv_sz);
                        for (j = 0; j < recv_sz; j++)
                            AT_RSP("%c", rx_buf[j]);
                        AT_RSP("\r\n");
                        AT_RSP_OK();
                    }
                }
            }
            if ((cip_info.cli[i].fd >= 0) && FD_ISSET(cip_info.cli[i].fd, &except_set)) {
                AT_TRACE("error %d\r\n", cip_info.cli[i].fd);
                close(cip_info.cli[i].fd);
                cip_info_cli_free(i);
            }
        }
    }

    /* Exit */
    for (i = 0; i < MAX_CLIENT_NUM; i++) {
        if (cip_info.cli[i].fd >= 0) {
            close(cip_info.cli[i].fd);
            cip_info_cli_free(i);
        }
    }
    /* close the listening socket */
    if (cip_info.local_srv_fd >= 0)
        close(cip_info.local_srv_fd);
    cip_info.local_srv_fd = -1;
    cip_info.local_srv_port = 0;

    sys_mfree(rx_buf);

Exit:
    sys_task_delete(NULL);
}

/*!
    \brief      the AT command ping
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_ping(int argc, char **argv)
{
    struct ping_info_t *ping_info = NULL;

    AT_RSP_START(128);
    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            char *domain = at_string_parse(argv[1]);
            struct addrinfo hints, *res;
            void *ptr;
#ifdef CONFIG_IPV6_SUPPORT
            char ip_addr[64];
#else
            char ip_addr[32];
#endif /* CONFIG_IPV6_SUPPORT */
            if (domain == NULL) {
                goto Error;
            }

            memset(&hints, 0, sizeof(hints));
            if (getaddrinfo(domain, NULL, &hints, &res) != 0) {
                goto Error;
            }

            ping_info = sys_zalloc(sizeof(struct ping_info_t));
            if (ping_info == NULL) {
                freeaddrinfo(res);
                goto Error;
            }
#ifdef CONFIG_IPV6_SUPPORT
            if (res->ai_family == AF_INET6) {
                ptr = &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
                ping_info->ip_type = IPADDR_TYPE_V6;
            } else
#endif /* CONFIG_IPV6_SUPPORT */
            {
                ptr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
            }
            inet_ntop(res->ai_family, ptr, ip_addr, sizeof(ip_addr));
            freeaddrinfo(res);
            memcpy(ping_info->ping_ip, ip_addr, sizeof(ping_info->ping_ip));
            ping_info->ping_cnt = 5;
            ping_info->ping_size = 120;
            ping_info->ping_interval = 1000;
            if (ping(ping_info) != ERR_OK)
                goto Error;
            AT_RSP("%s", ping_info->ping_res);
        }
    } else {
        goto Error;
    }

    if (ping_info)
        sys_mfree(ping_info);

    AT_RSP_OK();
    return;

Error:
    if (ping_info)
        sys_mfree(ping_info);
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+PING=<ip or domain name>\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command start a tcp or udp client
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_start(int argc, char **argv)
{
    char *type, *srv_ip;
    uint16_t srv_port;
    uint8_t bkeep_alive = 0;
    char *endptr = NULL;
    int ret = -1;
    uint16_t local_port = 0;

    AT_RSP_START(128);
    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            goto Error;
        }
    } else if ((argc == 4) || (argc == 5)) {
        type = at_string_parse(argv[1]);
        srv_ip = at_string_parse(argv[2]);
        if ((type == NULL) || (srv_ip == NULL)) {
            goto Error;
        }
        srv_port = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        if (argc == 5) {
            if (strncmp(type, "TCP", 3) == 0) {
                bkeep_alive = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
                if (*endptr != '\0') {
                    goto Error;
                }
            } else if (strncmp(type, "UDP", 3) == 0) {
                local_port = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
                if (*endptr != '\0') {
                    goto Error;
                }
            }
        }
        if (cip_info.cli_num >= MAX_CLIENT_NUM) {
            AT_TRACE("client full\r\n");
            goto Error;
        }

        if (strncmp(type, "TCP", 3) == 0) {
            if ((ret = tcp_client_start(srv_ip, srv_port, bkeep_alive) < 0))
                goto Error;
        } else if (strncmp(type, "UDP", 3) == 0) {
            if ((ret = udp_client_start(srv_ip, srv_port, local_port) < 0))
                goto Error;
        } else {
            goto Error;
        }
        if (cip_task_started == 0) {
            if (sys_task_create_dynamic((const uint8_t *)"Cip Rcv",
                            CIP_RECV_STACK_SIZE, CIP_RECV_PRIO,
                            (task_func_t)cip_recv_task, NULL) == NULL)
                goto Error;
            cip_task_started = 1;
        }
    } else {
        goto Error;
    }
    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+CIPSTART=<type:TCP or UDP>,<remote ip>,<remote port>,[udp local port],[tcp keep alive:0-1]\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command send a tcp or udp packet
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_send(int argc, char **argv)
{
    int fd, idx;
    uint32_t tx_len;
    char *srv_ip;
    uint16_t srv_port;
    char *endptr = NULL;

    AT_RSP_START(128);
    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            goto Error;
        }
    } else if (argc == 3) {
        fd = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        idx = cip_info_cli_find(fd);
        if ((*endptr != '\0') || idx < 0) {
            AT_TRACE("fd error\r\n");
            goto Error;
        }
        tx_len = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if ((*endptr != '\0') || (tx_len > 2048)) {
            goto Error;
        }
        //AT_TRACE("FD: %d, len %d\r\n", fd, tx_len);
        if (cip_info.cli[idx].type == CIP_TYPE_TCP) {
            if (at_tcp_send(fd, tx_len) <= 0) {
                goto Error;
            }
        } else {
            AT_TRACE("ip error\r\n");
            goto Error;
        }
    } else if (argc == 5) {
        fd = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        idx = cip_info_cli_find(fd);
        if ((*endptr != '\0') || idx < 0) {
            goto Error;
        }
        tx_len = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if ((*endptr != '\0') || (tx_len > 2048)) {
            goto Error;
        }
        srv_ip = at_string_parse(argv[3]);
        if (srv_ip == NULL) {
            goto Error;
        }
        srv_port = (uint32_t)strtoul((const char *)argv[4], &endptr, 10);
        if (*endptr != '\0') {
            goto Error;
        }
        AT_TRACE("FD: %d, len %d, ip %s, port %d\r\n", fd, tx_len, srv_ip, srv_port);
        if (cip_info.cli[idx].type == CIP_TYPE_TCP) {
            if (at_tcp_send(fd, tx_len) <= 0) {
                goto Error;
            }
        } else {
            if (at_udp_send(fd, tx_len, srv_ip, srv_port) <= 0) {
                goto Error;
            }
        }
    }else if (argc == 1) {
        if (cip_info.trans_mode == CIP_TRANS_MODE_PASSTHROUGH) {
            if (cip_info_valid_tcp_fd_cnt_get() > 1) {
                AT_TRACE("TCP Passthrough mode support only 1 TCP connection\r\n");
                goto Error;
            }

            idx = cip_passth_info.passth_fd_idx;
            if (idx == -1 || cip_info.cli[idx].fd < 0 || cip_info_valid_fd_cnt_get() == 0) {
                AT_TRACE("Invalid Passthrough fd\r\n");
                goto Error;
            }

            // AT_TRACE("PassThrough mode enter, fd=%d\r\n", cip_info.cli[idx].fd);
            AT_RSP_OK();

            at_passth_send(cip_info.cli[idx].fd, cip_info.cli[idx].type);

            return;
        } else {
            goto Error;
        }
    } else {
        goto Error;
    }
    AT_RSP("SEND OK\r\n");
    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+CIPSEND=<fd:0-4>,<len>,[remote ip],[remote port]\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command start or stop a tcp server
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_server(int argc, char **argv)
{
    uint8_t enable;
    uint16_t port = 0;
    char *endptr = NULL;

    AT_RSP_START(128);
    if ((argc == 2) || (argc == 3)) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            enable = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
            if ((*endptr != '\0') || (enable > 1)) {
                goto Error;
            }
            if (argc == 3) {
                port = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
                if (*endptr != '\0') {
                    goto Error;
                }
            }
            if (enable) {
                if (cip_info.trans_mode == CIP_TRANS_MODE_PASSTHROUGH &&
                    (cip_info_valid_fd_cnt_get() > 0 || cip_info.local_srv_fd >= 0))
                    goto Error;

                if (cip_info.local_srv_fd >= 0) {
                    AT_TRACE("Already run\r\n");
                    goto Error;
                }
                if (tcp_server_start(port) < 0) {
                    goto Error;
                }
                if (cip_task_started == 0) {
                    if (sys_task_create_dynamic((const uint8_t *)"Cip Rcv",
                                    CIP_RECV_STACK_SIZE, CIP_RECV_PRIO,
                                    (task_func_t)cip_recv_task, NULL) == NULL)
                        goto Error;
                    cip_task_started = 1;
                }
            } else {
                tcp_server_stop();
            }
        }
    } else {
        goto Error;
    }

    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+CIPSERVER:<mode:0-1>,[port]\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command free tcpip information, close client and server
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_close(int argc, char **argv)
{
    int fd, i;
    char *endptr = NULL;

    AT_RSP_START(128);
    if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            fd = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
            if ((*endptr != '\0') || (fd < 0)) {
                goto Error;
            }
            for (i = 0; i < MAX_CLIENT_NUM; i++) {
                if (fd == cip_info.cli[i].fd) {
                    cip_info_cli_free(i);
                    AT_TRACE("close %d\r\n", fd);
                    break;
                }
            }
            if (fd == cip_info.local_srv_fd) {
                AT_TRACE("server fd\r\n");
                goto Error;
            } else if (i < MAX_CLIENT_NUM) {
                close(fd);
            } else {
                goto Error;
            }
        }
    } else {
        goto Error;
    }
    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+CIPCLOSE: <fd>\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command show tcpip information status
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_status(int argc, char **argv)
{
    int i = 0;
    char type[4];
    int vif_idx = WIFI_VIF_INDEX_DEFAULT;

    AT_RSP_START(512);
    if (argc == 1) {
        if (wifi_vif_is_sta_connected(vif_idx)) {
            if (cip_info.cli_num > 0) {
                AT_RSP("STATUS: 3\r\n");
            } else {
                AT_RSP("STATUS: 2\r\n");
            }
        } else if (wifi_vif_is_sta_handshaked(vif_idx)) {
            AT_RSP("STATUS: 4\r\n");
        } else {
            AT_RSP("STATUS: 5\r\n");
        }

        for (i = 0; i < MAX_CLIENT_NUM; i++) {
            if (cip_info.cli[i].fd >= 0) {
                if (cip_info.cli[i].type == CIP_TYPE_TCP)
                    strcpy(type, "TCP");
                else
                    strcpy(type, "UDP");
                AT_RSP("+CIPSTATUS:%d,%s,"IP_FMT",%d,%d,%d\r\n",
                        cip_info.cli[i].fd, type, IP_ARG(cip_info.cli[i].remote_ip),
                        cip_info.cli[i].remote_port, cip_info.cli[i].local_port, cip_info.cli[i].role);
            }
        }
    } else {
        goto Error;
    }
    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
}


/*!
    \brief      the AT command set transfer interval
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/

void at_trans_interval(int argc, char **argv)
{
    int mode;

    AT_RSP_START(32);

    if (argc == 1) {
        AT_RSP("+TRANSINTVAL:%d\r\n", cip_info.trans_intvl);
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            cip_info.trans_intvl = atoi(argv[1]);
        }
    } else {
        goto Error;
    }

    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+TRANSINTVAL=<interval>\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command set transfer mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/

void at_cip_mode(int argc, char **argv)
{
    int mode;
    int fd, type, idx;

    AT_RSP_START(32);
    if (argc == 1) {
        AT_RSP("+CIPMODE:%d\r\n", cip_info.trans_mode);
    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION) {
            goto Usage;
        } else {
            mode = atoi(argv[1]);
            if (mode == 0) {
                cip_info.trans_mode = CIP_TRANS_MODE_NORMAL;
            } else if (mode == 1) {
                if (cip_info_valid_tcp_fd_cnt_get() >= 2) {
                    AT_TRACE("TCP Passthrough mode support only 1 TCP connection\r\n");
                    goto Error;
                }

                if (cip_passth_info.passth_fd_idx == -1) {
                    AT_TRACE("Invalid Passthrough fd\r\n");
                    goto Error;
                }
                cip_info.trans_mode = CIP_TRANS_MODE_PASSTHROUGH;
            } else {
                AT_TRACE("Unknown transfer mode:%d\r\n", mode);
                goto Error;
            }
        }
    } else {
        goto Error;
    }

    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+CIPMODE=<mode:0-1>\r\n");
    AT_RSP_OK();
    return;

}

int at_parse_ip4(char *str, uint32_t *ip)
{
    char *token;
    uint32_t a, i, j;

    token = strchr(str, '/');
    *ip = 0;

    for (i = 0; i < 4; i++) {
        if (i < 3) {
            token = strchr(str, '.');
            if (!token)
                return -1;
            *token++ = '\0';
        }
        for (j = 0; j < strlen(str); j++) {
            if (str[j] < '0' || str[j] > '9')
            return -1;
        }

        a = atoi(str);
        if (a > 255)
            return -1;
        str = token;
        *ip += (a << (i * 8));
    }

    return 0;
}

/*!
    \brief      the AT command set station ip
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_sta_ip(int argc, char **argv)
{
    struct wifi_ip_addr_cfg ip_cfg;

    AT_RSP_START(256);
    if (argc == 1) {
        if (!wifi_get_vif_ip(WIFI_VIF_INDEX_DEFAULT, &ip_cfg))
        {
            AT_RSP("+CIPSTA: "IP_FMT"\r\n", IP_ARG(ip_cfg.ipv4.addr));
            AT_RSP("+CIPSTA: "IP_FMT"\r\n", IP_ARG(ip_cfg.ipv4.mask));
            AT_RSP("+CIPSTA: "IP_FMT"\r\n", IP_ARG(ip_cfg.ipv4.gw));
#ifdef CONFIG_IPV6_SUPPORT
            {
                char ip6_local[IPV6_ADDR_STRING_LENGTH_MAX] = {0};
                char ip6_unique[IPV6_ADDR_STRING_LENGTH_MAX] = {0};
                if (!wifi_get_vif_ip6(WIFI_VIF_INDEX_DEFAULT, ip6_local, ip6_unique)) {
                    AT_RSP("+CIPSTA: [%s]\r\n", ip6_local);
                    AT_RSP("+CIPSTA: [%s]\r\n", ip6_unique);
                }
            }
#endif
        } else {
            goto Usage;
        }

    } else if (argc == 2) {
        if (argv[1][0] == AT_QUESTION)
            goto Usage;
        else
            goto Error;

    } else if (argc == 4) {
        ip_cfg.mode = IP_ADDR_STATIC_IPV4;
        net_if_use_static_ip(true);
        if (at_parse_ip4(at_string_parse(argv[1]), &ip_cfg.ipv4.addr))
            goto Usage;
        if (at_parse_ip4(at_string_parse(argv[2]), &ip_cfg.ipv4.mask))
            goto Usage;
        if (at_parse_ip4(at_string_parse(argv[3]), &ip_cfg.ipv4.gw))
            goto Usage;
        AT_TRACE("+CIPSTA: set "IP_FMT", "IP_FMT", "IP_FMT"\r\n",
                    IP_ARG(ip_cfg.ipv4.addr), IP_ARG(ip_cfg.ipv4.mask), IP_ARG(ip_cfg.ipv4.gw));

        wifi_set_vif_ip(WIFI_VIF_INDEX_DEFAULT, &ip_cfg);

    } else {
        goto Error;
    }

    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
Usage:
    AT_RSP("+CIPSTA:<ip>,<netmask>,<gw>\r\n");
    AT_RSP_OK();
    return;
}

/*!
    \brief      the AT command show ap and station ip
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void at_cip_ip_addr_get(int argc, char **argv)
{
    struct wifi_vif_tag *wvif = (struct wifi_vif_tag *)vif_idx_to_wvif(WIFI_VIF_INDEX_DEFAULT);
    struct wifi_ip_addr_cfg ip_cfg;

    AT_RSP_START(256);
    if (argc == 1) {
        if (wifi_get_vif_ip(WIFI_VIF_INDEX_DEFAULT, &ip_cfg)) {
            goto Error;
        }
        if (wvif->wvif_type == WVIF_AP) {
            AT_RSP("+CIFSR:APIP,"IP_FMT"\r\n", IP_ARG(ip_cfg.ipv4.addr));
            AT_RSP("+CIFSR:APMAC,"MAC_FMT"\r\n", MAC_ARG(wvif->mac_addr.array));
        } else if(wvif->wvif_type == WVIF_STA) {
            AT_RSP("+CIFSR:STAIP,"IP_FMT"\r\n", IP_ARG(ip_cfg.ipv4.addr));
            AT_RSP("+CIFSR:STAMAC,"MAC_FMT"\r\n", MAC_ARG(wvif->mac_addr.array));
        }
    } else {
        goto Error;
    }
    AT_RSP_OK();
    return;

Error:
    AT_RSP_ERR();
    return;
}
