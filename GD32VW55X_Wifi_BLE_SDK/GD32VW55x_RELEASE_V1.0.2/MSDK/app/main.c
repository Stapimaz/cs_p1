#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "gd32vw55x.h"
#include "gd32vw55x_platform.h"
#include "wrapper_os.h"
#include "dbg_print.h"

// BLE includes
#include "ble_init.h"

// Wi-Fi includes
#include "wifi_init.h"
#include "wifi_management.h"

// Flash
#include "raw_flash_api.h"

// Our new headers
#include "app_tasks.h"
#include "sensor_am2301.h"
#include "wifi_mqtt.h"
#include "storage.h"
#include "http_server.h"


int main(void)
{
    // Initialize OS, hardware platform, debug print, etc.
    sys_os_init();
    platform_init();

    dbg_print(NOTICE, "Firmware version: 1.1\r\n");

    // BLE init (from original code)
    ble_init(true);

    // Initialize Wi-Fi station driver
    if (wifi_init()) {
        dbg_print(ERR, "[MAIN] wifi_init failed!\r\n");
        for(;;) {}
    }

    // Allow concurrent SoftAP + STA
    wifi_management_concurrent_set(1);

    // Initialize raw flash
    raw_flash_init();

    // Create the main task (BLE scanning, Wi-Fi connect, offline flush, etc.)
    os_task_t mainTaskHandle = sys_task_create_dynamic(
        (uint8_t*)"main_task",
        4096,
        OS_TASK_PRIORITY(4),
        main_task,
        NULL
    );
    if (!mainTaskHandle) {
        dbg_print(ERR, "[MAIN] can't create main_task\r\n");
        for(;;) {}
    }


    // Create the sensor task
    os_task_t sensorTaskHandle = sys_task_create_dynamic(
        (uint8_t*)"sensor_task",
        2048,
        OS_TASK_PRIORITY(0),
        sensor_task,
        NULL
    );
    if (!sensorTaskHandle) {
        dbg_print(ERR, "[MAIN] can't create sensor_task\r\n");
        for(;;) {}
    }

    // Everything is ready; launch the OS
    sys_os_start();
    // Should never reach here.
    for(;;) {}
    return 0;
}
