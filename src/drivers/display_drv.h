//
// Created by jack on 7/10/20.
//

#ifndef JDSMARTWATCHPROJECT_SRC_DRIVERS_DISPLAY_DRV_H
#define JDSMARTWATCHPROJECT_SRC_DRIVERS_DISPLAY_DRV_H

#include <stdint.h>
//#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_config.h"

#define ST77XX_CASET   0x2A
#define ST77XX_RASET   0x2B
#define ST77XX_RAMWR   0x2C

typedef enum {
  DISPLAY_ROTATION_0,   // wear on right hand
  DISPLAY_ROTATION_90,
  DISPLAY_ROTATION_180, // wear on left hand
  DISPLAY_ROTATION_270
} eDisplayRotation;

typedef enum {
  BACKLIGHT_OFF,
  BACKLIGHT_LOW,
  BACKLIGHT_MID,
  BACKLIGHT_LOW_MID,
  BACKLIGHT_HIGH,
  BACKLIGHT_LOW_HIGH,
  BACKLIGHT_MID_HIGH,
  BACKLIGHT_LOW_MID_HIGH,
} eBacklightSetting;


void display_write_command(uint8_t cmd);
void display_write_byte(uint8_t data);
void display_write_data(uint8_t * data, size_t data_size);

void display_on(void);
void display_off(void);
void display_go_to_sleep(void);
void display_wake_up(void);
void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void display_draw_fill(uint8_t color);
void display_set_address_window(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1);
void display_set_rotation(eDisplayRotation rotation);
void display_backlight_set(eBacklightSetting setting);
void display_configure(void);

void display_dc_cmd_set(void);
void display_dc_cmd_clear(void);
void display_reset_set(void);
void display_reset_clear(void);

#endif //JDSMARTWATCHPROJECT_SRC_DRIVERS_DISPLAY_DRV_H
