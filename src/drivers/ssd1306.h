//
// Created by jack on 5/14/20.
//

#ifndef JDSMARTWATCHPROJECT_SSD1306_H
#define JDSMARTWATCHPROJECT_SSD1306_H

#include <stdint.h>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_twi_sensor.h"

#define BLACK 0
#define WHITE 1

#define SSD1306_INIT_TRANSFER_COUNT 26

#define SSD1306_ADDR                0x78

#define SSD1306_CMD_REG             0x00
#define SSD1306_WRITE_DISPLAY_REG   0x40

#define SSD1306_CMD_WRITE(cmd) \
    NRF_TWI_MNGR_WRITE(SSD1306_ADDR, SSD1306_CMD_REG, 1, 0), \
    NRF_TWI_MNGR_WRITE(SSD1306_ADDR, cmd, 1, 0)

//common parameters
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64
#define SSD1306_FBSIZE              1024 //128x8
#define SSD1306_MAXROW              8
//command macro
#define SSD1306_CMD_DISPLAY_OFF     0xAE//--turn off the OLED
#define SSD1306_CMD_DISPLAY_ON      0xAF//--turn on oled panel

void ssd1306_start_send(nrf_drv_twi_t const * p_instance, uint8_t row, uint8_t col);

#endif //JDSMARTWATCHPROJECT_SSD1306_H
