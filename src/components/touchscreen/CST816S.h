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

#define CST816S_GESTURE_ID_INDEX    0x03
#define CST816S_TOUCH_INDEX         0x02

#define CST816S_X_MSB               0x03
#define CST816S_X_LSB               0x04

#define CST816S_Y_MSB               0x05
#define CST816S_Y_LSB               0x06

#define  CST816S_SET_MSB(XY)         (uint16_t)((XY & 0x0F) << 8)

#define CST816S_TOUCH_STEP          0x06

enum {
  CST816S_REG_INT_CNT           = 0x8F,
  CST816S_REG_FLOW_WORK_CNT     = 0x91,
  CST816S_REG_WORKMODE          = 0x00,
  CST816S_REG_CHIP_ID           = 0xA3,
  CST816S_REG_CHIP_ID2          = 0x9F,
  CST816S_REG_POWER_MODE        = 0xA5,
  CST816S_REG_FW_VER            = 0xA6,
  CST816S_REG_VENDOR_ID         = 0xA8,
  CST816S_REG_LCD_BUSY_NUM      = 0xAB,
  CST816S_REG_FACE_DEC_MODE     = 0xB0,
  CST816S_REG_GLOVE_MODE        = 0xC0,
  CST816S_REG_COVER_MODE        = 0xC1,
  CST816S_REG_CHARGER_MODE      = 0x8B,
  CST816S_REG_GESTURE           = 0xD0,
  CST816S_REG_GESTURE_OUT_ADDR  = 0xD3,
  CST816S_REG_ESD_SATUREATE     = 0xED
};

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
  NO_GESTURE            = 0x00,
  GESTURE_SLIDE_DOWN    = 0x01,
  GESTURE_SLIDE_UP      = 0x02,
  GESTURE_SLIDE_LEFT    = 0x03,
  GESTURE_SLIDE_RIGHT   = 0x04,
  GESTURE_SINGLE_TAP    = 0x05,
  GESTURE_DOUBLE_TAP    = 0x0B,
  GESTURE_LONG_PRESS    = 0x0C
} eGesture;

typedef void (*i2c_read_reg)(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer);
typedef void (*i2c_write_reg)(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

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
uint8_t CST816S_init(void);
bool CST816S_read_touch(void);
bool CST816S_get_touch_active(void);
void CST816S_get_xy(uint16_t * x, uint16_t * y);

#endif //BLINKYEXAMPLEPROJECT_SRC_DRIVERS_CST816S_H
