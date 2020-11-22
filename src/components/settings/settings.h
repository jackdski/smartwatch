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
} eSetting;

typedef struct {
  bool military_time:1;
  bool date_format:1;
  bool right_hand:1;
  bool show_texts:1;
  bool show_emails:1;
  bool show_calls:1;
  bool show_news:1;
//  uint32_t memory_addr;  // TODO: read/write settings from flash
} Settings_t;

typedef struct {
  eSetting setting;
  bool value;
} ChangeSetting_t;

// Public Functions
void change_setting(Settings_t  * settings, eSetting new_setting);


#endif //JD_SMARTWATCH_SRC_COMPONENTS_SETTINGS_SETTINGS_H
