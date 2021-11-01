/* 
 *  app_heart_rate.c
 */

#include "app_heart_rate.h"
#include "HRS3300.h"
#include "alerts.h"

/*
 *  Defines
 */
#define HEART_RATE_UPPER_LIMIT  140U
#define HEART_RATE_LOWER_LIMIT  35U


HRS_t hrs;

bool app_heart_rate_init(void)
{
    hrs.channel = 0;
    hrs.heart_rate = 0;
    hrs.upper_limit = HEART_RATE_UPPER_LIMIT;
    hrs.lower_limit = HEART_RATE_LOWER_LIMIT;

    if(HRS3300_get_device_id())
    {
        HRS3300_update();
        return true;
    }
    return false;
}

void app_heart_rate(void)
{
    hrs.channel ^= 1;  // flip channels
    HRS3300_enable(true);
    hrs.heart_rate = HRS3300_get_sample(hrs.channel);
    HRS3300_enable(false);

    if(hrs.heart_rate >= HEART_RATE_UPPER_LIMIT)
    {
        set_alert(ALERT_HEART_RATE_HIGH);
    }
    else if((hrs.heart_rate > 0U) && (hrs.heart_rate <= HEART_RATE_LOWER_LIMIT))
    {
        set_alert(ALERT_HEART_RATE_LOW);
    }
    else
    {
        // no heart rate alerts
    }
}

uint16_t app_heart_rate_getHeartRate(void)
{
    return hrs.heart_rate;
}

