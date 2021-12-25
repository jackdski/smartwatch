//
// Created by jack on 8/22/20.
//

#include "bma421.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "app_config.h"
#include <string.h>

// TWI
#include "twi_driver.h"

// GPIO
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

// BMA421
#include "bma4.h"
#include "bma423.h"
#include "bma4_defs.h"

// Defines
BMA4_INTF_RET_TYPE twi_bus_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len, void *intf_ptr);
BMA4_INTF_RET_TYPE twi_bus_write(uint8_t reg_addr, const uint8_t *read_data, uint32_t len, void *intf_ptr);

#define STEP_COUNTER_WATERMARK_RESOLUTION   20U

// Typedefs
typedef struct
{
    BMA_State_E             state;
    BMA_InterruptSource_E   interrupt_source;
    bool                    interrupt_chg_req;
    uint32_t                step_count;
} bma_data_t;

typedef struct
{
    bool        step_counter_en;
    bool        interrupt_en;
    uint8_t     addr;
    uint32_t    stepCounterIntWaterMark;  // resolution of 20 steps
} bma421_config_t;

// Private Function Definitions
static void bma_private_delay(uint32_t period_us, void * intf_ptr);

// Private Structs
static struct bma4_dev bma =
{
    .intf = BMA4_I2C_INTF,
    .bus_read = twi_bus_read,
    .bus_write = twi_bus_write,
    .variant = BMA42X_VARIANT,
    .read_write_len = 8,
    .delay_us = bma_private_delay
};

static struct bma4_accel_config accel_conf =
{
    .odr        = BMA4_OUTPUT_DATA_RATE_25HZ,
    .range      = BMA4_ACCEL_RANGE_2G,
    .bandwidth  = BMA4_ACCEL_NORMAL_AVG4,
    .perf_mode  = BMA4_CIC_AVG_MODE,
};

static struct bma423_any_no_mot_config any_no_mot =
{
    .axes_en    = BMA423_EN_ALL_AXIS,
    .threshold  = 10,
    .duration   = 4,
};

static struct bma4_int_pin_config int_pin_conf =
{
    .output_en  = BMA4_OUTPUT_ENABLE,
    .edge_ctrl  = BMA4_EDGE_TRIGGER,
    .input_en   = BMA4_INPUT_DISABLE,
    .od         = BMA4_INPUT_DISABLE,
    .lvl        = BMA4_ACTIVE_LOW,
};

static nrf_drv_gpiote_in_config_t input_config =
{
    .pull               = NRF_GPIO_PIN_NOPULL,
    .is_watcher         = false,
    .hi_accuracy        = true,
    .skip_gpio_setup    = false,
    .pull               = NRF_GPIO_PIN_PULLUP,
    .sense              = NRF_GPIOTE_POLARITY_HITOLO,
};


static bma421_config_t bma421_config =
{
    .addr               = BMA4_I2C_ADDR_PRIMARY,
    .step_counter_en    = true,
    .interrupt_en       = true,
    .stepCounterIntWaterMark = 100U
};

static bma_data_t bma_data =
{
    .state              = BMA_STATE_INITIALIZATION,
    .interrupt_source   = BMA_INT_WRIST_WEAR,
    .interrupt_chg_req  = false,
    .step_count         = 0
};


// Private Functions
static void bma_private_delay(uint32_t period_us, void * intf_ptr)
{
    UNUSED_PARAMETER(intf_ptr);
    if(period_us >= 1000)
    {
        vTaskDelay(pdMS_TO_TICKS(period_us / 1000));
    }
    else
    {
        for(uint32_t i = 0; i < (period_us * 64); i++);
    }
}


// Public Functions
bool bma_init(void)
{
    uint16_t result;

    if (bma_data.state == BMA_STATE_LOW_POWER)
    {
        bma_data.state = BMA_STATE_WAKEUP;
    }
    else
    {
        bma_data.state = BMA_STATE_INITIALIZATION;
    }

//    ret = bma4_set_aux_if_mode(1, &bma);
    bma.intf_ptr = &bma421_config.addr;

    result = bma423_init(&bma);
    result = bma423_write_config_file(&bma);

    // uint16_t id;
    // bma423_get_config_id(&id, &bma);

    result |= bma4_set_accel_enable(1, &bma);

    result |= bma4_set_accel_config(&accel_conf, &bma);
    result |= bma423_set_any_mot_config(&any_no_mot, &bma);
    result |= bma423_set_no_mot_config(&any_no_mot, &bma);

    // enable features
    if (bma421_config.step_counter_en)
    {
        // bma423_step_detector_enable(BMA4_ENABLE, &bma);
        result |= bma423_feature_enable(BMA423_STEP_CNTR, 1, &bma);
        result |= bma423_step_counter_set_watermark((uint16_t)(bma421_config.stepCounterIntWaterMark / STEP_COUNTER_WATERMARK_RESOLUTION), &bma);
    }

    result |= bma423_feature_enable(BMA423_WRIST_WEAR, 1, &bma);

    // interrupts
    if (bma421_config.interrupt_en)
    {
        result |= bma4_set_int_pin_config(&int_pin_conf, BMA4_INTR1_MAP, &bma);
        result |= bma_setInterruptSource(bma_data.interrupt_source);
    }

    vTaskDelay(pdMS_TO_TICKS(150));
    bma_data.state = BMA_STATE_ACTIVE;
    return (result == 0);
}

void init_bmaGpioInterrupt(void * irq_pfn)
{
    ret_code_t err_code;
    if(nrf_drv_gpiote_is_init() == false)
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    // BMA421 Interrupt
    err_code = nrf_drv_gpiote_in_init(BMA421_INT_PIN, &input_config, irq_pfn);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BMA421_INT_PIN, true);
}

BMA4_INTF_RET_TYPE twi_bus_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len, void *intf_ptr)
{
    UNUSED_PARAMETER(intf_ptr);
    twi_rx(BMA4_I2C_ADDR_PRIMARY, reg_addr, read_data, len);
    return BMA4_INTF_RET_SUCCESS;
}

BMA4_INTF_RET_TYPE twi_bus_write(uint8_t reg_addr, const uint8_t * write_data, uint32_t len, void *intf_ptr)
{
    // TODO: update to contain all data in 1 array
    uint8_t data[len + 1];
    data[0] = reg_addr;
    memcpy(data+1, write_data, len);
    twi_tx(BMA4_I2C_ADDR_PRIMARY, data, len);
    return BMA4_INTF_RET_SUCCESS;
}

bool bma_getDeviceID(void)
{
    uint8_t rx_data = 0;
    twi_reg_read(BMA4_I2C_ADDR_PRIMARY, BMA4_CHIP_ID_ADDR, &rx_data);

    bool ret = false;
    if(rx_data == BMA423_CHIP_ID)
    {
        ret = true;
    }
    return ret;
}

bool bma_setInterruptSource(BMA_InterruptSource_E int_source)
{
    bool ret = false;
    ret |= bma423_map_interrupt(BMA4_INTR1_MAP, int_source, BMA4_ENABLE, &bma);
    return ret;
}

bool bma_getInterruptStatus(void)
{
    bool ret = false;
    if(nrf_gpio_pin_read(BMA421_INT_PIN))
    {
        ret = true;
    }
    return ret;
}

void bma_updateStepCount(void)
{
    bma423_step_counter_output(&bma_data.step_count, &bma);
}

bool bma_isActive(void)
{
    return (bma_data.state == BMA_STATE_ACTIVE);
}

uint32_t bma_getStepCount(void)
{
    return bma_data.step_count;
}

bool bma_goToLowPower(void)
{
    bma_data.state = BMA_STATE_LOW_POWER;
    return true;
}
