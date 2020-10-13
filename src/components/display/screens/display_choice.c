//
// Created by jack on 9/27/20.
//

#include "display_choice.h"


void display_choice(char * descriptions[], uint8_t n_choices)
{
    lv_obj_t * btns[n_choices];
    lv_obj_t * labels[n_choices];
    uint8_t i;
    for(i = 0; i < n_choices; i++)
    {
        btns[i] = lv_btn_create(lv_scr_act(), NULL);
//        lv_obj_set_event_cb();
        lv_obj_align(btns[i], NULL, LV_ALIGN_CENTER, 0, -20 * i);
        labels[i] = lv_label_create(btns[i], NULL);
        lv_label_set_text(labels[i], descriptions[i]);
    }
}