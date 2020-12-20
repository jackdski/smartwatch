//
// Created by jack on 11/29/20.
//

#ifndef JD_SMARTWATCH_SRC_DRIVERS_RTC_DRIVER_H
#define JD_SMARTWATCH_SRC_DRIVERS_RTC_DRIVER_H

#include "nrf_rtc.h"
#include "nrf_drv_rtc.h"

//#define RTC_TIME_SYNC_SEC  (10UL)


void rtc_config(void * handler);

#endif //JD_SMARTWATCH_SRC_DRIVERS_RTC_DRIVER_H
