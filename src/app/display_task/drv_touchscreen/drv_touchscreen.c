
#include "drv_touchscreen.h"
#include "lvgl/lvgl.h"

#include <stdint.h>
#include <stdbool.h>

#include "CST816S.h"

DrvTouchscreen_t touchscreen_pos =
{
    .x_last = 0,
    .y_last = 0,
    .x = 0,
    .y = 0
};

/** Touchscreen **/
bool touchscreen_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    if(CST816S_get_touch_num() == 1)
    {
        CST816S_get_xy(&touchscreen_pos.x, &touchscreen_pos.y);
        touchscreen_pos.x_last = touchscreen_pos.x;
        touchscreen_pos.y_last = touchscreen_pos.y;
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        touchscreen_pos.x = touchscreen_pos.x_last;
        touchscreen_pos.y = touchscreen_pos.y_last;
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = touchscreen_pos.x;
    data->point.y = touchscreen_pos.y;
    return false;
}