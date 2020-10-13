//
// Created by jack on 5/24/20.
//

#ifndef JDSMARTWATCHPROJECT_DISPLAY_H
#define JDSMARTWATCHPROJECT_DISPLAY_H

#include "lvgl/lvgl.h"
#include "../resources.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "app_config.h"
#include "drivers/display_drv.h"

extern lv_disp_buf_t lvgl_disp_buf;
extern lv_color_t lvgl_buf[LV_HOR_RES_MAX * 10];
extern lv_disp_drv_t lvgl_disp_drv;
//extern lv_color_t lvgl_buf_two[LV_HOR_RES_MAX * 4];


typedef enum {
    DISPLAY_STATE_INITIALIZATION,
    DISPLAY_STATE_RUN,
    DISPLAY_STATE_GO_TO_SLEEP,
    DISPLAY_STATE_SLEEP,
    DISPLAY_STATE_ERROR
} Display_States_E;

typedef enum {
    DISPLAY_SCREEN_INITIALIZATION,
    DISPLAY_SCREEN_TIME,
    DISPLAY_SCREEN_SETTINGS,
    DISPLAY_SCREEN_TEXT_MESSAGE,
    DISPLAY_SCREEN_PHONE_NOTIFICATION
} Display_Screens_E;

extern lv_obj_t * boot_up_scr;
extern lv_obj_t * home_scr;

// RTOS App
void Display_Task(void * arg);
void vDisplayTimeoutCallback( TimerHandle_t xTimer );
void display_timeout_refresh(void);
void display_timeout_enable(void);
void display_timeout_disable(void);

// Display App
void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void load_screen(lv_obj_t * scr);

// Display Bringup Test Functions
void display_brightness_test(void);
void display_color_fill_test(void);

#endif //JDSMARTWATCHPROJECT_DISPLAY_H
