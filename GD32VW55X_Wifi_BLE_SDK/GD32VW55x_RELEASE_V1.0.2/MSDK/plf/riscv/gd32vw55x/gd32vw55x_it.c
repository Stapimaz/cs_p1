/*!
    \file    gd32vw55x_it.c
    \brief   interrupt service routines

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/



#include "app_cfg.h"

#include "gd32vw55x_it.h"
#include "gd32vw55x.h"
#include "uart.h"
#include "wakelock.h"
#ifdef CFG_BLE_SUPPORT
#include "ble_export.h"
#endif
#include "dbg_print.h"
#include "trace_uart.h"
#include "gd32vw55x_platform.h"
#include "wrapper_os.h"

#ifdef CFG_WLAN_SUPPORT
#include "wlan_config.h"
#include "wifi_export.h"
#endif

#ifdef CFG_BLE_HCI_MODE
#include "ble_uart.h"
#endif

#ifdef TUYAOS_SUPPORT
#include "tkl_i2c.h"
#include "tkl_pwm.h"
#include "tkl_gpio.h"
#include "tkl_timer.h"
#endif

#if defined CONFIG_ATCMD
#include "atcmd.h"
#endif
/*!
    \brief      this function handles USART0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART0_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    uart_irq_hdl(USART0);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles USART1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UART1_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    uart_irq_hdl(UART1);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles USART1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UART2_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    uart_irq_hdl(UART2);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

#ifdef HCI_UART_RX_DMA
void DMA_Channel5_IRQHandler(void)
{
    hci_uart_dma_channel5_irq_hdl();
}
#endif

#ifdef TRACE_UART_DMA
#ifdef CONFIG_PLATFORM_ASIC
#ifdef CFG_BLE_HCI_MODE
void DMA_Channel6_IRQHandler(void)
{
    trace_uart_dma_channel_irq_hdl();
}
#else
void DMA_Channel7_IRQHandler(void)
{
    trace_uart_dma_channel_irq_hdl();
}
#endif      // CFG_BLE_HCI_MODE end
#else
void DMA_Channel1_IRQHandler(void)
{
    trace_uart_dma_channel_irq_hdl();
}
#endif     // CONFIG_PLATFORM_ASIC end
#endif     // TRACE_UART_DMA end

#if defined CONFIG_ATCMD && defined HCI_UART_RX_DMA
#error "THE ATCMD AND HCI_UART_RX_DMA SHOULD NOT USE SAME UART PORT AT THE SAME TIME"
#elif defined CONFIG_ATCMD && !defined HCI_UART_RX_DMA
void DMA_Channel2_IRQHandler(void)
{
    if (AT_UART == USART0) {
        at_uart_rx_dma_irq_hdl(DMA_CH2);
    }
}
void DMA_Channel0_IRQHandler(void)
{
    if (AT_UART == UART1) {
        at_uart_rx_dma_irq_hdl(DMA_CH0);
    }
}
void DMA_Channel5_IRQHandler(void)
{
    if (AT_UART == UART2) {
        at_uart_rx_dma_irq_hdl(DMA_CH5);
    }
}
#endif

void RTC_WKUP_IRQHandler(void)
{
    deep_sleep_exit();
    dbg_print(DEBUG, "rtc\n");
}

/*!
    \brief      this function handles EXTI5_9 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
    deep_sleep_exit();

#ifdef LOG_UART
    dbg_print(NOTICE, "WAKEUP For Console, Input Any Command or Press 'Enter' Key to Deep Sleep\r\n#\r\n");
    usart_command_enable(LOG_UART,USART_CMD_RXFCMD);
    sys_wakelock_acquire(LOCK_ID_USART);
#endif
#ifdef TUYAOS_SUPPORT
    gpio_irq_hdl(5);
#endif
}

#ifdef CFG_WLAN_SUPPORT
#ifdef CFG_LPS
void WIFI_WKUP_IRQHandler(void)
{
    wlan_exti_exit();
    deep_sleep_exit();
    wifi_wakeup_isr();
}
#elif defined (CFG_PS_HW_WAKE)
void WIFI_WKUP_IRQHandler(void)
{
    wlan_exti_exit();
    // HW is idle wake up from sleep,
    // We must set HW to active early to receive bcn
    wifi_wakeup(1);
    dbg_print(DEBUG, "ex\n");
}
#endif

void WIFI_INT_IRQHandler(void)
{
    intc_irq();
}

void WIFI_INTGEN_IRQHandler(void)
{
    //wake up wifi moudle if sleep
    wifi_wakeup(1);
#ifdef CFG_LPS
    if (!wifi_in_doze())
#endif
    {
        hal_machw_gen_handler();
#ifdef CFG_RTOS
        wifi_core_task_resume(true);
#endif
    }
}

void WIFI_PROT_IRQHandler(void)
{
    txl_prot_trigger();
#ifdef CFG_RTOS
    wifi_core_task_resume(true);
#endif
}

void LA_IRQHandler(void)
{
    hal_la_isr();
#ifdef CFG_RTOS
    wifi_core_task_resume(true);
#endif
}

void WIFI_RX_IRQHandler(void)
{
    rxl_mpdu_isr();
#ifdef CFG_RTOS
    wifi_core_task_resume(true);
#endif
}

void WIFI_TX_IRQHandler(void)
{
    txl_transmit_trigger();
#ifdef CFG_RTOS
    wifi_core_task_resume(true);
#endif
}
#endif // CFG_WLAN_SUPPORT

#ifdef CFG_BLE_SUPPORT
void BLE_POWER_STATUS_IRQHandler(void)
{
    /* ble core goes from active to sleep and ble_ps_fall_en will generate power status interrupt */
    if (ble_power_status_fall_status() != 0) {
        ble_power_status_fall_clear();

        /* power off */
        ble_pmu_config(0);

        /* configuring ble exti protection a power status rise interrupt was generated when configuring CPU deepsleep but no ble exti interrupt */
        ble_exti_enter();

        /* release the ble lock of cpu deepsleep */
        ble_wakelock_release();
    }

    /* ble core goes from sleep to active and ble_ps_rise_en will generate power status interrupt */
    if (ble_power_status_rise_status() != 0) {
        ble_power_status_rise_clear();

        /* power on */
        ble_pmu_config(1);

        /* ble pmu off, the modem is not saved and needs to be reconfigured */
        ble_modem_config();

        /* configuring ble exti exit */
        ble_exti_exit();

        /* acquire the ble lock of cpu deepsleep */
        ble_wakelock_acquire();
    }
}

void BLE_WKUP_IRQHandler(void)
{
    /* ble module clear exti by self */
    ble_exti_exit();
    deep_sleep_exit();
}

void BLE_HALF_SLOT_IRQHandler(void)
{
    ble_hslot_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_SLEEP_MODE_IRQHandler(void)
{
    ble_slp_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_ENCRYPTION_ENGINE_IRQHandler(void)
{
    ble_crypt_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_SW_TRIG_IRQHandler(void)
{
    ble_sw_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_FINE_TIMER_TARGET_IRQHandler(void)
{
    ble_fine_tgt_isr();
}

void BLE_STAMP_TARGET1_IRQHandler(void)
{
    ble_ts_tgt1_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_STAMP_TARGET2_IRQHandler(void)
{
    ble_ts_tgt2_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_STAMP_TARGET3_IRQHandler(void)
{
    ble_ts_tgt3_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_FREQ_SELECT_IRQHandler(void)
{
    ble_hop_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_ERROR_IRQHandler(void)
{
    ble_error_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}

void BLE_FIFO_ACTIVITY_IRQHandler(void)
{
    ble_fifo_isr();
#ifdef CFG_RTOS
    ble_stack_task_resume(true);
#endif
}
#endif // CFG_BLE_SUPPORT

#ifdef TUYAOS_SUPPORT
void I2C0_EV_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    i2c_irq_hdl(I2C0);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

void I2C0_ER_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    i2c_irq_hdl(I2C0);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

void I2C1_EV_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    i2c_irq_hdl(I2C1);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

void I2C1_ER_IRQHandler(void)
{
    //sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    i2c_irq_hdl(I2C1);

    //sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

void TIMER0_Channel_IRQHandler(void)
{
    pwm_cap_irq_hdl(TIMER0);
}

void TIMER15_IRQHandler(void)
{
    pwm_cap_irq_hdl(TIMER15);
}

void TIMER16_IRQHandler(void)
{
    pwm_cap_irq_hdl(TIMER16);
}

void TIMER1_IRQHandler(void)
{
    timer_irq_hdl(TIMER1);
}

void TIMER2_IRQHandler(void)
{
    timer_irq_hdl(TIMER2);
}

void EXTI0_IRQHandler(void)
{
    gpio_irq_hdl(0);
}

void EXTI1_IRQHandler(void)
{
    gpio_irq_hdl(1);
}

void EXTI2_IRQHandler(void)
{
    gpio_irq_hdl(2);
}

void EXTI3_IRQHandler(void)
{
    gpio_irq_hdl(3);
}

void EXTI4_IRQHandler(void)
{
    gpio_irq_hdl(4);
}

void EXTI10_15_IRQHandler(void)
{
    gpio_irq_hdl(6);
}
#endif
