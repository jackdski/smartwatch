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


static HRS_t hrs_data;

bool app_heart_rate_init(void)
{
    bool ret = true;

    hrs_data.channel = 0;
    hrs_data.heart_rate = 0;
    hrs_data.upper_limit = HEART_RATE_UPPER_LIMIT;
    hrs_data.lower_limit = HEART_RATE_LOWER_LIMIT;

    if (HRS3300_get_device_id())
    {
        HRS3300_update();
    }
    else
    {
        ret = false;
    }
    return ret;
}

void app_heart_rate(void)
{
    hrs_data.channel ^= 1;  // flip channels
    HRS3300_enable(true);
    hrs_data.heart_rate = HRS3300_get_sample(hrs_data.channel);
    HRS3300_enable(false);

    if (hrs_data.heart_rate >= HEART_RATE_UPPER_LIMIT)
    {
        set_alert(ALERT_HEART_RATE_HIGH);
    }
    else if ((hrs_data.heart_rate > 0U) &&
             (hrs_data.heart_rate <= HEART_RATE_LOWER_LIMIT))
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
    return hrs_data.heart_rate;
}

