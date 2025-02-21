#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "sensor_am2301.h"

#include "gd32vw55x.h"
#include "dbg_print.h"
#include "wrapper_os.h"

// Original definitions
#define AM2301_GPIO_PORT       GPIOB
#define AM2301_RCU_GPIO        RCU_GPIOB

#define AM2301_PIN_1           GPIO_PIN_11
#define AM2301_PIN_2           GPIO_PIN_12
#define AM2301_PIN_3           GPIO_PIN_13



static void am2301_busy_wait_us(uint32_t us)
{
    uint32_t cycles = us * (SystemCoreClock / 1000000UL) / 5;
    while (cycles--) {
        __NOP();
    }
}

static void am2301_init_pin(uint32_t pin)
{
    gpio_mode_set(AM2301_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
    gpio_output_options_set(AM2301_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, pin);
}

static void am2301_set_pin_output_low(uint32_t pin)
{
    gpio_mode_set(AM2301_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(AM2301_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, pin);
    gpio_bit_reset(AM2301_GPIO_PORT, pin);
}

static void am2301_set_pin_input_pullup(uint32_t pin)
{
    gpio_mode_set(AM2301_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
    gpio_output_options_set(AM2301_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, pin);
}

static bool am2301_read_sensor(uint32_t pin, float *out_humidity, float *out_temperature)
{
    uint32_t timeout;
    uint8_t data[5] = {0,0,0,0,0};

    __disable_irq();

    // 1) Send start signal (drive line low ~2ms)
    am2301_set_pin_output_low(pin);
    for(uint32_t ms = 0; ms < 2; ms++){
        am2301_busy_wait_us(1000);
    }

    // 2) Release line => input pull-up
    am2301_set_pin_input_pullup(pin);
    am2301_busy_wait_us(30);

    // Wait sensor response
    timeout = 1000;
    while (gpio_input_bit_get(AM2301_GPIO_PORT, pin) == 1) {
        if (--timeout == 0) {
            __enable_irq();
            return false;
        }
        am2301_busy_wait_us(1);
    }
    timeout = 1000;
    while (gpio_input_bit_get(AM2301_GPIO_PORT, pin) == 0) {
        if (--timeout == 0) {
            __enable_irq();
            return false;
        }
        am2301_busy_wait_us(1);
    }
    timeout = 1000;
    while (gpio_input_bit_get(AM2301_GPIO_PORT, pin) == 1) {
        if (--timeout == 0) {
            __enable_irq();
            return false;
        }
        am2301_busy_wait_us(1);
    }

    // 3) Read 40 bits
    for (int bit_index = 0; bit_index < 40; bit_index++) {
        // Wait for line to go HIGH
        timeout = 1000;
        while (gpio_input_bit_get(AM2301_GPIO_PORT, pin) == 0) {
            if (--timeout == 0) {
                __enable_irq();
                return false;
            }
            am2301_busy_wait_us(1);
        }
        // Once high, wait ~35us, then sample
        am2301_busy_wait_us(35);
        uint8_t bit_val = (gpio_input_bit_get(AM2301_GPIO_PORT, pin) ? 1 : 0);

        // Wait line LOW
        timeout = 1000;
        while (gpio_input_bit_get(AM2301_GPIO_PORT, pin) == 1) {
            if (--timeout == 0) {
                __enable_irq();
                return false;
            }
            am2301_busy_wait_us(1);
        }

        // Store bit
        int byte_index = bit_index / 8;
        data[byte_index] <<= 1;
        data[byte_index] |= bit_val;
    }
    __enable_irq();

    // Checksum
    uint8_t sum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (sum != data[4]) {
        return false;
    }

    uint16_t raw_rh = ((uint16_t)data[0] << 8) | data[1];
    uint16_t raw_t  = ((uint16_t)data[2] << 8) | data[3];
    bool negative = false;
    if (raw_t & 0x8000) {
        raw_t &= 0x7FFF;
        negative = true;
    }

    float rh_f = (float)raw_rh / 10.0f;
    float t_f  = (float)raw_t / 10.0f;
    if (negative) t_f = -t_f;

    *out_humidity    = rh_f;
    *out_temperature = t_f;
    return true;
}

void sensor_task(void *arg)
{
    (void)arg;

    // Enable clock for GPIOB
    rcu_periph_clock_enable(AM2301_RCU_GPIO);

    // Initialize pins
    am2301_init_pin(AM2301_PIN_1);
    am2301_init_pin(AM2301_PIN_2);
    am2301_init_pin(AM2301_PIN_3);

    sys_ms_sleep(60000);

    while (1)
    {
        float total_humidity = 0.0f;
        float total_temperature = 0.0f;
        int valid_sensor_count = 0;

        printf("####### TEMP AND HUMIDITY DATA #######\r\n");

        for (int i = 0; i < 3; i++)
        {
            float humidity = 0.0f;
            float temperature = 0.0f;
            bool ok = false;
            if (i == 0)      ok = am2301_read_sensor(AM2301_PIN_1, &humidity, &temperature);
            else if (i == 1) ok = am2301_read_sensor(AM2301_PIN_2, &humidity, &temperature);
            else             ok = am2301_read_sensor(AM2301_PIN_3, &humidity, &temperature);

            if (ok) {
                total_humidity += humidity;
                total_temperature += temperature;
                valid_sensor_count++;
            } else {
                printf("   Sensor #%d => Read Error!\r\n", i + 1);
            }
        }

        if (valid_sensor_count > 0) {
            float avg_humidity = total_humidity / valid_sensor_count;
            float avg_temperature = total_temperature / valid_sensor_count;

            printf("   Average from %d Sensor(s):\r\n", valid_sensor_count);
            printf("   RH = %.1f %% | Temp = %.1f °C\r\n", avg_humidity, avg_temperature);
        } else {
            printf("   No readings available.\r\n");
        }

        printf("###################################\r\n");
        sys_ms_sleep(20000);
    }
}
