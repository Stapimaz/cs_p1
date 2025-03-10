/*!
    \file    ping.c
    \brief   Implementation of ping .

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

#include "app_cfg.h"
#include "lwip/opt.h"
#if LWIP_PING
#include "lwip/mem.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/debug.h"
#include "ping.h"
#include "dbg_print.h"

#ifdef CONFIG_ATCMD
#include "cmd_shell.h"
#include "atcmd.h"
#endif

/** ping receive timeout - in milliseconds */
#define PING_RCV_TIMEO  2000
/** ping delay - in milliseconds */
#define PING_DELAY      10 //100

#define BUF_SIZE        1500

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID         0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE  32
#endif

struct _ip_addr {
    uint8_t addr0, addr1, addr2, addr3;
};

static uint8_t ping_terminate;
static os_task_t ping_task_hdl = NULL;

static uint32_t my_time_get()
{
    return (sys_os_now(false) * OS_MS_PER_TICK);
}

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct ping_info_t *ping_info, struct icmp_echo_hdr *iecho, uint16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

#if LWIP_IPV6
    if (ping_info->ip_type == IPADDR_TYPE_V6)
        ICMPH_TYPE_SET(iecho, ICMP6_TYPE_EREQ);
    else
#endif
        ICMPH_TYPE_SET(iecho, ICMP_ECHO);

    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_info->ping_seq_num);

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

#if LWIP_IPV6
    if (ping_info->ip_type != IPADDR_TYPE_V6)
#endif
        iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the socket ip */
static err_t ping_send(struct ping_info_t *ping_info, int s, ip_addr_t *addr, size_t size)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
#if LWIP_IPV6
    struct sockaddr_in6 *to_ipv6;
    struct sockaddr_storage sock_st;
#endif
    size_t ping_size;

    if (size == 0) {
        ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    } else {
        ping_size = sizeof(struct icmp_echo_hdr) + size;
    }

    LWIP_ASSERT("ping_size is too big\n\r", ping_size <= 0xffff);

    iecho = (struct icmp_echo_hdr *)sys_malloc(ping_size);
    if (iecho == NULL) {
        return ERR_MEM;
    }

    ping_prepare_echo(ping_info, iecho, (u16_t)ping_size);

#if LWIP_IPV6
    if (ping_info->ip_type == IPADDR_TYPE_V6) {
        memset(&sock_st, 0, sizeof(struct sockaddr_storage));
        to_ipv6 = (struct sockaddr_in6 *)&sock_st;
        to_ipv6->sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to_ipv6->sin6_addr, &addr->u_addr.ip6);
        err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&sock_st, sizeof(struct sockaddr_storage));
    } else {
        to.sin_len = sizeof(to);
        to.sin_family = AF_INET;
        to.sin_addr.s_addr = addr->u_addr.ip4.addr;
        err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));
    }
#else
    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = addr->addr;
    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));
#endif

    sys_mfree(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(struct ping_info_t *ping_info, int s)
{
    uint32_t reply_time;
    int len;
    struct sockaddr *from;
    struct sockaddr_in from_ip4;
#if LWIP_IPV6
    struct sockaddr_in6 *from_ip6;
    struct sockaddr_storage sock_st;
#endif
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    uint32_t ip_hdr_len;
    uint32_t fromlen;
    uint32_t delay;
    //struct _ip_addr *addr;
    //static uint32_t ping_recv_count = 0;
    uint32_t start_time = 0;
    uint32_t recv_size;

#if LWIP_IPV6
    if (ping_info->ip_type == IPADDR_TYPE_V6) {
        ip_hdr_len = sizeof(struct ip6_hdr);
        memset(&sock_st, 0, sizeof(struct sockaddr_storage));
        from_ip6 = (struct sockaddr_in6 *)&sock_st;
        from = (struct sockaddr*)&sock_st;
        fromlen = sizeof(struct sockaddr_storage);
    } else
#endif
    {
        ip_hdr_len = sizeof(struct ip_hdr);
        from = (struct sockaddr*)&from_ip4;
        fromlen = sizeof(struct sockaddr);
    }

    if ((ping_info->ping_size + sizeof(struct icmp_echo_hdr) + ip_hdr_len) > BUF_SIZE)
        recv_size = (ping_info->ping_size + sizeof(struct icmp_echo_hdr) + ip_hdr_len);
    else
        recv_size = BUF_SIZE;
    start_time = my_time_get();

    while ((len = lwip_recvfrom(s, ping_info->reply_buf, recv_size, 0, from, (socklen_t*)&fromlen)) > 0) {
        if (len >= (ip_hdr_len + sizeof(struct icmp_echo_hdr))) {
            //addr = (struct _ip_addr *)&(from.sin_addr);
            //app_print("@@@ping: recv %d.%d.%d.%d, len = %d, count = %d\n",
            //             addr->addr0, addr->addr1, addr->addr2, addr->addr3, len, ++ping_recv_count));
            reply_time = my_time_get();
            delay = reply_time - ping_info->ping_time;

#if LWIP_IPV6
            if (ping_info->ip_type == IPADDR_TYPE_V6) {
                iecho = (struct icmp_echo_hdr *)(ping_info->reply_buf+IP6_HLEN);
            } else
#endif
            {
                iphdr = (struct ip_hdr *)ping_info->reply_buf;
                iecho = (struct icmp_echo_hdr *)(ping_info->reply_buf+(IPH_HL(iphdr) * 4));
            }

            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_info->ping_seq_num))) {
#ifdef CONFIG_ATCMD
                //if (cmd_mode_type_get() == CMD_MODE_TYPE_AT)
                ping_info->ping_res_len += co_snprintf((ping_info->ping_res + ping_info->ping_res_len),
                                                        (sizeof(ping_info->ping_res) - ping_info->ping_res_len),
                                                        "+%d\r\n", delay);
                //else
#endif
                {
#if LWIP_IPV6
                    if (ping_info->ip_type == IPADDR_TYPE_V6) {
                        app_print("[ping_test] %d bytes from %s: icmp_seq=%d time=%d ms\n\r",
                            len - sizeof(struct ip6_hdr) - sizeof(struct icmp_echo_hdr), inet6_ntoa(from_ip6->sin6_addr),
                            htons(iecho->seqno), delay);
                    } else
#endif
                    {
                        app_print("[ping_test] %d bytes from %s: icmp_seq=%d time=%d ms\n\r",
                            len - sizeof(struct ip_hdr) - sizeof(struct icmp_echo_hdr), inet_ntoa(from_ip4.sin_addr),
                            htons(iecho->seqno), delay);
                    }
                }
                ping_info->ping_recv_count++;
                if (delay > ping_info->ping_max_delay) ping_info->ping_max_delay = delay;
                if (delay < ping_info->ping_min_delay) ping_info->ping_min_delay = delay;
                ping_info->ping_avg_delay += delay;
                return;
            } else {
                if (my_time_get() - start_time > PING_RCV_TIMEO) {
                    // app_print("my_time_get() %d\n\r", my_time_get());

                    len = -1;
                    break;
                }
            }

            if ((iecho->id == PING_ID) && (iecho->type == ICMP_ER)) {
/*
                int i;
                app_print("\n\rping: drop");
                app_print("------------------------------------------- \n");
                for (i = 0; i < len; i = i + 1) {
                    app_print("%02x:", *(ping_info->reply_buf + i));
                    if (i%16 == 15)
                        app_print("\n");
                }
                app_print("\n");
                app_print("------------------------------------------- \n");
*/
            }
        }
    }

    if (len <= 0) {
#ifdef CONFIG_ATCMD
    //if (cmd_mode_type_get() == CMD_MODE_TYPE_AT)
    ping_info->ping_res_len += co_snprintf((ping_info->ping_res + ping_info->ping_res_len),
                                            (sizeof(ping_info->ping_res) - ping_info->ping_res_len),
                                            "+timeout\r\n");
    //else
#endif
        app_print("[ping_test] timeout\n\r");
    }
}

err_t ping(struct ping_info_t *ping_info)
{
    int s;
    int timeout = PING_RCV_TIMEO;
    ip_addr_t ping_target;
    uint32_t send_count;
    char *target = ping_info->ping_ip;
    uint32_t count = ping_info->ping_cnt;
    size_t size = ping_info->ping_size;
    uint32_t interval = ping_info->ping_interval;
    //static uint32_t ping_send_count = 0;
    //uint16_t size_random; //add for random size ping test
    uint32_t recv_size;
    uint32_t ip_hdr_len;

    ping_info->ping_seq_num = 0;
    ping_info->ping_max_delay = 0;
    ping_info->ping_min_delay = (u16_t)-1;
    ping_info->ping_avg_delay = 0;
    ping_info->ping_recv_count = 0;
    send_count = 0;

#if LWIP_IPV6
    ping_target.type = 0;
    if (ping_info->ip_type == IPADDR_TYPE_V6) {
        if (inet6_aton(target, &ping_target.u_addr.ip6) == 0)
            return ERR_VAL;
        ping_target.type = IPADDR_TYPE_V6;
        ip_hdr_len = sizeof(struct ip6_hdr);
    } else
#endif
    {
        if (inet_aton(target, &ping_target) == 0)
            return ERR_VAL;
        ip_hdr_len = sizeof(struct ip_hdr);
        //addr = (struct _ip_addr*)&ping_target;
    }


    if ((ping_info->ping_size + sizeof(struct icmp_echo_hdr)+ ip_hdr_len) > BUF_SIZE)
        recv_size = (ping_info->ping_size + sizeof(struct icmp_echo_hdr)+ ip_hdr_len);
    else
        recv_size = BUF_SIZE;
    ping_info->reply_buf = sys_malloc(recv_size);
    if (NULL == ping_info->reply_buf){
        app_print("ping recv: allocate buffer failed.\n\r");
        return ERR_MEM;
    }

#if LWIP_IPV6
    if (ping_info->ip_type == IPADDR_TYPE_V6) {
        s = lwip_socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    } else
#endif
    {
        s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
    }
    if (s < 0) {
        app_print("ping: create socket failled\n\r");
        sys_mfree(ping_info->reply_buf);
        return ERR_MEM;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

#ifdef CONFIG_ATCMD
    //if (cmd_mode_type_get() != CMD_MODE_TYPE_AT)
#endif
    {
#if LWIP_IPV6
        if (ping_info->ip_type == IPADDR_TYPE_V6)
            app_print("[ping_test] PING %s %d bytes of data\n\r",
                inet6_ntoa(ping_target), size);
        else
#endif
            app_print("[ping_test] PING %s %d bytes of data\n\r",
                inet_ntoa(ping_target), size);
    }
    while (1) {
        //add for random size ping test
        //size_random = 32 + rand()%(1460-32+1);
        if (ping_terminate) {
            break;
        }

        //if (ping_send(s, &ping_target, size_random) == ERR_OK)
        if (ping_send(ping_info, s, &ping_target, size) == ERR_OK) {
            //app_print("@@@ping: send %d.%d.%d.%d, size = %d, count = %d\n",
            //            addr->addr0, addr->addr1, addr->addr2, addr->addr3, size, ++ping_send_count);
            ping_info->ping_time = my_time_get();
            ping_recv(ping_info, s);

        }

        send_count ++;
        if (send_count >= count) break; /* send ping times reached, stop */

        sys_ms_sleep(interval); /* take a delay */
    }
#ifdef CONFIG_ATCMD
    //if (cmd_mode_type_get() != CMD_MODE_TYPE_AT)
#endif
    {
        app_print("[ping_test] %d packets transmitted, %d received, %d%% packet loss\n\r",
            send_count, ping_info->ping_recv_count, (send_count-ping_info->ping_recv_count) * 100 / send_count);

        if (ping_info->ping_recv_count != 0) {
            app_print("[ping_test] delay: min %u ms, max %u ms, avg %u ms\n\r",
                ping_info->ping_min_delay, ping_info->ping_max_delay, ping_info->ping_avg_delay / ping_info->ping_recv_count);
        }
    }
    lwip_close(s);

    sys_mfree(ping_info->reply_buf);

    return ERR_OK;
}

static void ping_task_func(void *param)
{
    err_t err;
    struct ping_info_t *ping_info = (struct ping_info_t *)param;

    // ping_terminate = 0;
    err = ping(ping_info);
    if (err != ERR_OK) {
        app_print("ping: exit because of error!\n\r");
    }

    sys_mfree(ping_info);
    ping_task_hdl = NULL;
    sys_task_delete(NULL);
}

static int ping_parse_arguments(struct ping_info_t *ping_info, int argc, char **argv)
{
    uint32_t arg_cnt = 1, ip_len = 0;
    uint32_t ping_size;
    uint32_t ping_time;

    if (argc < 2) {
        goto Exit;
    } else {
#if LWIP_IPV6
        ping_info->ip_type = 0;
        if (strcmp(argv[arg_cnt], "-6") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ip_len = (strlen((const char *)argv[arg_cnt + 1]) < 64)? (strlen((const char *)argv[arg_cnt + 1]) + 1) : 64;
            memcpy(ping_info->ping_ip, (const char *)argv[arg_cnt + 1], ip_len);
            ping_info->ip_type = IPADDR_TYPE_V6;
            arg_cnt += 2;
        } else
#endif
        {
            ip_len = (strlen((const char *)argv[arg_cnt]) < 16)? (strlen((const char *)argv[arg_cnt]) + 1) : 16;
            memcpy(ping_info->ping_ip, (const char *)argv[arg_cnt], ip_len);
            arg_cnt ++;
        }
    }

    ping_info->ping_cnt = 5;
    ping_info->ping_size = 120;
    ping_info->ping_interval = 10;

    while (arg_cnt < argc) {
        if (strcmp(argv[arg_cnt], "-n") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_info->ping_cnt = (uint32_t)atoi(argv[arg_cnt + 1]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-l") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_size = (uint32_t)atoi(argv[arg_cnt + 1]);
            #if 0
            if ((ping_size > (MEM_SIZE - 256)) || (ping_size > (PBUF_POOL_SIZE * PBUF_POOL_BUFSIZE - 512))) {
                app_print("ping fail!!!\r\n");
                app_print("Please make sure:\r\n"\
                            "\t\t\tping size <= MEM_SIZE - 256(%d) in lwipopts.h\r\n"\
                            "\t\t\tping size <= (PBUF_POOL_SIZE * PBUF_POOL_BUFSIZE - 512)(%d) in lwipopts.h\r\n",\
                            (MEM_SIZE - 256), (PBUF_POOL_SIZE * PBUF_POOL_BUFSIZE - 512));
                return 0;
            }
            #else
            if (ping_size > (IP_REASS_MAX_PBUFS * 1480 - 8)) {
                app_print("ping fail!!!\r\n");
                /* 1480 = LLC_ETHER_MTU(1500) - IP Header len(20) */
                app_print("Please make sure:\r\n"\
                            "\t\t\tping size <= (IP_REASS_MAX_PBUFS * 1480 - 8)(%d) in lwipopts.h\r\n",\
                            (IP_REASS_MAX_PBUFS * 1480 - 8));
                return 0;
            }
            #endif
            ping_info->ping_size = ping_size;
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-i") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_info->ping_interval = (uint32_t)atoi(argv[arg_cnt + 1]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-t") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            ping_time = (uint32_t)atoi(argv[arg_cnt + 1]);
            if (ping_time > 0) {
                ping_info->ping_interval = 1000;
                ping_info->ping_cnt = ping_time;
            } else {
                app_print("invalid run time \r\n");
                goto Usage;
            }
            arg_cnt += 2;
        }
        else {
            goto Exit;
        }
    }
    return 1;

Exit:
    app_print("\rping: ping test command format error!\r\n");
Usage:
    app_print("\rUsage:\r\n");
    app_print("ping <target_ip | stop> [-n count] [-l size] [-i interval] [-t total time]\r\n");
    app_print("    target_ip <ipv4_addr> or <-6 ipv6_addr>(if IPV6 support)\r\n");
    app_print("    stop ping stop\r\n");
    app_print("    -n   number of echo request to run(default 5)\r\n");
    app_print("    -l   ping size of single ping test(default 120B)\r\n");
    app_print("    -i   time interval between single echo request(unit:ms) (default 10ms)\r\n");
    app_print("    -t   total run time(unit:second) (default not use)\r\n");
    app_print("Note: when -t option is enabled, time interval's default value is 1000ms,"\
                "and ping test number equals to time\r\n");
    return 0;
}


void cmd_ping(int argc, char **argv)
{
#if LWIP_PING
    struct ping_info_t *ping_info;

    if (argc >= 2 && (strcmp(argv[1], "stop") == 0)) {
        ping_terminate = 1;
        return;
    }

    if (ping_task_hdl != NULL) {
        app_print("ping task has been running, return\n\r");
        return;
    }

    ping_info = (struct ping_info_t *)sys_zalloc(sizeof(struct ping_info_t));
    if (ping_info == NULL) {
        app_print("ping: no memory for ping_info.\r\n");
        return;
    }

    if(ping_parse_arguments(ping_info, argc, argv)) {

        ping_task_hdl = sys_task_create_dynamic((const uint8_t *)"ping_task", PING_TASK_STACK_SIZE, PING_TASK_PRIO,
            (task_func_t)ping_task_func, ping_info);
        if (ping_task_hdl == NULL) {
            app_print("ping: create ping task failed.\r\n");
            sys_mfree(ping_info);
            return;
        }

        ping_terminate = 0;
    } else {
        sys_mfree(ping_info);
    }
#else
    app_print("\n\rLWIP_PING should be defined in lwipopts.h!\n");
#endif
}

#endif /* LWIP_PING */
