//
// Created by jack on 11/29/20.
//

#include "rtc_driver.h"
#include "nrf.h"


//const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2);
//
//void rtc_config(void * handler)
//{
//    uint32_t err_code;
//
//    //Initialize RTC instance
//    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
//    config.prescaler = 4095;
//    err_code = nrf_drv_rtc_init(&rtc, &config, handler);
//    APP_ERROR_CHECK(err_code);
//
//    //Enable tick event & interrupt
//    nrf_drv_rtc_tick_enable(&rtc,true);
//
//    //Set compare channel to trigger interrupt after RTC_TIME_SYNC_SEC seconds
//    err_code = nrf_drv_rtc_cc_set(&rtc, 0, RTC_TIME_SYNC_SEC * 8,true);
//    APP_ERROR_CHECK(err_code);
//
//    //Power on RTC instance
//    nrf_drv_rtc_enable(&rtc);
//}