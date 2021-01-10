//
// Created by jack on 10/13/20.
//

#include "display_brightness.h"

//#include <stdio.h>

#include "lvgl/lvgl.h"
#include "display_common.h"

static lv_obj_t * slider_label;

int16_t brightness_value = 0;


static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
//        snprintf(buf, 4, "%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label, buf);

        brightness_value = lv_slider_get_value(slider);
    }
}

void brightness_screen(void)
{
    lv_obj_clean(lv_scr_act());
    lv_obj_t * brightness_scr = lv_scr_act();
    display_battery_layer(brightness_scr);
    display_screen_title(brightness_scr, "Brightness");

    /* Create a slider in the center of the display */
    lv_obj_t * slider = lv_slider_create(brightness_scr, NULL);
    lv_obj_set_width(slider, LV_DPI * 2);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_slider_set_range(slider, 0, 100);

    /* Create a label below the slider */
    slider_label = lv_label_create(brightness_scr, NULL);
    lv_label_set_text(slider_label, "0");
    lv_obj_set_auto_realign(slider_label, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}