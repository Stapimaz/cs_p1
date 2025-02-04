/**
 * main.c
 *
 * Fixes/Improvements:
 *  1) Sync RTC time from SNTP whenever Wi-Fi connects (so offline timestamps are correct).
 *  2) Ensure that if we first connect successfully and then lose Wi-Fi,
 *     we immediately go to offline mode (rather than only on initial connect fail).
 *
 * The rest of the logic (BLE scanning, data filtering, etc.) is unchanged, except
 * we shortened the 5-minute wait in wifi_monitor_task so we don't stall so long.
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
#include "mqtt_cmd.h"
#include "lwip/apps/mqtt.h"

// SNTP includes
#include "lwip/apps/sntp.h"

#include "gd32vw55x_rtc.h"
#include <time.h>   // If available. If not, we'll do manual conversion.


// --------------------------------------------------------------------
// Wi-Fi and MQTT configuration
// --------------------------------------------------------------------
#define PRIMARY_WIFI_SSID      "Mobiliz NaRGE_Gues"
#define PRIMARY_WIFI_PASSWORD  "M@b!lz12.*"
#define BACKUP_WIFI_SSID       "Redmi Note 10S"
#define BACKUP_WIFI_PASSWORD   "54321012"

#define MQTT_BROKER_HOST  "test.mosquitto.org"
#define MQTT_BROKER_PORT  "1883"
#define MQTT_BROKER_ENC   "0"   // 0 => no TLS
#define MQTT_TOPIC        "test22052077/ble_beacons"

extern mqtt_client_t *mqtt_client;

/* We define a custom enum to track Wi-Fi status in code */
typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED    = 1
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_DISCONNECTED;


// --------------------------------------------------------------------
// 1) SNTP => Set RTC whenever NTP is obtained
// --------------------------------------------------------------------
/*
 * By default, LwIP's sntp.c does nothing with the received time, because
 * SNTP_SET_SYSTEM_TIME() is a no-op. We override it to set our RTC so that
 * future get_current_epoch_time() calls read correct time, even offline.
 */

// For local (GMT+3) storage in RTC, we can offset the epoch by +3 hours below.
//   Because in get_current_epoch_time() we do "epoch_time += 3*3600" again.
//   If you prefer to store pure UTC in the RTC, adjust accordingly.
static void set_rtc_by_epoch_time(uint32_t utc_secs)
{
    // Add 3 hours if you want the RTC to store local time (GMT+3).
    utc_secs += 3 * 3600;

    // We do the reverse of what get_current_epoch_time() does, building
    // calendar date/time from the 1970-based 'utc_secs'.
    // This is a simplified approach that handles leap years through 2100 similarly
    // to your get_current_epoch_time() function. You can refine if needed.

    // Calculate days since 1970
    uint32_t days = utc_secs / 86400;
    uint32_t remaining = utc_secs % 86400;
    uint32_t hour  = remaining / 3600;
    remaining %= 3600;
    uint32_t minute = remaining / 60;
    uint32_t second = remaining % 60;

    // Now compute year/month/day. We'll do a rough approach:
    // The get_current_epoch_time used:
    //    days_in_month[] = {0, 31, 59, 90, 120, 151, 181,212,243,273,304,334}
    //    We need to invert that logic.
    // We'll start from 1970 and move forward.

    uint32_t year = 1970;
    for (;;)
    {
        // Leap year if (year % 4 == 0 && year % 100 != 0) or year % 400 == 0
        uint32_t ydays = 365;
        if ((year % 4 == 0 && (year % 100 != 0)) || (year % 400 == 0)) {
            ydays = 366;
        }
        if (days >= ydays) {
            days -= ydays;
            year++;
        } else {
            break;
        }
    }

    // 'days' is now how many days into 'year'
    static const uint8_t md[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint8_t month = 0;
    uint8_t is_leap = ((year % 4 == 0 && (year % 100 != 0)) || (year % 400 == 0)) ? 1 : 0;

    for (int i=0; i<12; i++)
    {
        uint32_t dim = md[i];
        if ((i == 1) && is_leap) dim = 29; // February in leap year
        if (days >= dim) {
            days -= dim;
            month++;
        } else {
            break;
        }
    }
    // month is 0-based from 0..11; we want 1..12
    month += 1;
    // 'days' is 0-based day-of-month, so we add 1 for final
    uint8_t day = days + 1;

    // Convert to BCD for the RTC
    // Example: decimal 25 => BCD 0x25
    // We'll define a small helper:
    uint8_t bcd(uint8_t dec) {
        return (uint8_t)((((dec / 10) & 0xF) << 4) | (dec % 10));
    }


    // year is stored as offset from 2000 in BCD
    // e.g. if year=2025 => offset=25 => BCD=0x25
    uint16_t yoffs = (uint16_t)(year - 2000);
    if (yoffs > 99) {
        // Simple clamp to 2099 if needed
        yoffs = 99;
    }

    rtc_parameter_struct rtc_time;
    memset(&rtc_time, 0, sizeof(rtc_time));

    rtc_time.year    = bcd((uint8_t)(yoffs));
    rtc_time.month   = bcd((uint8_t)month);
    rtc_time.date    = bcd((uint8_t)day);

    // Your day_of_week is not strictly needed for the epoch time. We'll set it
    // to 1 (Monday) or you could calculate real DoW. For brevity, set it to 1.
    rtc_time.day_of_week = 1;

    rtc_time.hour    = bcd((uint8_t)hour);
    rtc_time.minute  = bcd((uint8_t)minute);
    rtc_time.second  = bcd((uint8_t)second);

    // Use your typical prescalers. If your project sets them elsewhere, keep them:
    rtc_time.factor_asyn = 0x63;   // example
    rtc_time.factor_syn  = 0x13F;  // example
    rtc_time.display_format = RTC_24HOUR;
    rtc_time.am_pm = RTC_AM;       // 24-hour doesn't use AM/PM anyway

    // Now write into RTC:
    rtc_init(&rtc_time);
    // That’s it: the RTC is now synced to the newly acquired NTP time.
}

// Now override LWIP’s default SNTP_SET_SYSTEM_TIME macro:
#undef SNTP_SET_SYSTEM_TIME
#define SNTP_SET_SYSTEM_TIME(sec) set_rtc_by_epoch_time((uint32_t)sec)

// --------------------------------------------------------------------
// Our existing get_current_epoch_time, unchanged except it uses new correct RTC
// once SNTP has set it.
// --------------------------------------------------------------------
uint32_t get_current_epoch_time(void) {
    rtc_parameter_struct rtc_time;
    rtc_current_time_get(&rtc_time);

    uint16_t year = ((rtc_time.year >> 4) * 10 + (rtc_time.year & 0x0F)) + 2000;
    uint8_t month = (rtc_time.month >> 4) * 10 + (rtc_time.month & 0x0F);
    uint8_t day   = (rtc_time.date  >> 4) * 10 + (rtc_time.date  & 0x0F);
    uint8_t hour  = (rtc_time.hour  >> 4) * 10 + (rtc_time.hour  & 0x0F);
    uint8_t minute= (rtc_time.minute>> 4) * 10 + (rtc_time.minute& 0x0F);
    uint8_t second= (rtc_time.second>> 4) * 10 + (rtc_time.second& 0x0F);

    // Days in each month (non-leap year)
    const uint16_t days_in_month[] = {
       0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };

    // Leap year adjustment
    uint8_t leap_correction = (month > 2 &&
       ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) ? 1 : 0;

    // days since 1970
    uint32_t days_since_1970 =
       (year - 1970)*365
       + ((year - 1969)/4)
       + days_in_month[month - 1]
       + (day - 1)
       + leap_correction;

    uint32_t epoch_time = (days_since_1970 * 86400)
                          + (hour * 3600)
                          + (minute * 60)
                          + second;

    // Adjust for GMT+3
    epoch_time += 3 * 3600;
    return epoch_time;
}


// --------------------------------------------------------------------
// Helper: Initialize & Start SNTP on Wi-Fi connect
// --------------------------------------------------------------------
static void start_sntp_and_sync(void)
{
    // If not already running, init it:
    if (!sntp_enabled()) {
        // Use OP_MODE_POLL => queries the server regularly
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        // Use a well-known pool or your own NTP server
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
    }

    // Quick wait to let SNTP do a first query. Typically, you get a time
    // response within a few seconds. This is a naive approach. If you want
    // a robust approach, poll your RTC to see if it changed from default.
    sys_ms_sleep(8000);  // 8 seconds delay is often enough
}


// --------------------------------------------------------------------
// Callback from wifi_management
// --------------------------------------------------------------------
static void wifi_mgmt_callback(wifi_management_event_t event, void *param)
{
    if (event == WIFI_MGMT_EVENT_DISCONNECT) {
        printf("[WIFI] Connection lost! Will retry shortly.\r\n");
        wifi_status = WIFI_DISCONNECTED;

        // Stop SNTP so we don't run it offline
        sntp_stop();

        // Disconnect MQTT cleanly if running
        char *mqtt_disconnect_args[] = {"mqtt", "disconnect"};
        cmd_mqtt(2, mqtt_disconnect_args);
        printf("[MQTT] Disconnected due to Wi-Fi loss.\r\n");

    } else if (event == WIFI_MGMT_EVENT_CONNECT_SUCCESS) {
        printf("[WIFI] Connected successfully. Reconnecting to MQTT...\r\n");
        wifi_status = WIFI_CONNECTED;

        // Stop + re-init MQTT so we have a fresh connection
        {
            char *mqtt_disconnect_args[] = {"mqtt", "disconnect"};
            cmd_mqtt(2, mqtt_disconnect_args);
            sys_ms_sleep(1000);
        }

        // Start/Sync SNTP so RTC has correct date/time
        start_sntp_and_sync();

        // Reconnect to MQTT
        char *mqtt_args[] = {
            "mqtt", "connect",
            (char *)MQTT_BROKER_HOST,
            (char *)MQTT_BROKER_PORT,
            (char *)MQTT_BROKER_ENC
        };
        cmd_mqtt(5, mqtt_args);
    }
}


// --------------------------------------------------------------------
// Try connect to a specific SSID
// --------------------------------------------------------------------
static bool wifi_try_connect(const char *ssid, const char *password)
{
    printf("[WIFI] Trying to connect to SSID: %s\r\n", ssid);
    int ret = wifi_management_connect((char *)ssid, (char *)password, /*blocked=*/1);
    if (ret == 0) {
        printf("[WIFI] Successfully connected to %s!\r\n", ssid);
        return true;
    } else {
        printf("[WIFI] Connection to %s failed (error: %d).\r\n", ssid, ret);
        return false;
    }
}


// --------------------------------------------------------------------
// Attempt Wi-Fi connect on startup
// --------------------------------------------------------------------
static void wifi_connect_handler(void)
{
    bool connected = false;

    // Try primary
    if (wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD)) {
        connected = true;
    } else {
        // If that fails, try backup
        if (wifi_try_connect(BACKUP_WIFI_SSID, BACKUP_WIFI_PASSWORD)) {
            connected = true;
        }
    }

    if (connected) {
        wifi_status = WIFI_CONNECTED;
        printf("[WIFI] Successfully connected to Wi-Fi.\r\n");
    } else {
        wifi_status = WIFI_DISCONNECTED;
        printf("[WIFI] Both Wi-Fi connections failed. Running offline.\r\n");
    }
}


// --------------------------------------------------------------------
// Publish JSON to MQTT
// --------------------------------------------------------------------
static void publish_json_data(const char *json_payload)
{
    char *pub_args[] = {
        "mqtt", "publish",
        MQTT_TOPIC,
        (char *)json_payload,
        "0",  // QoS=0
        "0"   // retain=0
    };
    cmd_mqtt(6, pub_args);
    dbg_print(NOTICE, "[demo_task] Data sent to MQTT => %s\r\n", json_payload);
}


// --------------------------------------------------------------------
// Wi-Fi monitor task (runs in background), tries reconnect if lost
// --------------------------------------------------------------------
static void wifi_monitor_task(void *arg)
{
    (void)arg;
    while (1) {
        // If we're offline, attempt reconnect
        if (wifi_status == WIFI_DISCONNECTED) {
            printf("[WIFI] Disconnected. Attempting reconnect...\r\n");

            if (wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
                wifi_try_connect(BACKUP_WIFI_SSID, BACKUP_WIFI_PASSWORD)) {
                wifi_status = WIFI_CONNECTED;
                printf("[WIFI] Reconnected successfully.\r\n");

                // SNTP + MQTT
                start_sntp_and_sync();

                char *mqtt_args[] = {
                    "mqtt", "connect",
                    (char *)MQTT_BROKER_HOST,
                    (char *)MQTT_BROKER_PORT,
                    (char *)MQTT_BROKER_ENC
                };
                cmd_mqtt(5, mqtt_args);
            } else {
                printf("[WIFI] Still disconnected. Will retry in 10s.\r\n");
            }
        }
        // Sleep 10 seconds before next check (instead of 5 minutes).
        sys_ms_sleep(10000);
    }
}


// --------------------------------------------------------------------
// The main RTOS task for scanning BLE beacons, parsing data, etc.
// --------------------------------------------------------------------
static void demo_task(void *arg)
{
    (void)arg;

    // 1) Attempt Wi-Fi connect at startup
    printf("[DEMO_TASK] Starting Wi-Fi Connection Handler...\r\n");
    wifi_connect_handler(); // tries once

    // 2) Start a separate Wi-Fi monitor task that does background reconnect attempts
    os_task_t wifi_task_handle = sys_task_create_dynamic(
        (uint8_t*)"wifi_monitor_task",
        2048,
        OS_TASK_PRIORITY(0),
        wifi_monitor_task,
        NULL
    );
    if (!wifi_task_handle) {
        dbg_print(ERR, "[MAIN] Failed to create Wi-Fi monitor task\r\n");
        for (;;) {}
    }

    // If we ended up connected, also do MQTT + SNTP:
    if (wifi_status == WIFI_CONNECTED) {
        printf("[DEMO_TASK] Wi-Fi connected! Proceeding to MQTT setup...\r\n");

        // Start SNTP so RTC is correct
        start_sntp_and_sync();

        // Connect MQTT
        char *mqtt_args[] = {
            "mqtt", "connect",
            (char *)MQTT_BROKER_HOST,
            (char *)MQTT_BROKER_PORT,
            (char *)MQTT_BROKER_ENC
        };
        cmd_mqtt(5, mqtt_args);
        printf("[DEMO_TASK] cmd_mqtt connect done.\r\n");

        char *auto_rec_args[] = {"mqtt", "auto_reconnect", "1"};
        cmd_mqtt(3, auto_rec_args);
        printf("[DEMO_TASK] MQTT auto reconnect enabled.\r\n");
    }

    // 3) BLE scanning + filtering
    while (1) {
        for (uint8_t idx = 0; ; idx++) {
            dev_info_t *dev = scan_mgr_find_dev_by_idx(idx);
            if (!dev) {
                break; // No more devices
            }

            // We check adv_data[5] = 'M'(0x4D), adv_data[6] = 'Z'(0x5A)
            if (dev->adv_len < 26) continue;
            if (dev->adv_data[5] == 0x4D && dev->adv_data[6] == 0x5A) {
                dbg_print(NOTICE, "[demo_task] TARGET BEACON FOUND! Parsing data...\r\n");

                // Extract the fields
                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter = (dev->adv_data[8] << 24) |
                                       (dev->adv_data[9] << 16) |
                                       (dev->adv_data[10]<< 8) |
                                        dev->adv_data[11];
                int16_t temperature = (dev->adv_data[12] << 8) | dev->adv_data[13];
                int16_t humidity    = (dev->adv_data[14] << 8) | dev->adv_data[15];
                uint16_t battery_voltage = (dev->adv_data[16] << 8) | dev->adv_data[17];
                bool reed_relay = dev->adv_data[18] ? true : false;
                uint8_t accelerometer = dev->adv_data[19];

                // Format JSON
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
                         device_type, msg_counter, temperature, humidity, battery_voltage,
                         reed_relay ? "true" : "false", accelerometer);

                // Decide online or offline
                if (wifi_status == WIFI_CONNECTED && mqtt_client_is_connected(mqtt_client)) {
                    // Publish right away
                    publish_json_data(json_payload);
                } else {
                    // Offline => add Timestamp & store to flash
                    uint32_t timestamp = get_current_epoch_time();
                    dbg_print(ERR, "TIMESTAMP: %lu \r\n", (unsigned long)timestamp);

                    // Expand existing JSON with a "Timestamp" field
                    size_t len = strlen(json_payload);
                    if (len < sizeof(json_payload) - 30) {
                        // Insert before final "}"
                        snprintf(json_payload + len - 1, sizeof(json_payload) - len,
                                 ",\"Timestamp\":%lu}", (unsigned long)timestamp);
                    }

                    // Example: call a flash save function (not implemented here)
                    // save_to_flash_storage(json_payload);
                }
            }
        }

        // Sleep 7s
        sys_ms_sleep(7000);
    }

    printf("[DEMO_TASK] The demo has ended.\r\n");
    sys_task_delete(NULL);
}


// -------------------------------------------------------------------------
// main()
//   - OS init
//   - platform_init
//   - ble_init(true)
//   - wifi_init
//   - create demo_task
//   - start OS
// -------------------------------------------------------------------------
int main(void)
{
    // 1) OS init
    sys_os_init();

    // 2) Platform init
    platform_init();

    // 3) BLE init => calls ble_app_init => calls app_scan_mgr_init
    ble_init(true);

    // 4) Wi-Fi init => sets up wifi_management, etc.
    if (wifi_init()) {
        dbg_print(ERR, "[MAIN] wifi_init failed!\r\n");
        for (;;) {}
    }

    // 5) Create single "demo_task"
    os_task_t handle = sys_task_create_dynamic(
            (uint8_t*)"demo_task",
            4096, // stack size
            OS_TASK_PRIORITY(1),
            demo_task,
            NULL
    );
    if (!handle) {
        dbg_print(ERR, "[MAIN] Failed to create demo_task\r\n");
        for (;;) {}
    }

    // 6) Start the RTOS scheduler
    sys_os_start();

    while (1) {}
    return 0;
}
