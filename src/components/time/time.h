//
// Created by jack on 11/29/20.
//

#ifndef JD_SMARTWATCH_SRC_COMPONENTS_TIME_TIME_H
#define JD_SMARTWATCH_SRC_COMPONENTS_TIME_TIME_H

#include <stdint.h>
#include "rtc_driver.h"

typedef enum {
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY,
  SUNDAY
} eWeekday;

typedef enum {
  JANUARY,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
} eMonth;

typedef struct {
  uint8_t     second;
  uint8_t     minute;
  uint8_t     hour;
  eWeekday    day_of_week;
  uint8_t     day_of_month;
  uint8_t     day;
  uint8_t     month;
  uint16_t    year;
} Time_t;

void rtc_handler(nrf_drv_rtc_int_type_t int_type);
void update_time(Time_t new_time);
void update_time_month(eMonth month);
void increment_time_second(uint8_t sec);
void increment_time_day(void);
void increment_time_month(eMonth month);
Time_t get_time(void);

char * get_year_str(void);
char * get_month_str(void);
char * get_weekday_str(void);
char * get_day_str(void);

uint16_t get_year(void);
uint8_t get_month(void);
uint8_t get_day(void);
uint8_t get_hour(void);
uint8_t get_minute(void);
uint8_t get_second(void);

void format_date(char * ret_str);
void format_time_military(char * ret_str);
void format_time_standard(char * ret_str);

#endif //JD_SMARTWATCH_SRC_COMPONENTS_TIME_TIME_H
