#ifndef DRV_TOUCHSCREEN_H_
#define DRV_TOUCHSCREEN_H_

#include <stdint.h>
#include <stdbool.h>

#include "lvgl/lvgl.h"


typedef struct 
{
    int16_t x_last;
    int16_t y_last;
    int16_t x;
    int16_t y;
} DrvTouchscreen_t;

bool touchscreen_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);


#endif /* DRV_TOUCHSCREEN_H_ */