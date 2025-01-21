/*!
    \file    app_datatrans_srv.c
    \brief   Datatrans Service Server Application Module entry point.

    \version 2024-07-2, V1.0.0, firmware for GD32VW55x
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

#include <string.h>
#include "app_datatrans_srv.h"
#include "ble_datatrans_srv.h"
#include "wrapper_os.h"
#include "dbg_print.h"
#include "app_dev_mgr.h"
#include "cmd_shell.h"

/*!
    \brief      APP datatrans service server tx callback
    \param[in]  data_len: data_length
    \param[in]  p_data: pointer to data to handle
    \param[out] none
    \retval     none
*/
static void app_datatrans_srv_tx_callback(uint16_t data_len, uint8_t *p_data)
{
    uint32_t conidx_bf = dm_get_conidx_bf();

    if (conidx_bf && (ble_datatrans_srv_tx_mtp(conidx_bf, p_data, data_len) == BLE_ERR_NO_ERROR)) {
        app_print("datatrans srv send data: \r\n");
        app_print("%s\r\n", p_data);
    }
}

/*!
    \brief      APP datatrans service server rx callback
    \param[in]  data_len: data_length
    \param[in]  p_data: pointer to data to handle
    \param[out] none
    \retval     none
*/
static void app_datatrans_srv_rx_callback(uint16_t data_len, uint8_t *p_data)
{
    uint8_t *p_str = sys_malloc(data_len + 1);

    if (p_str) {
        app_print("datatrans srv receive data: \r\n");
        memset(p_str, 0, data_len + 1);
        memcpy(p_str, p_data, data_len);
        app_print("%s\r\n", p_str);
        sys_mfree(p_str);
    }
}

/*!
    \brief      Init APP datatrans service server module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void app_datatrans_srv_init(void)
{
    cmd_unkwn_cmd_handler_reg(app_datatrans_srv_tx_callback);
    ble_datatrans_srv_init(app_datatrans_srv_rx_callback);
}

/*!
    \brief      Deinit APP datatrans service server module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void app_datatrans_srv_deinit(void)
{
    cmd_unkwn_cmd_handler_unreg();
    ble_datatrans_srv_deinit();
}

