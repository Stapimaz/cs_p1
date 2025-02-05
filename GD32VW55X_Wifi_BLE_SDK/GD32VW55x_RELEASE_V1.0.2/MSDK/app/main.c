/**
 * main.c
 *
 * Demonstrates:
 *  1) OTA firmware update support via ota_demo.c
 *  2) "save_to_flash_storage" function for offline BLE data storage
 *  3) Re-sending offline data upon Wi-Fi reconnect
 *
 * Key features retained from original code:
 *  - Connects to Wi-Fi
 *  - Syncs time via SNTP
 *  - Connects to broker test.mosquitto.org using cmd_mqtt connect
 *  - BLE scanning => either publishes or saves offline
 *  - Reconnect logic that tries every 5 minutes
 *  - Adds optional OTA logic
 *  - Adds offline flash storage for JSON
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
#include "mqtt_cmd.h"          // where cmd_mqtt(...) is declared
#include "lwip/apps/mqtt.h"    // for mqtt_client_is_connected
extern mqtt_client_t *mqtt_client; // declared in mqtt_cmd.c

// SNTP includes
#include "lwip/apps/sntp.h"

// RTC
#include "gd32vw55x_rtc.h"
#include <time.h>

// Flash raw API for storing offline data
#include "raw_flash_api.h"

// OTA includes (enabled if CONFIG_OTA_DEMO_SUPPORT is defined)
#include "ota_demo.h"

// --------------------------------------------------------------------
// Wi-Fi config
// --------------------------------------------------------------------
#define PRIMARY_WIFI_SSID       "Mobiliz NaRGE_Gues"
#define PRIMARY_WIFI_PASSWORD   "M@b!lz12.*"
#define BACKUP_WIFI_SSID        "Redmi Note 10S"
#define BACKUP_WIFI_PASSWORD    "54321012"

// --------------------------------------------------------------------
// MQTT config
// --------------------------------------------------------------------
#define MQTT_BROKER_HOST   "test.mosquitto.org"
#define MQTT_BROKER_PORT   "1883"
#define MQTT_BROKER_ENC    "0"   // 0 => no TLS

#define MQTT_TOPIC         "test22052077/ble_beacons"

// --------------------------------------------------------------------
// OTA config placeholders
// --------------------------------------------------------------------
#ifdef CONFIG_OTA_DEMO_SUPPORT
  #define OTA_SERVER_IP       "192.168.1.100"  // Just a placeholder
  #define OTA_IMAGE_URL       "my_firmware.bin"
#endif

// --------------------------------------------------------------------
// Our Wi-Fi status
// --------------------------------------------------------------------
typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTED    = 1
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_DISCONNECTED;


// --------------------------------------------------------------------
// RTC + SNTP (unchanged from your original code)
// --------------------------------------------------------------------
static uint8_t to_bcd(uint8_t dec) {
    return (uint8_t)((((dec / 10) & 0xF) << 4) | (dec % 10));
}

void set_rtc_by_epoch_time(uint32_t utc_secs)
{
    uint32_t days = utc_secs / 86400;
    uint32_t remain = utc_secs % 86400;
    uint32_t hour = remain / 3600;    remain %= 3600;
    uint32_t minute = remain / 60;
    uint32_t second = remain % 60;

    uint32_t year = 1970;
    while (1) {
        uint32_t ydays = 365;
        // leap year check
        if ((year % 4 == 0 && (year % 100 != 0)) || (year % 400 == 0)) {
            ydays = 366;
        }
        if (days >= ydays) {
            days -= ydays;
            year++;
        } else break;
    }

    static const uint8_t md[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint8_t month = 0;
    uint8_t is_leap = ((year%4==0 && year%100!=0)||(year%400==0)) ? 1:0;
    for (int i=0; i<12; i++){
        uint8_t dm = md[i];
        if(i==1 && is_leap) dm=29;
        if(days >= dm){
            days -= dm;
            month++;
        } else break;
    }
    month++;
    uint8_t day = days + 1;

    uint16_t yoffs = (uint16_t)(year - 2000);
    if(yoffs>99) yoffs=99;

    rtc_parameter_struct r;
    memset(&r, 0, sizeof(r));
    r.year          = to_bcd((uint8_t)yoffs);
    r.month         = to_bcd(month);
    r.date          = to_bcd(day);
    r.day_of_week   = 1; // not used
    r.hour          = to_bcd((uint8_t)hour);
    r.minute        = to_bcd((uint8_t)minute);
    r.second        = to_bcd((uint8_t)second);

    // Example prescalers
    r.factor_asyn    = 0x63;
    r.factor_syn     = 0x13F;
    r.display_format = RTC_24HOUR;
    r.am_pm          = RTC_AM;

    rtc_init(&r);
}

uint32_t get_current_epoch_time(void)
{
    rtc_parameter_struct t;
    rtc_current_time_get(&t);

    uint16_t year = ((t.year>>4)*10 + (t.year&0x0F)) + 2000;
    uint8_t  mon  = ((t.month>>4)*10 + (t.month&0x0F));
    uint8_t  day  = ((t.date >>4)*10 + (t.date &0x0F));
    uint8_t  hr   = ((t.hour >>4)*10 + (t.hour &0x0F));
    uint8_t  min  = ((t.minute>>4)*10 + (t.minute&0x0F));
    uint8_t  sec  = ((t.second>>4)*10 + (t.second&0x0F));

    static const uint16_t mdays[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

    uint8_t leap = (mon>2 && (((year%4==0)&&(year%100!=0))||(year%400==0))) ? 1:0;
    uint32_t days_since_1970 = (year-1970)*365 + ((year-1969)/4)
       + mdays[mon-1] + (day-1) + leap;

    uint32_t epoch = days_since_1970*86400 + hr*3600 + min*60 + sec;
    return epoch;
}

static void start_sntp_and_sync(void)
{
    if(!sntp_enabled()){
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
    }
    sys_ms_sleep(8000);
}


// --------------------------------------------------------------------
// Basic Wi-Fi connect logic
// --------------------------------------------------------------------
static bool wifi_try_connect(const char *ssid, const char *pw)
{
    printf("[WIFI] Trying connect => %s\r\n", ssid);
    int ret = wifi_management_connect((char*)ssid, (char*)pw, /*blocked=*/1);
    if (ret == 0){
        printf("[WIFI] Connect OK => %s\r\n", ssid);
        return true;
    }
    printf("[WIFI] Connect FAIL => %s (err=%d)\r\n", ssid, ret);
    return false;
}

static void wifi_connect_handler(void)
{
    if(wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
       wifi_try_connect(BACKUP_WIFI_SSID,  BACKUP_WIFI_PASSWORD)) {
        wifi_status = WIFI_CONNECTED;
        printf("[WIFI] Connected.\r\n");
    } else {
        wifi_status = WIFI_DISCONNECTED;
        printf("[WIFI] Offline.\r\n");
    }
}


// --------------------------------------------------------------------
// cmd_mqtt usage wrappers
// --------------------------------------------------------------------
static void my_mqtt_cmd_connect(void)
{
    char *args[] = {
        "mqtt", "connect",
        (char *)MQTT_BROKER_HOST,
        (char *)MQTT_BROKER_PORT,
        (char *)MQTT_BROKER_ENC
    };
    cmd_mqtt(5, args);
}

static void my_mqtt_cmd_disconnect(void)
{
    char *args[] = { "mqtt", "disconnect" };
    cmd_mqtt(2, args);
}

static void my_mqtt_cmd_publish(const char *payload)
{
    // e.g. "mqtt publish <topic> <payload> 0 0"
    char *args[] = {
        "mqtt", "publish",
        (char*)MQTT_TOPIC,
        (char*)payload,
        "0",  // QoS=0
        "0"   // Retain=0
    };
    cmd_mqtt(6, args);
}


// --------------------------------------------------------------------
// OTA DEMO Support
// --------------------------------------------------------------------
#ifdef CONFIG_OTA_DEMO_SUPPORT
static void start_ota_demo(void)
{
    // Just a placeholder function that starts OTA if you want
    // In a real scenario, you'd do DNS lookups, get image URL, etc.
    // Here, we assume direct IP input to ota_demo_cfg_init.
    if(ota_demo_cfg_init(OTA_SERVER_IP, OTA_IMAGE_URL) == 0){
        printf("[OTA] Starting OTA Demo => IP=%s, URL=%s\r\n", OTA_SERVER_IP, OTA_IMAGE_URL);
        if(ota_demo_start() != 0){
            printf("[OTA] ota_demo_start() failed.\r\n");
        }
    } else {
        printf("[OTA] ota_demo_cfg_init() failed.\r\n");
    }
}
#endif // CONFIG_OTA_DEMO_SUPPORT


// --------------------------------------------------------------------
// Offline Flash Storage Implementation
// --------------------------------------------------------------------
// We reserve a 64KB region at offset 0x00300000 in the 4MB flash.
// Adjust these if your memory map requires something else.
#define OFFLINE_DATA_BASE   (0x00300000)
#define OFFLINE_DATA_SIZE   (0x00010000)  // 64KB

// We'll store in flash:
//   [0..3]  = a "valid" magic or 0 if erased
//   [4..7]  = flash-stored offset (uint32_t, if we wanted robust power-loss recovery)
//   [8.. ]  = repeated blocks of [4 bytes length][message bytes]
//
// However, to avoid repeated rewriting of the offset in flash,
// we keep an *in-RAM* offset that increments for each new message.
// We only store offset to flash once on the first initialization
// so we mark the area valid. The offset in flash won't be updated
// each time. If you lose power while offline, data appended after
// the last offset update won't be recoverable, but for a simpler
// demonstration, this approach avoids bit-setting issues.
//
// If you require power-loss persistence for every message, you need
// a more advanced "log-based" or wear-leveling approach that can
// handle repeated writes without losing bits.
//
// Steps for this simpler approach:
//   1) If the region is invalid or blank, we erase the sector, then store a "valid" marker
//      plus set offset=0 in flash. We also store offset=0 in a RAM variable s_ram_offset.
//   2) Each time we store a JSON, we do not rewrite offset in flash. We only use s_ram_offset
//      to find the next location in the region, then raw_flash_write the new record.
//   3) On flush (when we reconnect), we read from [8.. s_ram_offset], publish each record,
//      then erase the entire region so offset goes back to 0 in both flash and RAM.

static bool offline_initialized = false;
static uint32_t s_ram_offset = 0;  // Our in-RAM offset for new messages.

#define OFFLINE_MAGIC_VALID  0xA5A5A5A5  // An arbitrary "valid" marker

/**
 * @brief Erase entire offline region, and set magic + offset=0
 */
static void offline_data_erase(void)
{
    // Erase 64KB region
    raw_flash_erase(OFFLINE_DATA_BASE, OFFLINE_DATA_SIZE);

    // Set magic + offset=0
    uint32_t init_data[2];
    init_data[0] = OFFLINE_MAGIC_VALID;
    init_data[1] = 0; // offset
    raw_flash_write(OFFLINE_DATA_BASE, init_data, sizeof(init_data));

    // Also set our RAM offset to 0
    s_ram_offset = 0;

    printf("[FLASH] Erased offline region => offset=0.\r\n");
}

/**
 * @brief Initialize offline data region
 */
static void offline_data_init(void)
{
    // read the first 8 bytes: [ magic, offset ]
    uint32_t init_data[2];
    raw_flash_read(OFFLINE_DATA_BASE, init_data, sizeof(init_data));
    uint32_t magic_in_flash = init_data[0];
    uint32_t flash_offset   = init_data[1];

    // If magic is invalid or 0xFFFFFFFF => we erase
    if(magic_in_flash != OFFLINE_MAGIC_VALID) {
        printf("[FLASH] Offline storage is uninitialized => erasing region.\r\n");
        offline_data_erase(); // sets s_ram_offset=0
    } else {
        // We have a valid region. Use that offset.
        s_ram_offset = flash_offset;
        printf("[FLASH] Offline region valid, stored offset=%u\r\n", (unsigned)s_ram_offset);
    }
    offline_initialized = true;
}

/**
 * @brief Save a JSON payload to flash (when offline)
 *
 * Using our RAM offset s_ram_offset. We do NOT rewrite offset in flash each time.
 */
static void save_to_flash_storage(const char *payload)
{
    if(!offline_initialized){
        offline_data_init();
    }

    size_t msg_len = strlen(payload);
    if(msg_len == 0){
        return;
    }

    // We'll store new record at OFFLINE_DATA_BASE + 8 + s_ram_offset
    // Format: [4 bytes length][payload bytes]
    uint32_t needed = 4 + msg_len;

    // Check if enough space remains
    //   total region size = OFFLINE_DATA_SIZE
    //   first 8 bytes used for magic + offset
    //   so max data area = OFFLINE_DATA_SIZE - 8
    if(s_ram_offset + needed > (OFFLINE_DATA_SIZE - 8)) {
        printf("[FLASH] Not enough space for offline storage. Erasing everything.\r\n");
        offline_data_erase();
    }

    // Record location
    uint32_t record_addr = OFFLINE_DATA_BASE + 8 + s_ram_offset;

    // 1) write length
    uint32_t length_le = (uint32_t)msg_len;
    raw_flash_write(record_addr, &length_le, sizeof(length_le));

    // 2) write payload
    record_addr += 4;
    raw_flash_write(record_addr, payload, msg_len);

    // 3) update in-RAM offset
    s_ram_offset += needed;

    printf("[FLASH] Offline data saved => msg_len=%u (RAM_offset=%u)\r\n",
           (unsigned)msg_len, (unsigned)s_ram_offset);
}

/**
 * @brief Read all stored JSON records from flash, publish them, then erase the region
 */
static void offline_flush_to_mqtt(void)
{
    if(!offline_initialized){
        offline_data_init();
    }

    // If no data was stored in this power cycle, s_ram_offset=0 => skip
    if(s_ram_offset == 0){
        printf("[FLASH] No offline data stored.\r\n");
        return;
    }

    printf("[FLASH] Found %u bytes of offline data => flushing...\r\n",(unsigned)s_ram_offset);

    uint32_t read_pos = 0;
    while(read_pos < s_ram_offset) {
        // read record length
        uint32_t length = 0;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &length, sizeof(length));
        read_pos += 4; // moved past length field

        if(length == 0 || length == 0xFFFFFFFF) {
            // blank or invalid => stop
            break;
        }
        if(read_pos + length > s_ram_offset) {
            // out of range => stop
            break;
        }

        // read JSON string
        char temp_buf[512];
        if(length >= sizeof(temp_buf)) {
            printf("[FLASH] skipping large record => len=%u\r\n", (unsigned)length);
            // skip it
            read_pos += length;
            continue;
        }

        memset(temp_buf, 0, sizeof(temp_buf));
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, temp_buf, length);
        read_pos += length;

        // Now publish this record
        printf("[FLASH] Publishing offline record: %s\r\n", temp_buf);
        my_mqtt_cmd_publish(temp_buf);

        sys_ms_sleep(200); // small delay between publishes
    }

    // After sending all, erase entire region => offset=0
    offline_data_erase();
    printf("[FLASH] Offline data flush complete.\r\n");
}


// --------------------------------------------------------------------
// Wi-Fi reconnect => tries every 5 min, then does SNTP + MQTT + flush
// --------------------------------------------------------------------
static void wifi_reconnect_task(void *arg)
{
    (void)arg;
    while(1) {
        if(wifi_status == WIFI_DISCONNECTED) {
            printf("[RECONNECT] Offline => next attempt in 5 mins.\r\n");
            sys_ms_sleep(5UL*60UL*1000UL);

            // Attempt connect
            if(wifi_try_connect(PRIMARY_WIFI_SSID, PRIMARY_WIFI_PASSWORD) ||
               wifi_try_connect(BACKUP_WIFI_SSID,  BACKUP_WIFI_PASSWORD)){
                wifi_status = WIFI_CONNECTED;
                printf("[RECONNECT] Wi-Fi reconnected!\r\n");
                start_sntp_and_sync();

                // forcibly disconnect old MQTT
                my_mqtt_cmd_disconnect();
                sys_ms_sleep(1000);

                // connect again
                my_mqtt_cmd_connect();

                // Flush offline data (if any) now that we're online
                if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
                    offline_flush_to_mqtt();
                }

#ifdef CONFIG_OTA_DEMO_SUPPORT
                // If you want an automatic OTA attempt upon reconnection, do:
                start_ota_demo();
#endif

            } else {
                // remain offline
                printf("[RECONNECT] Still offline.\r\n");
            }
        } else {
            // check if we truly remain connected
            extern struct wifi_vif_tag wifi_vif_tab[];
            #define WIFI_STA_STATE_CONNECTED 6
            if(wifi_vif_tab[0].sta.state != WIFI_STA_STATE_CONNECTED) {
                printf("[RECONNECT] Wi-Fi dropped.\r\n");
                wifi_status = WIFI_DISCONNECTED;

                // stop SNTP + MQTT
                sntp_stop();
                my_mqtt_cmd_disconnect();
            }
        }

        sys_ms_sleep(10000);
    }
}


// --------------------------------------------------------------------
// The main scanning/publishing task
// --------------------------------------------------------------------
static void demo_task(void *arg)
{
    (void)arg;
    printf("[demo_task] Wi-Fi connecting...\r\n");
    wifi_connect_handler();

    // Create background reconnect
    os_task_t rtask = sys_task_create_dynamic(
        (uint8_t*)"wifi_reconnect_task",
        2048,
        OS_TASK_PRIORITY(0),
        wifi_reconnect_task,
        NULL
    );
    if(!rtask) {
        dbg_print(ERR,"[MAIN] can't create wifi_reconnect_task\r\n");
        for(;;){}
    }

    // If connected => do SNTP + MQTT + optional OTA
    if(wifi_status==WIFI_CONNECTED){
        start_sntp_and_sync();
        my_mqtt_cmd_connect(); // "mqtt connect test.mosquitto.org 1883 0"

#ifdef CONFIG_OTA_DEMO_SUPPORT
        // Start OTA if you want it on first connect
        start_ota_demo();
#endif

        // Also flush any leftover data if MQTT is available
        if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
            offline_flush_to_mqtt();
        }
    }

    // BLE scanning forever
    while(1){
        for (uint8_t idx=0; ; idx++){
            dev_info_t* dev = scan_mgr_find_dev_by_idx(idx);
            if(!dev) break;
            if(dev->adv_len < 26) continue;
            // check adv_data[5]==0x4D && adv_data[6]==0x5A => "MZ"
            if(dev->adv_data[5]==0x4D && dev->adv_data[6]==0x5A) {
                printf("[demo_task] Found Target Beacon!\r\n");

                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter= (dev->adv_data[8]<<24)|(dev->adv_data[9]<<16)|
                                      (dev->adv_data[10]<<8)| dev->adv_data[11];
                int16_t temperature = (int16_t)((dev->adv_data[12]<<8)|dev->adv_data[13]);
                int16_t humidity    = (int16_t)((dev->adv_data[14]<<8)|dev->adv_data[15]);
                uint16_t batt       = (uint16_t)((dev->adv_data[16]<<8)|dev->adv_data[17]);
                bool reed_relay     = dev->adv_data[18]?true:false;
                uint8_t accel       = dev->adv_data[19];

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
                    device_type, msg_counter, temperature, humidity, batt,
                    reed_relay?"true":"false", accel
                );

                if(wifi_status==WIFI_CONNECTED && mqtt_client && mqtt_client_is_connected(mqtt_client)) {
                    // Publish now
                    my_mqtt_cmd_publish(json_payload);

                    // debug timestamp
                    uint32_t t = get_current_epoch_time();
                    printf("TIMESTAMP(Online): %lu\r\n", (unsigned long)t);

                } else {
                    // offline => store
                    uint32_t t = get_current_epoch_time();
                    printf("TIMESTAMP(Offline): %lu\r\n", (unsigned long)t);

                    // Add timestamp to JSON before storing offline
                    size_t curlen = strlen(json_payload);
                    if(curlen < (sizeof(json_payload)-30)) {
                        snprintf(json_payload+curlen-1, sizeof(json_payload)-curlen,
                            ",\"Timestamp\":%lu}", (unsigned long)t);
                    }
                    save_to_flash_storage(json_payload);
                }
            }
        }

        sys_ms_sleep(7000);
    }

    sys_task_delete(NULL);
}


// --------------------------------------------------------------------
// main => OS init, ble_init, wifi_init, raw_flash_init, create "demo_task"
// --------------------------------------------------------------------
int main(void)
{
    // Initialize OS and platform
    sys_os_init();
    platform_init();

    // Initialize BLE
    ble_init(true);

    // Initialize Wi-Fi
    if(wifi_init()){
        dbg_print(ERR,"[MAIN] wifi_init failed!\r\n");
        for(;;){}
    }

    // Initialize raw flash subsystem so we can do offline storage
    raw_flash_init();

    // Create the main demo task
    os_task_t handle = sys_task_create_dynamic(
        (uint8_t*)"demo_task",
        4096,
        OS_TASK_PRIORITY(1),
        demo_task,
        NULL
    );
    if(!handle){
        dbg_print(ERR,"[MAIN] can't create demo_task\r\n");
        for(;;){}
    }

    sys_os_start();
    for(;;){}
    return 0;
}
