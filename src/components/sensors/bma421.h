//
// Created by jack on 8/22/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_BMA421_H
#define BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_BMA421_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  SENSORS_INITIALIZATION,
  SENSORS_ACTIVE,
  SENSORS_LOW_POWER,
  SENSORS_WAKEUP
} eSensorsState;

bool init_bma(void);
void update_step_count(void);
uint32_t get_step_count(void);
void delay(uint32_t period_us, void * intf_ptr);
bool sleep_bma(void);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_BMA421_H
