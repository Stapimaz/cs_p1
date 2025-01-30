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
#define PRIMARY_WIFI_SSID        "Mobiliz NaRGE_Guest"
#define PRIMARY_WIFI_PASSWORD    "M@b!lz12.*"
#define BACKUP_WIFI_SSID        "Redmi Note 10S"
#define BACKUP_WIFI_PASSWORD    "54321012"

#define MQTT_BROKER_HOST    "test.mosquitto.org"
#define MQTT_BROKER_PORT    "1883"
#define MQTT_BROKER_ENC     "0"   // 0 => no TLS
#define MQTT_TOPIC          "test22052077/ble_beacons"


typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED = 1
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_DISCONNECTED;


static void wifi_mgmt_callback(wifi_management_event_t event, void *param) {
    if (event == WIFI_MGMT_EVENT_DISCONNECT) {
        printf("[WIFI] Connection lost! Will retry in 5 minutes.\n");
        wifi_status = WIFI_DISCONNECTED;

        // Disconnect MQTT cleanly if running
        char *mqtt_disconnect_args[] = { "mqtt", "disconnect" };
        cmd_mqtt(2, mqtt_disconnect_args);
        printf("[MQTT] Disconnected due to Wi-Fi loss.\n");
    } else if (event == WIFI_MGMT_EVENT_CONNECT_SUCCESS) {
        printf("[WIFI] Connected successfully. Reconnecting to MQTT...\n");
        wifi_status = WIFI_CONNECTED;

        // Ensure previous MQTT connection is closed before reconnecting
        char *mqtt_disconnect_args[] = { "mqtt", "disconnect" };
        cmd_mqtt(2, mqtt_disconnect_args);
        sys_ms_sleep(1000); // Short delay to ensure cleanup

        // Reconnect to MQTT after Wi-Fi restores
        char *mqtt_args[] = {
            "mqtt", "connect",
            (char *)MQTT_BROKER_HOST,
            (char *)MQTT_BROKER_PORT,
            (char *)MQTT_BROKER_ENC
        };
        cmd_mqtt(5, mqtt_args);
    }
}






static bool wifi_try_connect(const char *ssid, const char *password) {
    printf("[WIFI] Trying to connect to SSID: %s\n", ssid);
    int ret = wifi_management_connect((char *)ssid, (char *)password, /*blocked=*/1);

    if (ret == 0) {
        printf("[WIFI] Successfully connected to %s!\n", ssid);
        return true;
    } else {
        printf("[WIFI] Connection to %s failed (error: %d).\n", ssid, ret);
        return false;
    }
}



static void wifi_connect_handler(void) {
    bool connected = false;

    // Try PRIMARY Wi-Fi first
    if (wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD)) {
        connected = true;
    } else {
        // If primary Wi-Fi fails, try BACKUP Wi-Fi
        if (wifi_try_connect(BACKUP_WIFI_SSID, BACKUP_WIFI_PASSWORD)) {
            connected = true;
        }
    }

    if (connected) {
        wifi_status = WIFI_CONNECTED;
        printf("[WIFI] Successfully connected to Wi-Fi.\n");
    } else {
        wifi_status = WIFI_DISCONNECTED;
        printf("[WIFI] Both Wi-Fi connections failed. Running in offline mode.\n");
    }
}







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

static void wifi_monitor_task(void *arg) {
    (void)arg;

    while (1) {
        // Check if we are disconnected
        if (wifi_status == WIFI_DISCONNECTED) {
            printf("[WIFI] Disconnected. Retrying in 5 minutes...\n");
            sys_ms_sleep(300000);  // Wait 5 minutes before retrying

            printf("[WIFI] Retrying Wi-Fi connection...\n");

            // Try reconnecting
            if (wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
                wifi_try_connect(BACKUP_WIFI_SSID, BACKUP_WIFI_PASSWORD)) {
                wifi_status = WIFI_CONNECTED;
                printf("[WIFI] Reconnected successfully.\n");

                // Reconnect MQTT after Wi-Fi is back
                char *mqtt_args[] = {
                    "mqtt", "connect",
                    (char *)MQTT_BROKER_HOST,
                    (char *)MQTT_BROKER_PORT,
                    (char *)MQTT_BROKER_ENC
                };
                cmd_mqtt(5, mqtt_args);
            } else {
                printf("[WIFI] Still disconnected. Will retry in another 5 minutes.\n");
            }
        }

        // Sleep 5 seconds before checking again
        sys_ms_sleep(5000);
    }
}






// --------------------------------------------------------------------
// RTOS Task: Wi-Fi, MQTT, BLE Scanning, Data Filtering & Publishing
// --------------------------------------------------------------------
static void demo_task(void *arg)
{
    (void)arg;

    //----------------------------------------------------------------
    // 1) Initial Wi-Fi scan + connect
    //----------------------------------------------------------------
    printf("[DEMO_TASK] Starting Wi-Fi Connection Handler...\r\n");
    wifi_connect_handler();  // Only tries once at startup

    // Start Wi-Fi monitor task (runs in the background)
    os_task_t wifi_task_handle = sys_task_create_dynamic(
        (uint8_t*)"wifi_monitor_task",
        2048,  // Smaller stack size
        OS_TASK_PRIORITY(1),
        wifi_monitor_task,
        NULL
    );

    if (!wifi_task_handle) {
        dbg_print(ERR, "[MAIN] Failed to create Wi-Fi monitor task\r\n");
        for (;;) {}
    }

    //----------------------------------------------------------------
    // 2) Wait for Wi-Fi before MQTT connection
    //----------------------------------------------------------------
    while (wifi_status == WIFI_DISCONNECTED) {
        printf("[DEMO_TASK] Waiting for Wi-Fi...\n");
        sys_ms_sleep(5000);
    }

    printf("[DEMO_TASK] Wi-Fi connected! Proceeding to MQTT setup...\r\n");

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
        if (wifi_status == WIFI_DISCONNECTED) {
            printf("[DEMO_TASK] Wi-Fi is down. Running in offline mode.\n");
        } else {
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
        }

        // Sleep 7s before checking again
        sys_ms_sleep(7000); // BLE CHECK
    }

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

