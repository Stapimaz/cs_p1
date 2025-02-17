/**
 * main.c
 *
 * Demonstrates:
 *  1) OTA firmware update support via ota_demo.c
 *  2) Offline data storage in flash (only dynamic fields to save space)
 *  3) Re-sending offline data upon Wi-Fi *or MQTT* reconnect
 *  4) **A concurrent SoftAP that serves a simple HTML page** to configure
 *     PRIMARY/SECONDARY Wi-Fi SSID/PASS in real time.
 *
 * Key features retained:
 *  - Connects to Wi-Fi (station mode)
 *  - Syncs time via SNTP
 *  - Connects to broker test.mosquitto.org (MQTT)
 *  - BLE scanning => either publishes immediately or saves offline
 *  - Reconnect logic tries every 5 minutes if Wi-Fi is disconnected (modified to 10s for first time)
 *  - If Wi-Fi is up but MQTT is disconnected => reconnect MQTT
 *  - Optional OTA logic
 *  - Stores only dynamic fields in flash for offline use
 *  - **SoftAP + basic HTTP server** for on-the-fly Wi-Fi config
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "gd32vw55x_platform.h"
#include "wrapper_os.h"

// Wi-Fi includes
#include "wifi_init.h"
#include "wifi_management.h"
#include "dbg_print.h"

// BLE includes
#include "ble_init.h"
#include "app_scan_mgr.h"

// MQTT command includes
#include "mqtt_cmd.h"
#include "lwip/apps/mqtt.h"
extern mqtt_client_t *mqtt_client; // declared in mqtt_cmd.c

// SNTP includes
#include "lwip/apps/sntp.h"

// RTC
#include "gd32vw55x_rtc.h"
#include <time.h>

// Flash raw API for storing offline data
#include "raw_flash_api.h"

// OTA includes (enabled if CONFIG_OTA_DEMO_SUPPORT is defined)
#include "ota_demo.h"

// Adapter manager includes, needed for app_get_local_mac()
#include "app_adapter_mgr.h"

// LWIP includes for sockets
#include "lwip/sockets.h"
#include "lwip/errno.h"

// --------------------------------------------------------------------
// Wi-Fi config (changeable at runtime via the new AP/HTML page)
// --------------------------------------------------------------------
static char g_primary_ssid[64]     = "Mobiliz NaRGE_Guest";
static char g_primary_password[64] = "M@b!lz12.*";
static char g_backup_ssid[64]      = "Redmi Note 10S";
static char g_backup_password[64]  = "54321012";

#define PRIMARY_WIFI_SSID        g_primary_ssid
#define PRIMARY_WIFI_PASSWORD    g_primary_password
#define BACKUP_WIFI_SSID         g_backup_ssid
#define BACKUP_WIFI_PASSWORD     g_backup_password

// --------------------------------------------------------------------
// MQTT config
// --------------------------------------------------------------------
#define MQTT_BROKER_HOST   "test.mosquitto.org"
#define MQTT_BROKER_PORT   "1883"
#define MQTT_BROKER_ENC    "0"   // 0 => no TLS
#define MQTT_TOPIC         "test22052077/ble_beacons"

// --------------------------------------------------------------------
// OTA config placeholders
// --------------------------------------------------------------------
#ifdef CONFIG_OTA_DEMO_SUPPORT
    #define OTA_SERVER_IP  "91.93.129.88"
    #define OTA_IMAGE_URL  "/kazim/gigadevice/image-ota222.bin"
#endif

// --------------------------------------------------------------------
// Our Wi-Fi status
// --------------------------------------------------------------------
typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED    = 1
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_DISCONNECTED;

/**
 * We make this global so the HTTP handler can reset it immediately
 * if the user triggers a manual Wi-Fi reconnect.
 */
bool g_first_offline_retry = true; // used by wifi_reconnect_task()

// --------------------------------------------------------------------
// Store Gateway MAC in Flash
// --------------------------------------------------------------------
#define GW_MAC_FLASH_ADDR  (0x002F0000U)    // Example sector, adjust if needed
#define GW_MAC_FLASH_SIZE  (0x1000U)        // We'll assume a 4KB sector here
#define GW_MAC_FLASH_MAGIC (0xA5123456UL)

static int ONLINE_SCAN_INTERVAL = 7000;
static int OFFLINE_SCAN_INTERVAL = 14000;

typedef struct {
    uint32_t magic;         // Marker to confirm we've stored
    char     mac_str[18];   // "AA:BB:CC:DD:EE:FF"
} gw_mac_store_t;

static char g_gateway_mac[18] = {0};

static void gw_mac_read_from_flash(void)
{
    gw_mac_store_t info;
    raw_flash_read(GW_MAC_FLASH_ADDR, &info, sizeof(info));

    if ((info.magic == GW_MAC_FLASH_MAGIC) && (info.mac_str[0] != '\0')) {
        strncpy(g_gateway_mac, info.mac_str, sizeof(g_gateway_mac));
    } else {
        g_gateway_mac[0] = '\0';
    }
}

static void gw_mac_write_to_flash(const char* mac_str)
{
    // Erase the 4KB sector first
    raw_flash_erase(GW_MAC_FLASH_ADDR, GW_MAC_FLASH_SIZE);

    gw_mac_store_t info;
    info.magic = GW_MAC_FLASH_MAGIC;
    memset(info.mac_str, 0, sizeof(info.mac_str));
    strncpy(info.mac_str, mac_str, sizeof(info.mac_str) - 1);

    raw_flash_write(GW_MAC_FLASH_ADDR, &info, sizeof(info));
    printf("[GW MAC FLASH] MAC '%s' stored at 0x%08X.\r\n", mac_str, (unsigned)GW_MAC_FLASH_ADDR);
}

static void init_gw_mac(void)
{
    uint8_t mac[6];
    app_get_local_mac(mac);  // from hardware

    char temp[18];
    snprintf(temp, sizeof(temp),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

    printf("[GW MAC] (Hardware) => %s\r\n", temp);
    strncpy(g_gateway_mac, temp, sizeof(g_gateway_mac));

    // Overwrite each time for simplicity
    gw_mac_write_to_flash(g_gateway_mac);
}

// --------------------------------------------------------------------
// RTC + SNTP
// --------------------------------------------------------------------
static uint8_t to_bcd(uint8_t dec) {
    return (uint8_t)((((dec / 10) & 0xF) << 4) | (dec % 10));
}

void set_rtc_by_epoch_time(uint32_t utc_secs)
{
    uint32_t days = utc_secs / 86400;
    uint32_t remain = utc_secs % 86400;
    uint32_t hour = remain / 3600;    remain %= 3600;
    uint32_t minute = remain / 60;
    uint32_t second = remain % 60;

    uint32_t year = 1970;
    while (1) {
        uint32_t ydays = 365;
        if ((year % 4 == 0 && (year % 100 != 0)) || (year % 400 == 0)) {
            ydays = 366;
        }
        if (days >= ydays) {
            days -= ydays;
            year++;
        } else break;
    }

    static const uint8_t md[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint8_t month = 0;
    uint8_t is_leap = ((year%4==0 && year%100!=0)||(year%400==0)) ? 1:0;
    for (int i=0; i<12; i++){
        uint8_t dm = md[i];
        if(i==1 && is_leap) dm=29;
        if(days >= dm){
            days -= dm;
            month++;
        } else break;
    }
    month++;
    uint8_t day = days + 1;

    uint16_t yoffs = (uint16_t)(year - 2000);
    if(yoffs>99) yoffs=99;

    rtc_parameter_struct r;
    memset(&r, 0, sizeof(r));
    r.year          = to_bcd((uint8_t)yoffs);
    r.month         = to_bcd(month);
    r.date          = to_bcd(day);
    r.day_of_week   = 1; // not used
    r.hour          = to_bcd((uint8_t)hour);
    r.minute        = to_bcd((uint8_t)minute);
    r.second        = to_bcd((uint8_t)second);

    r.factor_asyn    = 0x63;
    r.factor_syn     = 0x13F;
    r.display_format = RTC_24HOUR;
    r.am_pm          = RTC_AM;

    rtc_init(&r);
}

uint32_t get_current_epoch_time(void)
{
    rtc_parameter_struct t;
    rtc_current_time_get(&t);

    uint16_t year = ((t.year>>4)*10 + (t.year&0x0F)) + 2000;
    uint8_t  mon  = ((t.month>>4)*10 + (t.month&0x0F));
    uint8_t  day  = ((t.date >>4)*10 + (t.date &0x0F));
    uint8_t  hr   = ((t.hour >>4)*10 + (t.hour &0x0F));
    uint8_t  min  = ((t.minute>>4)*10 + (t.minute&0x0F));
    uint8_t  sec  = ((t.second>>4)*10 + (t.second&0x0F));

    static const uint16_t mdays[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

    uint8_t leap = (mon>2 && (((year%4==0)&&(year%100!=0))||(year%400==0))) ? 1:0;
    uint32_t days_since_1970 = (year-1970)*365 + ((year-1969)/4)
                  + mdays[mon-1] + (day-1) + leap;

    uint32_t epoch = days_since_1970*86400 + hr*3600 + min*60 + sec;
    return epoch;
}

static void start_sntp_and_sync(void)
{
    if(!sntp_enabled()){
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
    }
    // Let time sync
    sys_ms_sleep(8000);
}

// --------------------------------------------------------------------
// Basic Wi-Fi connect logic
// --------------------------------------------------------------------
static bool wifi_try_connect(const char *ssid, const char *pw)
{
    printf("[WIFI] Trying connect => %s\r\n", ssid);
    int ret = wifi_management_connect((char*)ssid, (char*)pw, /*blocked=*/1);
    if (ret == 0){
        printf("[WIFI] Connect OK => %s\r\n", ssid);
        return true;
    }
    printf("[WIFI] Connect FAIL => %s (err=%d)\r\n", ssid, ret);
    return false;
}

static void wifi_connect_handler(void)
{
    if(wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
       wifi_try_connect(BACKUP_WIFI_SSID,  BACKUP_WIFI_PASSWORD)) {
        wifi_status = WIFI_CONNECTED;
        printf("[WIFI] Connected.\r\n");
    } else {
        wifi_status = WIFI_DISCONNECTED;
        printf("[WIFI] Offline.\r\n");
    }
}

// --------------------------------------------------------------------
// cmd_mqtt usage wrappers
// --------------------------------------------------------------------
static void my_mqtt_cmd_connect(void)
{
    char *args[] = {
        "mqtt", "connect",
        (char *)MQTT_BROKER_HOST,
        (char *)MQTT_BROKER_PORT,
        (char *)MQTT_BROKER_ENC
    };
    cmd_mqtt(5, args);
}

static void my_mqtt_cmd_disconnect(void)
{
    char *args[] = { "mqtt", "disconnect" };
    cmd_mqtt(2, args);
}

static void my_mqtt_cmd_publish(const char *payload)
{
    char *args[] = {
        "mqtt", "publish",
        (char*)MQTT_TOPIC,
        (char*)payload,
        "0",  // QoS=0
        "0"   // Retain=0
    };
    cmd_mqtt(6, args);
}

// --------------------------------------------------------------------
// OTA DEMO
// --------------------------------------------------------------------
#ifdef CONFIG_OTA_DEMO_SUPPORT
static void start_ota_demo(void)
{
    if(ota_demo_cfg_init(OTA_SERVER_IP, OTA_IMAGE_URL) == 0){
        printf("[OTA] Starting OTA Demo => IP=%s, URL=%s\r\n", OTA_SERVER_IP, OTA_IMAGE_URL);
        if(ota_demo_start() != 0){
            printf("[OTA] ota_demo_start() failed.\r\n");
        }
    } else {
        printf("[OTA] ota_demo_cfg_init() failed.\r\n");
    }
}
#endif

// --------------------------------------------------------------------
// Offline Flash Storage
// --------------------------------------------------------------------
#define OFFLINE_DATA_BASE   (0x00320000)
#define OFFLINE_DATA_SIZE   (0x000C0000)  // 768KB

static bool offline_initialized = false;
static uint32_t s_ram_offset = 0;  // Our in-RAM offset for new records.

#define OFFLINE_MAGIC_VALID  0xA5A5A5A5

typedef struct __attribute__((packed)) {
    uint8_t  peer_mac[6];
    uint8_t  device_type;
    uint32_t msg_counter;
    int16_t  temperature;
    int16_t  humidity;
    uint16_t battery;
    uint8_t  reed_relay;
    uint8_t  accel;
    uint8_t  offline;
    uint32_t timestamp;
} offline_record_t;

static void offline_data_erase(void)
{
    sys_ms_sleep(1500);
    raw_flash_erase(OFFLINE_DATA_BASE, OFFLINE_DATA_SIZE);
    uint32_t init_data[2];
    init_data[0] = OFFLINE_MAGIC_VALID;
    init_data[1] = 0;
    sys_ms_sleep(2000);
    raw_flash_write(OFFLINE_DATA_BASE, init_data, sizeof(init_data));
    s_ram_offset = 0;
    printf("[FLASH] Erased offline region => offset=0.\r\n");
}

static void offline_data_init(void)
{
    uint32_t init_data[2];
    raw_flash_read(OFFLINE_DATA_BASE, init_data, sizeof(init_data));
    uint32_t magic_in_flash = init_data[0];
    uint32_t flash_offset   = init_data[1];

    if(magic_in_flash != OFFLINE_MAGIC_VALID) {
        printf("[FLASH] Offline storage is uninitialized => erasing region.\r\n");
        offline_data_erase();
    } else {
        s_ram_offset = flash_offset;
        printf("[FLASH] Offline region valid, stored offset=%u\r\n", (unsigned)s_ram_offset);
    }
    offline_initialized = true;
}

static void save_offline_record(const offline_record_t *rec)
{
    if(!offline_initialized){
        offline_data_init();
    }
    const uint32_t record_size = sizeof(offline_record_t);
    const uint32_t needed = 4 + record_size; // length + record

    if(s_ram_offset + needed > (OFFLINE_DATA_SIZE - 8)) {
        printf("[FLASH] Not enough space for offline record. Erasing everything.\r\n");
        offline_data_erase();
    }

    uint32_t record_addr = OFFLINE_DATA_BASE + 8 + s_ram_offset;
    raw_flash_write(record_addr, &record_size, 4);
    record_addr += 4;
    raw_flash_write(record_addr, rec, record_size);

    s_ram_offset += needed;
    printf("[FLASH] Offline record saved => offset now=%u\r\n", (unsigned)s_ram_offset);
}

static void offline_flush_to_mqtt(void)
{
    if(!offline_initialized){
        offline_data_init();
    }
    if(s_ram_offset == 0){
        printf("[FLASH] No offline data stored.\r\n");
        return;
    }

    printf("[FLASH] Found %u bytes of offline data => flushing...\r\n",(unsigned)s_ram_offset);
    uint32_t read_pos = 0;

    // Delay before starting flush
    sys_ms_sleep(1000);  // Give MQTT connection time to stabilize

    while(read_pos < s_ram_offset) {
        // Verify MQTT is still connected before each record
        if(!mqtt_client || !mqtt_client_is_connected(mqtt_client)) {
            printf("[FLASH] MQTT disconnected during flush - aborting\r\n");
            return;
        }

        uint32_t length = 0;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &length, 4);
        read_pos += 4;

        if(length == 0 || length == 0xFFFFFFFF) {
            break;
        }
        if(read_pos + length > s_ram_offset) {
            break;
        }
        if(length != sizeof(offline_record_t)) {
            printf("[FLASH] Found record with invalid length=%u => erasing region.\r\n",(unsigned)length);
            offline_data_erase();
            return;
        }

        offline_record_t rec;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &rec, length);
        read_pos += length;

        char json_payload[256];
        snprintf(json_payload, sizeof(json_payload),
                 "{"
                 "\"MAC\":\"%02X:%02X:%02X:%02X:%02X:%02X\","
                 "\"GW_MAC\":\"%s\","
                 "\"DeviceType\":%u,"
                 "\"MessageCounter\":%u,"
                 "\"Temperature\":%d,"
                 "\"Humidity\":%d,"
                 "\"BatteryVoltage\":%u,"
                 "\"ReedRelay\":%s,"
                 "\"Accelerometer\":%u,"
                 "\"Timestamp\":%lu"
                 "}",
                 rec.peer_mac[5], rec.peer_mac[4], rec.peer_mac[3],
                 rec.peer_mac[2], rec.peer_mac[1], rec.peer_mac[0],
                 g_gateway_mac,
                 rec.device_type,
                 rec.msg_counter,
                 rec.temperature,
                 rec.humidity,
                 rec.battery,
                 (rec.reed_relay ? "true" : "false"),
                 rec.accel,
                 (unsigned long)rec.timestamp
        );

        printf("[FLASH] Publishing offline record: %s\r\n", json_payload);
        my_mqtt_cmd_publish(json_payload);

        // Delay between records
        sys_ms_sleep(100);
    }

    // Delay before erasing
    sys_ms_sleep(1500);
    offline_data_erase();

    // Final delay after erase
    sys_ms_sleep(1000);
    printf("[FLASH] Offline data flush complete.\r\n");
}

// --------------------------------------------------------------------
// Wi-Fi reconnect => tries every 5 min if offline.
// first time offline => wait 10s, subsequent => 5 min
// --------------------------------------------------------------------
static void wifi_reconnect_task(void *arg)
{
    (void)arg;

    while(1) {
        if(wifi_status == WIFI_DISCONNECTED) {
            if(g_first_offline_retry) {
                printf("[RECONNECT] Offline => next attempt in 10 seconds.\r\n");
                sys_ms_sleep(10000);
                g_first_offline_retry = false; // after doing the 10s attempt
            } else {
                printf("[RECONNECT] Offline => next attempt in 5 mins.\r\n");
                sys_ms_sleep(5UL*60UL*1000UL);
            }

            if(wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
               wifi_try_connect(BACKUP_WIFI_SSID,  BACKUP_WIFI_PASSWORD)){
                wifi_status = WIFI_CONNECTED;
                printf("[RECONNECT] Wi-Fi reconnected!\r\n");

                sys_ms_sleep(500);
                start_sntp_and_sync();

                my_mqtt_cmd_disconnect();
                sys_ms_sleep(1000);
                my_mqtt_cmd_connect();

                if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
                    offline_flush_to_mqtt();
                }
#ifdef CONFIG_OTA_DEMO_SUPPORT
                start_ota_demo();
#endif
            } else {
                printf("[RECONNECT] Still offline.\r\n");
            }
        }

        // If Wi-Fi is up but MQTT is disconnected => reconnect
        if(wifi_status == WIFI_CONNECTED && (!(mqtt_client && mqtt_client_is_connected(mqtt_client)))) {
            printf("[RECONNECT] MQTT disconnected => reconnecting...\r\n");
            my_mqtt_cmd_disconnect();
            sys_ms_sleep(1000);
            my_mqtt_cmd_connect();
            if(mqtt_client && mqtt_client_is_connected(mqtt_client)) {
                offline_flush_to_mqtt();
#ifdef CONFIG_OTA_DEMO_SUPPORT
                start_ota_demo();
#endif
            }
        }

        // Check if Wi-Fi is still connected
        extern struct wifi_vif_tag wifi_vif_tab[];
        #define WIFI_STA_STATE_CONNECTED 6
        if(wifi_status == WIFI_CONNECTED && wifi_vif_tab[0].sta.state != WIFI_STA_STATE_CONNECTED) {
            printf("[RECONNECT] Wi-Fi dropped.\r\n");
            wifi_status = WIFI_DISCONNECTED;
            sntp_stop();
            my_mqtt_cmd_disconnect();
        }

        sys_ms_sleep(10000);
    }
}

// --------------------------------------------------------------------
// Helper for URL decoding
// Decodes %xx to a character, and + to space, in-place
// --------------------------------------------------------------------
static int hex_val(char c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return -1;
}

// Decodes in-place: e.g. "M%40b%21lz" -> "M@b!lz"
static void url_decode_in_place(char *s)
{
    char *p = s, *q = s;
    while (*p) {
        if (*p == '+') {
            // + becomes space
            *q++ = ' ';
            p++;
        } else if (*p == '%' && p[1] && p[2]) {
            int hi = hex_val(p[1]);
            int lo = hex_val(p[2]);
            if (hi >= 0 && lo >= 0) {
                *q++ = (char)((hi << 4) | lo);
                p += 3;
            } else {
                // malformed, just copy as-is
                *q++ = *p++;
            }
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
}

// --------------------------------------------------------------------
// Minimal HTTP server for SoftAP
// --------------------------------------------------------------------
static void parse_url_params(const char *query,
                             char *p_ssid, size_t p_ssid_len,
                             char *p_pass, size_t p_pass_len,
                             char *b_ssid, size_t b_ssid_len,
                             char *b_pass, size_t b_pass_len)
{
    #define SCAN_PARAM(q, name, dest, maxlen) do {         \
        char *p = strstr((q), name "=");                   \
        if(p){                                             \
            p += strlen(name) + 1;                         \
            size_t i=0;                                    \
            while(*p && *p!='&' && i<(maxlen-1)){          \
                if(*p == '+') dest[i] = ' ';               \
                else dest[i] = *p;                         \
                i++; p++;                                  \
            }                                              \
            dest[i] = '\0';                                \
        }                                                  \
    } while(0)

    SCAN_PARAM(query, "p_ssid",  p_ssid,  p_ssid_len);
    SCAN_PARAM(query, "p_pass",  p_pass,  p_pass_len);
    SCAN_PARAM(query, "b_ssid",  b_ssid,  b_ssid_len);
    SCAN_PARAM(query, "b_pass",  b_pass,  b_pass_len);

    // Now do a proper decode of any '%xx' sequences
    url_decode_in_place(p_ssid);
    url_decode_in_place(p_pass);
    url_decode_in_place(b_ssid);
    url_decode_in_place(b_pass);

    #undef SCAN_PARAM
}

// Here is the actual HTML form with {{MAC}} placeholder
static const char html_form[] =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n\r\n"
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<title>MZ-GW Wi-Fi Config</title>"
"<style>"
"  body { background: #fafafa; font-family: Arial, sans-serif; padding:20px; }"
"  .container { background: #fff; padding: 20px; border-radius: 6px; max-width:400px; margin:auto; text-align:center; }"
"  .container h2 { margin-top:0; }"
"  .logo { display:block; margin:20px auto; max-width:200px; }"
"  label { font-weight:bold; display:block; text-align:left; margin:10px 0 5px 0; }"
"  input { display:block; margin:0 auto 10px auto; width:90%; padding:8px; }"
"  .btn { background:#90ee90; color:#000; border:none; padding:10px 20px; cursor:pointer; display:block; margin:20px auto 0 auto; }"
"  .btn:hover { background:#80dd80; }"
"</style>"
"</head>"
"<body>"
"<div class='container'>"
"<svg class='logo' id='Layer_1' data-name='Layer 1' xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1496 486'>"
"<title>tr</title>"
"<circle cx='253.73' cy='249' r='208.82' style='fill:transparent'/>"
"<path d='M510.88,186.59H659q32,0,46.28,12.56T719.6,240v92.67H680.78V245.12c0-10.64-1.59-17.71-4.85-21.16s-9.9-5.23-20-5.23H634.23V332.67H594.87V218.73H549.68V332.67h-38.8Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M785.05,259.7c0,13.76,3.29,24.61,9.72,32.42s15.64,11.67,27.26,11.67,20.57-3.86,27.15-11.67,9.91-18.66,9.91-32.42-3.26-24.6-9.82-32.38S833.63,215.67,822,215.67s-20.58,3.87-27.14,11.65S785.05,245.89,785.05,259.7Zm114.51,0q0,34.41-20.92,54.91c-14,13.64-32.71,20.47-56.39,20.47s-42.41-6.83-56.55-20.56-21.11-32-21.11-54.82,7.07-41.12,21.11-54.93,33-20.71,56.55-20.71c23.4,0,42.07,6.86,56.2,20.62S899.56,236.8,899.56,259.73Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M961.94,133.64v53H995c14.57,0,26.32,1.72,35.17,5.2A53.8,53.8,0,0,1,1053,209.05a69.89,69.89,0,0,1,11.83,22.16,83.74,83.74,0,0,1,4,26.56q0,35.09-18.75,55t-52.42,19.9H923.36v-199Zm23.54,84.53H961.94v83.19h26.54c14.61,0,25-3.12,30.93-9.3s9-16.64,9-31.4c0-15.16-3.21-26-9.88-32.6S1000.89,218.17,985.48,218.17Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<rect x='1092.52' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<rect x='1165.98' y='133.64' width='40.47' height='199.03' style='fill:#231f20'/>"
"<rect x='1239.58' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<path d='M1237.58,146.14A21.85,21.85,0,1,1,1259.41,168,21.85,21.85,0,0,1,1237.58,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M1090.9,146.14A21.85,21.85,0,1,1,1112.73,168,21.86,21.86,0,0,1,1090.9,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<polygon points='1313.04 301.39 1399.54 218.17 1313.04 218.17 1313.04 186.59 1450.05 186.59 1450.05 218.11 1363.56 301.36 1450.05 301.36 1450.05 332.67 1313.04 332.67 1313.04 301.39' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M301.93,45.78a166.5,166.5,0,0,1-3.78,319.35,4.93,4.93,0,0,1-2.75-9.46A138.83,138.83,0,0,0,243.07,85c-69,4.77-124.62,61-128.72,130a138.88,138.88,0,0,0,94.76,140.23,4.91,4.91,0,0,1,3.4,4.65V360a4.92,4.92,0,0,1-6.23,4.77A166.51,166.51,0,0,1,87.09,190.23C92.77,124,138.1,68.5,198.8,47.49,110.12,71.61,44.91,152.68,44.91,249c0,105.39,78.07,192.53,179.53,206.77V286.61a49,49,0,0,0,57.55,0v169.3C384,442.11,462.55,354.74,462.55,249,462.55,150.27,394,67.55,301.93,45.78ZM210.18,314.47a4.78,4.78,0,0,1,2.33,4.08v.16a4.84,4.84,0,0,1-7.06,4.3,103.7,103.7,0,0,1-55.39-103.74c5.21-47.71,43.59-86.18,91.3-91.47a103.8,103.8,0,0,1,57.55,196.32,4.82,4.82,0,0,1-4.52-8.51,82.5,82.5,0,0,0-50.58-153.46c-38.93,4.3-69.91,36.53-72.86,75.59A82.38,82.38,0,0,0,210.18,314.47Zm43-33.79A28.78,28.78,0,1,1,282,251.9,28.78,28.78,0,0,1,253.22,280.68Z' style='fill:#3dae2b'/>"
"</svg>"
"<h2>Wi-Fi Config<br>MZ-GW/{{MAC}}</h2>"
"<form method='GET' action='/set'>"
"  <label>Primary SSID:</label>"
"  <input type='text' name='p_ssid'>"
"  <label>Primary PASS:</label>"
"  <input type='text' name='p_pass'>"
"  <label>Backup SSID:</label>"
"  <input type='text' name='b_ssid'>"
"  <label>Backup PASS:</label>"
"  <input type='text' name='b_pass'>"
"  <input class='btn' type='submit' value='Save & Reconnect'/>"
"</form>"
"</div>"
"</body></html>\r\n";

static void http_server_handle_request(int fd)
{
    char recv_buf[512];
    int ret = recv(fd, recv_buf, sizeof(recv_buf)-1, 0);
    if(ret <= 0) {
        return;
    }
    recv_buf[ret] = '\0';

    if(strncmp(recv_buf, "GET /set?", 9)==0) {
        // 1) Extract the query
        char *qs = &recv_buf[9];
        char *end = strstr(qs, " ");
        if(end) *end = '\0';

        // 2) Parse parameters
        char new_p_ssid[64]  = {0};
        char new_p_pass[64]  = {0};
        char new_b_ssid[64]  = {0};
        char new_b_pass[64]  = {0};

        parse_url_params(qs,
                         new_p_ssid, sizeof(new_p_ssid),
                         new_p_pass, sizeof(new_p_pass),
                         new_b_ssid, sizeof(new_b_ssid),
                         new_b_pass, sizeof(new_b_pass));

        // 3) Update global SSID/PASS
        if(strlen(new_p_ssid)>0) {
            strncpy(g_primary_ssid, new_p_ssid, sizeof(g_primary_ssid));
        }
        if(strlen(new_p_pass)>0) {
            strncpy(g_primary_password, new_p_pass, sizeof(g_primary_password));
        }
        if(strlen(new_b_ssid)>0) {
            strncpy(g_backup_ssid, new_b_ssid, sizeof(g_backup_ssid));
        }
        if(strlen(new_b_pass)>0) {
            strncpy(g_backup_password, new_b_pass, sizeof(g_backup_password));
        }

        printf("[HTTP] Updating Wi-Fi config => disconnect + reconnect...\r\n");
        // 4) Manual disconnect+connect to new config
        my_mqtt_cmd_disconnect();
        wifi_management_disconnect();
        sys_ms_sleep(2000);

        wifi_connect_handler();

        if(wifi_status == WIFI_CONNECTED) {
            // We do the immediate chain so we're not stuck offline:
            printf("[HTTP] Manual connect success => start SNTP, reconnect MQTT.\r\n");
            start_sntp_and_sync();
            my_mqtt_cmd_disconnect();
            sys_ms_sleep(1000);
            my_mqtt_cmd_connect();

            if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
                offline_flush_to_mqtt();
            }
#ifdef CONFIG_OTA_DEMO_SUPPORT
            start_ota_demo();
#endif
            // Next time we lose Wi-Fi, let's do 10s first again:
            g_first_offline_retry = false;

        } else {
            // Connect failed => ensure we do a short 10s wait next time
            g_first_offline_retry = true;
        }

        // 5) Respond
        const char resp_ok[] =
        "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
        "<html><body><h3>New Wi-Fi config saved. Attempting reconnection now.</h3>"
        "<a href='/'>Back</a>"
        "</body></html>\r\n";
        send(fd, resp_ok, strlen(resp_ok), 0);

    } else {
        // Return the HTML form with dynamic MAC placeholder

        // Copy the entire html_form into a buffer so we can replace {{MAC}}
        char dynamic_html[sizeof(html_form) + 32];
        memset(dynamic_html, 0, sizeof(dynamic_html));
        strncpy(dynamic_html, html_form, sizeof(dynamic_html) - 1);

        // Find {{MAC}} and substitute with g_gateway_mac
        char *title_placeholder = strstr(dynamic_html, "{{MAC}}");
        if (title_placeholder) {
            char temp[32];
            snprintf(temp, sizeof(temp), "%s", g_gateway_mac);

            size_t mac_len = strlen(temp);
            memmove(title_placeholder + mac_len,
                    title_placeholder + 7,
                    strlen(title_placeholder + 7) + 1);
            memcpy(title_placeholder, temp, mac_len);
        }

        // Send to the client
        send(fd, dynamic_html, strlen(dynamic_html), 0);
    }
}

static void softap_http_server_task(void *param)
{
    (void)param;

    // 1) Start SoftAP
    char ap_ssid[32];
    if (strlen(g_gateway_mac) > 0) {
        snprintf(ap_ssid, sizeof(ap_ssid), "MZ-GW/%s", g_gateway_mac);
    } else {
        snprintf(ap_ssid, sizeof(ap_ssid), "MZ-GW/");
    }

    char *ap_password = "12345678";
    uint8_t channel   = 6;
    wifi_ap_auth_mode_t auth_mode = AUTH_MODE_WPA2_WPA3;
    uint8_t is_hidden = 0;

    printf("[HTTP] Starting Wi-Fi SoftAP: %s\r\n", ap_ssid);
    int ret = wifi_management_ap_start(ap_ssid, ap_password, channel, auth_mode, is_hidden);
    if(ret != 0){
        printf("[HTTP] wifi_management_ap_start failed, code=%d\r\n", ret);
        goto exit_ap;
    }
    printf("[HTTP] SoftAP started successfully.\r\n");

    // 2) Start a minimal HTTP server on port 80
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0){
        printf("[HTTP] Socket create error!\r\n");
        goto exit_ap;
    }
    int reuse=1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family      = AF_INET;
    srv_addr.sin_port        = htons(80);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listen_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr))<0){
        printf("[HTTP] Bind error!\r\n");
        goto exit_server;
    }
    if(listen(listen_fd, 4)<0){
        printf("[HTTP] Listen error!\r\n");
        goto exit_server;
    }

    printf("[HTTP] Listening on port 80 (SoftAP) ...\r\n");

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int cli_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addr_len);
        if(cli_fd < 0) {
            if(errno == EAGAIN) {
                continue;
            }
            printf("[HTTP] accept error %d\r\n", errno);
            break;
        }
        http_server_handle_request(cli_fd);
        shutdown(cli_fd, SHUT_RD);
        close(cli_fd);
    }

exit_server:
    if(listen_fd >= 0){
        shutdown(listen_fd, SHUT_RD);
        close(listen_fd);
    }
exit_ap:
    sys_ms_sleep(500);
    wifi_management_ap_stop();
    printf("[HTTP] SoftAP server ended.\r\n");

    sys_task_delete(NULL);
}

// --------------------------------------------------------------------
// The main scanning/publishing task
// --------------------------------------------------------------------
static void demo_task(void *arg)
{
    (void)arg;

    // (A) Read MAC from flash if available
    gw_mac_read_from_flash();

    printf("[demo_task] Launching SoftAP concurrency first...\r\n");

    // Create SoftAP first
    sys_task_create_dynamic(
        (uint8_t*)"softap_http_server_task",
        4096,
        OS_TASK_PRIORITY(0),
        softap_http_server_task,
        NULL
    );

    // Let the SoftAP get started properly
    sys_ms_sleep(2000);

    printf("[demo_task] Now connecting STA...\r\n");
    wifi_connect_handler();  // Attempt station connect

    // If connected => SNTP + MQTT + optional OTA + flush offline
    if(wifi_status==WIFI_CONNECTED){
        start_sntp_and_sync();
        my_mqtt_cmd_connect();
#ifdef CONFIG_OTA_DEMO_SUPPORT
        start_ota_demo();
#endif
        if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
            offline_flush_to_mqtt();
        }
    }

    // Create background reconnect logic
    os_task_t rtask = sys_task_create_dynamic(
            (uint8_t*)"wifi_reconnect_task",
            2048,
            OS_TASK_PRIORITY(0),
            wifi_reconnect_task,
            NULL
    );
    if(!rtask) {
        dbg_print(ERR,"[MAIN] can't create wifi_reconnect_task\r\n");
        for(;;){}
    }

    // We still call init_gw_mac() => writes to flash for future boots
    init_gw_mac();
    sys_ms_sleep(500);

    // BLE scanning + offline logic
    while(1){
        for (uint8_t idx=0; ; idx++){
            dev_info_t* dev = scan_mgr_find_dev_by_idx(idx);
            if(!dev) break;
            if(dev->adv_len < 26) continue;

            // check adv_data[5]==0x4D && adv_data[6]==0x5A => "MZ"
            if(dev->adv_data[5]==0x4D && dev->adv_data[6]==0x5A) {
                printf("[demo_task] Found Target Beacon!\r\n");

                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter = (dev->adv_data[8]<<24)|(dev->adv_data[9]<<16)|
                                       (dev->adv_data[10]<<8)|dev->adv_data[11];
                int16_t temperature  = (int16_t)((dev->adv_data[12]<<8)|dev->adv_data[13]);
                int16_t humidity     = (int16_t)((dev->adv_data[14]<<8)|dev->adv_data[15]);
                uint16_t batt        = (uint16_t)((dev->adv_data[16]<<8)|dev->adv_data[17]);
                bool reed_relay      = dev->adv_data[18]?true:false;
                uint8_t accel        = dev->adv_data[19];

                if(wifi_status==WIFI_CONNECTED && mqtt_client && mqtt_client_is_connected(mqtt_client)) {
                    char json_payload[256];
                    snprintf(json_payload, sizeof(json_payload),
                             "{"
                             "\"MAC\":\"%02X:%02X:%02X:%02X:%02X:%02X\","
                             "\"GW_MAC\":\"%s\","
                             "\"DeviceType\":%u,"
                             "\"MessageCounter\":%u,"
                             "\"Temperature\":%d,"
                             "\"Humidity\":%d,"
                             "\"BatteryVoltage\":%u,"
                             "\"ReedRelay\":%s,"
                             "\"Accelerometer\":%u"
                             "}",
                             dev->peer_addr.addr[5], dev->peer_addr.addr[4],
                             dev->peer_addr.addr[3], dev->peer_addr.addr[2],
                             dev->peer_addr.addr[1], dev->peer_addr.addr[0],
                             g_gateway_mac,
                             device_type,
                             msg_counter,
                             temperature,
                             humidity,
                             batt,
                             reed_relay?"true":"false",
                             accel
                    );
                    my_mqtt_cmd_publish(json_payload);

                    uint32_t t = get_current_epoch_time();
                    printf("TIMESTAMP(Online): %lu\r\n", (unsigned long)t);
                    sys_ms_sleep(100);

                } else {
                    offline_record_t rec;
                    memset(&rec, 0, sizeof(rec));
                    memcpy(rec.peer_mac, dev->peer_addr.addr, 6);

                    rec.device_type = device_type;
                    rec.msg_counter = msg_counter;
                    rec.temperature = temperature;
                    rec.humidity    = humidity;
                    rec.battery     = batt;
                    rec.reed_relay  = (reed_relay ? 1 : 0);
                    rec.accel       = accel;
                    rec.offline     = 1;
                    rec.timestamp   = get_current_epoch_time();

                    save_offline_record(&rec);
                    printf("TIMESTAMP(Offline): %lu\r\n", (unsigned long)rec.timestamp);
                }
            }
        }
        if(wifi_status==WIFI_CONNECTED && mqtt_client && mqtt_client_is_connected(mqtt_client))
            sys_ms_sleep(ONLINE_SCAN_INTERVAL);
        else
            sys_ms_sleep(OFFLINE_SCAN_INTERVAL);
    }

    sys_task_delete(NULL);
}

// --------------------------------------------------------------------
// main => OS init, ble_init, wifi_init, raw_flash_init, start tasks
// --------------------------------------------------------------------
int main(void)
{
    sys_os_init();
    platform_init();

    dbg_print(NOTICE, "Firmware version: 1.0\r\n");

    // BLE init
    ble_init(true);

    // Initialize Wi-Fi (station driver)
    if(wifi_init()){
        dbg_print(ERR,"[MAIN] wifi_init failed!\r\n");
        for(;;){}
    }

    // Enable concurrency mode
    wifi_management_concurrent_set(1);

    // Initialize raw flash
    raw_flash_init();

    // Create the main demo task
    os_task_t handle = sys_task_create_dynamic(
            (uint8_t*)"demo_task",
            4096,
            OS_TASK_PRIORITY(1),
            demo_task,
            NULL
    );
    if(!handle){
        dbg_print(ERR,"[MAIN] can't create demo_task\r\n");
        for(;;){}
    }

    // Start OS
    sys_os_start();
    for(;;){}
    return 0;
}
