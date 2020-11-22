//
// Created by jack on 8/18/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_DRIVERS_CST816S_H
#define BLINKYEXAMPLEPROJECT_SRC_DRIVERS_CST816S_H

#include "CST816S.h"
#include <stdint.h>
#include <stdbool.h>

#define CST816S_ADDR                0x15

#define CST816S_GESTURE             1U

#define CST816S_POWER_MODE_ADDR     0xA5
#define CST816S_POWER_MODE_SLEEP    0x03    // reset pin low for 5ms, reset pin high for 50ms, then write value

#define CST816S_TOUCH_EVENT_INDEX   0x01
#define CST816S_X_MSB               0x03
#define CST816S_X_LSB               0x04

#define CST816S_Y_MSB               0x05
#define CST816S_Y_LSB               0x06

#define CST816S_TOUCH_STEP          0x06


typedef struct {
  uint8_t blank_0;
  uint8_t gesture;
  uint8_t x_msb;
  uint8_t x_lsb;
  uint8_t touchID_y_msb;
  uint8_t y_lsb;
  uint8_t pressure;
  uint8_t blank_1;
} CST816S_TouchEvent_t;

typedef enum {
  NO_GESTURE = 0x00,
  GESTURE_SLIDE_DOWN,
  GESTURE_SLIDE_UP,
  GESTURE_SLIDE_LEFT,
  GESTURE_SLIDE_RIGHT,
  GESTURE_SINGLE_TAP,
  GESTURE_DOUBLE_TAP = 0x0B,
  GESTURE_LONG_PRESS = 0x0C
} eGesture;

typedef struct {
  uint16_t x;
  uint16_t y;
} TouchPoint_t;

typedef struct {
  TouchPoint_t  point;
  eGesture      gesture;
  eGesture      prev_gesture;       // TODO: remove if unneeded
  uint32_t      twi_base;
  bool          touch_active;
  bool          asleep;
} CST816S_t;

typedef struct {
  eGesture       gestureID;
  uint8_t        numTouchPoints;
  TouchPoint_t   point;
  uint8_t        pressure;
  bool           touch_active;
  bool           asleep;
} CST816S_Event_t;

void CST816S_pin_configure(void);
void CST816S_init(void);
bool CST816S_read_touch(uint16_t * x, uint16_t * y);
bool CST816S_isTouchActive(void);

#endif //BLINKYEXAMPLEPROJECT_SRC_DRIVERS_CST816S_H
