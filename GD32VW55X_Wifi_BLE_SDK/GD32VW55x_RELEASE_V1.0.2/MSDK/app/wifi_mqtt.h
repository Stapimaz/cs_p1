#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

extern char g_primary_ssid[64];
extern char g_primary_password[64];
extern char g_backup_ssid[64];
extern char g_backup_password[64];

extern bool resetAfterOTA;

// Our Wi-Fi status
typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED    = 1
} wifi_status_t;

// Exposed getters/setters for Wi-Fi status
wifi_status_t wifi_get_status(void);
void set_wifi_status(wifi_status_t new_status);

// External references to these SSIDs, used by wifi_reconnect_task
extern char g_primary_ssid[64];
extern char g_primary_password[64];
extern char g_backup_ssid[64];
extern char g_backup_password[64];

// Connect logic
void wifi_connect_handler(void);
bool wifi_try_connect(const char *ssid, const char *pw);

// MQTT wrappers
void my_mqtt_cmd_connect(void);
void my_mqtt_cmd_disconnect(void);
void my_mqtt_cmd_publish(const char *payload);
bool mqtt_client_is_connected_ok(void);

// SNTP + RTC
void start_sntp_and_sync(void);
uint32_t get_current_epoch_time(void);
void sntp_stop(void);

// OTA
void start_ota_demo(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MQTT_H

