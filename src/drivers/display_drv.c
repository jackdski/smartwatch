//
// Created by jack on 7/10/20.
//

#include "display_drv.h"
#include "components/resources.h"
#include "drivers/spi_driver.h"

#include "nrf_delay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "ST7789.h"

extern SemaphoreHandle_t spi_mutex;


void display_on(void)
{
    display_write_command(ST7789_DISPON);
}

void display_off(void)
{
    display_write_command(ST7789_DISPOFF);
}

void display_go_to_sleep(void)
{
    display_write_command(ST7789_SLPIN);
}

void display_wake_up(void)
{
    display_write_command(ST7789_SLPOUT);
}

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x < 0) || (x >= DISPLAY_WIDTH) || (y < 0) || (y >= DISPLAY_HEIGHT))
    {
        return;
    }

    display_set_address_window(x, x+1, y, y+1);
//    uint8_t rgb[] = {(color & 0x1F), ((color & 0x03) << 3) | (color & 0xE0), (color & 0xF8)};
    uint8_t rgb[] = {((color >> 8) & 0xFF), (color & 0xFF)};
    display_write_data(rgb, sizeof(rgb));
}

void display_draw_fill(uint8_t color)
{
    display_set_address_window(0, DISPLAY_WIDTH, 0, DISPLAY_HEIGHT);

    uint32_t i;
    for(i = 0; i < (DISPLAY_WIDTH * DISPLAY_HEIGHT); i++)
    {
        uint8_t rgb[] = {color, color};
        display_write_data(rgb, sizeof(rgb));
    }
}

void display_set_address_window(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
    uint8_t cmds[3] = {ST77XX_CASET, ST77XX_RASET, ST77XX_RAMWR};

    // Set Column Address
    display_write_command(cmds[0]);
//    display_write_command(ST7789_CASET);
    {
        uint8_t data[] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
        display_write_data(data, sizeof(data));
    }

    // Set Row Address
    display_write_command(cmds[1]);
//    display_write_command(ST77XX_RASET);
    {
        uint8_t data[] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
        display_write_data(data, sizeof(data));
    }

    // Write to RAM
    display_write_command(cmds[2]);
//    display_write_command(ST77XX_RAMWR);
}

void display_set_rotation(eDisplayRotation rotation)
{
    display_write_command(ST7789_MADCTL);

    switch (rotation) {
    case DISPLAY_ROTATION_0:
        display_write_byte(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
        break;
    case DISPLAY_ROTATION_90:
        display_write_byte(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
        break;
    case DISPLAY_ROTATION_180:
        display_write_byte(ST7789_MADCTL_RGB);
        break;
    case DISPLAY_ROTATION_270:
        display_write_byte(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
        break;
    default:
        // DISPLAY_ROTATION_0
        display_write_byte(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
        break;
    }
}

void display_configure(void)
{
    // hardware reset
    NRF_LOG_INFO("Display HW reset");
    display_reset_set();
    display_reset_clear();
//    vTaskDelay(pdMS_TO_TICKS(10));
    nrf_delay_ms(10);
    display_reset_set();
    NRF_LOG_INFO("Display reset");

    // software reset
    display_write_command(ST7789_SWRESET);
//    vTaskDelay(pdMS_TO_TICKS(150));
    nrf_delay_ms(150);

    // SleepOut
    display_write_command(ST7789_SLPOUT);

    NRF_LOG_INFO("Display Chkpt 1");

    // ColMod
    display_write_command(ST7789_COLMOD);
    display_write_byte(ST7789_COLOR_MODE_16bit);
//    vTaskDelay(pdMS_TO_TICKS(10));
    nrf_delay_ms(10);

    // MemDataAccessCtl
    display_write_command(ST7789_MADCTL);
    display_write_byte(0x00);

    NRF_LOG_INFO("Display Chkpt 2");

// Column Address Set
    display_write_command(ST7789_CASET);
    {
        uint8_t data[] = {0x00, 0x00, 240U >> 8, 240u & 0xFFu};
        display_write_data(data, sizeof(data));
    }

    // RowAddress
    display_write_command(ST7789_RASET);
    {
        uint8_t data[] = {0x00, 0x00, 240u >> 8, 240u & 0xFFu};
        display_write_data(data, sizeof(data));
    }

    // inversion
    display_write_command(ST7789_INVON);
//    vTaskDelay(pdMS_TO_TICKS(10));
    nrf_delay_ms(10);

    display_write_command(ST7789_NORON);
//    vTaskDelay(pdMS_TO_TICKS(10));
    nrf_delay_ms(10);

    // Display On
    display_on();
    NRF_LOG_INFO("Display Config Finished");
}

void display_write_command(uint8_t cmd)
{
    display_dc_cmd_clear();
    spi_write(DISPLAY_CS_PIN, &cmd, sizeof(cmd));
}

void display_write_byte(uint8_t data)
{
    display_dc_cmd_set();
    spi_write(DISPLAY_CS_PIN, &data, sizeof(data));
}

void display_write_data(uint8_t * data, size_t data_size)
{
    display_dc_cmd_set();
    spi_write(DISPLAY_CS_PIN, data, data_size);
}

void display_reset_set(void)
{
    nrf_gpio_pin_write(DISPLAY_RESET_PIN, 1);
}

void display_reset_clear(void)
{
    nrf_gpio_pin_write(DISPLAY_RESET_PIN, 0);
}

void display_dc_cmd_set(void)
{
    nrf_gpio_pin_write(DISPLAY_DC_PIN, 1);
}

void display_dc_cmd_clear(void)
{
    nrf_gpio_pin_write(DISPLAY_DC_PIN, 0);
}

void display_backlight_set(eBacklightSetting setting)
{
    switch(setting) {
    case BACKLIGHT_OFF:
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_MID, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_HIGH, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_LOW, 1);
        break;
    case BACKLIGHT_LOW:
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_LOW, 0);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_MID, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_HIGH, 1);
        break;
    case BACKLIGHT_MID:
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_LOW, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_MID, 0);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_HIGH, 1);
        break;
    case BACKLIGHT_HIGH:
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_LOW, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_MID, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_HIGH, 0);
        break;
    default:
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_LOW, 1);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_MID, 0);
        nrf_gpio_pin_write(DISPLAY_BACKLIGHT_HIGH, 1);
        break;
    }
}
