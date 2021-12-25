//
// Created by jack on 10/13/20.
//

#ifndef DISPLAY_COMMON_H_
#define DISPLAY_COMMON_H_

#include "lvgl/lvgl.h"
#include "nordic_common.h"

typedef enum
{
    BATTERY_FULL,
    BATTERY_ALMOST_FULL,
    BATTERY_HALF_FULL,
    BATTERY_LOW,
    BATTERY_EMPTY
} DisplayBatteryStatus_E;

typedef enum
{
    DISPLAY_SCREEN_INITIALIZATION,
    DISPLAY_SCREEN_ALERT,
    DISPLAY_SCREEN_HOME,
    DISPLAY_SCREEN_APP_SELECTION,
    DISPLAY_SCREEN_SETTINGS,
    DISPLAY_SCREEN_BRIGHTNESS,
    DISPLAY_SCREEN_STEPS,
    DISPLAY_SCREEN_HEART_RATE,
    DISPLAY_SCREEN_TEXT_MESSAGE,
    DISPLAY_SCREEN_PHONE_NOTIFICATION,

    DISPLAY_SCREEN_UNKNOWN
} DisplayScreens_E;


void display_change_screen(DisplayScreens_E screen);
DisplayScreens_E display_get_parent_screen(DisplayScreens_E screen);
void display_battery_layer(lv_obj_t * parent);
void display_screen_title(lv_obj_t * parent, char * title);

#endif //DISPLAY_COMMON_H_
