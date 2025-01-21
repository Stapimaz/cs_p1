/*!
    \file    tuya_ble_hal.h
    \brief   BLE HAL for TUYA

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef _TUYA_BLE_HAL_H_
#define _TUYA_BLE_HAL_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>          // Standard Integer Definition

#include "tkl_bluetooth.h"
#include "tuya_error_code.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
OPERATE_RET tuya_hal_gap_callback_register(CONST TKL_BLE_GAP_EVT_FUNC_CB gap_evt);

OPERATE_RET tuya_hal_gatt_callback_register(CONST TKL_BLE_GATT_EVT_FUNC_CB gatt_evt);

OPERATE_RET tuya_hal_init(UCHAR_T role);

OPERATE_RET tuya_hal_scan_start(TKL_BLE_GAP_SCAN_PARAMS_T CONST *p_scan_params);

OPERATE_RET tuya_hal_scan_stop(void);

OPERATE_RET tuya_hal_adv_start(TKL_BLE_GAP_ADV_PARAMS_T CONST *p_adv_params);

OPERATE_RET tuya_hal_adv_stop(void);

OPERATE_RET tuya_hal_adv_rsp_data_set(TKL_BLE_DATA_T CONST *p_adv, TKL_BLE_DATA_T CONST *p_scan_rsp);

OPERATE_RET tuya_hal_adv_rsp_data_update(TKL_BLE_DATA_T CONST *p_adv, TKL_BLE_DATA_T CONST *p_scan_rsp);

OPERATE_RET tuya_hal_ble_connect(TKL_BLE_GAP_ADDR_T CONST *p_peer_addr, TKL_BLE_GAP_SCAN_PARAMS_T CONST *p_scan_params, TKL_BLE_GAP_CONN_PARAMS_T CONST *p_conn_params);

OPERATE_RET tuya_hal_ble_disconnect(USHORT_T conn_handle, UCHAR_T hci_reason);

OPERATE_RET tuya_hal_ble_conn_param_update(USHORT_T conn_handle, TKL_BLE_GAP_CONN_PARAMS_T CONST *p_conn_params);

OPERATE_RET tuya_hal_ble_rssi_get(USHORT_T conn_handle);

OPERATE_RET tuya_hal_ble_name_set(CHAR_T *p_name);

OPERATE_RET tuya_hal_gatts_service_add(TKL_BLE_GATTS_PARAMS_T *p_service);

OPERATE_RET tuya_hal_gatts_value_set(USHORT_T conn_handle, USHORT_T char_handle, UCHAR_T *p_data, USHORT_T length);

OPERATE_RET tuya_hal_gatts_value_notify(USHORT_T conn_handle, USHORT_T char_handle, UCHAR_T *p_data, USHORT_T length);

OPERATE_RET tuya_hal_gatts_value_indicate(USHORT_T conn_handle, USHORT_T char_handle, UCHAR_T *p_data, USHORT_T length);

OPERATE_RET tuya_hal_gattc_all_service_discovery(USHORT_T conn_handle);

OPERATE_RET tuya_hal_gattc_all_char_discovery(USHORT_T conn_handle, USHORT_T start_handle, USHORT_T end_handle);

OPERATE_RET tuya_hal_gattc_char_desc_discovery(USHORT_T conn_handle, USHORT_T start_handle, USHORT_T end_handle);

OPERATE_RET tuya_hal_gattc_write_without_rsp(USHORT_T conn_handle, USHORT_T char_handle, UCHAR_T *p_data, USHORT_T length);

OPERATE_RET tuya_hal_gattc_write(USHORT_T conn_handle, USHORT_T char_handle, UCHAR_T *p_data, USHORT_T length);

OPERATE_RET tuya_hal_gattc_read(USHORT_T conn_handle, USHORT_T char_handle);

void tuya_wait_ble_ready(void);
#endif // _TUYA_BLE_HAL_H_
