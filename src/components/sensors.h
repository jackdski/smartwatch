//
// Created by jack on 5/11/20.
//

#ifndef JDSMARTWATCHPROJECT_SENSORS_H
#define JDSMARTWATCHPROJECT_SENSORS_H

#include "nrf_twi_mngr.h"
#include "nrf_twi_sensor.h"
#include "nrf_twi.h"
#include "nrf_gpio.h"

#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "drivers/drv2605l.h"

#define TWI_INSTANCE_ID             0
#define MAX_PENDING_TRANSACTIONS    10


void twi_init(void);
void haptic_buzz_callback(TimerHandle_t xTimer);
void haptic_task(void * arg);
ret_code_t haptic_init(DRV2605L_t * p_inst);
void haptic_request_vbatt(void);


#endif //JDSMARTWATCHPROJECT_SENSORS_H
