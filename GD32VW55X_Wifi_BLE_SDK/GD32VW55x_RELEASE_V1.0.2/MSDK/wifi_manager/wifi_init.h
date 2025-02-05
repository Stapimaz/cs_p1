/*!
    \file    wifi_init.h
    \brief   Header file of wifi initialization.

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/


#ifndef _WIFI_INIT_H_
#define _WIFI_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "macif_types.h"

int wifi_init(void);
int wifi_sw_init(void);
void wifi_sw_deinit(void);

void wifi_task_ready(enum wifi_task_id task_id);
int wifi_wait_ready(void);
void wifi_task_terminated(enum wifi_task_id task_id);
int wifi_wait_terminated(enum wifi_task_id task_id);

#ifdef __cplusplus
}
#endif

#endif /* _WIFI_INIT_H_ */
