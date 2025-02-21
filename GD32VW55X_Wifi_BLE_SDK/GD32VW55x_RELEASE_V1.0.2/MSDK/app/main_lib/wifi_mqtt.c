#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "wifi_mqtt.h"

#include "gd32vw55x.h"           // For RTC, etc.
#include "gd32vw55x_rtc.h"
#include "wrapper_os.h"
#include "dbg_print.h"

// Wi-Fi includes
#include "wifi_management.h"

// MQTT command includes
#include "mqtt_cmd.h"
#include "lwip/apps/mqtt.h"
extern mqtt_client_t *mqtt_client; // declared in mqtt_cmd.c

// SNTP includes
#include "lwip/apps/sntp.h"

// OTA includes
#include "ota_demo.h"

// For the hardware MAC
#include "app_adapter_mgr.h"

//for software reset
#include "gd32vw55x_fwdgt.h"


bool resetAfterOTA = false;

// We keep these global so they're accessible from wifi_reconnect_task
char g_primary_ssid[64]     = "Mobiliz NaRGE_Guest";
char g_primary_password[64] = "M@b!lz12.*";
char g_backup_ssid[64]      = "Redmi Note 10S";
char g_backup_password[64]  = "54321012";

// Wi-Fi status
static volatile wifi_status_t s_wifi_status = WIFI_DISCONNECTED;

wifi_status_t wifi_get_status(void)
{
    return s_wifi_status;
}

void set_wifi_status(wifi_status_t new_status)
{
    s_wifi_status = new_status;
}

// Attempt connect to single SSID
bool wifi_try_connect(const char *ssid, const char *pw)
{
    printf("[WIFI] Trying connect => %s\r\n", ssid);
    int ret = wifi_management_connect((char*)ssid, (char*)pw, /*blocked=*/1);
    if (ret == 0) {
        printf("[WIFI] Connect OK => %s\r\n", ssid);
        return true;
    }
    printf("[WIFI] Connect FAIL => %s (err=%d)\r\n", ssid, ret);
    return false;
}

// Called once to attempt station connect (primary or backup)
void wifi_connect_handler(void)
{
    if (wifi_try_connect(g_primary_ssid, g_primary_password) ||
        wifi_try_connect(g_backup_ssid,  g_backup_password)) {
        set_wifi_status(WIFI_CONNECTED);
        printf("[WIFI] Connected.\r\n");
    } else {
        set_wifi_status(WIFI_DISCONNECTED);
        printf("[WIFI] Offline.\r\n");
    }
}

// --------------------------------------------------------------------
// MQTT broker config from original
// --------------------------------------------------------------------
#define MQTT_BROKER_HOST  "test.mosquitto.org"
#define MQTT_BROKER_PORT  "1883"
#define MQTT_BROKER_ENC   "0"  // 0 => no TLS
#define MQTT_TOPIC        "test22052077/ble_beacons"

void my_mqtt_cmd_connect(void)
{
    char *args[] = {
        "mqtt", "connect",
        (char *)MQTT_BROKER_HOST,
        (char *)MQTT_BROKER_PORT,
        (char *)MQTT_BROKER_ENC
    };
    cmd_mqtt(5, args);
}

void my_mqtt_cmd_disconnect(void)
{
    char *args[] = { "mqtt", "disconnect" };
    cmd_mqtt(2, args);
}

void my_mqtt_cmd_publish(const char *payload)
{
    char *args[] = {
        "mqtt", "publish",
        (char *)MQTT_TOPIC,
        (char *)payload,
        "0",  // QoS=0
        "0"   // Retain=0
    };
    cmd_mqtt(6, args);
}

bool mqtt_client_is_connected_ok(void)
{
    if (!mqtt_client) return false;
    return mqtt_client_is_connected(mqtt_client);
}



void system_reset(void)
{
    /* Enable write access to the watchdog */
    fwdgt_write_enable();

    /* Set the watchdog timeout to its maximum value (to ensure reset) */
    fwdgt_config(0x0FFF, FWDGT_PSC_DIV64);

    /* Start the watchdog timer */
    fwdgt_enable();

    /* Wait for reset (this will not return) */
    while(1);
}


void start_ota_demo(void)
{
#define OTA_SERVER_IP  "91.93.129.88"
#define OTA_IMAGE_URL  "/kazim/gigadevice/image-ota222.bin"

if(ota_demo_cfg_init(OTA_SERVER_IP, OTA_IMAGE_URL) == 0){
    printf("[OTA] Starting OTA => IP=%s, URL=%s\r\n", OTA_SERVER_IP, OTA_IMAGE_URL);
    if(ota_demo_start() != 0){
        printf("[OTA] ota_demo_start() failed.\r\n");
    	resetAfterOTA = false;
    }
} else {
	resetAfterOTA = false;
    printf("[OTA] ota_demo_cfg_init() failed.\r\n");
}

	if(resetAfterOTA){
	    system_reset();
	}

}

// --------------------------------------------------------------------
// SNTP + RTC
// --------------------------------------------------------------------
static uint8_t to_bcd(uint8_t dec) {
    return (uint8_t)((((dec / 10) & 0xF) << 4) | (dec % 10));
}

void set_rtc_by_epoch_time(uint32_t utc_secs)
{
    uint32_t days = utc_secs / 86400;
    uint32_t remain = utc_secs % 86400;
    uint32_t hour = remain / 3600;  remain %= 3600;
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
        if (i==1 && is_leap) dm=29;
        if (days >= dm){
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

void start_sntp_and_sync(void)
{
    if(!sntp_enabled()){
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
    }
    // Let time sync
    sys_ms_sleep(8000);
}

