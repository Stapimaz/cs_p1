/*!
    \file    ble_scan.h
    \brief   Module for handling the BLE scanning.

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

#ifndef _BLE_SCAN_H__
#define _BLE_SCAN_H__

#include <stdint.h>

#include "ble_gap.h"
#include "ble_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Enumeration for scann events */
typedef enum
{
    BLE_SCAN_EVT_STATE_CHG,             /*!< Event notify for scan state changed  */
    BLE_SCAN_EVT_ADV_RPT,               /*!< Send notification to the main application when a device is found */
} ble_scan_evt_t;

/* Enumeration for scan state. These states are propagated with event @ref BLE_SCAN_EVT_STATE_CHG */
typedef enum
{
    BLE_SCAN_STATE_DISABLED,            /*!< Scan state for disabled */
    BLE_SCAN_STATE_ENABLING,            /*!< Scan state for enabling */
    BLE_SCAN_STATE_ENABLED,             /*!< Scan state for enabled */
    BLE_SCAN_STATE_DISABLING,           /*!< Scan state for disabling */
} ble_scan_state_t;

/* Scan state change structure */
typedef struct
{
    ble_scan_state_t scan_state;    /*!< Scan state */
    uint16_t         reason;        /*!< State change reason */
} ble_scan_state_chg_t;

/* Scan event data */
typedef union ble_scan_data
{
    ble_gap_adv_report_info_t   *p_adv_rpt;     /*!< Advertising report */
    ble_scan_state_chg_t         scan_state;    /*!< Scan state change */
} ble_scan_data_u;

/* Prototype of BLE scan event handler */
typedef void (*ble_scan_evt_handler_t)(ble_scan_evt_t event, ble_scan_data_u *p_data);

/*!
    \brief      Init BLE scan module
    \param[in]  own_addr_type: Local address type used during scan
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_init(ble_gap_local_addr_type_t own_addr_type);

/*!
    \brief      Reinit BLE scan module
    \details    If BLE scan module has already been initialized before, this function can re-init BLE scan module.
                For example, use @ref ble_scan_init to init scan module with own_addr_type set to public.
                And then use @ref ble_scan_reinit to re-init scan module with own_addr_type changed to RPA.
    \param[in]  own_addr_type: local address type used during scan
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_reinit(ble_gap_local_addr_type_t own_addr_type);

/*!
    \brief      Register callback function to handle BLE scan events
    \param[in]  callback: BLE scan event handler function
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_callback_register(ble_scan_evt_handler_t callback);

/*!
    \brief      Unregister callback function from BLE scan module
    \param[in]  callback: BLE scan event handler function
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_callback_unregister(ble_scan_evt_handler_t callback);

/*!
    \brief      Enable BLE scan
    \param[in]  none
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_enable(void);

/*!
    \brief      Disable BLE scan
    \param[in]  none
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_disable(void);

/*!
    \brief      Set BLE scan parameters
    \param[in]  param: pointer to scan parameters
    \param[out] none
    \retval     ble_status_t: BLE_ERR_NO_ERROR on success, otherwise an error code
*/
ble_status_t ble_scan_param_set(ble_gap_scan_param_t *p_param);

#ifdef __cplusplus
}
#endif

#endif // _BLE_SCAN_H__
