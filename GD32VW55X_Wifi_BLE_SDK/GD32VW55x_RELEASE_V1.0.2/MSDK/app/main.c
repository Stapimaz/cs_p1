/**
 * main.c
 *
 * Example showing:
 *  - Single RTOS task
 *  - Wi-Fi scan + connect
 *  - MQTT connect (using cmd_mqtt)
 *  - BLE init + scanning (using app_scan_mgr's public API)
 *  - Filtering for a specific beacon based on adv_data[5] = 'M' and adv_data[6] = 'Z'
 *  - Parsing data and publishing in JSON format
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

// --------------------------------------------------------------------
// Wi-Fi and MQTT configuration
// --------------------------------------------------------------------
#define MY_WIFI_SSID        "Mobiliz NaRGE_Guest"
#define MY_WIFI_PASSWORD    "M@b!lz12.*"

#define MQTT_BROKER_HOST    "test.mosquitto.org"
#define MQTT_BROKER_PORT    "1883"
#define MQTT_BROKER_ENC     "0"   // 0 => no TLS
#define MQTT_TOPIC          "test2205/ble_beacons"

// --------------------------------------------------------------------
// Function to publish JSON data to MQTT
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
// RTOS Task: Wi-Fi, MQTT, BLE Scanning, Data Filtering & Publishing
// --------------------------------------------------------------------
static void demo_task(void *arg)
{
    (void)arg;
    int ret;

    //----------------------------------------------------------------
    // 1) Wi-Fi scan + connect
    //----------------------------------------------------------------
    printf("[DEMO_TASK] Start Wi-Fi scan.\r\n");
    ret = wifi_management_scan(/*blocked=*/1, MY_WIFI_SSID);
    if (ret != 0) {
        printf("[DEMO_TASK] Wi-Fi scan failed => %d\r\n", ret);
        goto demo_exit;
    }

    printf("[DEMO_TASK] Connecting to Wi-Fi SSID='%s'...\r\n", MY_WIFI_SSID);
    ret = wifi_management_connect((char *)MY_WIFI_SSID, (char *)MY_WIFI_PASSWORD, /*blocked=*/1);
    if (ret != 0) {
        printf("[DEMO_TASK] Wi-Fi connect failed => %d\r\n", ret);
        goto demo_exit;
    }
    printf("[DEMO_TASK] Wi-Fi connected!\r\n");

    //----------------------------------------------------------------
    // 2) Connect to MQTT broker
    //----------------------------------------------------------------
    char *mqtt_args[] = {
        "mqtt", "connect",
        (char *)MQTT_BROKER_HOST,
        (char *)MQTT_BROKER_PORT,
        (char *)MQTT_BROKER_ENC
    };
    cmd_mqtt(5, mqtt_args);
    printf("[DEMO_TASK] cmd_mqtt connect done.\r\n");

    char *auto_rec_args[] = { "mqtt", "auto_reconnect", "1" };
    cmd_mqtt(3, auto_rec_args);
    printf("[DEMO_TASK] MQTT auto reconnect enabled.\r\n");

    //----------------------------------------------------------------
    // 3) BLE Scanning & Filtering for Target Beacon
    //----------------------------------------------------------------
    while (1)
    {
        for (uint8_t idx = 0; ; idx++)
        {
            dev_info_t *dev = scan_mgr_find_dev_by_idx(idx);
            if (!dev) {
                break; // No more devices
            }

            // Check for target beacon: adv_data[5] = 'M' (0x4D), adv_data[6] = 'Z' (0x5A)
            if (dev->adv_len < 26) continue; // Ensure enough bytes

            if (dev->adv_data[5] == 0x4D && dev->adv_data[6] == 0x5A)
            {
                dbg_print(NOTICE, "[demo_task] TARGET BEACON FOUND! Parsing data...\r\n");

                // Extract values from advertisement packet
                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter = (dev->adv_data[8] << 24) | (dev->adv_data[9] << 16) |
                                       (dev->adv_data[10] << 8) | dev->adv_data[11];

                int16_t temperature = (dev->adv_data[12] << 8) | dev->adv_data[13];
                int16_t humidity = (dev->adv_data[14] << 8) | dev->adv_data[15];
                uint16_t battery_voltage = (dev->adv_data[16] << 8) | dev->adv_data[17];

                bool reed_relay = dev->adv_data[18] ? true : false;
                uint8_t accelerometer = dev->adv_data[19];

                // Format JSON string
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

                // Publish JSON to MQTT
                publish_json_data(json_payload);
            }
        }

        // Sleep 5s before checking again
        sys_ms_sleep(5000);
    }

demo_exit:
    printf("[DEMO_TASK] The demo has ended.\r\n");
    sys_task_delete(NULL);
}

/* -------------------------------------------------------------------------
 *  main()
 *    - OS init
 *    - platform_init
 *    - ble_init(true)  (makes sure app_scan_mgr_init() is invoked)
 *    - wifi_init
 *    - Create single RTOS 'demo_task'
 * ------------------------------------------------------------------------*/
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
        4096,        // stack size
        OS_TASK_PRIORITY(0),
        demo_task,
        NULL
    );
    if (!handle) {
        dbg_print(ERR, "[MAIN] Failed to create demo_task\r\n");
        for (;;) {}
    }

    // 6) Start FreeRTOS (or your RTOS) scheduler
    sys_os_start();

    while (1) {}
    return 0;
}
