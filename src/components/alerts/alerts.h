#ifndef ALERTS_H_
#define ALERTS_H_

typedef enum
{
    NO_ACTIVE_ALERT,
    ALERT_HEART_RATE_HIGH,
    ALERT_HEART_RATE_LOW,
    ALERT_BATTERY_LOW,
    ALERT_BATTERY_CHARGED
} Alerts_E;

void set_alert(Alerts_E new_alert);


#endif /* ALERTS_H_ */