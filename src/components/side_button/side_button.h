#ifndef SIDE_BUTTON_H_
#define SIDE_BUTTON_H_

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

void init_side_button(void * irq_pfn);
void button_irq_callback(void);
void button_debounce_callback(TimerHandle_t xTimer);    

#endif /* SIDE_BUTTON_H_ */