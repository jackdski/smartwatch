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

BMA4_INTF_RET_TYPE twi_bus_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len, void *intf_ptr);
BMA4_INTF_RET_TYPE twi_bus_write(uint8_t reg_addr, const uint8_t *read_data, uint32_t len, void *intf_ptr);

static struct bma4_dev bma = {
    .intf = BMA4_I2C_INTF,
    .bus_read = twi_bus_read,
    .bus_write = twi_bus_write,
    .variant = BMA42X_VARIANT,
    .read_write_len = 8,
    .delay_us = delay
};

static bma_ctrl_t bma_ctrl = {
    .state              = SENSORS_INITIALIZATION,
    .interrupt_source   = BMA_INT_WRIST_WEAR,
    .interrupt_chg_req  = false,
    .step_count         = 0
};

static struct bma4_accel_config accel_conf;
static struct bma423_any_no_mot_config any_no_mot;
static struct bma4_int_pin_config int_pin_conf;
uint8_t bma_address = BMA4_I2C_ADDR_PRIMARY;

bool bma_init(void)
{
    int16_t ret;

//    ret = bma4_set_aux_if_mode(1, &bma);

    bma.intf_ptr = &bma_address;
    ret = bma423_init(&bma);
    NRF_LOG_INFO("BMA Init Ret: %d", ret);

    ret = bma423_write_config_file(&bma);
    NRF_LOG_INFO("BMA Config Ret: %d", ret);

    uint16_t id;
    bma423_get_config_id(&id, &bma);
    NRF_LOG_INFO("BMA421 ID: %d", id);

    accel_conf.odr = BMA4_OUTPUT_DATA_RATE_25HZ;
    accel_conf.range = BMA4_ACCEL_RANGE_2G;
    accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    accel_conf.perf_mode = BMA4_CIC_AVG_MODE;
    ret |= bma4_set_accel_config(&accel_conf, &bma);

    any_no_mot.axes_en = BMA423_EN_ALL_AXIS;
    any_no_mot.threshold = 10;
    any_no_mot.duration = 4;
    ret |= bma423_set_any_mot_config(&any_no_mot, &bma);
    ret |= bma423_set_no_mot_config(&any_no_mot, &bma);

    // enable features
    bma423_step_detector_enable(BMA4_ENABLE, &bma);
    ret |= bma423_feature_enable(BMA423_STEP_CNTR, 1, &bma);
    ret |= bma423_feature_enable(BMA423_WRIST_WEAR, 1, &bma);

    // interrupts
    int_pin_conf.output_en = BMA4_OUTPUT_ENABLE;
    int_pin_conf.edge_ctrl = BMA4_EDGE_TRIGGER;
    int_pin_conf.input_en = BMA4_INPUT_DISABLE;
    int_pin_conf.od = BMA4_INPUT_DISABLE;
    int_pin_conf.lvl = BMA4_ACTIVE_LOW;
    ret |= bma4_set_int_pin_config(&int_pin_conf, BMA4_INTR1_MAP, &bma);
    ret |= bma423_set_interrupt_source(bma_ctrl.interrupt_source);

    vTaskDelay(pdMS_TO_TICKS(150));
    NRF_LOG_INFO("BMA421 Init: %d", ret);
    return (ret == 0);
}

BMA4_INTF_RET_TYPE twi_bus_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len, void *intf_ptr)
{
    UNUSED_PARAMETER(intf_ptr);
    twi_rx(BMA4_I2C_ADDR_PRIMARY, reg_addr, read_data, len);
    return BMA4_INTF_RET_SUCCESS;
}

BMA4_INTF_RET_TYPE twi_bus_write(uint8_t reg_addr, const uint8_t * write_data, uint32_t len, void *intf_ptr)
{
    uint8_t data[len + 1];
    data[0] = reg_addr;
    memcpy(data+1, write_data, len);
    twi_tx(BMA4_I2C_ADDR_PRIMARY, data, len);
    return BMA4_INTF_RET_SUCCESS;
}

bool bma423_get_device_id(void)
{
    uint8_t rx_data = 0;
    twi_reg_read(BMA4_I2C_ADDR_PRIMARY, BMA4_CHIP_ID_ADDR, &rx_data);
    NRF_LOG_INFO("BMA ID: %d", rx_data);

    bool ret = false;
    if(rx_data == BMA423_CHIP_ID)
    {
        ret = true;
    }
    return ret;
}

bool bma423_set_interrupt_source(eBMAInterruptSource int_source)
{
    bool ret = false;
    ret |= bma423_map_interrupt(BMA4_INTR1_MAP, int_source, BMA4_ENABLE, &bma);
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
    bma423_step_counter_output(&bma_ctrl.step_count, &bma);
}

uint32_t get_step_count(void)
{
    return bma_ctrl.step_count;
}

void delay(uint32_t period_us, void * intf_ptr)
{
    UNUSED_PARAMETER(intf_ptr);
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
