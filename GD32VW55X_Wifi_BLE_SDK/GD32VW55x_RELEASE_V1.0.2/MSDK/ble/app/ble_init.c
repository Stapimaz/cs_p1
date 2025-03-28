/*!
    \file    ble_init.c
    \brief   Implementation of the BLE initialization.

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/



#include <stdbool.h>
#include "app_cfg.h"
#include "ble_app_config.h"
#include "ble_config.h"

#include "ble_profile_config.h"

#include "ble_init.h"
#include "ble_export.h"

#include "ble_adapter.h"
#include "ble_adv.h"
#include "ble_scan.h"
#include "ble_conn.h"
#include "ble_l2cap_coc.h"
#include "ble_sec.h"

#include "app_adapter_mgr.h"
#include "ble_storage.h"
#include "wrapper_os.h"
#include "ble_gatts.h"
#if (BLE_APP_GATT_CLIENT_SUPPORT)
#include "ble_gattc.h"
#if (BLE_APP_GATTC_CO_ITF_SUPPORT)
#include "ble_gattc_co.h"
#endif
#endif
#include "app_cmd.h"
#include "app_adv_mgr.h"
#include "app_scan_mgr.h"
#include "app_list_mgr.h"
#include "app_sec_mgr.h"
#include "app_dev_mgr.h"
#include "app_conn_mgr.h"
#include "app_per_sync_mgr.h"
#include "app_iso_mgr.h"
#include "app_l2cap.h"
#include "app_http_proxy_server.h"
#include "app_http_proxy_client.h"
#include "app_prox_rpt.h"
#include "app_prox_monitor.h"
#include "app_diss.h"
#include "app_bass.h"
#include "app_cscss.h"
#include "app_blue_courier_link.h"
#include "app_datatrans_srv.h"

#include "dbg_print.h"

#include "raw_flash_api.h"
#include "gd32vw55x_platform.h"
#include "dbg_print.h"
#ifdef CFG_VIRTUAL_HCI_MODE
#include "app_virtual_hci.h"
#endif

#ifdef TUYAOS_SUPPORT
#ifdef CFG_VIRTUAL_HCI_MODE
extern void tkl_virtual_hci_init(void);
#else
extern void tuya_adp_init(uint8_t role);
#endif
#endif

#ifdef CFG_COEX
typedef void (*coex_ble_evt_notify)(uint32_t evt_start, uint32_t evt_window, uint32_t iso_evt);
extern void ble_coex_evt_notify_register(coex_ble_evt_notify func);
extern void coex_ble_event_notify(uint32_t event_start, uint32_t event_window, uint32_t iso_event);
#endif

/* Definitions of the different BLE task priorities */
enum
{
    BLE_STACK_TASK_PRIORITY = OS_TASK_PRIORITY(2),      /*!< Priority of the BLE stack task */
    BLE_APP_TASK_PRIORITY   = OS_TASK_PRIORITY(1),      /*!< Priority of the BLE APP task */
};

/* Definitions of the different BLE task stack size requirements */
enum
{
    BLE_STACK_TASK_STACK_SIZE = 768,        /*!< BLE stack task stack size */
    BLE_APP_TASK_STACK_SIZE   = 512,        /*!< BLE APP task stack size */
};

/* BLE sleep mode when flash erase need to execute */
uint8_t flash_erase_sleep_mode = 0;

/* Semaphore signaled when all BLE tasks are ready */
static os_sema_t ble_ready_sem;

/*!
    \brief      Function to notify other modules that BLE is in ready state
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_task_ready(void)
{
    sys_sema_up(&ble_ready_sem);
}

/*!
    \brief      Function for other modules to wait BLE into ready state
    \param[in]  none
    \param[out] none
    \retval     int: 0 if successfully enter wait state, otherwise -1
*/
int ble_wait_ready(void)
{
    if (ble_ready_sem == NULL)
        return -1;

    if (sys_sema_down(&ble_ready_sem, 0))
        return -1;

    // always re-signal the semaphore in case it is called by several tasks.
    sys_sema_up(&ble_ready_sem);
    return 0;
}

/*!
    \brief      Function to handle before/after flash erase execute
    \param[in]  type: erase handle type
    \param[out] none
    \retval     none
*/
void ble_flash_erase_handler(raw_erase_type_t type)
{
    if (type == RAW_FLASH_ERASE_BLE_PRE_HANDLE) {
        flash_erase_sleep_mode = ble_sleep_mode_get();
        if (flash_erase_sleep_mode != 0) {
            ble_sleep_mode_set(0);
            ble_stack_task_resume(false);
            // wait ble pmu on, timeout 10ms
            ble_wait_pmu_on(10);
        }
    } else if (type == RAW_FLASH_ERASE_BLE_AFTER_HANDLE) {
        if (flash_erase_sleep_mode != 0) {
            ble_sleep_mode_set(flash_erase_sleep_mode);
        }
    }

    return;
}

/*!
    \brief      Init BLE stack task and BLE APP task
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void ble_task_init(void)
{
    ble_stack_task_init(BLE_STACK_TASK_STACK_SIZE, BLE_STACK_TASK_PRIORITY);

#if (BLE_APP_SUPPORT)
    ble_app_task_init(BLE_APP_TASK_STACK_SIZE, BLE_APP_TASK_PRIORITY);
#endif

    sys_sema_init_ext(&ble_ready_sem, 1, 0);
}

/*!
    \brief      Deinit BLE stack task and BLE APP task
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void ble_task_deinit(void)
{
    sys_sema_free(&ble_ready_sem);

    ble_stack_task_deinit();

#if (BLE_APP_SUPPORT)
    ble_app_task_deinit();
#endif
}

#if (BLE_APP_SUPPORT)
/*!
    \brief      Init BLE component modules needed
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_module_init(void)
{
    ble_storage_init();

    if (ble_adp_init() != BLE_ERR_NO_ERROR) {
        dbg_print(NOTICE, "adapter init fail! check and restart again or reload image if necessary\r\n");
        return;
    }

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_BROADCASTER | BLE_CFG_ROLE_PERIPHERAL))
    ble_adv_init();
#endif

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_OBSERVER | BLE_CFG_ROLE_CENTRAL))
    ble_scan_init(BLE_GAP_LOCAL_ADDR_STATIC);
#endif

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_PERIPHERAL | BLE_CFG_ROLE_CENTRAL))
    ble_sec_init();
    ble_l2cap_init();
    ble_conn_init();
#endif

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_PERIPHERAL | BLE_CFG_ROLE_CENTRAL))
    ble_gatts_init();
#if (BLE_APP_GATT_CLIENT_SUPPORT)
    ble_gattc_init();
#if (BLE_APP_GATTC_CO_ITF_SUPPORT)
    ble_gattc_co_init();
#endif
#endif
#endif

}

/*!
    \brief      Init BLE profiles needed
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_profile_init(void)
{
#if BLE_PROFILE_DIS_SERVER
    ble_app_diss_init();
#endif

#if BLE_PROFILE_SAMPLE_SERVER
    ble_sample_srv_init();
#endif

#if BLE_PROFILE_SAMPLE_CLIENT
    ble_sample_cli_init();
#endif

#if BLE_PROFILE_HPS_SERVER
    app_hpss_init();
#endif

#if BLE_PROFILE_HPS_CLIENT
    app_hpsc_init();
#endif

#if BLE_PROFILE_PROX_SERVER
    app_prox_rpt_init();
#endif

#if BLE_PROFILE_PROX_CLIENT
    app_proxm_init();
#endif

#if BLE_PROFILE_THROUGHPUT_SERVER
    ble_throughput_srv_init();
#endif

#if BLE_PROFILE_THROUGHPUT_CLIENT
    ble_throughput_cli_init();
#endif

#if BLE_PROFILE_BAS_SERVER
    ble_app_bass_init();
#endif

#if BLE_PROFILE_CSCS_SERVER
    ble_app_cscss_init();
#endif

#if BLE_PROFILE_BLUE_COURIER_SERVER
    app_blue_courier_init();
#endif
}

/*!
    \brief      Deinit BLE profiles
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_profile_deinit(void)
{
#if BLE_PROFILE_DIS_SERVER
    ble_app_diss_deinit();
#endif

#if BLE_PROFILE_SAMPLE_SERVER
    ble_sample_srv_deinit();
#endif

#if BLE_PROFILE_SAMPLE_CLIENT
    ble_sample_cli_deinit();
#endif

#if BLE_PROFILE_THROUGHPUT_SERVER
    ble_throughput_srv_deinit();
#endif

#if BLE_PROFILE_THROUGHPUT_CLIENT
    ble_throughput_cli_deinit();
#endif

#if BLE_PROFILE_BAS_SERVER
    ble_app_bass_deinit();
#endif

#if BLE_PROFILE_CSCS_SERVER
    ble_app_cscss_deinit();
#endif

#if BLE_PROFILE_BLUE_COURIER_SERVER
    app_blue_courier_deinit();
#endif
}

/*!
    \brief      Init BLE application modules needed
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_app_init(void)
{
	dbg_print(ERR, "BLE APP INIT \r\n");
    app_adapter_init();

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_BROADCASTER | BLE_CFG_ROLE_PERIPHERAL))
    app_adv_mgr_init();
#endif

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_OBSERVER | BLE_CFG_ROLE_CENTRAL))
    app_scan_mgr_init();
#endif

    app_l2cap_mgr_init();

    app_dm_init();

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_PERIPHERAL | BLE_CFG_ROLE_CENTRAL))
    app_conn_mgr_init();
    app_sec_mgr_init();
#endif

#if (BLE_APP_PER_ADV_SUPPORT)
    app_per_sync_mgr_init();
#endif

    app_list_mgr_init();

#if (BLE_APP_BIS_SUPPORT || BLE_APP_CIS_SUPPORT)
    app_iso_mgr_init();
#endif // (BLE_APP_BIS_SUPPORT || BLE_APP_CIS_SUPPORT)

    ble_profile_init();

#if FEAT_SUPPORT_BLE_DATATRANS
    app_datatrans_srv_init();
#endif
}

/*!
    \brief      Deinit BLE application modules
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_app_deinit(void)
{
    app_adapter_deinit();

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_BROADCASTER | BLE_CFG_ROLE_PERIPHERAL))
    app_adv_mgr_deinit();
#endif

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_OBSERVER | BLE_CFG_ROLE_CENTRAL))
    app_scan_mgr_deinit();
#endif

    app_l2cap_mgr_deinit();

    app_dm_deinit();

#if (BLE_CFG_ROLE & (BLE_CFG_ROLE_PERIPHERAL | BLE_CFG_ROLE_CENTRAL))
    app_conn_mgr_deinit();
    app_sec_mgr_deinit();
#endif

#if (BLE_APP_PER_ADV_SUPPORT)
    app_per_sync_mgr_deinit();
#endif

    app_list_mgr_deinit();

#if (BLE_APP_BIS_SUPPORT || BLE_APP_CIS_SUPPORT)
    app_iso_mgr_deinit();
#endif // (BLE_APP_BIS_SUPPORT || BLE_APP_CIS_SUPPORT)

    ble_profile_deinit();

#if FEAT_SUPPORT_BLE_DATATRANS
    app_datatrans_srv_deinit();
#endif
}

#endif // (BLE_APP_SUPPORT)

/*!
    \brief      Initialization of the BLE module
    \details    This function allocates all the resources needed by the different BLE sub-modules
                This function initialize the command processing
                This function will use the wrapper_os API to create tasks, semaphores, etc
    \param[in]  all:  true to init all modules, false to only init BLE CLI if needed.
    \param[out] none
    \retval     none
*/
void ble_init(bool all)
{
    ble_status_t ret;

    ble_os_api_t os_interface = {
      .os_malloc = sys_malloc,
      .os_calloc = sys_calloc,
      .os_mfree = sys_mfree,
      .os_memset = sys_memset,
      .os_memcpy = sys_memcpy,
      .os_memcmp = sys_memcmp,
      .os_task_create = sys_task_create,
      .os_task_init_notification = sys_task_init_notification,
      .os_task_wait_notification = sys_task_wait_notification,
      .os_task_notify = sys_task_notify,
      .os_task_delete = sys_task_delete,
      .os_ms_sleep = sys_ms_sleep,
      .os_current_task_handle_get = sys_current_task_handle_get,
      .os_queue_init = sys_queue_init,
      .os_queue_free = sys_queue_free,
      .os_queue_write = sys_queue_write,
      .os_queue_read = sys_queue_read,
      .os_random_bytes_get = sys_random_bytes_get,
    };

#if (BLE_APP_CMD_SUPPORT && !defined(CFG_MATTER))
    ble_cli_init();
#endif  // (BLE_APP_CMD_SUPPORT && !defined(CFG_MATTER))

    if (all == false)
        return;

#ifndef TUYAOS_SUPPORT
    ble_power_on();
    dbg_print(ERR, "BLE POWER ON \r\n");

#ifdef CFG_VIRTUAL_HCI_MODE
    app_virtual_hci_init();
#endif

    ret = ble_stack_init(0, &os_interface);
    if (ret != BLE_ERR_NO_ERROR) {
        dbg_print(ERR, "ble stack init fail status 0x%x", ret);
    }

    ble_task_init();

#ifdef CFG_VIRTUAL_HCI_MODE
    app_virtual_hci_enable();
#endif

#if (BLE_APP_SUPPORT)
    ble_module_init();

    ble_app_init();
#endif // (BLE_APP_SUPPORT)

    /* ble need to close deep sleep before flash erase */
    //KK BURAYA BAK
    raw_flash_erase_handler_register(ble_flash_erase_handler);
#else
#ifdef CFG_VIRTUAL_HCI_MODE
    tkl_virtual_hci_init();
#else
    // Workaround for tuya project
    // If Tuya change the workflow, this code can be removed
    tuya_adp_init(3);
#endif
#endif

#ifdef CFG_COEX
    ble_coex_evt_notify_register(coex_ble_event_notify);
#endif



    ble_work_status_set(BLE_WORK_STATUS_ENABLE);
}

/*!
    \brief      Deinitialization of the BLE module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ble_deinit(void)
{
    raw_flash_erase_handler_unregister(ble_flash_erase_handler);

#if (BLE_APP_SUPPORT)
    ble_app_deinit();
#endif // (BLE_APP_SUPPORT)

    ble_power_off();

    ble_task_deinit();
}

