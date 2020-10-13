//
// Created by jack on 10/13/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_DISPLAY_DISPLAY_COMMON_H
#define BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_DISPLAY_DISPLAY_COMMON_H

#include "lvgl/lvgl.h"

typedef enum {
  BATTERY_FULL,
  BATTERY_ALMOST_FULL,
  BATTERY_HALF_FULL,
  BATTERY_LOW,
  BATTERY_EMPTY
} eDisplayBatteryStatus;

void display_battery_layer(lv_obj_t * par);
void display_screen_title(lv_obj_t * par, char * title);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_DISPLAY_DISPLAY_COMMON_H
