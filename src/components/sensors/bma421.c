//
// Created by jack on 8/22/20.
//

#include "bma421.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// TWI
#include "twi_driver.h"
#include "nrf_gpio.h"

// BMA421
#include "bma4.h"
#include "bma423.h"
#include "bma4_defs.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"

static struct bma4_dev bma = {
    .intf = BMA4_I2C_INTF,
    .bus_read = (bma4_read_fptr_t)twi_rx,
    .bus_write = (bma4_write_fptr_t)twi_tx,
    .variant = BMA42X_VARIANT,
    .read_write_len = 8,
    .delay_us = delay
};
static struct bma4_accel_config accel_conf;
static struct bma423_any_no_mot_config any_no_mot;
static uint32_t step_count = 0;
static uint8_t bma_address = BMA4_I2C_ADDR_PRIMARY;

bool bma_init(void)
{
    int8_t ret;

    bma.intf_ptr = &bma_address;
    ret = bma423_init(&bma);
    ret |= bma423_write_config_file(&bma);

    uint16_t id;
    bma423_get_config_id(&id, &bma);
    NRF_LOG_INFO("BMA421 ID: %d", id);

    accel_conf.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    accel_conf.range = BMA4_ACCEL_RANGE_2G;
    accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    accel_conf.perf_mode = BMA4_CIC_AVG_MODE;
    ret |= bma4_set_accel_config(&accel_conf, &bma);
    ret |= bma423_feature_enable(BMA423_STEP_CNTR, 1, &bma);
    ret |= bma423_feature_enable(BMA423_WRIST_WEAR, 1, &bma);

    any_no_mot.axes_en = BMA423_EN_ALL_AXIS;
    any_no_mot.threshold = 10;
    any_no_mot.duration = 4;
    ret |= bma423_set_any_mot_config(&any_no_mot, &bma);
    ret |= bma423_set_no_mot_config(&any_no_mot, &bma);
    ret |= bma423_map_interrupt(BMA4_INTR1_MAP, BMA423_WRIST_WEAR_INT, BMA4_ENABLE, &bma);

    if(ret != 0)
    {
        NRF_LOG_INFO("BMA421 Init Failed");
        return false;
    }
    else
    {
        return true;
    }
}

bool bma423_get_device_id(void)
{
    uint8_t rx_data = 0;
    twi_read_reg(BMA4_I2C_ADDR_PRIMARY, BMA4_CHIP_ID_ADDR, &rx_data, 1);
    NRF_LOG_INFO("BMA ID: %d", rx_data);

    bool ret = false;
    if(rx_data == BMA423_CHIP_ID)
    {
        ret = true;
    }
    return ret;
}

bool bma423_set_interrupt_source(uint8_t source)
{
    bool ret = false;
    ret |= bma423_map_interrupt(BMA4_INTR1_MAP, source, BMA4_ENABLE, &bma);
    return ret;
}

bool bma423_get_interrupt_status(void)
{
    bool ret = false;
    if(nrf_gpio_pin_read(BMA421_INT_PIN))
    {
        ret = true;
    }
    return ret;
}

void update_step_count(void)
{
    bma423_step_counter_output(&step_count, &bma);
}

uint32_t get_step_count(void)
{
    return step_count;
}


void delay(uint32_t period_us, void * intf_ptr)
{
    if(period_us >= 1000)
    {
        vTaskDelay(pdMS_TO_TICKS(period_us / 1000));
    }
    else {
        uint32_t i;
        for(i = 0; i < (period_us * 64); i++) {}
    }
}

bool sleep_bma(void)
{
    return true;
}
