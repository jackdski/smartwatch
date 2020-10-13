//
// Created by jack on 10/13/20.
//

#include "display_steps.h"

#include "lvgl/lvgl.h"
#include "display_common.h"

//extern lv_obj_t home_scr;
//extern lv_obj_t * settings_scr;
//extern lv_obj_t * brightness_scr;
extern lv_obj_t * steps_scr;
//extern lv_obj_t * heart_rate_scr;

lv_obj_t * step_count_label;
lv_obj_t * step_count_date_label;

void steps_screen(void)
{
    display_battery_layer(steps_scr);
    display_screen_title(steps_scr, "Steps");

    static lv_style_t steps_count_style;
    lv_style_init(&steps_count_style);
    lv_style_set_text_font(&steps_count_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_30);

    // Style "Settings" text
    static lv_style_t steps_date_style;
    lv_style_init(&steps_date_style);
    lv_style_set_text_font(&steps_date_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_20);

    step_count_label = lv_label_create(steps_scr, NULL);
    lv_label_set_text(step_count_label, "4775");
    lv_obj_align(step_count_label, NULL, LV_ALIGN_CENTER, 0, -15);
    lv_obj_add_style(step_count_label, LV_LABEL_PART_MAIN, &steps_count_style);

    step_count_date_label = lv_label_create(steps_scr, NULL);
    lv_label_set_text(step_count_date_label, "October 10, 2020");
    lv_obj_align(step_count_date_label, NULL, LV_ALIGN_CENTER, 0, 20);
    lv_obj_add_style(step_count_date_label, LV_LABEL_PART_MAIN, &steps_date_style);
}

