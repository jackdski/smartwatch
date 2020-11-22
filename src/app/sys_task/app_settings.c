//
// Created by jack on 11/8/20.
//

#include "app_settings.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

extern QueueHandle_t settings_queue;

Settings_t settings = {
    .military_time = false,
    .date_format = false,
    .right_hand = false,
    .show_texts = false,
    .show_calls = false,
    .show_news = false
};

void run_settings_app(void)
{
    // TODO: read settings from memory on init
    ChangeSetting_t new_setting = {0};

    if(xQueueReceive(settings_queue, &new_setting, 0))
    {
        change_setting(&settings, new_setting.setting);
        // TODO: on change save to flash
    }
}