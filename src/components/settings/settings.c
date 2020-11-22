//
// Created by jack on 11/1/20.
//

#include "settings.h"

void change_setting(Settings_t  * settings, eSetting new_setting)
{
    switch(new_setting)
    {
    case SETTING_MILITARY_TIME:
        settings->military_time ^= 1;
        break;
    case SETTING_DATE_FORMAT:
        settings->date_format ^= 1;
        break;
    case SETTING_RIGHT_HAND:
        settings->right_hand ^= 1;
        break;
    case SETTING_SHOW_TEXTS:
        settings->show_texts ^= 1;
        break;
    case SETTING_SHOW_EMAILS:
        settings->show_emails ^= 1;
        break;
    case SETTING_SHOW_CALLS:
        settings->show_calls ^= 1;
        break;
    case SETTING_SHOW_NEWS:
        settings->show_news ^= 1;
        break;
    default:
        break;
    }
}
