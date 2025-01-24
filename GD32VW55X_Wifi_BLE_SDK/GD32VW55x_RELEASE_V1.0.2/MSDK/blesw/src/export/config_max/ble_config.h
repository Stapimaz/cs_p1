/*!
    \file    ble_config.h
    \brief   Ble MAX config definitions.

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/



#ifndef _BLE_CONFIG_H_
#define _BLE_CONFIG_H_

#define BLE_CFG_ROLE_BROADCASTER    0x01
#define BLE_CFG_ROLE_PERIPHERAL     0x02
#define BLE_CFG_ROLE_OBSERVER       0x04
#define BLE_CFG_ROLE_CENTRAL        0x08
#define BLE_CFG_ROLE_ALL            (BLE_CFG_ROLE_BROADCASTER | BLE_CFG_ROLE_PERIPHERAL  |         \
                                     BLE_CFG_ROLE_OBSERVER    | BLE_CFG_ROLE_CENTRAL)

#define BLE_CFG_ROLE                BLE_CFG_ROLE_ALL

#define BLE_MAX_CONN_NUM            4

#define BLE_HOST_SUPPORT

#define BLE_PER_ADV_SUPPORT

#ifdef BLE_PER_ADV_SUPPORT
#define BLE_PAST_SUPPORT
#endif

#define BLE_DATA_LEN_EXTEN_SUPPORT

#define BLE_PHY_UPDATE_SUPPORT

#define BLE_PWR_CTRL_SUPPORT

#define BLE_PING_SUPPORT

#define BLE_SCA_SUPPORT

#define BLE_GATT_CLIENT_SUPPORT

#define BLE_EATT_SUPPORT

#define BLE_GATT_CACHING_SUPPORT

#define BLE_SMP_SC_SUPPORT

#ifdef BLE_PER_ADV_SUPPORT
#define BLE_BIS_SUPPORT
#endif

#define BLE_CIS_SUPPORT

#endif  //_BLE_CONFIG_H_
