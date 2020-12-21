//
// Created by jack on 11/29/20.
//

#include "time.h"
#include "rtc_driver.h"
#include <string.h>

#include "nrf_drv_rtc.h"

#define SHORT_DAY_MONTHS        (FEBRUARY)
#define IS_LEAP_YEAR(YEAR)      ((YEAR % 4) == 0)

#define THIRTY_DAY_MONTHS       (APRIL | JUNE | SEPTEMBER | NOVEMBER)
#define THIRTY_ONE_DAY_MONTHS   (JANUARY | MARCH | MAY | JUNE | JULY | AUGUST | OCTOBER | DECEMBER)


static Time_t time = {
    .hour = 12,
    .minute = 10,
    .second = 0,
    .day_of_week = MONDAY,
    .day = 1,
    .month = NOVEMBER,
    .year = 2020
};

/** RTC HANDLER **/
//void rtc_handler(nrf_drv_rtc_int_type_t int_type)
//{
//    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
//    {
//        increment_time_second(RTC_TIME_SYNC_SEC);
//    }
//}

/** TIME **/
void update_time(Time_t new_time)
{
    memcpy(&time, &new_time, sizeof(Time_t));
}

void update_time_month(eMonth month)
{
    time.month = month;
}

void update_time_weekday(eWeekday weekday)
{
    time.day_of_week = weekday;
}

void increment_time_second(uint8_t sec)
{
    time.second += sec;
    if(time.second >= 60)
    {
        time.minute++;
        time.second = 0;
    }
    if(time.minute >= 60)
    {
        time.hour++;
        time.minute = 0;
    }
    if(time.hour >= 24)
    {
        increment_time_day();
    }
}

void increment_time_day(void)
{
    time.day++;
    time.hour = 0;

    bool increment_month = false;
    if((time.month & THIRTY_DAY_MONTHS) && (time.day > 30))
    {
        increment_month = true;
    }
    else if((time.month & THIRTY_ONE_DAY_MONTHS) && (time.day > 31))
    {
        increment_month = true;
    }
    else if((time.month & IS_LEAP_YEAR(time.year)) && (time.day > 29))
    {
        increment_month = true;
    }
    else if((time.month & !IS_LEAP_YEAR(time.year)) && (time.day > 28))
    {
        increment_month = true;
    }
    else
    {
        // nothing - month does not need to be incremented
    }

    if(increment_month == true)
    {
        increment_time_month(time.month);
    }
}

void increment_time_month(eMonth month)
{
    switch(month) {
    case JANUARY: time.month = FEBRUARY; break;
    case FEBRUARY: time.month = MARCH; break;
    case MARCH: time.month = APRIL; break;
    case APRIL: time.month = MAY; break;
    case MAY: time.month = JUNE; break;
    case JUNE: time.month = JULY; break;
    case JULY: time.month = AUGUST; break;
    case AUGUST: time.month = SEPTEMBER; break;
    case SEPTEMBER: time.month = OCTOBER; break;
    case OCTOBER: time.month = NOVEMBER; break;
    case NOVEMBER: time.month = DECEMBER; break;
    case DECEMBER: time.month = JANUARY; break;
    }

    time.day = 1;
}

Time_t get_time(void)
{
    return time;
}

// ENUM TO STRINGS
char * get_month_str(void)
{
    switch(time.month) {
    case JANUARY: return "January";
    case FEBRUARY: return "February";
    case MARCH: return "March";
    case APRIL: return "April";
    case MAY: return "May";
    case JUNE: return "June";
    case JULY: return "July";
    case AUGUST: return "August";
    case SEPTEMBER: return "September";
    case OCTOBER: return "October";
    case NOVEMBER: return "November";
    case DECEMBER: return "December";
    }
    return "ERR";
//    return month_of_year[time.month];
}

char * get_weekday_str(void)
{
    switch(time.day_of_week){
    case MONDAY: return "Monday";
    case TUESDAY: return "Tuesday";
    case WEDNESDAY: return "Wednesday";
    case THURSDAY: return "Thursday";
    case FRIDAY: return "Friday";
    case SATURDAY: return "Saturday";
    case SUNDAY: return "Sunday";
    }
    return "ERR";
//    return day_of_week[time.day_of_week];
}

uint16_t get_year(void)
{
    return time.year;
}

uint8_t get_month(void)
{
    return time.month;
}

uint8_t get_day(void)
{
    return time.day;
}

uint8_t get_hour(void)
{
    return time.hour;
}

uint8_t get_minute(void)
{
    return time.minute;
}

uint8_t get_second(void)
{
    return time.second;
}

void format_date(char * ret_str)
{
    char * date_str = get_month_str();
    char * weekday_str = get_weekday_str();
    char str[30];
    sprintf(str, "%s %s %d, %d", weekday_str, date_str, get_day(), get_year());
    strcpy(ret_str, str);
}

void format_time_military(char * ret_str)
{
    char str[10];
    sprintf(str, "%d:%d", time.hour, time.minute);
}

void format_time_standard(char * ret_str)
{
    char str[10];
    uint8_t hour = time.hour;
    if(hour > 12)
    {
        hour -= 12;
        sprintf(str, "%d:%d%s", hour, time.minute, "pm");
    }
    else
    {
        sprintf(str, "%d:%d%s", hour, time.minute, "am");
    }
    strcpy(ret_str, str);
}
