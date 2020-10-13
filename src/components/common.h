//
// Created by jack on 9/8/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_COMMON_H
#define BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_COMMON_H

#include <stdint.h>
#include <stdbool.h>

#include "nrf_ble_ancs_c.h"

#define COMPONENT_LIST_ALL  ((COMPONENT_SYSTEM_TASK << 1) - 1)
#define NUM_COMPONENTS      6

typedef enum {
  COMPONENT_DISPLAY         = (1 << 0),
  COMPONENT_BATTERY_MONITOR = (1 << 1),
  COMPONENT_TWI             = (1 << 2),
  COMPONENT_SPI             = (1 << 3),
  COMPONENT_HAPTIC          = (1 << 4),
  COMPONENET_BLE_GENERAL    = (1 << 5),
  COMPONENT_SYSTEM_TASK     = (1 << 6),
  COMPONENT_BUTTON          = (1 << 7),
  COMPONENT_SENSOR_IMU      = (1 << 8),
  COMPONENT_SENSOR_HRS      = (1 << 9),
  COMPONENT_TOUCHSCREEN     = (1 << 10),
  COMPONENT_NVMEM           = (1 << 11),
  COMPONENT_EXT_FLASH       = (1 << 12)
} eComponentList;

typedef struct {
  uint8_t     second;
  uint8_t     minute;
  uint8_t     hour;
  uint8_t     day_of_week;
  uint8_t     day_of_month;
  uint8_t     month;
  uint16_t    year;
} Time_t;

typedef enum {
  DISPLAY_BLE_MSG_NONE,
  DISPLAY_BLE_MSG_CONFIRM,
  DISPLAY_BLE_MSG_REJECT,
  DISPLAY_BLE_MSG_DISCONNECT,
} eDisplayBLE_Message;

typedef enum {
  DISPLAY_UPDATE_NONE,
  DISPLAY_UPDATE_BLE_EVENT_DISCONNECTED,
  DISPLAY_UPDATE_BLE_EVENT_CONNECTED,
  DISPLAY_UPDATE_START_CHARGING,
  DISPLAY_UPDATE_STOP_CHARGING
} eDisplayUpdate;

typedef enum {
  BLE_NEGATIVE_ACTION,
  BLE_POSITIVE_ACTION,
} eBLEResponse;

typedef struct {
  ble_ancs_c_evt_t evt;
  eBLEResponse response;
} BLEMsg_t;


bool get_low_power_state(void);


#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_COMMON_H
