#include <stdint.h>
#include "gd32vw55x_platform.h"
#include "wrapper_os.h"
#include "wifi_export.h"
#include "mqtt_cmd.h"         // For MQTT commands
#include "wifi_management.h"  // For Wi-Fi management
#include "ble_init.h"         // For ble_init(), if not already included
#include "app_scan_mgr.h"     // For app_scan_enable(), if needed
#include "dbg_print.h"        // For app_print() or dbg_print()

// Wi-Fi credentials
#define WIFI_SSID      "Mobiliz NaRGE_Guest"
#define WIFI_PASSWORD  "M@b!lz12.*"

// Retry configuration
#define WIFI_RETRY_LIMIT       5
#define WIFI_RETRY_INTERVAL    2000 // in milliseconds

/**
 * @brief Connect to Wi-Fi with retries
 *
 * @return int 0 if connected successfully, -1 otherwise
 */
static int wifi_connect_with_retry(void) {
    int retry_count = 0;
    int status;

    while (retry_count < WIFI_RETRY_LIMIT || WIFI_RETRY_LIMIT == 0) {
        app_print("Attempting to connect to Wi-Fi...\n");
        status = wifi_management_connect(WIFI_SSID, WIFI_PASSWORD, 1); // Blocked connect
        if (status == 0) {
            app_print("Wi-Fi connected successfully.\n");
            return 0;
        }

        app_print("Wi-Fi connection failed. Retrying...\n");
        sys_ms_sleep(WIFI_RETRY_INTERVAL);
        retry_count++;
    }

    app_print("Wi-Fi connection failed after maximum retries.\n");
    return -1;
}

int main(void)
{
    // 1. Platform init
    platform_init();

    // 2. Initialize BLE
    //    Pass 'true' so it initializes all modules (stack, adapter, scanning, etc.)
    //    If your BLE library requires a second call like ble_wait_ready(), do that too.
    ble_init(true);
    // If your code has ble_wait_ready():
    // ble_wait_ready();

    // 3. Optionally enable scanning if not auto‐enabled.
    //    (If your code auto‐starts scanning elsewhere, skip this.)
    //    For example:
    app_scan_enable(false);
    // 'false' => do not re‐add device if only RSSI changes

    // 4. Initialize Wi‐Fi management
    if (wifi_management_init() != 0) {
        app_print("Failed to initialize Wi-Fi management. Halting...\n");
        return -1;
    }

    // 5. Connect to Wi‐Fi with retry mechanism
    if (wifi_connect_with_retry() != 0) {
        app_print("Failed to connect to Wi-Fi. Halting...\n");
        return -1;
    }

    // 6. Start the OS so that BLE tasks, Wi‐Fi tasks, etc. can run
    sys_os_start();

    // 7. Connect to the MQTT server
    //    (You can replace "broker.emqx.io" with your broker address or IP.)
    char *mqtt_connect_args[] = {
        "mqtt",
        "connect",
        "broker.emqx.io",  // e.g.  "192.168.1.100"
        "1883",            // port
        "0",               // encryption = 0 for none
        // "username",      // Optional
        // "password"       // Optional
    };
    cmd_mqtt(7, mqtt_connect_args);

    // 8. Publish a test message to confirm the connection
    char *mqtt_publish_args[] = {
        "mqtt",
        "publish",
        "test/topic",      // Topic
        "Hello, MQTT!",    // Message
        "0",               // QoS 0
        "0"                // Retain flag
    };
    cmd_mqtt(6, mqtt_publish_args);

    // 9. Subscribe to a test topic
    char *mqtt_subscribe_args[] = {
        "mqtt",
        "subscribe",
        "test/topic",    // Topic
        "0",             // QoS 0
        "1"              // Subscribe (1) vs Unsubscribe (0)
    };
    cmd_mqtt(5, mqtt_subscribe_args);

    // 10. Main task loop
    //     This loop is optional if your OS tasks handle everything,
    //     but sometimes you want an idle or housekeeping loop.
    for (;;) {
        // e.g., check if you want to do anything else
        sys_ms_sleep(1000); // Sleep 1 second
    }

    return 0;
}
