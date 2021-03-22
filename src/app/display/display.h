//
// Created by jack on 5/24/20.
//

#ifndef JDSMARTWATCHPROJECT_DISPLAY_H
#define JDSMARTWATCHPROJECT_DISPLAY_H

#include "lvgl/lvgl.h"
#include "display_common.h"
#include "settings.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "app_config.h"
#include "app_sensors.h"
#include "display_drv.h"

//extern lv_disp_buf_t lvgl_disp_buf;
//extern lv_color_t lvgl_buf[LV_HOR_RES_MAX * 4];
//extern lv_color_t lvgl_buf_two[LV_HOR_RES_MAX * 4];
extern lv_disp_drv_t lvgl_disp_drv;
extern lv_indev_drv_t indev_drv;

typedef enum {
    DISPLAY_STATE_INITIALIZATION,
    DISPLAY_STATE_RUN,
    DISPLAY_STATE_GO_TO_SLEEP,
    DISPLAY_STATE_SLEEP,
    DISPLAY_STATE_ERROR
} Display_States_E;

typedef enum {
    DISPLAY_SCREEN_INITIALIZATION,
    DISPLAY_SCREEN_HOME,
    DISPLAY_SCREEN_SETTINGS,
    DISPLAY_SCREEN_BRIGHTNESS,
    DISPLAY_SCREEN_STEPS,
    DISPLAY_SCREEN_HEART_RATE,
    DISPLAY_SCREEN_TEXT_MESSAGE,
    DISPLAY_SCREEN_PHONE_NOTIFICATION
} Display_Screens_E;

typedef struct {
  bool                  initialized:1;
  bool                  active:1;
  bool                  always_on:1;
  bool                  debug:1;
  bool                  charging:1;
  bool                  button_pressed:1;
  bool                  touch_active:1;
  eDisplayBatteryStatus battery_status;
  Display_States_E      state;
  Display_Screens_E     screen;
  eBacklightSetting     backlight_setting;
  eDisplayRotation      rotation_setting;
  uint8_t               soc;
  uint16_t              heart_rate;
} Display_Control_t;

// Display App
void init_display(void);
void Display_Task(void * arg);
void vDisplayTimeoutCallback(TimerHandle_t xTimer);
void vDisplayUpdateCallback(TimerHandle_t xTimer);
void display_timeout_refresh(void);
void display_timeout_enable(void);
void display_timeout_disable(void);
void display_change_screen(Display_Screens_E screen);
Display_Screens_E display_get_parent_screen(Display_Screens_E screen);
bool display_get_charging_status(void);
eDisplayBatteryStatus display_get_battery_status(void);

// UI Update
void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// Display Bringup Test Functions
void display_brightness_test(void);
void display_color_fill_test(void);

// Touchscreen
bool touchscreen_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

// Shared Data
void display_setting_changed(eSetting setting);

#endif //JDSMARTWATCHPROJECT_DISPLAY_H
