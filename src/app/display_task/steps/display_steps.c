//
// Created by jack on 10/13/20.
//

#include "display_steps.h"

#include "lvgl/lvgl.h"
#include "display_common.h"

lv_obj_t * step_count_label;
lv_obj_t * step_count_date_label;

void steps_screen(void)
{
    lv_obj_clean(lv_scr_act());

    display_battery_layer(lv_scr_act());
    display_screen_title(lv_scr_act(), "Steps");

    static lv_style_t steps_count_style;
    lv_style_copy(&steps_count_style, &lv_style_plain);
    steps_count_style.text.font = &lv_font_roboto_22;

    // Style "Settings" text
    static lv_style_t steps_date_style;
    lv_style_copy(&steps_date_style, &lv_style_plain);
    steps_date_style.text.font = &lv_font_roboto_16;

    step_count_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_style(step_count_label, LV_LABEL_STYLE_MAIN, &steps_count_style);
    lv_label_set_text(step_count_label, "4775");
    lv_obj_align(step_count_label, NULL, LV_ALIGN_CENTER, 0, -15);

    step_count_date_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_style(step_count_date_label, LV_LABEL_STYLE_MAIN, &steps_date_style);
    lv_label_set_text(step_count_date_label, "October 10, 2020");
    lv_obj_align(step_count_date_label, NULL, LV_ALIGN_CENTER, 0, 20);
}

