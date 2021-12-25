//
// Created by jack on 8/22/20.
//

#ifndef BMA421_H_
#define BMA421_H_

#include <stdint.h>
#include <stdbool.h>

#include "bma423.h"

typedef enum {
  BMA_STATE_INITIALIZATION,
  BMA_STATE_ACTIVE,
  BMA_STATE_LOW_POWER,
  BMA_STATE_WAKEUP
} BMA_State_E;

typedef enum {
  BMA_INT_SINGLE_TAP    = BMA423_SINGLE_TAP_INT,
  BMA_INT_STEP_CNTR     = BMA423_STEP_CNTR_INT,
  BMA_INT_ACTIVITY      = BMA423_ACTIVITY_INT,
  BMA_INT_WRIST_WEAR    = BMA423_WRIST_WEAR_INT,
  BMA_INT_DOUBLE_TAP    = BMA423_DOUBLE_TAP_INT,
  BMA_INT_ANY_MOT       = BMA423_ANY_MOT_INT,
  BMA_INT_NO_MOT        = BMA423_NO_MOT_INT,
  BMA_INT_ERROR         = BMA423_ERROR_INT,
} BMA_InterruptSource_E;

typedef struct {
  BMA_State_E           state;
  BMA_InterruptSource_E interrupt_source;
  bool                  interrupt_chg_req;
  uint32_t              step_count;
} bma_ctrl_t;

bool bma_init(void);
void init_bmaGpioInterrupt(void * irq_pfn);
bool bma_getDeviceID(void);
bool bma_setInterruptSource(BMA_InterruptSource_E int_source);
bool bma_getInterruptStatus();
void bma_updateStepCount(void);
uint32_t bma_getStepCount(void);
bool bma_isActive(void);
bool bma_goToLowPower(void);

#endif /* BMA421_H_s */
