/*!
    \file    ota_demo.h
    \brief   Header file of OTA demonstration program for GD32VW55x SDK.

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/



#ifndef _OTA_DEMO_H_
#define _OTA_DEMO_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

/*
 * DEFINITIONS
 ****************************************************************************************
 */

#define OTA_IMAGE_URL_MAX_LEN       128

/*
 * FUNCTIONS
 ****************************************************************************************
 */

int32_t ota_demo_cfg_init(const char *srv_addr, const char *image_url);

int32_t ota_demo_start(void);

#endif /* _OTA_DEMO_H_ */
