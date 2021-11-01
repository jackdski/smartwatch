//
// Created by jack on 7/19/20.
//

#ifndef SYS_TASK_H_
#define SYS_TASK_H_

#include "haptic.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "timers.h"

/* ENUMS */

typedef enum
{
	SYSTEM_INITIALIZATION,
	SYSTEM_RUN,
	SYSTEM_SLEEP
} systemStatus_E;

typedef enum
{
	DISPLAY_NO_MSG,
	DISPLAY_BUTTON_PRESSED,
	DISPLAY_CHARGING_STARTED,
	DISPLAY_CHARGING_ENDED
} displayMessage_E;


/* STRUCTS */

typedef struct
{
	bool      goToSleep;
	bool      display_active;
	systemStatus_E state;
	uint32_t  free_heap;
} System_t;

void sys_task(void * arg);
void haptic_timer_callback(TimerHandle_t timerx);
//const char * get_systask_name(eMessage msg);

#endif //SYS_TASK_H_
