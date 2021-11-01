/* 
 *  app_heart_rate.h
 */

#ifndef APP_HEART_RATE_H_
#define APP_HEART_RATE_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t      heart_rate;
    uint16_t      upper_limit;
    uint16_t      lower_limit;
    bool          channel:1;
} HRS_t;


bool app_heart_rate_init(void);
void app_heart_rate(void);
uint16_t app_heart_rate_getHeartRate(void);

#endif // APP_HEART_RATE