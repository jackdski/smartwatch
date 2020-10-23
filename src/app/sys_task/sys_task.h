//
// Created by jack on 7/19/20.
//

#ifndef JDSMARTWATCHPROJECT_SRC_COMPONENTS_SYS_TASK_H
#define JDSMARTWATCHPROJECT_SRC_COMPONENTS_SYS_TASK_H

#include "resources.h"
#include "haptic.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "timers.h"

/* ENUMS */
//typedef enum {
//  NO_MSG,
//  INITIALIZATION,
//  // Sleep/Low Power
//  GO_TO_SLEEP,
//  // Display
//  DISPLAY_INACTIVITY_TIMER_EXPIRED,
//  DISPLAY_ACTIVE,
//  // Button
//  BUTTON_PRESSED
//} eMessage;

typedef enum {
  BATTERY_DISCHARGE           = (1 << 0),
  BATTERY_CHARGING            = (1 << 1),
  BATTERY_CHARGING_STARTED    = (1 << 2),
  BATTERY_CHARGING_STOPPED    = (1 << 3),
  BATTERY_CHARGING_COMPLETE   = (1 << 4),
  BATTERY_LOW_POWER           = (1 << 5)
} eBatteryEvent;

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
  bool      charging;
  bool      prev_charging;
  bool      display_active;
  uint8_t   soc;
  uint16_t  battery_voltage;
  uint32_t  step_count;
} System_t;

void sys_task(void * arg);
void system_button_handler(void);
void haptic_timer_callback(TimerHandle_t timerx);
//const char * get_systask_name(eMessage msg);

#endif //JDSMARTWATCHPROJECT_SRC_COMPONENTS_SYS_TASK_H
