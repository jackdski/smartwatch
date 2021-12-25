//
// Created by jack on 11/1/20.
//

#ifndef JD_SMARTWATCH_SRC_COMPONENTS_SETTINGS_SETTINGS_H
#define JD_SMARTWATCH_SRC_COMPONENTS_SETTINGS_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SETTING_MILITARY_TIME,
    SETTING_DATE_FORMAT,
    SETTING_RIGHT_HAND,
    SETTING_SHOW_TEXTS,
    SETTING_SHOW_EMAILS,
    SETTING_SHOW_CALLS,
    SETTING_SHOW_NEWS,

    SETTINGS_COUNT
} Settings_E;

typedef struct {
    bool    options[SETTINGS_COUNT];
//  uint32_t memory_addr;  // TODO: read/write settings from flash
} Settings_t;

// Public Functions
void set_setting(Settings_E setting, bool active);
void toggle_setting(Settings_E setting);
bool get_setting(Settings_E setting);


#endif //JD_SMARTWATCH_SRC_COMPONENTS_SETTINGS_SETTINGS_H
