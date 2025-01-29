/**
 * main.c
 *
 * Example showing:
 *  - Single RTOS task
 *  - Wi-Fi scan + connect
 *  - MQTT connect (using cmd_mqtt)
 *  - BLE init + scanning (using app_scan_mgr's public API)
 *  - Optionally parse scanned devices in the task's while loop
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
// Adjust these constants for your Wi-Fi and MQTT environment
// --------------------------------------------------------------------
#define MY_WIFI_SSID        "Mobiliz NaRGE_Guest"
#define MY_WIFI_PASSWORD    "M@b!lz12.*"

// Example broker
#define MQTT_BROKER_HOST    "broker.hivemq.com"
#define MQTT_BROKER_PORT    "1883"
#define MQTT_BROKER_ENC     "0"   // 0 => no TLS

// --------------------------------------------------------------------
// Single RTOS task that does:
//   1) Wi-Fi connect
//   2) MQTT connect
//   3) BLE scanning via app_scan_enable()
//   4) Periodically check the scanned device list
//   5) If a known device is found, do an MQTT publish
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
    // 2) Connect to MQTT broker using cmd_mqtt
    //----------------------------------------------------------------
    {
        // Format: mqtt connect <server_ip> <server_port> <encryption_mode> [<username> <password>]
        char *mqtt_args[] = {
            "mqtt", "connect",
            (char *)MQTT_BROKER_HOST,
            (char *)MQTT_BROKER_PORT,
            (char *)MQTT_BROKER_ENC
        };
        cmd_mqtt(5, mqtt_args);
        printf("[DEMO_TASK] cmd_mqtt connect done.\r\n");
    }

    // Optionally enable auto reconnect to broker
    {
        char *auto_rec_args[] = { "mqtt", "auto_reconnect", "1" };
        cmd_mqtt(3, auto_rec_args);
        printf("[DEMO_TASK] MQTT auto reconnect enabled.\r\n");
    }

    //----------------------------------------------------------------
    // 3) Enable BLE scanning
    //
    //    Because ble_init(true) was called in main(), the internal
    //    BLE modules + app_scan_mgr_init() are ready. We can now
    //    start scanning with:
    //        app_scan_enable(false);
    //----------------------------------------------------------------
//    printf("[DEMO_TASK] Enabling BLE scanning.\r\n");
//    sys_ms_sleep(1000);
//    app_scan_enable(false);

    //----------------------------------------------------------------
    // 4) Periodically check the scanned device list
    //    The default scan_mgr_report_hdlr logs new devices,
    //    but let's do an example "publish if known device found."
    //----------------------------------------------------------------
    while (1)
    {
        // Just show the current device list in logs:
        // This prints "new device addr ..." or "dev idx..." in the logs.
        //scan_mgr_list_scanned_devices();

        // OPTIONAL: Example of iterating over each device in the list:
        //   1) We find known MAC addresses
        //   2) If found, we publish an example message
        for (uint8_t idx = 0; ; idx++)
        {
            dev_info_t *dev = scan_mgr_find_dev_by_idx(idx);
            if (!dev) {
                break; // No more devices
            }

            // Suppose we are looking for a known beacon MAC, e.g.:
            //   4F:3A:22:11:66:77  (just an example)
            // Compare reversed or forward depending on your usage.
            if (dev->peer_addr.addr[0] == 0xc5 &&
                dev->peer_addr.addr[1] == 0xb3 &&
                dev->peer_addr.addr[2] == 0xbd &&
                dev->peer_addr.addr[3] == 0x7f &&
                dev->peer_addr.addr[4] == 0x2b &&
                dev->peer_addr.addr[5] == 0xc6)
            {
            	dbg_print(ERR, "EXACT DEVICE FOUND! \n\r");
                // Example: publish to an MQTT topic
                char msg_payload[80];
                snprintf(msg_payload, sizeof(msg_payload),
                         "Found Beacon: %02X:%02X:%02X:%02X:%02X:%02X",
                         dev->peer_addr.addr[5], dev->peer_addr.addr[4],
                         dev->peer_addr.addr[3], dev->peer_addr.addr[2],
                         dev->peer_addr.addr[1], dev->peer_addr.addr[0]);

                // Publish via cmd_mqtt
                char *pub_args[] = {
                    "mqtt", "publish",
                    "test/ble_beacons",   // topic
                    msg_payload,          // payload
                    "0",                  // QoS=0
                    "0"                   // retain=0
                };
                cmd_mqtt(6, pub_args);

                // Possibly remove from the list or do something else
                // scan_mgr_clear_dev_list(); // if you want to clear
            }
        }

        // Sleep 5s before re-checking
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

    // Should never arrive here
    while (1) {}
    return 0;
}
