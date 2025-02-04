/*!
    \file    mqtt_cmd.h
    \brief   mqtt command for GD32VW55x SDK.

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/



#ifndef _MQTT_CMD_H_
#define _MQTT_CMD_H_

#include "app_cfg.h"

#ifdef CONFIG_MQTT
#include "co_list.h"

#define MQTT_DEFAULT_PORT 1883

enum mqtt_mode {
    MODE_TYPE_MQTT = 1U,
    MODE_TYPE_MQTT5 = 2U,
};

typedef struct publish_msg {
    struct co_list_hdr hdr;
    char* topic;
    char* msg;
    uint8_t qos;
    uint8_t retain;
} publish_msg_t;

typedef struct sub_msg {
    struct co_list_hdr hdr;
    char* topic;
    uint8_t qos;
    bool sub_or_unsub;
} sub_msg_t;

void mqtt_mode_type_set(enum mqtt_mode cmd_mode);
enum mqtt_mode mqtt_mode_type_get(void);
void cmd_mqtt(int argc, char **argv);
void mqtt_connect_server(int argc, char **argv);
void mqtt_msg_pub(int argc, char **argv);
void mqtt_msg_sub(int argc, char **argv);
void mqtt_client_disconnect(int argc, char **argv);
void mqtt_auto_reconnect_set(int argc, char **argv);
void mqtt_task_resume(bool isr);

#endif

#endif // _MQTT_CMD_H_
