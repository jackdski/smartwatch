#ifndef DISPLAY_ALERTS_H_
#define DISPLAY_ALERTS_H_

#include "display.h"

#include <stdbool.h>

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

bool display_check_for_alerts(void);
void set_return_screen(DisplayScreens_E screen);
void display_alert(void);
void display_alert_callback(TimerHandle_t xTimer);


#endif /* DISPLAY_ALERTS_H_ */