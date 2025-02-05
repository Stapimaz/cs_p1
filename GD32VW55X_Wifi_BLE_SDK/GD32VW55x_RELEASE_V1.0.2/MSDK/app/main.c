/**
 * main.c
 *
 * Using cmd_mqtt(...) so that we don't have to do manual DNS or raw lwIP MQTT calls.
 *
 * Key features:
 *  - Connects to Wi-Fi
 *  - Syncs time via SNTP
 *  - Connects to the broker test.mosquitto.org using cmd_mqtt connect
 *  - If offline, tries again in 5 minutes
 *  - BLE scanning => either publishes or saves offline
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
#include "mqtt_cmd.h"          // where cmd_mqtt(...) is declared
#include "lwip/apps/mqtt.h"    // for mqtt_client_is_connected
extern mqtt_client_t *mqtt_client; // declared in mqtt_cmd.c

// SNTP includes
#include "lwip/apps/sntp.h"

// RTC
#include "gd32vw55x_rtc.h"
#include <time.h>

// --------------------------------------------------------------------
// Wi-Fi config
// --------------------------------------------------------------------
#define PRIMARY_WIFI_SSID       "Mobiliz NaRGE_Gues"
#define PRIMARY_WIFI_PASSWORD   "M@b!lz12.*"
#define BACKUP_WIFI_SSID        "Redmi Note 10S"
#define BACKUP_WIFI_PASSWORD    "54321012"

// --------------------------------------------------------------------
// MQTT config
// --------------------------------------------------------------------
#define MQTT_BROKER_HOST   "test.mosquitto.org"
#define MQTT_BROKER_PORT   "1883"
#define MQTT_BROKER_ENC    "0"   // 0 => no TLS

#define MQTT_TOPIC         "test22052077/ble_beacons"

// --------------------------------------------------------------------
// Our Wi-Fi status
// --------------------------------------------------------------------
typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED    = 1
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_DISCONNECTED;


// --------------------------------------------------------------------------
// RTC + SNTP
// --------------------------------------------------------------------------
static uint8_t to_bcd(uint8_t dec) {
    return (uint8_t)((((dec / 10) & 0xF) << 4) | (dec % 10));
}

/** Set RTC from epoch, offset if storing local time. */
void set_rtc_by_epoch_time(uint32_t utc_secs)
{
    // Example: if you want local time GMT+3:
    // utc_secs += 3*3600;

    uint32_t days = utc_secs / 86400;
    uint32_t remain = utc_secs % 86400;
    uint32_t hour = remain / 3600;    remain %= 3600;
    uint32_t minute = remain / 60;
    uint32_t second = remain % 60;

    uint32_t year = 1970;
    while (1) {
        uint32_t ydays = 365;
        // leap year check
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

    // Example prescalers
    r.factor_asyn    = 0x63;
    r.factor_syn     = 0x13F;
    r.display_format = RTC_24HOUR;
    r.am_pm          = RTC_AM;

    rtc_init(&r);
}

/** Return epoch from RTC. */
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

    // if storing local GMT+3 in RTC:
    // epoch += 3*3600;
    return epoch;
}

/** Start SNTP, wait ~8 seconds. */
static void start_sntp_and_sync(void)
{
    if(!sntp_enabled()){
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
    }
    sys_ms_sleep(8000);
}


// --------------------------------------------------------------------------
// Basic Wi-Fi connect logic
// --------------------------------------------------------------------------
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

/** Try primary => backup, set wifi_status. */
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

// --------------------------------------------------------------------------
// Use cmd_mqtt to connect to test.mosquitto.org
// "mqtt connect <server_ip> <port> <encryption> <user> <pwd>"
// We can pass "test.mosquitto.org" as server, "1883" as port, "0" as encryption
// --------------------------------------------------------------------------
static void my_mqtt_cmd_connect(void)
{
    // if we want to do: "mqtt connect test.mosquitto.org 1883 0"
    // That is 4 arguments after "mqtt"
    // so total is 5 => "mqtt", "connect", "test.mosquitto.org", "1883", "0"
    char *args[] = {
        "mqtt", "connect",
        (char *)MQTT_BROKER_HOST,
        (char *)MQTT_BROKER_PORT,
        (char *)MQTT_BROKER_ENC
    };
    cmd_mqtt(5, args);
}

// --------------------------------------------------------------------------
// We'll do a disconnect via cmd_mqtt as well
// --------------------------------------------------------------------------
static void my_mqtt_cmd_disconnect(void)
{
    char *args[] = { "mqtt", "disconnect" };
    cmd_mqtt(2, args);
}

// --------------------------------------------------------------------------
// We'll do a publish using cmd_mqtt as well
// "mqtt publish <topic> <data> <qos> <retain>"
// For Qos=0, retain=0, we have 4 additional arguments => total 5
// --------------------------------------------------------------------------
static void my_mqtt_cmd_publish(const char *payload)
{
    // e.g. topic= MQTT_TOPIC, data= payload, Qos=0, Retain=0
    char *args[] = {
        "mqtt", "publish",
        (char*)MQTT_TOPIC,
        (char*)payload,
        "0",  // Qos=0
        "0"   // Retain=0
    };
    cmd_mqtt(6, args);
}

// --------------------------------------------------------------------------
// Wi-Fi reconnect => tries every 5 min, then does SNTP + MQTT
// --------------------------------------------------------------------------
static void wifi_reconnect_task(void *arg)
{
    (void)arg;
    while(1) {
        if(wifi_status == WIFI_DISCONNECTED) {
            printf("[RECONNECT] Offline => next attempt in 5 mins.\r\n");
            sys_ms_sleep(5UL*60UL*1000UL);

            // Attempt connect
            if(wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
               wifi_try_connect(BACKUP_WIFI_SSID,  BACKUP_WIFI_PASSWORD)){
                wifi_status = WIFI_CONNECTED;
                printf("[RECONNECT] Wi-Fi reconnected!\r\n");
                start_sntp_and_sync();

                // if we had an old MQTT connect, let's forcibly disconnect
                my_mqtt_cmd_disconnect();
                sys_ms_sleep(1000);

                my_mqtt_cmd_connect();
            } else {
                // remain offline
                printf("[RECONNECT] Still offline.\r\n");
            }
        } else {
            // check if we truly remain connected
            extern struct wifi_vif_tag wifi_vif_tab[];
            #define WIFI_STA_STATE_CONNECTED 6
            if(wifi_vif_tab[0].sta.state != WIFI_STA_STATE_CONNECTED) {
                printf("[RECONNECT] Wi-Fi dropped.\r\n");
                wifi_status = WIFI_DISCONNECTED;

                // stop SNTP + MQTT
                sntp_stop();
                my_mqtt_cmd_disconnect();
            }
        }

        sys_ms_sleep(10000);
    }
}


// --------------------------------------------------------------------------
// BLE scanning => build JSON => if online + mqtt_client => publish
// else offline => add timestamp, store (commented out).
// --------------------------------------------------------------------------
static void demo_task(void *arg)
{
    (void)arg;
    printf("[demo_task] Wi-Fi connecting...\r\n");
    wifi_connect_handler();

    // Create background reconnect
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

    // If connected => do SNTP + MQTT
    if(wifi_status==WIFI_CONNECTED){
        start_sntp_and_sync();
        my_mqtt_cmd_connect(); // "mqtt connect test.mosquitto.org 1883 0"
    }

    // BLE scanning forever
    while(1){
        for (uint8_t idx=0; ; idx++){
            dev_info_t* dev = scan_mgr_find_dev_by_idx(idx);
            if(!dev) break;
            if(dev->adv_len < 26) continue;
            // check adv_data[5]==0x4D && adv_data[6]==0x5A => "MZ"
            if(dev->adv_data[5]==0x4D && dev->adv_data[6]==0x5A) {
                printf("[demo_task] Found Target Beacon!\r\n");

                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter= (dev->adv_data[8]<<24)|(dev->adv_data[9]<<16)|
                                      (dev->adv_data[10]<<8)| dev->adv_data[11];
                int16_t temperature = (int16_t)((dev->adv_data[12]<<8)|dev->adv_data[13]);
                int16_t humidity    = (int16_t)((dev->adv_data[14]<<8)|dev->adv_data[15]);
                uint16_t batt       = (uint16_t)((dev->adv_data[16]<<8)|dev->adv_data[17]);
                bool reed_relay     = dev->adv_data[18]?true:false;
                uint8_t accel       = dev->adv_data[19];

                char json_payload[256];
                snprintf(json_payload, sizeof(json_payload),
                    "{"
                    "\"MAC\":\"%02X:%02X:%02X:%02X:%02X:%02X\","
                    "\"DeviceType\":%u,"
                    "\"MessageCounter\":%u,"
                    "\"Temperature\":%d,"
                    "\"Humidity\":%d,"
                    "\"BatteryVoltage\":%u,"
                    "\"ReedRelay\":%s,"
                    "\"Accelerometer\":%u"
                    "}",
                    dev->peer_addr.addr[5], dev->peer_addr.addr[4], dev->peer_addr.addr[3],
                    dev->peer_addr.addr[2], dev->peer_addr.addr[1], dev->peer_addr.addr[0],
                    device_type, msg_counter, temperature, humidity, batt,
                    reed_relay?"true":"false", accel
                );

                // If we want to check if MQTT is connected => we check "mqtt_client" + "mqtt_client_is_connected"
                if(wifi_status==WIFI_CONNECTED && mqtt_client && mqtt_client_is_connected(mqtt_client)) {
                    // publish
                    my_mqtt_cmd_publish(json_payload);

                    // debug timestamp
                    uint32_t t = get_current_epoch_time();
                    printf("TIMESTAMP(Online): %lu\r\n", (unsigned long)t);
                } else {
                    // offline => store
                    uint32_t t = get_current_epoch_time();
                    printf("TIMESTAMP(Offline): %lu\r\n", (unsigned long)t);

                    // add timestamp to the JSON
                    size_t curlen = strlen(json_payload);
                    if(curlen < (sizeof(json_payload)-30)) {
                        snprintf(json_payload+curlen-1, sizeof(json_payload)-curlen,
                            ",\"Timestamp\":%lu}", (unsigned long)t);
                    }
                    // save_to_flash_storage(json_payload); // future
                }
            }
        }

        sys_ms_sleep(7000);
    }

    sys_task_delete(NULL);
}


// --------------------------------------------------------------------------
// main => OS init, ble_init, wifi_init, create "demo_task"
// --------------------------------------------------------------------------
int main(void)
{
    sys_os_init();
    platform_init();
    ble_init(true);

    if(wifi_init()){
        dbg_print(ERR,"[MAIN] wifi_init failed!\r\n");
        for(;;){}
    }

    // create demo_task
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

    sys_os_start();
    for(;;){}
    return 0;
}
