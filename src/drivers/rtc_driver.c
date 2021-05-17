//
// Created by jack on 11/29/20.
//

#include "rtc_driver.h"
#include "nrf.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

#include <stdint.h>

#define RTC_SECOND_TICK_CHANNEL     0
#define RTC_ALARM_CHANNEL           1


const nrf_drv_rtc_t rtc = {
    .p_reg            = NRF_RTC2,
    .irq              = RTC2_IRQn,
    .instance_id      = NRFX_RTC0_INST_IDX,
    .cc_channel_count = 4
};

void rtc_config(void * handler)
{
   uint32_t err_code;

   //Initialize RTC instance
   nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
   config.prescaler = 4095;
   err_code = nrf_drv_rtc_init(&rtc, &config, handler);
   APP_ERROR_CHECK(err_code);

   //Enable tick event & interrupt
   nrf_drv_rtc_tick_enable(&rtc,true);

   //Set compare channel to trigger interrupt after RTC_TIME_SYNC_SEC seconds
   err_code = nrf_drv_rtc_cc_set(&rtc, RTC_SECOND_TICK_CHANNEL, RTC_TIME_SYNC_SEC, true);
   APP_ERROR_CHECK(err_code);

   //enabel RTC instance
   nrf_drv_rtc_enable(&rtc);
}

void rtc_set_alarm(uint32_t seconds)
{
    uint32_t err_code;
    err_code = nrf_drv_rtc_cc_set(&rtc, RTC_ALARM_CHANNEL, RTC_TIME_SYNC_SEC, true);
    APP_ERROR_CHECK(err_code);
}