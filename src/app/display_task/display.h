//
// Created by jack on 5/24/20.
//

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "lvgl/lvgl.h"
#include "display_common.h"
#include "settings.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "app_config.h"
#include "app_sensors.h"
#include "display_drv.h"

typedef enum
{
    DISPLAY_STATE_INITIALIZATION,
    DISPLAY_STATE_RUN,
    DISPLAY_STATE_GO_TO_SLEEP,
    DISPLAY_STATE_SLEEP,
    DISPLAY_STATE_ERROR
} Display_States_E;

typedef struct
{
  bool                  initialized:1;
  bool                  active:1;
  bool                  always_on:1;
  bool                  debug:1;
  bool                  button_pressed:1;
  bool                  touch_active:1;
  Display_States_E      state;
  DisplayScreens_E      screen;
  eBacklightSetting     backlight_setting;
  eDisplayRotation      rotation_setting;
} Display_Control_t;

typedef struct
{
  bool                    charging:1;
  DisplayBatteryStatus_E  battery_status;
  uint8_t                 soc;
  uint16_t                heart_rate;
} DisplayData_t;

// Display App
void UIupdate_Task(void * arg);
void Display_Task(void * arg);
void vDisplayTimeoutCallback(TimerHandle_t xTimer);
void display_timeout_refresh(void);
void display_timeout_enable(void);
void display_timeout_disable(void);
void display_alert_timer_enable(void);
void display_alert_timer_disable(void);
bool display_get_charging_status(void);
DisplayBatteryStatus_E display_get_battery_status(void);
DisplayScreens_E display_get_current_screen(void);

// UI Update
void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// Display Bringup Test Functions
void display_brightness_test(void);
void display_color_fill_test(void);

// Shared Data
void display_setting_changed(eSetting setting);

#endif //DISPLAY_H_
