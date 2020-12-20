//
// Created by jack on 10/13/20.
//

#include "display_common.h"
#include "lvgl/lvgl.h"
#include "display.h"

lv_obj_t * battery_info_label;
lv_obj_t * charging_info_label;

static const char * get_battery_status_icon(eDisplayBatteryStatus battery_status)
{
    switch(battery_status)
    {
    case BATTERY_FULL:
        return LV_SYMBOL_BATTERY_FULL;
    case BATTERY_ALMOST_FULL:
        return LV_SYMBOL_BATTERY_3;
    case BATTERY_HALF_FULL:
        return LV_SYMBOL_BATTERY_2;
    case BATTERY_LOW:
        return LV_SYMBOL_BATTERY_1;
    case BATTERY_EMPTY:
    default:
        return LV_SYMBOL_BATTERY_EMPTY;
    }
}

void display_battery_layer(lv_obj_t * par)
{
    bool charging_status = display_get_charging_status();
    eDisplayBatteryStatus battery_status = display_get_battery_status();


    battery_info_label = lv_label_create(par, NULL);
    lv_label_set_text(battery_info_label, get_battery_status_icon(battery_status));
    lv_obj_set_pos(battery_info_label, 215, 6);

    if(charging_status == true)
    {
        charging_info_label = lv_label_create(par, NULL);
        lv_label_set_text(charging_info_label, LV_SYMBOL_CHARGE);
        lv_obj_set_pos(charging_info_label, 195, 6);
    }
}

// Title Functions
void display_screen_title(lv_obj_t * par, char * title)
{
    // Style text
    static lv_style_t style;
    style.text.font = &lv_font_roboto_22;

    // Label
    lv_obj_t * label = lv_label_create(par, NULL);
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &style);
    lv_label_set_text(label, title);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 10);

    // line
    static lv_point_t line_points[] = { {5, 25}, {LV_HOR_RES_MAX - 5, 25} };
    static lv_style_t style_line;
    style_line.line.width = 1;
    style_line.line.color = LV_COLOR_WHITE;
    style_line.line.rounded = 1;

    /*Create a line */
    lv_obj_t * line1;
    line1 = lv_line_create(par, NULL);
    lv_line_set_points(line1, line_points, 2);
    lv_line_set_style(line1, LV_LINE_STYLE_MAIN, &style_line);
    lv_obj_set_pos(line1, 0, 15);

}