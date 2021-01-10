//
// Created by jack on 10/13/20.
//

#include "display_heart_rate.h"

#include "lvgl/lvgl.h"
#include "display_common.h"

//extern lv_obj_t * heart_rate_scr;

lv_obj_t * heart_rate_count_label;
lv_obj_t * bpm_label;
uint16_t heart_rate = 0;

uint16_t hr_data[] = {60, 57, 59, 63, 64, 69, 67, 62};
uint8_t hr_index = 0;

void update_heart_rate(uint16_t new_heart_rate)
{
    if (hr_index == sizeof(hr_data) / 2) {
    }
    else {
        hr_index = 0;
    }
    heart_rate = hr_data[hr_index];
    hr_index++;
}

void update_heart_rate_text(void)
{
    static char buf[4];
//    snprintf(buf, 4, "%u", heart_rate);
    lv_label_set_text(heart_rate_count_label, buf);
}

void heart_rate_screen(void)
{
    lv_obj_clean(lv_scr_act());
    lv_obj_t * heart_rate_scr = lv_scr_act();
    display_battery_layer(heart_rate_scr);
    display_screen_title(heart_rate_scr, "Heart Rate");

    // TODO: heart graphic

    static lv_style_t steps_count_style;
    lv_style_copy(&steps_count_style, &lv_style_plain);
    steps_count_style.text.font = &lv_font_roboto_28;

    // Style bpm text
    static lv_style_t bpm_label_style;
    lv_style_copy(&bpm_label_style, &lv_style_plain);
    steps_count_style.text.font = &lv_font_roboto_22;

    heart_rate_count_label = lv_label_create(heart_rate_scr, NULL);
    lv_label_set_style(heart_rate_scr, LV_LABEL_STYLE_MAIN, &steps_count_style);
    lv_label_set_text(heart_rate_count_label, "60");
    lv_obj_set_pos(heart_rate_count_label, (LV_HOR_RES_MAX / 2) - 45, 100);

    bpm_label = lv_label_create(heart_rate_scr, NULL);
    lv_label_set_style(bpm_label, LV_LABEL_STYLE_MAIN, &bpm_label_style);
    lv_label_set_text(bpm_label, "bpm");
    lv_obj_set_pos(bpm_label, (LV_HOR_RES_MAX / 2) + 10, 110);
}
