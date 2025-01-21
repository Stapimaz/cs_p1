/**
 * @file tkl_gpio.c
 * @brief this file was auto-generated by tuyaos v&v tools, developer can add implements between BEGIN and END
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

// --- BEGIN: user defines and implements ---
#include "gd32vw55x_gpio.h"
#include "gd32vw55x_rcu.h"
#include "tkl_gpio.h"
#include "tuya_error_code.h"
// --- END: user defines and implements ---

#ifdef TUYAOS_SUPPORT

typedef struct gpio_port {
    UINT32_T gpio_group;
    UINT32_T pin_num;
} gpio_port_t;

typedef struct {
    UINT32_T            exti_line;
    UCHAR_T             is_irq;
    TUYA_GPIO_IRQ_CB    cb;
    void                *args;
} pin_dev_map_t;

static pin_dev_map_t gpio_pin_map[TUYA_GPIO_NUM_MAX] = {
    // PA0-PA15
    {EXTI_0,  false, NULL, NULL}, {EXTI_1,  false, NULL, NULL},
    {EXTI_2,  false, NULL, NULL}, {EXTI_3,  false, NULL, NULL},
    {EXTI_4,  false, NULL, NULL}, {EXTI_5,  false, NULL, NULL},
    {EXTI_6,  false, NULL, NULL}, {EXTI_7,  false, NULL, NULL},
    {EXTI_8,  false, NULL, NULL}, {EXTI_9,  false, NULL, NULL},
    {EXTI_10, false, NULL, NULL}, {EXTI_11, false, NULL, NULL},
    {EXTI_12, false, NULL, NULL}, {EXTI_13, false, NULL, NULL},
    {EXTI_14, false, NULL, NULL}, {EXTI_15, false, NULL, NULL},
    // PB0-PB4
    {EXTI_0,  false, NULL, NULL}, {EXTI_1,  false, NULL, NULL},
    {EXTI_2,  false, NULL, NULL}, {EXTI_3,  false, NULL, NULL},
    {EXTI_4,  false, NULL, NULL},
    // PB11-PB15
    {EXTI_11, false, NULL, NULL}, {EXTI_12, false, NULL, NULL},
    {EXTI_13, false, NULL, NULL}, {EXTI_14, false, NULL, NULL},
    {EXTI_15, false, NULL, NULL},
    // PC8 PC13 PC15
    {EXTI_8,  false, NULL, NULL},
    // PC13
    {EXTI_13, false, NULL, NULL},
    // PC15
    {EXTI_15, false, NULL, NULL},
};

void gpio_irq_hdl(UINT32_T exti_line_num)
{
    UCHAR_T i;
    INT_T index = 0;
    CHAR_T exti_pin_num[] = {2, 2, 2, 2, 2, 6, 13};
    CHAR_T exti_pin_index_map[7][13] = {{0, 16}, {1, 17}, {2, 18}, {3, 19}, {4, 20}, {5, 6, 7, 8, 9, 26},\
                                    {10, 11, 12, 13, 14, 15, 21, 22, 23, 24, 25, 27, 28}};

    for (i = 0; i < exti_pin_num[exti_line_num]; i++) {
        index = exti_pin_index_map[exti_line_num][i];
        if (gpio_pin_map[index].is_irq == true) {
            if (RESET != exti_interrupt_flag_get(gpio_pin_map[index].exti_line)) {
                gpio_pin_map[index].cb(gpio_pin_map[index].args);
                exti_interrupt_flag_clear(gpio_pin_map[index].exti_line);
            }
        }
    }
}

static int pin_id_2_gpio_port(TUYA_GPIO_NUM_E pin_id, struct gpio_port *port)
{
    if (pin_id >= TUYA_GPIO_NUM_0 && pin_id <= TUYA_GPIO_NUM_15) {          // PA0-PA15
        port->gpio_group = GPIOA;
        port->pin_num = BIT(pin_id);
    } else if (pin_id >= TUYA_GPIO_NUM_16 && pin_id <= TUYA_GPIO_NUM_20) {  // PB0-PB4
        port->gpio_group = GPIOB;
        port->pin_num = BIT((pin_id - 16));
    } else if (pin_id >= TUYA_GPIO_NUM_21 && pin_id <= TUYA_GPIO_NUM_24) {  // PB11-PB13 & PB15
        port->gpio_group = GPIOB;
        if (pin_id == TUYA_GPIO_NUM_24) {
            port->pin_num = GPIO_PIN_15;
        } else {
            port->pin_num = BIT((pin_id - 10));
        }
    } else if (pin_id >= TUYA_GPIO_NUM_25 && pin_id < TUYA_GPIO_NUM_MAX) {  // PC8 & PC13 & PC15
        port->gpio_group = GPIOC;
        if (pin_id == TUYA_GPIO_NUM_25) {
            port->pin_num = GPIO_PIN_8;
        } else {
            port->pin_num = BIT((pin_id - 13));
        }
    } else {
        return -1;
    }

    return 0;
}

/**
 * @brief gpio init
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 * @param[in] cfg:  gpio config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_init(TUYA_GPIO_NUM_E pin_id, CONST TUYA_GPIO_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    struct gpio_port port;

    if (pin_id_2_gpio_port(pin_id, &port))
        return OPRT_INVALID_PARM;

    switch (port.gpio_group)
    {
    case GPIOA:
        rcu_periph_clock_enable(RCU_GPIOA);
        break;
    case GPIOB:
        rcu_periph_clock_enable(RCU_GPIOB);
        break;
    case GPIOC:
        rcu_periph_clock_enable(RCU_GPIOC);
        break;
    default:
        break;
    }

    switch (cfg->mode) {
    case TUYA_GPIO_PULLUP:            // for input
        gpio_mode_set(port.gpio_group, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, port.pin_num);
        break;
    case TUYA_GPIO_PULLDOWN:          // for input
        gpio_mode_set(port.gpio_group, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, port.pin_num);
        break;
    case TUYA_GPIO_FLOATING:          // for input
        gpio_mode_set(port.gpio_group, GPIO_MODE_INPUT, GPIO_PUPD_NONE, port.pin_num);
        break;
    case TUYA_GPIO_PUSH_PULL:         // for output
        gpio_mode_set(port.gpio_group, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, port.pin_num);
        gpio_output_options_set(port.gpio_group, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, port.pin_num);
        break;
    case TUYA_GPIO_OPENDRAIN:         // for output
        gpio_mode_set(port.gpio_group, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, port.pin_num);
        gpio_output_options_set(port.gpio_group, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, port.pin_num);
        break;
    case TUYA_GPIO_OPENDRAIN_PULLUP:  // for output
        gpio_mode_set(port.gpio_group, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, port.pin_num);
        gpio_output_options_set(port.gpio_group, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, port.pin_num);
        break;
    case TUYA_GPIO_HIGH_IMPEDANCE:    // for input
        gpio_mode_set(port.gpio_group, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, port.pin_num);
        gpio_output_options_set(port.gpio_group, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, port.pin_num);
    default:
        return OPRT_INVALID_PARM;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio deinit
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_deinit(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TUYA_GPIO_NUM_MAX)
        return OPRT_INVALID_PARM;

    if (gpio_pin_map[pin_id].is_irq == true) {
        gpio_pin_map[pin_id].is_irq = false;
        gpio_pin_map[pin_id].args = NULL;
        gpio_pin_map[pin_id].cb = NULL;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio write
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 * @param[in] level: gpio output level value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_write(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E level)
{
    // --- BEGIN: user implements ---
    struct gpio_port port;

    if (pin_id_2_gpio_port(pin_id, &port))
        return OPRT_INVALID_PARM;

    gpio_bit_write(port.gpio_group, port.pin_num, (FlagStatus)level);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio read
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 * @param[out] level: gpio output level
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_read(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E *level)
{
    // --- BEGIN: user implements ---
    struct gpio_port port;

    if (pin_id_2_gpio_port(pin_id, &port))
        return OPRT_INVALID_PARM;

    *level = (TUYA_GPIO_LEVEL_E)gpio_input_bit_get(port.gpio_group, port.pin_num);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq init
 * NOTE: call this API will not enable interrupt
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 * @param[in] cfg:  gpio irq config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_irq_init(TUYA_GPIO_NUM_E pin_id, CONST TUYA_GPIO_IRQ_T *cfg)
{
    // --- BEGIN: user implements --
    struct gpio_port port;

    if (pin_id_2_gpio_port(pin_id, &port))
        return OPRT_INVALID_PARM;

    // se t as input mode
    gpio_mode_set(port.gpio_group, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, port.pin_num);

    rcu_periph_clock_enable(RCU_SYSCFG);

    switch (port.gpio_group)
    {
    case GPIOA:
        syscfg_exti_line_config(EXTI_SOURCE_GPIOA, (31 - __CLZ(port.pin_num)));
        break;
    case GPIOB:
        syscfg_exti_line_config(EXTI_SOURCE_GPIOB, (31 - __CLZ(port.pin_num)));
        break;
    case GPIOC:
        syscfg_exti_line_config(EXTI_SOURCE_GPIOC, (31 - __CLZ(port.pin_num)));
        break;
    default:
        break;
    }

    switch (cfg->mode)
    {
    case TUYA_GPIO_IRQ_RISE:
        exti_init(gpio_pin_map[pin_id].exti_line, EXTI_INTERRUPT, EXTI_TRIG_RISING);
        break;
    case TUYA_GPIO_IRQ_FALL:
        exti_init(gpio_pin_map[pin_id].exti_line, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        break;
    case TUYA_GPIO_IRQ_RISE_FALL:
        exti_init(gpio_pin_map[pin_id].exti_line, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
        break;
    case TUYA_GPIO_IRQ_LOW:
    case TUYA_GPIO_IRQ_HIGH:
    default:
        return OPRT_INVALID_PARM;
    }

    gpio_pin_map[pin_id].args = cfg->arg;
    gpio_pin_map[pin_id].cb = cfg->cb;
    gpio_pin_map[pin_id].is_irq = true;

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq enable
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_irq_enable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TUYA_GPIO_NUM_MAX)
        return OPRT_INVALID_PARM;

    if (gpio_pin_map[pin_id].is_irq == false)
        return OPRT_INVALID_PARM;

    switch (gpio_pin_map[pin_id].exti_line)
    {
    case EXTI_0:
        eclic_irq_enable(EXTI0_IRQn, 8, 0);
        break;
    case EXTI_1:
        eclic_irq_enable(EXTI1_IRQn, 8, 0);
        break;
    case EXTI_2:
        eclic_irq_enable(EXTI2_IRQn, 8, 0);
        break;
    case EXTI_3:
        eclic_irq_enable(EXTI3_IRQn, 8, 0);
        break;
    case EXTI_4:
        eclic_irq_enable(EXTI4_IRQn, 8, 0);
        break;
    case EXTI_5 ... EXTI_9:
        eclic_irq_enable(EXTI5_9_IRQn, 8, 0);
        break;
    case EXTI_10 ... EXTI_15:
        eclic_irq_enable(EXTI10_15_IRQn, 8, 0);
        break;
    default:
        return OPRT_INVALID_PARM;
    }

    exti_interrupt_flag_clear(gpio_pin_map[pin_id].exti_line);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq disable
 *
 * @param[in] pin_id: gpio pin id, id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_irq_disable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TUYA_GPIO_NUM_MAX)
        return OPRT_INVALID_PARM;

    switch (gpio_pin_map[pin_id].exti_line)
    {
    case EXTI_0:
        eclic_irq_disable(EXTI0_IRQn);
        break;
    case EXTI_1:
        eclic_irq_disable(EXTI1_IRQn);
        break;
    case EXTI_2:
        eclic_irq_disable(EXTI2_IRQn);
        break;
    case EXTI_3:
        eclic_irq_disable(EXTI3_IRQn);
        break;
    case EXTI_4:
        eclic_irq_disable(EXTI4_IRQn);
        break;
    case EXTI_5 ... EXTI_9:
        eclic_irq_disable(EXTI5_9_IRQn);
        break;
    case EXTI_10 ... EXTI_15:
        eclic_irq_disable(EXTI10_15_IRQn);
        break;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

#endif /* TUYAOS_SUPPORT */
