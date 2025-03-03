#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "storage.h"
#include "raw_flash_api.h"
#include "dbg_print.h"

// For Wi-Fi + MQTT checks
#include "wifi_mqtt.h"

// For sys_ms_sleep
#include "wrapper_os.h"

// We'll assume your environment already has sys_ms_sleep(...) declared.
// If not, you can do a small static wrapper as in app_tasks.c or remove it
// if your environment does not need it.

// Gateway MAC definitions from original code
#define GW_MAC_FLASH_ADDR  (0x002F0000U)    // Example sector
#define GW_MAC_FLASH_SIZE  (0x1000U)        // We'll assume a 4KB sector
#define GW_MAC_FLASH_MAGIC (0xA5123456UL)

static char g_gateway_mac[18] = {0};

typedef struct {
    uint32_t magic;
    char     mac_str[18];
} gw_mac_store_t;

void gw_mac_read_from_flash(void)
{
    gw_mac_store_t info;
    raw_flash_read(GW_MAC_FLASH_ADDR, &info, sizeof(info));

    if ((info.magic == GW_MAC_FLASH_MAGIC) && (info.mac_str[0] != '\0')) {
        strncpy(g_gateway_mac, info.mac_str, sizeof(g_gateway_mac));
    } else {
        g_gateway_mac[0] = '\0';
    }
}

void gw_mac_write_to_flash(const char* mac_str)
{
    // read existing
    gw_mac_store_t info;
    raw_flash_read(GW_MAC_FLASH_ADDR, &info, sizeof(info));

    // check if same
    if ((info.magic == GW_MAC_FLASH_MAGIC) &&
        (strncmp(info.mac_str, mac_str, sizeof(info.mac_str)) == 0))
    {
        printf("[GW MAC FLASH] Stored MAC: '%s' \r\n", mac_str);
        return;
    }

    // erase & write
    printf("[GW MAC FLASH] Writing new MAC '%s'.\r\n", mac_str);
    raw_flash_erase(GW_MAC_FLASH_ADDR, GW_MAC_FLASH_SIZE);

    gw_mac_store_t new_info;
    new_info.magic = GW_MAC_FLASH_MAGIC;
    memset(new_info.mac_str, 0, sizeof(new_info.mac_str));
    strncpy(new_info.mac_str, mac_str, sizeof(new_info.mac_str) - 1);

    raw_flash_write(GW_MAC_FLASH_ADDR, &new_info, sizeof(new_info));
    printf("[GW MAC FLASH] MAC '%s' stored at 0x%08X.\r\n", mac_str, (unsigned)GW_MAC_FLASH_ADDR);
}

void init_gw_mac(void)
{
    // from hardware
    uint8_t mac[6];
    app_get_local_mac(mac);

    char temp[18];
    snprintf(temp, sizeof(temp),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

    printf("[GW MAC] (Hardware) => %s\r\n", temp);
    strncpy(g_gateway_mac, temp, sizeof(g_gateway_mac));

    gw_mac_write_to_flash(g_gateway_mac);
}

const char* get_gateway_mac(void)
{
    return g_gateway_mac;
}

// --------------------------------------------------------------------
// Offline data region from original code
// --------------------------------------------------------------------
#define OFFLINE_DATA_BASE   (0x00320000)
#define OFFLINE_DATA_SIZE   (0x000C0000)  // 768KB
#define OFFLINE_MAGIC_VALID 0xA5A5A5A5

static bool offline_initialized = false;
static uint32_t s_ram_offset = 0;



void offline_data_erase(void)
{
    // Short wait to ensure no immediate transmissions
    sys_ms_sleep(1000);

    // Erase the entire OFFLINE_DATA_SIZE in 4KB increments
    const uint32_t page_size = 0x1000; // 4 KB
    uint32_t start = OFFLINE_DATA_BASE;
    uint32_t end   = OFFLINE_DATA_BASE + OFFLINE_DATA_SIZE;

    for (uint32_t addr = start; addr < end; addr += page_size) {
        // Each call erases one 4KB page
        raw_flash_erase(addr, page_size);

        // Let the OS + Wi-Fi driver handle interrupts for a few ms
        sys_ms_sleep(5);
    }

    // After erasing, write our magic + offset header
    uint32_t init_data[2];
    init_data[0] = OFFLINE_MAGIC_VALID;
    init_data[1] = 0;

    // Another short gap
    sys_ms_sleep(200);

    raw_flash_write(OFFLINE_DATA_BASE, init_data, sizeof(init_data));

    s_ram_offset = 0;
    printf("[FLASH] Erased offline region in chunks => offset=0.\r\n");
}


void offline_data_init(void)
{
    uint32_t init_data[2];
    raw_flash_read(OFFLINE_DATA_BASE, init_data, sizeof(init_data));
    uint32_t magic_in_flash = init_data[0];
    uint32_t flash_offset   = init_data[1];

    if (magic_in_flash != OFFLINE_MAGIC_VALID) {
        printf("[FLASH] Offline storage is uninitialized => erasing region.\r\n");
        offline_data_erase();
    } else {
        s_ram_offset = flash_offset;
        printf("[FLASH] Offline region valid, stored offset=%u\r\n", (unsigned)s_ram_offset);
    }
    offline_initialized = true;
}

void save_offline_record(const offline_record_t *rec)
{
    if(!offline_initialized){
        offline_data_init();
    }
    const uint32_t record_size = sizeof(offline_record_t);
    const uint32_t needed = 4 + record_size; // length + record

    if(s_ram_offset + needed > (OFFLINE_DATA_SIZE - 8)) {
        printf("[FLASH] Not enough space for offline record. Erasing everything.\r\n");
        offline_data_erase();
    }

    uint32_t record_addr = OFFLINE_DATA_BASE + 8 + s_ram_offset;
    raw_flash_write(record_addr, &record_size, 4);
    record_addr += 4;
    raw_flash_write(record_addr, rec, record_size);

    s_ram_offset += needed;
    printf("[FLASH] Offline record saved => offset now=%u\r\n", (unsigned)s_ram_offset);
}

void offline_flush_to_mqtt(void)
{
    if(!offline_initialized){
        offline_data_init();
    }
    if(s_ram_offset == 0){
        printf("[FLASH] No offline data stored.\r\n");
        return;
    }

    printf("[FLASH] Found %u bytes of offline data => flushing...\r\n",(unsigned)s_ram_offset);
    uint32_t read_pos = 0;

    // Delay before flush
    sys_ms_sleep(200);

    while(read_pos < s_ram_offset) {
        // Verify MQTT
        if(!mqtt_client_is_connected_ok()) {
            printf("[FLASH] MQTT disconnected during flush - aborting\r\n");
            return;
        }

        uint32_t length = 0;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &length, 4);
        read_pos += 4;

        if(length == 0 || length == 0xFFFFFFFF) {
            break;
        }
        if(read_pos + length > s_ram_offset) {
            break;
        }
        if(length != sizeof(offline_record_t)) {
            printf("[FLASH] Found record with invalid length=%u => erasing region.\r\n",(unsigned)length);
            offline_data_erase();
            return;
        }

        offline_record_t rec;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &rec, length);
        read_pos += length;

        char json_payload[256];
        snprintf(json_payload, sizeof(json_payload),
                 "{"
                   "\"MAC\":\"%02X:%02X:%02X:%02X:%02X:%02X\","
                   "\"GW_MAC\":\"%s\","
                   "\"DeviceType\":%u,"
                   "\"MessageCounter\":%u,"
                   "\"Temperature\":%d,"
                   "\"Humidity\":%d,"
                   "\"BatteryVoltage\":%u,"
                   "\"ReedRelay\":%s,"
                   "\"Accelerometer\":%u,"
                   "\"Timestamp\":%lu"
                 "}",
                 rec.peer_mac[5], rec.peer_mac[4], rec.peer_mac[3],
                 rec.peer_mac[2], rec.peer_mac[1], rec.peer_mac[0],
                 get_gateway_mac(),
                 rec.device_type,
                 rec.msg_counter,
                 rec.temperature,
                 rec.humidity,
                 rec.battery,
                 (rec.reed_relay ? "true" : "false"),
                 rec.accel,
                 (unsigned long)rec.timestamp
        );

        printf("[FLASH] Publishing offline record: %s\r\n", json_payload);
        my_mqtt_cmd_publish(json_payload);

        sys_ms_sleep(100);
    }

    // Delay before erasing
    sys_ms_sleep(1500);
    offline_data_erase();

    // Final delay
    sys_ms_sleep(1000);
    printf("[FLASH] Offline data flush complete.\r\n");
}
