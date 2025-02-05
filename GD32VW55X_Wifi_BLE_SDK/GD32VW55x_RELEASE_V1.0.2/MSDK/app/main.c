/**
 * main.c
 *
 * Fixes/Improvements:
 *   1) Sync RTC time from SNTP whenever Wi-Fi connects (so offline timestamps are correct).
 *   2) Ensure that if we first connect successfully and then lose Wi-Fi,
 *      we immediately go offline, close MQTT, etc.
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

// MQTT includes
#include "mqtt_cmd.h"
#include "lwip/apps/mqtt.h"

// SNTP includes (but we do NOT do #undef SNTP_SET_SYSTEM_TIME here!)
#include "lwip/apps/sntp.h"

// RTC
#include "gd32vw55x_rtc.h"

#include <time.h> // optional if you want

#define PRIMARY_WIFI_SSID      "Mobiliz NaRGE_Gues"
#define PRIMARY_WIFI_PASSWORD  "M@b!lz12.*"
#define BACKUP_WIFI_SSID       "Redmi Note 10S"
#define BACKUP_WIFI_PASSWORD   "54321012"

#define MQTT_BROKER_HOST       "test.mosquitto.org"
#define MQTT_BROKER_PORT       "1883"
#define MQTT_BROKER_ENC        "0"   // 0 => no TLS
#define MQTT_TOPIC             "test22052077/ble_beacons"

extern mqtt_client_t *mqtt_client;

typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED    = 1
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_DISCONNECTED;

/* --------------------------------------------------------------------------
 * Provide the function that sntp_opts.h macro calls (option A or B).
 * We'll do it as "set_rtc_by_epoch_time" for clarity.
 * -------------------------------------------------------------------------*/

// BCD conversion function
static uint8_t to_bcd(uint8_t dec) {
    return (uint8_t)((((dec / 10) & 0xF) << 4) | (dec % 10));
}

void set_rtc_by_epoch_time(uint32_t utc_secs)
{


    // same logic as before...
    uint32_t days   = utc_secs / 86400;
    uint32_t remain = utc_secs % 86400;
    uint32_t hour   = remain / 3600; remain %= 3600;
    uint32_t minute = remain / 60;
    uint32_t second = remain % 60;

    // compute year, etc...
    uint32_t year = 1970;
    while (1) {
        uint32_t ydays = 365;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            ydays = 366;
        }
        if (days >= ydays) {
            days -= ydays;
            year++;
        } else {
            break;
        }
    }
    static const uint8_t md[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint8_t month=0;
    uint8_t is_leap = ((year % 4 == 0 && (year % 100 != 0)) || (year % 400 == 0)) ? 1 : 0;
    for (int i=0; i<12; i++) {
        uint8_t dm = md[i];
        if (i==1 && is_leap) dm=29;
        if (days >= dm) {
            days -= dm;
            month++;
        } else {
            break;
        }
    }
    month++;
    uint8_t day = days + 1;


    uint16_t yoffs = (uint16_t)(year - 2000);
    if (yoffs>99) { yoffs=99; } // clamp

    rtc_parameter_struct rtc_time;
    memset(&rtc_time, 0, sizeof(rtc_time));
    rtc_time.year           = to_bcd((uint8_t)yoffs);
    rtc_time.month          = to_bcd(month);
    rtc_time.date           = to_bcd(day);
    rtc_time.day_of_week    = 1;    // not used
    rtc_time.hour           = to_bcd((uint8_t)hour);
    rtc_time.minute         = to_bcd((uint8_t)minute);
    rtc_time.second         = to_bcd((uint8_t)second);

    // example prescalers; adapt if needed
    rtc_time.factor_asyn    = 0x63;
    rtc_time.factor_syn     = 0x13F;
    rtc_time.display_format = RTC_24HOUR;
    rtc_time.am_pm          = RTC_AM;

    // set hardware RTC
    rtc_init(&rtc_time);
}

/* --------------------------------------------------------------------------
 * Helper: get_current_epoch_time() => same as your final version
 * -------------------------------------------------------------------------*/
uint32_t get_current_epoch_time(void)
{
    rtc_parameter_struct rtc_time;
    rtc_current_time_get(&rtc_time);

    uint16_t year = ((rtc_time.year >> 4)*10 + (rtc_time.year &0x0F)) + 2000;
    uint8_t  month= ((rtc_time.month>>4)*10 + (rtc_time.month&0x0F));
    uint8_t  day  = ((rtc_time.date >>4)*10 + (rtc_time.date &0x0F));
    uint8_t  hour = ((rtc_time.hour >>4)*10 + (rtc_time.hour &0x0F));
    uint8_t  min  = ((rtc_time.minute>>4)*10 + (rtc_time.minute&0x0F));
    uint8_t  sec  = ((rtc_time.second>>4)*10 + (rtc_time.second&0x0F));

    static const uint16_t days_in_month[] = {0,31,59,90,120,151,181,212,243,273,304,334};
    uint8_t leap_correction = (month>2 && ((year%4==0 &&year%100!=0)||(year%400==0)))?1:0;
    uint32_t days_since_1970 = (year-1970)*365 + ((year-1969)/4)
         + days_in_month[month-1] + (day-1) + leap_correction;

    uint32_t epoch_time = days_since_1970 * 86400 + hour*3600 + min*60 + sec;
    // shift for GMT+3 if stored that way
    epoch_time += 3*3600;
    return epoch_time;
}

/* --------------------------------------------------------------------------
 * Start SNTP on Wi-Fi connect
 * -------------------------------------------------------------------------*/
static void start_sntp_and_sync(void)
{
    if (!sntp_enabled()) {
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
    }
    // wait e.g. 8s
    sys_ms_sleep(8000);
}

/* --------------------------------------------------------------------------
 * Wi-Fi mgmt callback
 * -------------------------------------------------------------------------*/
static void wifi_mgmt_callback(wifi_management_event_t event, void *param)
{
    if (event == WIFI_MGMT_EVENT_DISCONNECT) {
        printf("[WIFI] Connection lost!\r\n");
        wifi_status = WIFI_DISCONNECTED;

        // stop SNTP
        sntp_stop();

        // close MQTT
        char *args[] = {"mqtt","disconnect"};
        cmd_mqtt(2, args);
        printf("[MQTT] Disconnected.\r\n");

    } else if (event == WIFI_MGMT_EVENT_CONNECT_SUCCESS) {
        printf("[WIFI] Connected. Reconnecting MQTT.\r\n");
        wifi_status = WIFI_CONNECTED;

        // forcibly disconnect old MQTT
        {
            char *args[] = {"mqtt","disconnect"};
            cmd_mqtt(2,args);
            sys_ms_sleep(1000);
        }

        // sntp
        start_sntp_and_sync();

        // connect MQTT
        char *mqtt_args[] = {
           "mqtt","connect",
           (char*)MQTT_BROKER_HOST,
           (char*)MQTT_BROKER_PORT,
           (char*)MQTT_BROKER_ENC
        };
        cmd_mqtt(5, mqtt_args);
    }
}

/* --------------------------------------------------------------------------
 * wifi_try_connect
 * -------------------------------------------------------------------------*/
static bool wifi_try_connect(const char *ssid, const char *pw)
{
    printf("[WIFI] Trying connect to SSID: %s\r\n", ssid);
    int ret = wifi_management_connect((char*)ssid,(char*)pw,1);
    if (!ret) {
        printf("[WIFI] Connected => %s\r\n", ssid);
        return true;
    } else {
        printf("[WIFI] Connect fail => %d\r\n", ret);
        return false;
    }
}

/* --------------------------------------------------------------------------
 * wifi_connect_handler => first attempt
 * -------------------------------------------------------------------------*/
static void wifi_connect_handler(void)
{
    if (wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
        wifi_try_connect(BACKUP_WIFI_SSID, BACKUP_WIFI_PASSWORD))
    {
        wifi_status = WIFI_CONNECTED;
        printf("[WIFI] Connected.\r\n");
    } else {
        wifi_status = WIFI_DISCONNECTED;
        printf("[WIFI] Both Wi-Fi failed => offline.\r\n");
    }
}

/* --------------------------------------------------------------------------
 * publish_json_data => MQTT
 * -------------------------------------------------------------------------*/
static void publish_json_data(const char *payload)
{
    char *args[] = {
      "mqtt","publish",
      (char*)MQTT_TOPIC,
      (char*)payload,
      "0","0"
    };
    cmd_mqtt(6,args);
    dbg_print(NOTICE,"[demo_task] Data => %s\r\n", payload);
}

/* --------------------------------------------------------------------------
 * wifi_reconnect_handler => tries reconnect in background
 * -------------------------------------------------------------------------*/
static void wifi_reconnect_handler(void *arg)
{

    printf("[WIFI] RECONNECT HANDLER INIT.\r\n");

    (void)arg;
    while (1) {
       if (wifi_status == WIFI_DISCONNECTED) {
           printf("[WIFI] Disconnected => Reconnect attempt.\r\n");
           if (wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
               wifi_try_connect(BACKUP_WIFI_SSID, BACKUP_WIFI_PASSWORD))
           {
               wifi_status = WIFI_CONNECTED;
               printf("[WIFI] Reconnected.\r\n");

               start_sntp_and_sync();

               char *mqtt_args[] = {
                 "mqtt","connect",
                 (char*)MQTT_BROKER_HOST,
                 (char*)MQTT_BROKER_PORT,
                 (char*)MQTT_BROKER_ENC
               };
               cmd_mqtt(5, mqtt_args);
           } else {
               printf("[WIFI] Still disconnected => retry in 10s.\r\n");
           }
       }
       sys_ms_sleep(300000);
    }
}

/* --------------------------------------------------------------------------
 * The main RTOS task => does BLE scanning, etc.
 * -------------------------------------------------------------------------*/
static void demo_task(void *arg)
{
    (void)arg;
    printf("[DEMO_TASK] Wi-Fi connecting...\r\n");
    wifi_connect_handler();

    // create wifi monitor task
    os_task_t wtask = sys_task_create_dynamic(
        (uint8_t*)"wifi_reconnect_handler",
        2048,
        OS_TASK_PRIORITY(0),
        wifi_reconnect_handler,
        NULL
    );
    if (!wtask) {
        dbg_print(ERR, "[MAIN] cannot create wifi_reconnect_handler\r\n");
        for(;;){}
    }

    // if connected => do SNTP + MQTT
    if (wifi_status == WIFI_CONNECTED) {
        start_sntp_and_sync();
        char *cargs[] = {
          "mqtt","connect",
          (char*)MQTT_BROKER_HOST,
          (char*)MQTT_BROKER_PORT,
          (char*)MQTT_BROKER_ENC
        };
        cmd_mqtt(5,cargs);
        printf("[DEMO_TASK] MQTT connected.\r\n");

        char *auto_rec_args[]={"mqtt","auto_reconnect","1"};
        cmd_mqtt(3,auto_rec_args);
    }

    // BLE scanning
    while(1) {
        for (uint8_t idx=0; ; idx++) {
            dev_info_t *dev = scan_mgr_find_dev_by_idx(idx);
            if (!dev) break;

            // check adv_data
            if (dev->adv_len<26) continue;
            if (dev->adv_data[5]==0x4D && dev->adv_data[6]==0x5A) {
                dbg_print(NOTICE, "[demo_task] Found target beacon!\r\n");

                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter = (dev->adv_data[8]<<24)|(dev->adv_data[9]<<16)|
                                       (dev->adv_data[10]<<8)|(dev->adv_data[11]);
                int16_t temperature = (dev->adv_data[12]<<8)|dev->adv_data[13];
                int16_t humidity    = (dev->adv_data[14]<<8)|dev->adv_data[15];
                uint16_t batt_volt  = (dev->adv_data[16]<<8)|dev->adv_data[17];
                bool reed_relay     = dev->adv_data[18]?true:false;
                uint8_t accel       = dev->adv_data[19];

                char json_payload[256];
                snprintf(json_payload,sizeof(json_payload),
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
                  device_type, msg_counter, temperature, humidity, batt_volt,
                  reed_relay?"true":"false", accel
                );

                if (wifi_status==WIFI_CONNECTED && mqtt_client_is_connected(mqtt_client)) {
                    // online => publish
                    publish_json_data(json_payload);

                    //TIMESTAMP PRINT FOR TESTING
                    uint32_t stamp = get_current_epoch_time();
                    printf("TIMESTAMP: %lu\r\n", (unsigned long)stamp);

                    //dbg_print(ERR,"TIMESTAMP: %lu\r\n",(unsigned long)stamp);
                } else {
                    // offline => add a timestamp + store
                    uint32_t stamp = get_current_epoch_time();

                    printf("TIMESTAMP: %lu\r\n", (unsigned long)stamp);
//                    dbg_print(ERR,"TIMESTAMP: %lu\r\n",(unsigned long)stamp);

                    // add timestamp
                    size_t len=strlen(json_payload);
                    if (len<(sizeof(json_payload)-30)) {
                        snprintf(json_payload+len-1,sizeof(json_payload)-len,
                           ",\"Timestamp\":%lu}",(unsigned long)stamp);
                    }
                    // save_to_flash_storage(json_payload);
                }
            }
        }

        sys_ms_sleep(7000);
    }
    sys_task_delete(NULL);
}

/* --------------------------------------------------------------------------
 * main() => OS init, ble_init, wifi_init, create demo_task, run
 * -------------------------------------------------------------------------*/
int main(void)
{
    sys_os_init();
    platform_init();
    ble_init(true);
    if (wifi_init()) {
        dbg_print(ERR,"[MAIN] wifi_init failed!\r\n");
        for(;;){}
    }
    wifi_management_register_callback(wifi_mgmt_callback);

    os_task_t handle = sys_task_create_dynamic(
       (uint8_t*)"demo_task",
       4096,
       OS_TASK_PRIORITY(1),
       demo_task,
       NULL
    );
    if (!handle) {
        dbg_print(ERR,"[MAIN] cannot create demo_task\r\n");
        for(;;){}
    }
    sys_os_start();
    for(;;){}
    return 0;
}
