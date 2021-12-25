//
// Created by jack on 10/13/20.
//

#include "display_heart_rate.h"

#include "lvgl/lvgl.h"
#include "display_common.h"

#include <stdlib.h>


typedef struct
{
    uint16_t heart_rate;
} heartRateData_t;

//extern lv_obj_t * heart_rate_scr;

lv_obj_t * heart_rate_count_label;
lv_obj_t * bpm_label;

heartRateData_t heartRateData =
{
    .heart_rate = 0U,
};

void update_heart_rate(uint16_t new_heart_rate)
{
    heartRateData.heart_rate = new_heart_rate;
}

void update_heart_rate_text(void)
{
    static char buf[4U];
    // snprintf(buf, 4, "%u", heart_rate);
    lv_label_set_text(heart_rate_count_label, buf);
}

void heart_rate_screen(void)
{
    lv_obj_clean(lv_scr_act());
    lv_obj_t * heart_rate_scr = lv_scr_act();
    display_battery_layer(heart_rate_scr);
    display_screen_title(heart_rate_scr, "Heart Rate");

    // TODO: heart graphic

    static lv_style_t heart_rate_style;
    lv_style_copy(&heart_rate_style, &lv_style_plain);
    heart_rate_style.text.font = &lv_font_roboto_28;

    // Style bpm text
    static lv_style_t bpm_label_style;
    lv_style_copy(&bpm_label_style, &lv_style_plain);
    heart_rate_style.text.font = &lv_font_roboto_22;

    heart_rate_count_label = lv_label_create(heart_rate_scr, NULL);
    lv_label_set_style(heart_rate_scr, LV_LABEL_STYLE_MAIN, &heart_rate_style);
    char heartRate[3] = {" "};
    lv_label_set_text(heart_rate_count_label, __itoa(heartRateData.heart_rate, heartRate, 10U));
    lv_obj_set_pos(heart_rate_count_label, (LV_HOR_RES_MAX / 2U) - 45U, 100U);

    bpm_label = lv_label_create(heart_rate_scr, NULL);
    lv_label_set_style(bpm_label, LV_LABEL_STYLE_MAIN, &bpm_label_style);
    lv_label_set_text(bpm_label, "bpm");
    lv_obj_set_pos(bpm_label, (LV_HOR_RES_MAX / 2U) + 10U, 110U);
}
