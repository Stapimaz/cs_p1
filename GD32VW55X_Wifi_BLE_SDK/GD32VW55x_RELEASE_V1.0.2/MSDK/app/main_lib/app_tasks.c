#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "app_tasks.h"
#include "dbg_print.h"
#include "wrapper_os.h"

// BLE scanning includes
#include "app_scan_mgr.h"

// For concurrency (SoftAP + HTTP server)
#include "http_server.h"

// For Wi-Fi and MQTT calls
#include "wifi_mqtt.h"
#include "wifi_management.h"
#include "storage.h"

// For OTA
// (original code just calls start_ota_demo() directly)
extern void start_ota_demo(void);



// Our scanning intervals (same values as original)
static int ONLINE_SCAN_INTERVAL  = 7000;
static int OFFLINE_SCAN_INTERVAL = 14000;

void main_task(void *arg)
{
	(void)arg;

	// (A) Read MAC from flash if available
	gw_mac_read_from_flash();

	printf("[main_task] Launching SoftAP concurrency first...\r\n");
	// Create SoftAP first
	sys_task_create_dynamic(
			(uint8_t*)"softap_http_server_task",
			4096,
			OS_TASK_PRIORITY(12),
			softap_http_server_task,
			NULL
	);

	// Let the SoftAP get started properly
	sys_ms_sleep(2000);

	printf("[main_task] Now connecting STA...\r\n");
	wifi_connect_handler();
	sys_ms_sleep(200);

	// If connected => SNTP + MQTT + optional OTA + flush offline
	if (wifi_get_status() == WIFI_CONNECTED) {
		start_sntp_and_sync();
		my_mqtt_cmd_connect();
		sys_ms_sleep(3000);

		if (mqtt_client_is_connected_ok()) {
			offline_flush_to_mqtt();
		}
	}

	sys_ms_sleep(200);
	start_ota_demo(); //only ota attempt at initial boot

	// Create background reconnect logic
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

	// We still call init_gw_mac() => writes to flash for future boots
	init_gw_mac();
	sys_ms_sleep(500);

	// BLE scanning + offline logic
	while (1) {
		for (uint8_t idx=0; ; idx++){
			dev_info_t* dev = scan_mgr_find_dev_by_idx(idx);
			if(!dev) break;
			if(dev->adv_len < 26) continue;

			// check adv_data[5]==0x4D && adv_data[6]==0x5A => "MZ"
			if(dev->adv_data[5]==0x4D && dev->adv_data[6]==0x5A) {
				printf("[main_task] Found Target Beacon!\r\n");

				uint8_t device_type = dev->adv_data[7];
				uint32_t msg_counter = (dev->adv_data[8]<<24)|(dev->adv_data[9]<<16)|
						(dev->adv_data[10]<<8)|dev->adv_data[11];
				int16_t temperature  = (int16_t)((dev->adv_data[12]<<8)|dev->adv_data[13]);
				int16_t humidity     = (int16_t)((dev->adv_data[14]<<8)|dev->adv_data[15]);
				uint16_t batt        = (uint16_t)((dev->adv_data[16]<<8)|dev->adv_data[17]);
				bool reed_relay      = dev->adv_data[18]?true:false;
				uint8_t accel        = dev->adv_data[19];

				// If online => publish directly, else => store offline
						if (wifi_get_status() == WIFI_CONNECTED && mqtt_client_is_connected_ok()) {
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
									get_gateway_mac(),
									device_type,
									msg_counter,
									temperature,
									humidity,
									batt,
									(reed_relay?"true":"false"),
									accel
							);
							my_mqtt_cmd_publish(json_payload);
							uint32_t t = get_current_epoch_time();
							printf("TIMESTAMP(Online): %lu\r\n", (unsigned long)t);
							sys_ms_sleep(100);
						} else {
							// offline => store
							offline_record_t rec;
							memset(&rec, 0, sizeof(rec));
							memcpy(rec.peer_mac, dev->peer_addr.addr, 6);

							rec.device_type = device_type;
							rec.msg_counter = msg_counter;
							rec.temperature = temperature;
							rec.humidity    = humidity;
							rec.battery     = batt;
							rec.reed_relay  = (reed_relay ? 1 : 0);
							rec.accel       = accel;
							rec.offline     = 1;
							rec.timestamp   = get_current_epoch_time();

							save_offline_record(&rec);
							printf("TIMESTAMP(Offline): %lu\r\n", (unsigned long)rec.timestamp);
							sys_ms_sleep(100);
						}
			}
		}

		if (wifi_get_status() == WIFI_CONNECTED && mqtt_client_is_connected_ok()){
			sys_ms_sleep(ONLINE_SCAN_INTERVAL);
		}else{
			sys_ms_sleep(OFFLINE_SCAN_INTERVAL);}
	}

	// Should never reach here
	sys_task_delete(NULL);
}

// --------------------------------------------------------------------
// wifi_reconnect_task code from original main.c
// --------------------------------------------------------------------

static bool g_first_offline_retry = true;  // used by wifi_reconnect_task()

void wifi_reconnect_task(void *arg)
{
	(void)arg;

	while(1) {
		if (wifi_get_status() == WIFI_DISCONNECTED) {
			if(g_first_offline_retry) {
				printf("[RECONNECT] Offline => next attempt in 10 seconds.\r\n");
				sys_ms_sleep(10000);
				g_first_offline_retry = false;
			} else {
				printf("[RECONNECT] Offline => next attempt in 5 mins.\r\n");
				sys_ms_sleep(5UL*60UL*1000UL);
			}

			if (wifi_try_connect(g_primary_ssid, g_primary_password) ||
					wifi_try_connect(g_backup_ssid,  g_backup_password)){
				set_wifi_status(WIFI_CONNECTED);
				printf("[RECONNECT] Wi-Fi reconnected!\r\n");
				g_first_offline_retry = true;
				sys_ms_sleep(500);
				start_sntp_and_sync();

				my_mqtt_cmd_disconnect();
				sys_ms_sleep(1000);
				my_mqtt_cmd_connect();
				sys_ms_sleep(3000);
				if (mqtt_client_is_connected_ok()) {
					offline_flush_to_mqtt();
				}
				sys_ms_sleep(200);

				//start_ota_demo();

			} else {
				printf("[RECONNECT] Still offline.\r\n");
			}
		}

		// If Wi-Fi is up but MQTT is disconnected => reconnect
		if (wifi_get_status() == WIFI_CONNECTED &&
				(!mqtt_client_is_connected_ok()))
		{
			printf("[RECONNECT] MQTT disconnected => reconnecting...\r\n");
			my_mqtt_cmd_disconnect();
			sys_ms_sleep(1000);
			my_mqtt_cmd_connect();
			sys_ms_sleep(3000);
			if (mqtt_client_is_connected_ok()) {
				offline_flush_to_mqtt();
				sys_ms_sleep(200);
				//start_ota_demo();
			}
		}

		// Check if Wi-Fi physically dropped
		extern struct wifi_vif_tag wifi_vif_tab[];
#define WIFI_STA_STATE_CONNECTED 6
		if(wifi_get_status() == WIFI_CONNECTED &&
				wifi_vif_tab[0].sta.state != WIFI_STA_STATE_CONNECTED)
		{
			printf("[RECONNECT] Wi-Fi dropped.\r\n");
			set_wifi_status(WIFI_DISCONNECTED);
			sntp_stop();
			my_mqtt_cmd_disconnect();
		}

		sys_ms_sleep(6000);
	}
}
