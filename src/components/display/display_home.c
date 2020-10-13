//
// Created by jack on 7/12/20.
//

#include "display_home.h"
#include "display.h"
#include <stdlib.h>
#include <string.h>

void display_home(char * hours, char * minutes)
{
    static lv_point_t valid_pos[] = {{0,0}, {0,1}, {1,1}};
    lv_obj_t * home_tileview;
    home_tileview = lv_tileview_create(home_scr, NULL);
    lv_tileview_set_valid_positions(home_tileview, valid_pos, 3);
    lv_tileview_set_edge_flash(home_tileview, true);

    lv_obj_t * home_tile = lv_obj_create(home_tileview, NULL);
    lv_obj_set_size(home_tile, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(home_tileview, home_tile);

    // Time Label
    lv_obj_t * time_label = lv_label_create(home_tile, NULL);

    // TODO: add setting to disply AM/PM. Turn off when in 24hr mode
    char time_str[strlen(hours) + strlen(minutes) + 1];
    strcat(time_str, hours);
//    strcat(time_str, &colon);
    strcat(time_str, minutes);

    lv_label_set_text(time_label, time_str);
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, 0);

    // Options list
    lv_obj_t * home_list = lv_list_create(home_tileview, NULL);
    lv_obj_set_size(home_list, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(home_list, 0, LV_VER_RES);
    lv_list_set_scroll_propagation(home_list, true);
    lv_list_set_scrollbar_mode(home_list, LV_SCROLLBAR_MODE_OFF);

    lv_list_add_btn(home_list, NULL, "Settings");
    lv_list_add_btn(home_list, NULL, "Option Two");
    lv_list_add_btn(home_list, NULL, "Option Three");
}

// TODO: handle settings