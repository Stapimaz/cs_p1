/*
tuya_iot_config.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

/* AUTO-GENERATED FILE. DO NOT MODIFY !!!
*
* This config file is automatically generated by tuya cross-build system.
* It should not be modified by hand.
*/

#ifndef TUYA_IOT_CONFIG_H
#define TUYA_IOT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* default definitons */

#define SYSTEM_SMALL_MEMORY_BEGIN       0       /*small memory systems begin */
#define SYSTEM_REALTEK8710_1M           1
#define SYSTEM_REALTEK8710_2M           2
#define SYSTEM_NON_OS					3		/*no operating system*/
#define SYSTEM_FREERTOS                 98
#define SYSTEM_SMALL_MEMORY_END         99      /*small memory systems end */
#define SYSTEM_LINUX                    100
#define SYSTEM_LITEOS                   120

#define TLS_DISABLE                     0       /* disable tls function */
#define TLS_TUYA_PSK_ONLY               2       /* only enable ciper 0xAE */
#define TLS_TUYA_ECC_PSK                3       /* enable ciper 0xAE && 0xC027 */
#define TLS_TUYA_ECC_ONLY               4       /* only enable ciper 0xC027 */
#define TLS_TUYA_ECC_ONLY_NOSTRIP       5       /* only enable ciper 0xC027, but enable most of mbed-tls configs */
#define TLS_TUYA_ECC_CLIENT_AUTH        6       /* TLS Client Authentication and Server Authentication mode */

#define TUYA_SL_0           0       /* security level 0,Applies to: Resource-constrained devices;Feature: Can only access Tuya Cloud */
#define TUYA_SL_1           1       /* security level 1,Applies to: Resource-constrained devices;Feature: One-way authentication */
#define TUYA_SL_2           2       /* security level 2,Applies to: Resource-rich equipment;Feature: Two-way authentication */
#define TUYA_SL_3           3       /* security level 3,Applies to: Resource-rich equipment;Feature: Two-way authentication,Devices use security chips to protect sensitive information */
#define TUYA_MODIFY_MBEDTLS             1

#define TUYA_OPERATOR_DISABLE           0x0
#define TUYA_OPERATOR_CUCC              0x01
#define TUYA_OPERATOR_CTCC              0x02
#define TUYA_OPERATOR_CMCC              0x04
#define TUYA_OPERATOR_CMCC_ANDLINK      0x08
#define TUYA_OPERATOR_ALL               (TUYA_OPERATOR_CUCC | TUYA_OPERATOR_CTCC | TUYA_OPERATOR_CMCC)


#define TUYA_LAN_V31              0x1
#define TUYA_LAN_V32              0x2
#define TUYA_LAN_V33              0x4
#define TUYA_LAN_V35              0x8
#define TUYA_LAN_ALL              (TUYA_LAN_V31 | TUYA_LAN_V32 | TUYA_LAN_V33 | TUYA_LAN_V35)

#define TUYA_MQ_V21               0x1
#define TUYA_MQ_V22               0x2
#define TUYA_MQ_V23               0x4
#define TUYA_MQ_ALL               (TUYA_MQ_V21 | TUYA_MQ_V22 | TUYA_MQ_V23)

/* custom settings */
#define BUILD_DATE           "2023_08_14"
#define BUILD_TIME           "20_23_18"
#define GIT_USER             "ci_manage"
#define IOT_SDK_VER          "3.8.0"
#define SDK_BETA_VER         ""
#define TARGET_PLATFORM      "gd32vw553"
#define PROJECT_NAME         "tuyaos-iot"
#define SDK_ID               "tuyaos-iot_3.8.0_gd32vw553_wifi-ble-com_0.0.1"
// Generated by Tuya (https://www.tuya.com)
// 基于项目 tuyaos-iot (3.8.0)
// 基于工具链 gd32vw553 (0.0.1-beta.1)


#define ENABLE_LOG 1

#define STACK_SIZE_TIMERQ 4096

// CONFIG_ENABLE_PRINTF_CHECK is not set

// CONFIG_ENABLE_SET_SOCK_OPT is not set

#define ENABLE_NETWORK_OPTIONS 1

#define TUYA_FD_MAX_COUNT 64

// CONFIG_DISABLE_BIND_INTERFACE is not set

// CONFIG_ENABLE_VENDOR_PRIVATE_NETSTACK is not set


#define STACK_SIZE_HEALTH_MONITOR 2048

#define ENABLE_OSS_COMPILE_WARNINGS 1

#define ENABLE_LOG_UPLOAD 1

#define PACKET_SIZE_PER_HTTP_SEND 307200

#define HTTP_SEND_INTERVAL 100

#define PACKET_SIZE_PER_LAN_SEND 8192

#define LAN_SEND_INTERVAL 100

#define MAX_UPD_FILE_SIZE 5242880


#define KV_PROTECTED_ENABLE 1

// CONFIG_KV_64BIT_MODE is not set

// CONFIG_ENABLE_KV_FILE is not set

#define ENABLE_KV_PROTECTED 1

#define ENABLE_KV_KEY_SEED 1

#define KV_KEY_SEED "8710_2M"


// CONFIG_ENABLE_UF_HUGE_FILE is not set

#define MAX_LENGTH_OF_FILE_FULL_PATH 128


#define EVENT_NAME_MAX_LEN 16

#define ENABLE_TUYA_LAN 1

#define TUYA_LAN_VERSION 0x8

#define ENABLE_LAN_ENCRYPTION 1

// CONFIG_ENABLE_RTSP_SERVER is not set

#define STACK_SIZE_LAN 4096


#define STACK_SIZE_UPGRADE 4096

// CONFIG_ENABLE_TUYA_DIFF_OTA is not set

// CONFIG_ENABLE_FIRMWARE_SIGN_TUYA is not set

// CONFIG_ENABLE_FIRMWARE_SIGN_USER is not set


// CONFIG_ENABLE_PRODUCT_AUTOTEST is not set

// CONFIG_ENABLE_MF_STANDALONE_SECTION is not set

#define STACK_SIZE_MF_TEST 5120

// CONFIG_ENABLE_LWIP is not set

#define ENABLE_BT_SERVICE 1

#define ENABLE_BT_NETCFG 1

#define ENABLE_BT_ACTIVE 1

#define ENABLE_BT_CTRL 1

#define BT_ADV_INTERVAL_MIN 30

#define BT_ADV_INTERVAL_MAX 60

// CONFIG_ENABLE_BT_REMOTE_CTRL is not set

#define BT_REMOTE_CTRL_SCAN_INTERVAL 30

#define BT_REMOTE_CTRL_SCAN_WINDOWS 10

#define ENABLE_BT_TIMER 1

#define ENABLE_BT_MF 1

#define ENABLE_MF_BY_BT 1

// CONFIG_ENABLE_BT_MESH_NODE is not set

// CONFIG_ENABLE_BT_VIRTUAL is not set

#define TUYA_BLE_VER 4

#define ENABLE_TAL_BLUETOOTH 1

#define TAL_BLUETOOTH 1

#define TAL_BLE_SERVICE_VERSION 2

// CONFIG_ENABLE_TUYA_BLE_STACK is not set

#define TY_BT_MOD 1

#define BT_NETCFG 1

#define BT_ACTIVE 1

#define BT_CTRL 1

// CONFIG_BT_REMOTE_CTRL is not set

#define BT_TIMER 1

#define MF_BT 1

// CONFIG_BT_MESH_NODE is not set

// CONFIG_BT_VIRTUAL is not set

#define WIFI_GW 1

// CONFIG_TUYA_HOSTAPD_SUPPORT is not set

#define ENABLE_WIFI_SERVICE 1

// CONFIG_ENABLE_WIFI_EZ is not set

// CONFIG_ENABLE_WIFI_FFS is not set

// CONFIG_ENABLE_WIFI_PEGASUS is not set

#define ENABLE_WIFI_QRCODE 1

#define ENABLE_WIFI_PROTECT 1

#define ENABLE_WIFI_FFC 1


// CONFIG_TUYA_WIFI_FFC_MASTER is not set

#define TUYA_WIFI_FFC_SLAVER 1

// CONFIG_ENABLE_HOSTAPD is not set

#define ENABLE_STATION_AP_MODE 0

#define WIFI_AP_BUF_SIZE 10240

// CONFIG_QRCODE_ACTIVE_MODE is not set

#define ENABLE_TUYA_CLOUD 1

#define ENABLE_ASTRO_TIMER 1

#define ENABLE_CALENDAR_TIMER 1

// CONFIG_ENABLE_QRCODE_ACTIVE is not set

#define ENABLE_DEVICE_DATA_COLLECT 1

#define ENABLE_LOCAL_KEY_UPDATE 1

#define ENABLE_COMMUNICATE_PRIORITY 1


// CONFIG_ENABLE_BLACK_WHITE_LIST is not set

#define MAX_SECURITY_LEVEL 0

#define TUYA_SECURITY_LEVEL 0

// CONFIG_ENABLE_MBEDTLS_DEBUG is not set

// CONFIG_ENABLE_CUSTOM_CONFIG is not set


#define HTTP_MAX_REQ_RESP_HDR_SIZE 1024

#define MAX_HTTP_SESSION_NUM 16

// CONFIG_ENABLE_HTTP_SESSION is not set


#define TUYA_MQ_VERSION 0x4

#define STACK_SIZE_MQTT_CLIENT 4096

#define QOS1_PUBACK_TIMEOUT 3

#define QOS1_PUBLISH_SLOT_CNT 8

#define QOS1_PUBACK_TIMEOUT_LIMIT 3

#define MAX_RANDOM_DELAY_BEFORE_RECONNECT 300

#define MQTT_HEATBEAT_INTERVAL 60

#define MQTT_RECV_BUF_SIZE 512

#define MQTT_MAX_MESSGAE_SIZE 4096

#define MQTT_RECV_SELECT_TIMEOUT 3000

// CONFIG_ENABLE_MQTT_QOS1_RESEND is not set

// CONFIG_ENABLE_MATOP is not set

#define MATOP_REQ_TIMEOUT 6

#define TUYA_IOT_DEBUG 1


#define STACK_SIZE_WORK_QUEUE 5120

#define MAX_NODE_NUM_WORK_QUEUE 100

#define STACK_SIZE_MSG_QUEUE 4096

#define MAX_NODE_NUM_MSG_QUEUE 100

#define ENABLE_IOT_DEBUG 1

#define MAX_SIZE_OF_DEBUG_BUF 1024

#define ENABLE_LOG_OUTPUT_FORMAT 1

#define TUYA_REDUCE_BIN 2

// CONFIG_ENABLE_WEBSOCKET is not set
// Generated by Tuya (https://www.tuya.com)
// 基于工具链 gd32vw553 (0.0.1)


#define OPERATING_SYSTEM 98

#define LITTLE_END 1

// CONFIG_ENABLE_FILE_SYSTEM is not set

#define ENABLE_RTC 1

#define ENABLE_ADC 1

#define ENABLE_UART 1

// CONFIG_ENABLE_UART_INTERRUPT is not set

#define ENABLE_WATCHDOG 1

#define ENABLE_FLASH 1

#define TUYA_FLASH_TYPE_MAX_PARTITION_NUM 10

#define ENABLE_PWM 1

#define ENABLE_GPIO 1

#define ENABLE_I2C 1

#define ENABLE_SPI 1

// CONFIG_ENABLE_ZIGBEE is not set

#define ENABLE_WIFI 1

// CONFIG_ENABLE_WIRED is not set

#define ENABLE_TIMER 1

#define ENABLE_BLUETOOTH 1

// CONFIG_ENABLE_HCI is not set

// CONFIG_ENABLE_DISPLAY is not set

// CONFIG_ENABLE_MEDIA is not set

// CONFIG_ENABLE_PM is not set

// CONFIG_GPRS_GW is not set

// CONFIG_TUYA_CAT1 is not set

// CONFIG_ENABLE_CELLULAR is not set

// CONFIG_ENABLE_STORAGE is not set

// CONFIG_ENABLE_NB is not set

// CONFIG_ENABLE_REGISTER is not set

// CONFIG_ENABLE_DAC is not set

// CONFIG_ENABLE_I2S is not set

#define ENABLE_PINMUX 1

// CONFIG_ENABLE_PLATFORM_AES is not set

// CONFIG_ENABLE_PLATFORM_SHA256 is not set

// CONFIG_ENABLE_PLATFORM_MD5 is not set

// CONFIG_ENABLE_PLATFORM_SHA1 is not set

// CONFIG_ENABLE_PLATFORM_RSA is not set

// CONFIG_ENABLE_PLATFORM_ECC is not set

// CONFIG_ENABLE_WAKEUP is not set
#define KERNEL_VER "2.0.1-beta.6"
#ifndef TUYA_LAN_VERSION
#define TUYA_LAN_VERSION TUYA_LAN_V35
#endif

#ifndef TUYA_MQ_VERSION
#define TUYA_MQ_VERSION TUYA_MQ_V23
#endif

#ifndef ENABLE_DEVICE_DATA_COLLECT
#define ENABLE_DEVICE_DATA_COLLECT 0
#endif

#ifndef TUYA_WEAK_ATTRIBUTE
#define TUYA_WEAK_ATTRIBUTE __attribute__ ((weak))
#endif

/* custom settings */

#ifdef __cplusplus
}
#endif


#endif
