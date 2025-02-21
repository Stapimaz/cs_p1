#ifndef STORAGE_H
#define STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <app_adapter_mgr.h>

/**
 * Gateway MAC storage
 */
void gw_mac_read_from_flash(void);
void gw_mac_write_to_flash(const char *mac_str);
void init_gw_mac(void);
const char* get_gateway_mac(void);

/**
 * Offline beacon storage
 */
typedef struct __attribute__((packed)) {
    uint8_t  peer_mac[6];
    uint8_t  device_type;
    uint32_t msg_counter;
    int16_t  temperature;
    int16_t  humidity;
    uint16_t battery;
    uint8_t  reed_relay;
    uint8_t  accel;
    uint8_t  offline;
    uint32_t timestamp;
} offline_record_t;

void offline_data_init(void);
void offline_data_erase(void);
void save_offline_record(const offline_record_t *rec);
void offline_flush_to_mqtt(void);

#ifdef __cplusplus
}
#endif

#endif // STORAGE_H

