//
// Created by jack on 10/13/20.
//

#ifndef DISPLAY_APP_SELECTION_H
#define DISPLAY_APP_SELECTION_H

#include <stdint.h>

typedef enum
{
    DISPLAY_APP_SETTINGS,
    DISPLAY_APP_BRIGHTNESS,
    DISPLAY_APP_STEPS,
    DISPLAY_APP_HEART_RATE,

    DISPLAY_APP_COUNT
} DisplayApps_E;

void app_selection_screen(void);

#endif //DISPLAY_APP_SELECTION_H
