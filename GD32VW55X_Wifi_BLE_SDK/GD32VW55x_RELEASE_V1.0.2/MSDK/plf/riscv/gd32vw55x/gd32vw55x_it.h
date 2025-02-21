/*!
    \file    gd32vw55x_it.h
    \brief   the header file of the ISR

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/


#ifndef GD32VW55X_IT_H
#define GD32VW55X_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

void USART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void WIFI_WKUP_IRQHandler(void);
void WIFI_INT_IRQHandler(void);
void WIFI_INTGEN_IRQHandler(void);
void WIFI_PROT_IRQHandler(void);
void WIFI_RX_IRQHandler(void);
void WIFI_TX_IRQHandler(void);
void I2C0_EV_IRQHandler(void);
void I2C0_ER_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);

void BLE_POWER_STATUS_IRQHandler(void);
void BLE_WKUP_IRQHandler(void);
void BLE_HALF_SLOT_IRQHandler(void);
void BLE_SLEEP_MODE_IRQHandler(void);
void BLE_ENCRYPTION_ENGINE_IRQHandler(void);
void BLE_SW_TRIG_IRQHandler(void);
void BLE_FINE_TIMER_TARGET_IRQHandler(void);
void BLE_STAMP_TARGET1_IRQHandler(void);
void BLE_STAMP_TARGET2_IRQHandler(void);
void BLE_STAMP_TARGET3_IRQHandler(void);
void BLE_FREQ_SELECT_IRQHandler(void);
void BLE_ERROR_IRQHandler(void);
void BLE_FIFO_ACTIVITY_IRQHandler(void);

void DMA_Channel1_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);
void EXTI5_9_IRQHandler(void);
#ifdef __cplusplus
}
#endif

#endif /* GD32VW55X_IT_H */
