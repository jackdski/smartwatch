//
// Created by jack on 5/11/20.
//

#include "sensors.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

// Task Handles
extern TaskHandle_t thHaptic;

// Private Variables
static DRV2605L_t drv_settings;
static TimerHandle_t haptic_buzz_timer;
static TimerHandle_t haptic_status_check;

NRF_TWI_MNGR_DEF(m_nrf_twi_mngr, 10, TWI_INSTANCE_ID);

// TWI
void twi_init(void) {
    uint32_t err_code;

    nrf_drv_twi_config_t const config = {
            .scl                = ARDUINO_SCL_PIN,
            .sda                = ARDUINO_SDA_PIN,
            .frequency          = NRF_DRV_TWI_FREQ_100K,
            .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
            .clear_bus_init     = false
    };

    err_code = nrf_twi_mngr_init(&m_nrf_twi_mngr, &config);
    APP_ERROR_CHECK(err_code);
}

// timers
void haptic_buzz_callback(TimerHandle_t xTimer) {
    drv_settings.state = DRV_Pulse;
}

void haptic_status_check_callback(TimerHandle_t xTimer) {
    drv_settings.state = DRV_Status_Check;
}

// Tasks

void haptic_task(void * arg) {
    UNUSED_PARAMETER(arg);
    ret_code_t  err;
    drv2605l_config(&drv_settings);

    haptic_init(&drv_settings);

    haptic_buzz_timer = xTimerCreate("Buzz", pdMS_TO_TICKS(10000), pdTRUE, ( void * ) 0, haptic_buzz_callback);
    haptic_status_check = xTimerCreate("StatusCheck", pdMS_TO_TICKS(60000), pdTRUE, (void *)0, haptic_status_check_callback);

    while(1) {
        switch (drv_settings.state) {
            case DRV_Get_Voltage: {
                if(drv_settings.initialized) {
                    haptic_request_vbatt();
                    set_drv_vbatt(&drv_settings);
                    xTimerStart(haptic_buzz_timer, 10);
                }
                drv_settings.state = DRV_Idle;
                break;
            }
            case DRV_Pulse: {
                drv_settings.enable = true;
                nrf_gpio_pin_write(drv_settings.en_pin, 1);
                APP_ERROR_CHECK(nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, drv2605l_set_active_transfers, 2, NULL));

                drv2605l_set_pwm_duty_cycle(40);                // start PWM signal
                vTaskDelay(pdMS_TO_TICKS(500));    // run for 1s
                drv2605l_set_pwm_duty_cycle(0);                 // stop PWM signal

                // set standby mode and set GO bit low
                APP_ERROR_CHECK(nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, drv2605l_set_standby_transfers, 2, NULL));

                drv_settings.enable = false;
                nrf_gpio_pin_write(drv_settings.en_pin, 0);

                drv_settings.state = DRV_Get_Voltage;
                break;
            }
            case DRV_Status_Check: {
                ret_code_t err;
                static uint8_t status_reg;
                static nrf_twi_mngr_transfer_t const transfers[2] = {
                        DRV2605L_READ(&drv2605l_status_reg_addr, &status_reg, 1)
                };
                err = nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, transfers, 2, NULL);

                if (err == NRF_SUCCESS) {
                    // check for errors
                    if (status_reg & DRV_Over_Curr_Temp) {
                        drv_settings.errors = DRV_Over_Curr_Temp;
                        NRF_LOG_INFO("HAPTIC: Error - Over Temp");
                        NRF_LOG_INFO("HAPTIC: Error - Over Current");
                    } else if (status_reg & DRV_Over_Temperature) {
                        drv_settings.errors = DRV_Over_Temperature;
                        NRF_LOG_INFO("HAPTIC: Error - Over Temp");
                    } else if (status_reg & DRV_Over_Current) {
                        drv_settings.errors = DRV_Over_Current;
                        NRF_LOG_INFO("HAPTIC: Error - Over Current");
                    } else {
                        drv_settings.errors = DRV_No_Errors;
                        NRF_LOG_INFO("HAPTIC: No status errors");
                    }
                    // change state to Idle
                    drv_settings.state = DRV_Idle;
                }
                else {
                    NRF_LOG_DEBUG("HAPTIC: Could not get status");
                }
            }
            case DRV_Init: {
                if(!drv_settings.initialized) {
                    haptic_init(&drv_settings);
                }
                else {
                    drv_settings.state = DRV_Get_Voltage;
                }
                break;
            }
            case DRV_Idle:  {
                nrf_gpio_pin_write(drv_settings.en_pin, false);
                // TODO: vTaskSuspend(thHaptic);
                break;
            }
            default: {
                NRF_LOG_DEBUG("HAPTIC: Unknown State");
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Haptic Functions
ret_code_t haptic_init(DRV2605L_t * p_inst) {
    ret_code_t err = NRF_SUCCESS;
    nrf_gpio_cfg_output(p_inst->en_pin);
    nrf_gpio_pin_write(p_inst->en_pin, true);

    if (p_inst->motor_type == LRA_MODE) {
        err = nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, drv2605l_init_transfers_lra,
                                             DRV2605L_INIT_TRANSFER_COUNT, NULL);
    }
    else if (p_inst->motor_type == ERM_MODE) {
        err = nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, drv2605l_init_transfers_erm,
                                             DRV2605L_INIT_TRANSFER_COUNT, NULL);
    }

    // see if device was NACK'd
    if(err != NRF_SUCCESS) {
        p_inst->initialized = false;
        return NRF_ERROR_BUSY;
    }

    // read DIAG_RESULT until auto-calibration is complete
    // read STATUS reg, bit 3
    uint8_t static status_reg, attempts = 10;
    while (attempts > 0 && !(status_reg & DRV_STATUS_DIAG_BIT)) {
        static nrf_twi_mngr_transfer_t const transfers[2] = {
                DRV2605L_READ(&drv2605l_status_reg_addr, &status_reg, 1)
        };
        err = nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, transfers, 2, NULL);

        // see if device was NACK'd
        if(err != NRF_SUCCESS) {
            p_inst->initialized = false;
            return NRF_ERROR_BUSY;
        }
        attempts--;
    }

    if(status_reg & DRV_STATUS_DIAG_BIT) {
        NRF_LOG_INFO("HAPTIC: Error Occurred during auto-calibration");
    }

    p_inst->state = DRV_Get_Voltage;
    p_inst->initialized = true;
    return NRF_SUCCESS;
}

void haptic_request_vbatt(void) {
    static nrf_twi_mngr_transfer_t vbatt_transfer[2] = {
            DRV2605L_READ(&drv2605l_vbatt_reg_addr, &drv_settings.vbatt.vbatt_reading, 1)
    };
    APP_ERROR_CHECK(nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, vbatt_transfer, 2, NULL));
}