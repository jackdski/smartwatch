//
// Created by jack on 5/11/20.
//

#include "drv2605l.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

APP_PWM_INSTANCE(PWM1, 1); // Setup a PWM instance with TIMER 1

// initialization
//uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_default_config[] = { DRV_MODE_REG, 0x80 };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_mode[]         = {DRV_MODE_REG, DRV_MODE_AUTO_CALIBRATION };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_feedback_lra[] = {DRV_FEEDBACK_REG, 0xB6 };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_feedback_erm[] = {DRV_FEEDBACK_REG, 0x36 };

uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_rv_lra[]       = {DRV_RATEDVOLT_REG, DRV_RATEDVOLT_LRA_VAL };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_rv_erm[]       = {DRV_RATEDVOLT_REG, DRV_RATEDVOLT_ERM_VAL };

uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_od_clamp_lra[] = {DRV_OVERDRIVECLAMP_REG, DRV_OD_CLAMP_LRA_VAL };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_od_clamp_erm[] = {DRV_OVERDRIVECLAMP_REG, DRV_OD_CLAMP_ERM_VAL };

uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_init_ctl_two[]      = {DRV_CONTROL2_REG, 0x6F };

// general use
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_go_on_reg[]     = { DRV_GO_REG, 0x01 };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_go_stop_reg[]   = { DRV_GO_REG, 0x00 };

uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_mode_pwm_active[]       = {DRV_MODE_REG, (DRV_MODE_PWM_INPUT | DRV_MODE_STANDBY_BIT) };
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_mode_standby[]          = {DRV_MODE_REG, (DRV_MODE_PWM_INPUT)};

// read-able registers
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_status_reg_addr = DRV_STATUS_REG;
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_mode_reg_addr   = DRV_MODE_REG;
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_go_reg_addr     = DRV_GO_REG;
uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_vbatt_reg_addr  = DRV_VBATMONITOR_REG;

// default configuration - LRA
nrf_twi_mngr_transfer_t const drv2605l_init_transfers_lra[DRV2605L_INIT_TRANSFER_COUNT] = {
//        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_default_config, sizeof(drv2605_default_config), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_mode, sizeof(drv2605_init_mode), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_feedback_lra, sizeof(drv2605_init_feedback_lra), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_rv_lra, sizeof(drv2605_init_rv_lra), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_od_clamp_lra, sizeof(drv2605_init_od_clamp_lra), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_ctl_two, sizeof(drv2605_init_ctl_two), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605l_go_on_reg, sizeof(drv2605l_go_on_reg), 0),
};

// default configuration - ERM
nrf_twi_mngr_transfer_t const drv2605l_init_transfers_erm[DRV2605L_INIT_TRANSFER_COUNT] = {
//        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_default_config, sizeof(drv2605_default_config), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_mode, sizeof(drv2605_init_mode), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_feedback_erm, sizeof(drv2605_init_feedback_erm), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_rv_erm, sizeof(drv2605_init_rv_erm), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_od_clamp_erm, sizeof(drv2605_init_od_clamp_erm), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_init_ctl_two, sizeof(drv2605_init_ctl_two), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605l_go_on_reg, sizeof(drv2605l_go_on_reg), 0),
};

nrf_twi_mngr_transfer_t const drv2605l_set_active_transfers[2] = {
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605l_go_on_reg, sizeof(drv2605l_go_on_reg), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_mode_pwm_active, sizeof(drv2605_mode_pwm_active), 0),
};

nrf_twi_mngr_transfer_t const drv2605l_set_standby_transfers[2] = {
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605l_go_stop_reg, sizeof(drv2605l_go_stop_reg), 0),
        NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, drv2605_mode_standby, sizeof(drv2605_mode_standby), 0)
};


static bool vbatt_data_available = false;

// Functions
ret_code_t drv2605l_config(DRV2605L_t * p_inst) {
    if(p_inst == NULL) {
        return NRF_ERROR_NULL;
    }

    p_inst->en_pin      = DRV_EN_PIN;
    p_inst->state       = DRV_Get_Voltage; // DRV_Init;
    p_inst->period_us   = DRV_PERIOD_US;
    p_inst->motor_type  = LRA_MODE;
    p_inst->loop_gain   = LOOP_GAIN_MEDIUM;
    p_inst->enable      = true;
    p_inst->go_bit      = false;
    p_inst->vbatt_voltage = -1.0;

    nrf_gpio_pin_write(p_inst->en_pin, 0);
    drv2605l_pwm_config(p_inst);

    return NRF_SUCCESS;
}

void drv2605l_pwm_config(DRV2605L_t * p_inst) {
    ret_code_t  err_code;

    app_pwm_config_t pwm1_cfg = {
            .pins               = {DRV_PWM_PIN, APP_PWM_NOPIN},
            .pin_polarity       = {APP_PWM_POLARITY_ACTIVE_HIGH, APP_PWM_POLARITY_ACTIVE_LOW},
            .num_of_channels    = 1,
            .period_us          = p_inst->period_us
    };

    err_code = app_pwm_init(&PWM1, &pwm1_cfg, NULL);
    APP_ERROR_CHECK(err_code);

    drv2605l_set_pwm_duty_cycle(0);
    app_pwm_enable(&PWM1);
}


void drv2605l_general_callback(ret_code_t result, void * p_user_data) {
    UNUSED_PARAMETER(p_user_data);
    if (result != NRF_SUCCESS) {
        NRF_LOG_WARNING("DRV2605L - error: %d", (int)result);
        return;
    }
}

void drv2605l_vbatt_callback(ret_code_t result, void * p_user_data) {
    UNUSED_PARAMETER(p_user_data);
    if (result != NRF_SUCCESS) {
        NRF_LOG_WARNING("DRV2605L VBatt - error: %d", (int)result);
        return;
    }
    vbatt_data_available = true;
}


void set_drv_vbatt(DRV2605L_t * p_inst) {
    p_inst->vbatt_voltage = p_inst->vbatt_reading * 5.6 / 255;
    NRF_LOG_INFO("DRV2605L Vbatt: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(p_inst->vbatt_voltage));
    vbatt_data_available = false;
}

bool get_vbatt_data_available(void) {
    return vbatt_data_available;
}

bool set_vbatt_data_available(bool status) {
    vbatt_data_available = status;
    return vbatt_data_available;
}

ret_code_t drv2605l_set_pwm_duty_cycle(uint8_t duty_cycle) {
    ret_code_t err_code;
    err_code = app_pwm_channel_duty_set(&PWM1, 0, duty_cycle);
    return err_code;
}