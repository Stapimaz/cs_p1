/**
 * main.c
 *
 * Demonstrates:
 *  1) OTA firmware update support via ota_demo.c
 *  2) Offline data storage in flash (only dynamic fields to save space)
 *  3) Re-sending offline data upon Wi-Fi *or MQTT* reconnect
 *
 * Key features retained from original code:
 *  - Connects to Wi-Fi
 *  - Syncs time via SNTP
 *  - Connects to broker test.mosquitto.org using cmd_mqtt connect
 *  - BLE scanning => either publishes immediately or saves offline
 *  - Reconnect logic that tries every 5 minutes if Wi-Fi is disconnected
 *  - If Wi-Fi is connected but MQTT is disconnected => we reconnect MQTT
 *  - Adds optional OTA logic
 *  - Stores only dynamic fields in flash for offline use
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
#include "mqtt_cmd.h"
#include "lwip/apps/mqtt.h"
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

// Adapter manager includes, needed for app_get_local_mac()
#include "app_adapter_mgr.h"

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
    #define OTA_SERVER_IP  "91.93.129.88"
    #define OTA_IMAGE_URL  "/kazim/gigadevice/image-ota222.bin"
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
// Store Gateway MAC (initialized in demo_task, not main())
// --------------------------------------------------------------------
static char g_gateway_mac[18]; // "AA:BB:CC:DD:EE:FF" + null terminator

static void init_gw_mac(void)
{
    uint8_t mac[6] = {0};
    // This function must be declared in app_adapter_mgr.h and
    // implemented in app_adapter_mgr.c, for example:
    //   void app_get_local_mac(uint8_t *mac_out)
    //   {
    //       memcpy(mac_out, app_env.id_addr.addr, 6);
    //   }
    app_get_local_mac(mac);

    snprintf(g_gateway_mac, sizeof(g_gateway_mac),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    printf("[GW MAC] => %s\r\n", g_gateway_mac);
}

// --------------------------------------------------------------------
// RTC + SNTP
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

// We have increased this to 768KB to store more messages.
#define OFFLINE_DATA_BASE   (0x00320000)
#define OFFLINE_DATA_SIZE   (0x000C0000)  // 768KB

static bool offline_initialized = false;
static uint32_t s_ram_offset = 0;  // Our in-RAM offset for new records.

#define OFFLINE_MAGIC_VALID  0xA5A5A5A5  // An arbitrary "valid" marker

/**
 * @brief A compact struct of dynamic fields only
 *        so we don't waste space on JSON formatting in flash.
 */
typedef struct __attribute__((packed)) {
    uint8_t  peer_mac[6];     // scanned beacon MAC
    uint8_t  device_type;     // from adv_data[7]
    uint32_t msg_counter;     // from adv_data[8..11]
    int16_t  temperature;     // adv_data[12..13]
    int16_t  humidity;        // adv_data[14..15]
    uint16_t battery;         // adv_data[16..17]
    uint8_t  reed_relay;      // adv_data[18] => 0 or 1
    uint8_t  accel;           // adv_data[19]
    uint8_t  offline;         // 1 if was offline at time of scanning
    uint32_t timestamp;       // valid if offline=1
} offline_record_t; // => 24 bytes packed

/**
 * @brief Erase entire offline region, and set magic + offset=0
 */
static void offline_data_erase(void)
{
    // Erase entire region
    raw_flash_erase(OFFLINE_DATA_BASE, OFFLINE_DATA_SIZE);

    // Write [ magic, offset=0 ] at start
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

    if(magic_in_flash != OFFLINE_MAGIC_VALID) {
        printf("[FLASH] Offline storage is uninitialized => erasing region.\r\n");
        offline_data_erase(); // sets s_ram_offset=0
    } else {
        s_ram_offset = flash_offset;
        printf("[FLASH] Offline region valid, stored offset=%u\r\n", (unsigned)s_ram_offset);
    }
    offline_initialized = true;
}

/**
 * @brief Write a new record into flash (dynamic fields only).
 */
static void save_offline_record(const offline_record_t *rec)
{
    if(!offline_initialized){
        offline_data_init();
    }

    // We store: [4 bytes length][the record struct]
    const uint32_t record_size = sizeof(offline_record_t); // 24
    const uint32_t needed = 4 + record_size;                // 28

    // check space
    if(s_ram_offset + needed > (OFFLINE_DATA_SIZE - 8)) {
        printf("[FLASH] Not enough space for offline record. Erasing everything.\r\n");
        offline_data_erase();
    }

    // The new record location => after the first 8 bytes + s_ram_offset
    uint32_t record_addr = OFFLINE_DATA_BASE + 8 + s_ram_offset;

    // 1) Write length
    raw_flash_write(record_addr, &record_size, sizeof(record_size));
    record_addr += 4;

    // 2) Write the record
    raw_flash_write(record_addr, rec, record_size);

    // 3) Update in-RAM offset
    s_ram_offset += needed;

    printf("[FLASH] Offline record saved => offset now=%u\r\n", (unsigned)s_ram_offset);
}

/**
 * @brief Read all stored records from flash, reconstruct JSON, publish them,
 *        then erase the region if we detect any mismatch or after finishing.
 */
static void offline_flush_to_mqtt(void)
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
    while(read_pos < s_ram_offset) {
        // read record length
        uint32_t length = 0;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &length, sizeof(length));
        read_pos += 4; // move past length field

        // Check for blank or invalid
        if(length == 0 || length == 0xFFFFFFFF) {
            break;
        }

        // Check if out of bounds
        if(read_pos + length > s_ram_offset) {
            break;
        }

        // Must match our struct size exactly
        if(length != sizeof(offline_record_t)) {
            // This means old/corrupt data -> erase region now
            printf("[FLASH] Found record with invalid length=%u => erasing region.\r\n",(unsigned)length);
            offline_data_erase();
            return;
        }

        // read the record
        offline_record_t rec;
        raw_flash_read(OFFLINE_DATA_BASE + 8 + read_pos, &rec, length);
        read_pos += length;

        // Reconstruct the JSON with static text + dynamic fields
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
                 "\"Accelerometer\":%u",
                 rec.peer_mac[5], rec.peer_mac[4], rec.peer_mac[3],
                 rec.peer_mac[2], rec.peer_mac[1], rec.peer_mac[0],
                 g_gateway_mac,
                 rec.device_type,
                 rec.msg_counter,
                 rec.temperature,
                 rec.humidity,
                 rec.battery,
                 (rec.reed_relay ? "true" : "false"),
                 rec.accel
        );

        // If offline=1 => add timestamp
        if(rec.offline) {
            char buf[40];
            snprintf(buf, sizeof(buf), ",\"Timestamp\":%lu}", (unsigned long)rec.timestamp);
            strncat(json_payload, buf, sizeof(json_payload) - strlen(json_payload) - 1);
        } else {
            strncat(json_payload, "}", sizeof(json_payload) - strlen(json_payload) - 1);
        }

        // Publish
        printf("[FLASH] Publishing offline record: %s\r\n", json_payload);
        my_mqtt_cmd_publish(json_payload);

        sys_ms_sleep(200); // small delay
    }

    // After sending all (or hitting a blank), erase entire region => offset=0
    offline_data_erase();
    printf("[FLASH] Offline data flush complete.\r\n");
}


// --------------------------------------------------------------------
// Wi-Fi reconnect => tries every 5 min if offline. Also tries MQTT
// reconnect if Wi-Fi is connected but MQTT is down.
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

                my_mqtt_cmd_disconnect();
                sys_ms_sleep(1000);

                my_mqtt_cmd_connect();

                // Flush offline data (if any) now that we're online
                if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
                    offline_flush_to_mqtt();
                }

                #ifdef CONFIG_OTA_DEMO_SUPPORT
                start_ota_demo();
                #endif

            } else {
                // remain offline
                printf("[RECONNECT] Still offline.\r\n");
            }
        } else {
            // check if we truly remain connected to Wi-Fi
            extern struct wifi_vif_tag wifi_vif_tab[];
            #define WIFI_STA_STATE_CONNECTED 6
            if(wifi_vif_tab[0].sta.state != WIFI_STA_STATE_CONNECTED) {
                printf("[RECONNECT] Wi-Fi dropped.\r\n");
                wifi_status = WIFI_DISCONNECTED;
                sntp_stop();
                my_mqtt_cmd_disconnect();
            } else {
                // Wi-Fi is up, check MQTT connection
                if(!(mqtt_client && mqtt_client_is_connected(mqtt_client))) {
                    printf("[RECONNECT] MQTT disconnected => reconnecting...\r\n");
                    my_mqtt_cmd_disconnect();
                    sys_ms_sleep(1000);
                    my_mqtt_cmd_connect();

                    // If reconnect success, flush offline
                    if(mqtt_client && mqtt_client_is_connected(mqtt_client)) {
                        offline_flush_to_mqtt();

                        #ifdef CONFIG_OTA_DEMO_SUPPORT
                        start_ota_demo();
                        #endif
                    }
                }
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

    // Create background reconnect task
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

    // If connected => do SNTP + MQTT + optional OTA + flush
    if(wifi_status==WIFI_CONNECTED){
        start_sntp_and_sync();
        my_mqtt_cmd_connect();

        #ifdef CONFIG_OTA_DEMO_SUPPORT
        start_ota_demo();
        #endif

        if(mqtt_client && mqtt_client_is_connected(mqtt_client)){
            offline_flush_to_mqtt();
        }
    }

    // ---- Move init_gw_mac here so the MAC is valid. ----
      init_gw_mac();

    // BLE scanning forever
    while(1){
        for (uint8_t idx=0; ; idx++){
            dev_info_t* dev = scan_mgr_find_dev_by_idx(idx);
            if(!dev) break;
            if(dev->adv_len < 26) continue;

            // check adv_data[5]==0x4D && adv_data[6]==0x5A => "MZ"
            if(dev->adv_data[5]==0x4D && dev->adv_data[6]==0x5A) {
                printf("[demo_task] Found Target Beacon!\r\n");

                // parse dynamic fields
                uint8_t device_type = dev->adv_data[7];
                uint32_t msg_counter = (dev->adv_data[8]<<24)|(dev->adv_data[9]<<16)|
                                       (dev->adv_data[10]<<8)| dev->adv_data[11];
                int16_t temperature  = (int16_t)((dev->adv_data[12]<<8)|dev->adv_data[13]);
                int16_t humidity     = (int16_t)((dev->adv_data[14]<<8)|dev->adv_data[15]);
                uint16_t batt        = (uint16_t)((dev->adv_data[16]<<8)|dev->adv_data[17]);
                bool reed_relay      = dev->adv_data[18]?true:false;
                uint8_t accel        = dev->adv_data[19];

                // We'll create the JSON if we are online, else store a compact record
                if(wifi_status==WIFI_CONNECTED && mqtt_client && mqtt_client_is_connected(mqtt_client)) {
                    // Construct JSON with a new "GW_MAC" right after "MAC"
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
                             "\"Accelerometer\":%u"
                             "}",
                             dev->peer_addr.addr[5], dev->peer_addr.addr[4],
                             dev->peer_addr.addr[3], dev->peer_addr.addr[2],
                             dev->peer_addr.addr[1], dev->peer_addr.addr[0],
                             g_gateway_mac,
                             device_type,
                             msg_counter,
                             temperature,
                             humidity,
                             batt,
                             reed_relay?"true":"false",
                             accel
                    );

                    // Publish now
                    my_mqtt_cmd_publish(json_payload);

                    // debug timestamp
                    uint32_t t = get_current_epoch_time();
                    printf("TIMESTAMP(Online): %lu\r\n", (unsigned long)t);

                } else {
                    // offline => store to flash (compact struct)
                    offline_record_t rec;
                    memset(&rec, 0, sizeof(rec));
                    // store MAC in same order we read it
                    memcpy(rec.peer_mac, dev->peer_addr.addr, 6);

                    rec.device_type = device_type;
                    rec.msg_counter = msg_counter;
                    rec.temperature = temperature;
                    rec.humidity    = humidity;
                    rec.battery     = batt;
                    rec.reed_relay  = (reed_relay ? 1 : 0);
                    rec.accel       = accel;

                    rec.offline     = 1; // we were offline
                    rec.timestamp   = get_current_epoch_time();

                    save_offline_record(&rec);

                    printf("TIMESTAMP(Offline): %lu\r\n", (unsigned long)rec.timestamp);
                }
            }
        }

        sys_ms_sleep(7000); // every 7 seconds
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

    dbg_print(NOTICE, "Firmware version: 1.0\r\n");

    // BLE init in main
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

    // Start OS
    sys_os_start();
    for(;;){}
    return 0;
}
