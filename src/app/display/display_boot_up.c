//
// Created by jack on 7/12/20.
//

#include "display_boot_up.h"
#include "display.h"

//extern lv_obj_t * boot_up_scr;

void display_boot_up(void)
{    lv_obj_t * main_label = lv_label_create(lv_scr_act(), NULL);

    static lv_style_t boot_style;
    lv_style_copy(&boot_style, &lv_style_plain);
    boot_style.body.main_color = LV_COLOR_BLACK;
    boot_style.body.radius = 3;
    boot_style.text.color = lv_color_hex3(0x000);   // white text
    boot_style.text.font = &lv_font_roboto_22;

    lv_label_set_style(main_label, LV_LABEL_STYLE_MAIN, &boot_style);
    /* Create the main label */

    /* Position the main label */
    lv_label_set_align(main_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(main_label, "JD");
    lv_obj_set_width(main_label, 200);
    lv_obj_align(main_label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(main_label, "JD Smartwatch");
}
