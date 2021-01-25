//
// Created by jack on 7/19/20.
//

#ifndef JDSMARTWATCHPROJECT_SRC_COMPONENTS_SYS_TASK_H
#define JDSMARTWATCHPROJECT_SRC_COMPONENTS_SYS_TASK_H

#include "haptic.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "timers.h"

/* ENUMS */
typedef enum {
  NO_MSG,
  INITIALIZATION,
  // Sleep/Low Power
  GO_TO_SLEEP,
  // Display
  DISPLAY_INACTIVITY_TIMER_EXPIRED,
  DISPLAY_ACTIVE,
  // Button
  BUTTON_PRESSED
} eMessage;


typedef enum {
  DISPLAY_NO_MSG,
  DISPLAY_BUTTON_PRESSED,
  DISPLAY_CHARGING_STARTED,
  DISPLAY_CHARGING_ENDED
} eDisplayMessage;


/* STRUCTS */

typedef struct {
  bool      initialized;
  bool      sleep;
  bool      wakeup;
  bool      display_active;
} System_t;

void sys_task(void * arg);
void system_button_handler(void);
void haptic_timer_callback(TimerHandle_t timerx);
//const char * get_systask_name(eMessage msg);

#endif //JDSMARTWATCHPROJECT_SRC_COMPONENTS_SYS_TASK_H
