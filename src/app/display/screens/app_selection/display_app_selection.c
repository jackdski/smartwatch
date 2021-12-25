#include "display_app_selection.h"

#include "lvgl/lvgl.h"
#include "display_common.h"

#include <stdlib.h>

//--- DEFINES ---//


//--- TYPEDEFs ---//

typedef struct
{
    char                    * text;
    void                    (* eventHandler)(lv_obj_t * obj, lv_event_t event);
} DisplayAppChannelData_t;

//--- PRIVATE FUNCTIONS ---//

static void settings_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_PRESSED)
    {
        display_change_screen(DISPLAY_SCREEN_SETTINGS);
    }
}

static void brightness_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_PRESSED)
    {
        display_change_screen(DISPLAY_SCREEN_BRIGHTNESS);
    }
}

static void steps_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_PRESSED)
    {
        display_change_screen(DISPLAY_SCREEN_STEPS);
    }
}

static void heart_rate_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_PRESSED)
    {
        display_change_screen(DISPLAY_SCREEN_HEART_RATE);
    }
}

//--- PRIVATE DATA ---//

static const DisplayAppChannelData_t appSelectionData[DISPLAY_APP_COUNT] =
{
    [DISPLAY_APP_SETTINGS] =
    {
        .text = LV_SYMBOL_SETTINGS "   Settings",
        .eventHandler = settings_event_handler,
    },
    [DISPLAY_APP_BRIGHTNESS] =
    {
        .text = LV_SYMBOL_EYE_OPEN "   Brightness",
        .eventHandler = brightness_event_handler,
    },
    [DISPLAY_APP_STEPS] =
    {
        .text = LV_SYMBOL_GPS "   Steps",
        .eventHandler = steps_event_handler,
    },
    [DISPLAY_APP_HEART_RATE] =
    {
        .text = LV_SYMBOL_IMAGE "   Heart Rate",
        .eventHandler = heart_rate_event_handler,
    },
};

//--- PUBLIC FUNCTIONS ---//

void app_selection_screen(void)
{
    lv_obj_clean(lv_scr_act());
    lv_obj_t * app_selection_scr = lv_scr_act();
    display_battery_layer(app_selection_scr);
    display_screen_title(app_selection_scr, "Apps");

    lv_obj_t * list = lv_list_create(lv_scr_act(), NULL);

    /* List*/
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(list, 0, LV_VER_RES);
    lv_list_set_scroll_propagation(list, true);
    // lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_HIDE);

    // Buttons
    static lv_style_t btns_style;
    lv_style_copy(&btns_style, &lv_style_plain);
    btns_style.text.font = &lv_font_roboto_16;
    btns_style.body.padding.left = 10U;

    for (DisplayApps_E i = (DisplayApps_E)0U; i < DISPLAY_APP_COUNT; i++)
    {
        lv_obj_t * settings_btn = lv_btn_create(lv_scr_act(), NULL);
        lv_btn_set_style(settings_btn, LV_BTN_STYLE_REL, &btns_style);
        settings_btn = lv_list_add_btn(list, NULL, appSelectionData[i].text);
        lv_obj_set_event_cb(settings_btn, appSelectionData[i].eventHandler);
    }
}