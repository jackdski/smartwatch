//
// Created by jack on 6/13/20.
//

#include "SGM40561.h"
#include "nrf_saadc.h"
#include "nrf_drv_saadc.h"

#include "app_config.h"

#define SAADC_CALIBRATION_INTERVAL          5

/**
 * TODO: run SAADC Calibration Routine on wakeup and every 5 sets of readings
 */

static nrf_saadc_value_t battery_adc_result[BATTERY_SAADC_BUFFER_SIZE];
static uint32_t adc_evt_counter = 0;

nrf_saadc_channel_config_t battery_saadc_channel_config = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain = NRF_SAADC_GAIN1_5,
    .reference = NRF_SAADC_REFERENCE_INTERNAL,
    .acq_time = NRF_SAADC_ACQTIME_3US,
    .mode = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst = NRF_SAADC_BURST_DISABLED,
    .pin_p = NRF_SAADC_INPUT_AIN7,
    .pin_n = NRF_SAADC_INPUT_DISABLED
};

//const nrf_drv_saadc_config_t battery_saadc_config = {
//    .low_power_mode = true,
//    .resolution = NRF_SAADC_RESOLUTION_12BIT,
//    .oversample = NRF_SAADC_OVERSAMPLE_128X,
//    .interrupt_priority = APP_IRQ_PRIORITY_LOW
////    .buffer = battery_adc_result,
////    .buffer_size = BATTERY_SAADC_BUFFER_SIZE,
//};

// Private Functions

static void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    ret_code_t  err_code;

    if(p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        if((adc_evt_counter % SAADC_CALIBRATION_INTERVAL) == 0)
        {
            nrf_drv_saadc_abort();
            while(nrf_drv_saadc_calibrate_offset() != NRF_SUCCESS);
            adc_evt_counter = 0;
        }

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, BATTERY_SAADC_BUFFER_SIZE);
        APP_ERROR_CHECK(err_code);
        adc_evt_counter++;
    }
    else if(p_event->type == NRF_DRV_SAADC_EVT_CALIBRATEDONE)
    {
        err_code = nrf_drv_saadc_buffer_convert(battery_adc_result, BATTERY_SAADC_BUFFER_SIZE);
        APP_ERROR_CHECK(err_code);
    }
}

static void battery_monitor_sleep(void)
{
    while(nrf_saadc_busy_check());
    nrf_saadc_disable();
}

static void battery_monitor_wakeup(void)
{
    nrf_saadc_enable();
}


// Public Functions

void config_SGM40561(void)
{
    ret_code_t err_code;

    // Config Pin Interrupts
    // TODO: use interrupt/gpiote event to detect when power presence state changes
    // TODO: use interrupt/gpiote event to detect when charging state changes

    // SAADC to read BATTERY_VOLTAGE_PIN
    nrfx_saadc_config_t adcConfig = NRFX_SAADC_DEFAULT_CONFIG;
    err_code = nrf_drv_saadc_init(&adcConfig, saadc_callback);
    nrfx_saadc_init(&adcConfig, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_saadc_channel_init(BATTERY_VOLTAGE_SAADC_CH,
                           &battery_saadc_channel_config);

//    err_code = nrf_drv_saadc_buffer_convert(battery_adc_result, BATTERY_SAADC_BUFFER_SIZE);
//    APP_ERROR_CHECK(err_code);
}

bool SGM40561_is_power_present(void)
{
    return (nrf_gpio_pin_read(POWER_PRESENCE_PIN) == 0) ? true : false;
}

/**
 * Can also use interrupt/gpiote event to detect when charging state changes
 * @return bool: true if battery is charging
 */
bool SGM40561_is_charging(void)
{
    return (nrf_gpio_pin_read(CHARGE_INDICATION_PIN) == 0) ? true : false;
}

/*
 * Note:
 *   A 1M-Ohm voltage divider is used to half the battery voltage
 *   https://wiki.pine64.org/index.php/PineTime#Battery_measurement
 */
uint16_t SGM40561_sample_battery_voltage(void)
{
//    uint8_t i = 0;
//    uint32_t avg = 0;
//    for(i = 0; i < BATTERY_SAADC_BUFFER_SIZE; i++) {
//        avg += battery_adc_result[i];
//    }
//    avg = (avg / BATTERY_SAADC_BUFFER_SIZE);
//    return ((avg * 2000) / 1241);
    nrf_saadc_value_t value = 0;
    nrfx_saadc_sample_convert(BATTERY_VOLTAGE_SAADC_CH, &value);
    return (value * 2) / (1024 / 3);
}

uint16_t SGM40562_sample_battery_mv(void)
{
    uint16_t mv;
    battery_monitor_wakeup();
    mv = SGM40561_sample_battery_voltage();
    battery_monitor_sleep();
    return mv;
}
