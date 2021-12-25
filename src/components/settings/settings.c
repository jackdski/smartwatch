//
// Created by jack on 11/1/20.
//

#include "settings.h"

#include <string.h>  // memset()

static Settings_t  * settings;

void init_settings(void)
{
    memset(settings, 0U, sizeof(settings));
}

void set_setting(Settings_E setting, bool active)
{
    if (setting != SETTINGS_COUNT)
    {
        settings->options[setting] = active;
    }
}

void toggle_setting(Settings_E setting)
{
    if (setting != SETTINGS_COUNT)
    {
        settings->options[setting] ^= 1U;
    }
}

bool get_setting(Settings_E setting)
{
    bool ret = false;
    if (setting != SETTINGS_COUNT)
    {
        ret = settings->options[setting];
    }

    return ret;
}
