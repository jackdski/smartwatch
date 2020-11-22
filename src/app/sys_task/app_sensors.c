//
// Created by jack on 11/8/20.
//

#include "app_sensors.h"
#include "bma421.h"
#include "HRS3300.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"


// RTOS Variables
extern TaskHandle_t thDisplay;
extern TaskHandle_t thSysTask;

static IMU_t imu = {
    .interrupt_active = false,
    .interrupt_source = 0x08,
    .steps = 0
};

static uint32_t heart_rate = 0;

// Apps
void run_accel_app(void)
{
    update_step_count();
    imu.steps = get_step_count();
    NRF_LOG_INFO("Steps: %d", imu.steps);

    // alternate between wrist wear int. and single tap int.
    imu.interrupt_source ^= 0x08;
    bma423_set_interrupt_source(imu.interrupt_source);
    if(bma423_get_interrupt_status())
    {
        NRF_LOG_INFO("ACCEL: Int. Detected");
        vTaskResume(thDisplay);
    }
}

void run_heart_rate_app(void)
{
    static bool channel = 1;
    channel ^= 1;
    HRS3300_enable();
    heart_rate = HRS3300_get_sample(channel);
    NRF_LOG_INFO("Heart rate: %d", heart_rate);
}

void run_sensor_update_display(void)
{
//    SensorData_t data;
//    data.steps = imu.steps;
//    data.heart_rate = heart_rate;

    // TODO: send to display over queue if display task is active
}