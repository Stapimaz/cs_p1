/*
 * iperf, Copyright (c) 2014, 2015, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
#ifndef __IPERF_H
#define __IPERF_H


typedef uint64_t iperf_size_t;

struct iperf_interval_results
{
    iperf_size_t bytes_transferred; /* bytes transfered in this interval */
    struct timeval interval_start_time;
    struct timeval interval_end_time;
    float interval_duration;

    /* for UDP */
    int interval_packet_count;
    int interval_outoforder_packets;
    int interval_cnt_error;
    int packet_count;
    float jitter;
    int outoforder_packets;
    int cnt_error;

    int omitted;
    /* Just placeholders, never accessed. */
    char *tcpInfo;

    int interval_retrans;
    int interval_sacks;
    int snd_cwnd;
    TAILQ_ENTRY(iperf_interval_results) irlistentries;
    void *custom_data;
    int rtt;
};

struct iperf_stream_result
{
    iperf_size_t bytes_received;
    iperf_size_t bytes_sent;
    iperf_size_t bytes_received_this_interval;
    iperf_size_t bytes_sent_this_interval;
    int stream_prev_total_retrans;
    int stream_retrans;
    int stream_prev_total_sacks;
    int stream_sacks;
    int stream_max_rtt;
    int stream_min_rtt;
    int stream_sum_rtt;
    int stream_count_rtt;
    int stream_max_snd_cwnd;
    struct timeval start_time;
    struct timeval end_time;
    TAILQ_HEAD(irlisthead, iperf_interval_results) interval_results;
    void *data;
};

#define COOKIE_SIZE 37        /* size of an ascii uuid */
struct iperf_settings
{
    int domain;               /* AF_INET or AF_INET6 */
    int socket_bufsize;       /* window size for TCP */
    int blksize;              /* size of read/writes (-l) */
    uint32_t rate;            /* target data rate */
    int burst;                /* packets per burst */
    int mss;                  /* for TCP MSS */
    int ttl;                  /* IP TTL option */
    int tos;                  /* type of service bit */
    int flowlabel;            /* IPv6 flow label */
    iperf_size_t bytes;       /* number of bytes to send */
    iperf_size_t blocks;      /* number of blocks (packets) to send */
    char unit_format;         /* -f */
    int num_ostreams;         /* SCTP initmsg settings */
};

struct iperf_test;

#if 0
struct sockaddr_storage {
    unsigned char sa_len;
    unsigned char sa_family;
    unsigned char padding[128];
};
#endif

struct iperf_stream
{
    struct iperf_test* test;

    /* configurable members */
    int local_port;
    int remote_port;
    int socket;
    int id;
    /* XXX: is settings just a pointer to the same struct in iperf_test? if not,
        should it be? */
    struct iperf_settings *settings;    /* pointer to structure settings */

    /* non configurable members */
    struct iperf_stream_result *result;    /* structure pointer to result */
    Timer *send_timer;
    int green_light;
    int buffer_fd;    /* data to send, file descriptor */
    char *buffer;        /* data to send, mmapped */

    /*
     * for udp measurements - This can be a structure outside stream, and
     * stream can have a pointer to this
     */
    int packet_count;
    int omitted_packet_count;
    float jitter;
    // float prev_transit;  //change to timeval because the precision of float is not enough when timeval.sec is large.
    struct timeval prev_transit;
    int outoforder_packets;
    int cnt_error;
    uint32_t target;

    struct sockaddr_storage local_addr;
    struct sockaddr_storage remote_addr;

    int (*rcv) (struct iperf_stream * stream);
    int (*snd) (struct iperf_stream * stream);

    /* chained send/receive routines for -F mode */
    int (*rcv2) (struct iperf_stream * stream);
    int (*snd2) (struct iperf_stream * stream);

//    struct iperf_stream *next;
    SLIST_ENTRY(iperf_stream) streams;

    void *data;
};

struct protocol {
    int id;
    char *name;
    int (*accept)(struct iperf_test *);
    int (*listen)(struct iperf_test *);
    int (*connect)(struct iperf_test *);
    int (*send)(struct iperf_stream *);
    int (*recv)(struct iperf_stream *);
    int (*init)(struct iperf_test *);
    SLIST_ENTRY(protocol) protocols;
};

struct iperf_textline {
    char *line;
    TAILQ_ENTRY(iperf_textline) textlineentries;
};

struct xbind_entry {
    char *name;
    struct addrinfo *ai;
    TAILQ_ENTRY(xbind_entry) link;
};

struct iperf_test
{
    char task_tcb_index;

    char role;                             /* 'c' lient or 's' erver */
    int sender;                           /* client & !reverse or server & reverse */
    int sender_has_retransmits;
    struct protocol *protocol;
    signed char state;
    char *server_hostname;                  /* -c option */
    //char *tmp_template;
    char *bind_address;                     /* first -B option */
    //TAILQ_HEAD(xbind_addrhead, xbind_entry) xbind_addrs; /* all -X opts */
    int bind_port;                        /* --cport option */
    int server_port;
    int omit;                             /* duration of omit period (-O flag) */
    int duration;                         /* total duration of test (-t flag) */
    char *title;                /* -T option */

    int send_multicast;
    char *multicast_addr;

    int ctrl_sck;
    int listener;
    int prot_listener;

    /* boolean variables for Options */
    int daemon;                           /* -D option */
    int one_off;                          /* -1 option */
    int no_delay;                         /* -N option */
    int reverse;                          /* -R option */
    int verbose;                          /* -V option - verbose mode */
    int zerocopy;                         /* -Z option - use sendfile */
    int debug;                /* -d option - enable debug */
    int udp_counters_64bit;        /* --use-64-bit-udp-counters */

    int multisend;

    /* Select related parameters */
    int    max_fd;
    fd_set read_set;                         /* set of read sockets */
    fd_set write_set;                        /* set of write sockets */

    /* Interval related members */
    int    omitting;
    float  stats_interval;
    float  reporter_interval;
    void   (*stats_callback) (struct iperf_test *);
    void   (*reporter_callback) (struct iperf_test *);
    Timer  *omit_timer;
    Timer  *timer;
    int     done;
    Timer  *stats_timer;
    Timer  *reporter_timer;

    float  cpu_util[3];                            /* cpu utilization of the test - total, user, system */
    float  remote_cpu_util[3];                     /* cpu utilization for the remote host/client - total, user, system */

    int num_streams;                      /* total streams in the test (-P) */

    iperf_size_t bytes_sent;
    iperf_size_t blocks_sent;
    char cookie[COOKIE_SIZE];
    // struct iperf_stream *streams;               /* pointer to list of struct stream */
    SLIST_HEAD(slisthead, iperf_stream) streams;
    struct iperf_settings *settings;

    SLIST_HEAD(plisthead, protocol) protocols;

    /* callback functions */
    void (*on_new_stream)(struct iperf_stream *);
    void (*on_test_start)(struct iperf_test *);
    void (*on_connect)(struct iperf_test *);
    void (*on_test_finish)(struct iperf_test *);

};

/* default settings */
#define PORT        5201  /* default port to listen on (don't use the same port as iperf2) */
#define uS_TO_NS    1000
#define SEC_TO_US   1000000LL
#define SEC_TO_US_FLOAT   1000000.0
#define UDP_RATE    (1024 * 1024) /* 1 Mbps */
#define OMIT        0 /* seconds */
#define DURATION    10 /* seconds */

#define SEC_TO_NS           1000000000LL    /* too big for enum/const on some platforms */
#define MAX_RESULT_STRING   4096

/* constants for command line arg sanity checks */
#define MB                  (1024 * 1024)
#define MAX_TCP_BUFFER      (512 * MB)
#define MAX_BLOCKSIZE       MB
/* Maximum size UDP send is (64K - 1) - IP and UDP header sizes */
#define MAX_UDP_BLOCKSIZE   (65535 - 8 - 20)
#define MIN_INTERVAL        0.1
#define MAX_INTERVAL        60.0
#define MAX_TIME            7 * 86400 // 7 days
#define MAX_BURST           1000
#define MAX_MSS             (9 * 1024)
#define MAX_STREAMS         128

#endif /* !__IPERF_H */
