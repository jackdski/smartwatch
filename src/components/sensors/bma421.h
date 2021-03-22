//
// Created by jack on 8/22/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_BMA421_H
#define BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_BMA421_H

#include <stdint.h>
#include <stdbool.h>

#include "bma423.h"

typedef enum {
  SENSORS_INITIALIZATION,
  SENSORS_ACTIVE,
  SENSORS_LOW_POWER,
  SENSORS_WAKEUP
} eSensorsState;

typedef enum {
  BMA_INT_SINGLE_TAP    = BMA423_SINGLE_TAP_INT,
  BMA_INT_STEP_CNTR     = BMA423_STEP_CNTR_INT,
  BMA_INT_ACTIVITY      = BMA423_ACTIVITY_INT,
  BMA_INT_WRIST_WEAR    = BMA423_WRIST_WEAR_INT,
  BMA_INT_DOUBLE_TAP    = BMA423_DOUBLE_TAP_INT,
  BMA_INT_ANY_MOT       = BMA423_ANY_MOT_INT,
  BMA_INT_NO_MOT        = BMA423_NO_MOT_INT,
  BMA_INT_ERROR         = BMA423_ERROR_INT,
} eBMAInterruptSource;

typedef struct {
  eSensorsState         state;
  eBMAInterruptSource   interrupt_source;
  bool                  interrupt_chg_req;
  uint32_t              step_count;
} bma_ctrl_t;

bool bma_init(void);
bool bma423_get_device_id(void);
bool bma423_set_interrupt_source(eBMAInterruptSource int_source);
bool bma423_get_interrupt_status();
void update_step_count(void);
uint32_t get_step_count(void);
void delay(uint32_t period_us, void * intf_ptr);
bool sleep_bma(void);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_BMA421_H
