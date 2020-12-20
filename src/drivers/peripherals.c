//
// Created by jack on 8/22/20.
//

#include "peripherals.h"
#include "nrf_drv_gpiote.h"
#include "pwm_driver.h"


void config_pinout(void)
{
    /** SPI **/
    nrf_gpio_cfg_output(SPI_SCK_PIN);
    nrf_gpio_pin_set(SPI_SCK_PIN);
    nrf_gpio_cfg_output(SPI_MOSI_PIN);
    nrf_gpio_pin_clear(SPI_MOSI_PIN);
    nrf_gpio_cfg_input(SPI_MISO_PIN, NRF_GPIO_PIN_NOPULL);

    SPIM_BASE->PSEL.MISO = SPI_MISO_PIN;
    SPIM_BASE->PSEL.MOSI = SPI_MOSI_PIN;
    SPIM_BASE->PSEL.SCK = SPI_SCK_PIN;

    /** TWIM **/
    NRF_GPIO->PIN_CNF[SENSORS_SCL_PIN] = ((uint32_t)GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos)
        | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
        | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
        | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
        | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

    NRF_GPIO->PIN_CNF[SENSORS_SDA_PIN] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
        | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
        | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
        | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
        | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

    nrf_twim_pins_set(SENSORS_NRF_TWIM,
                      SENSORS_SCL_PIN,
                      SENSORS_SDA_PIN);

    /** ST7789 **/
    nrf_gpio_cfg_output(DISPLAY_DC_PIN);
    nrf_gpio_pin_write(DISPLAY_DC_PIN, 1);

    nrf_gpio_cfg_output(DISPLAY_RESET_PIN);
    nrf_gpio_pin_write(DISPLAY_RESET_PIN, 1);

    nrf_gpio_cfg_output(DISPLAY_CS_PIN);
    nrf_gpio_pin_write(DISPLAY_CS_PIN, 1);

    nrf_gpio_pin_write(DISPLAY_BACKLIGHT_LOW, 0);
    nrf_gpio_pin_write(DISPLAY_BACKLIGHT_MID, 0);
    nrf_gpio_pin_write(DISPLAY_BACKLIGHT_HIGH, 0);


    nrf_gpio_cfg_output(DISPLAY_BACKLIGHT_LOW);
    nrf_gpio_cfg_output(DISPLAY_BACKLIGHT_MID);
    nrf_gpio_cfg_output(DISPLAY_BACKLIGHT_HIGH);

    /** CST816S **/
    nrf_gpio_cfg_output(TP_RESET_PIN);  // Reset Pin
    nrf_gpio_cfg_sense_input(TP_INT_PIN, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_LOW);

    /** BMA421 **/
    nrf_gpio_cfg_input(BMA421_INT_PIN, NRF_GPIO_PIN_PULLDOWN);

    /** Button **/
    nrf_gpio_cfg_output(PUSH_BUTTON_OUT_PIN);
    nrf_gpio_cfg_input(PUSH_BUTTON_IN_PIN, NRF_GPIO_PIN_PULLDOWN);

    /** SGM40561 **/
    nrf_gpio_cfg_input(POWER_PRESENCE_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(CHARGE_INDICATION_PIN, NRF_GPIO_PIN_PULLUP);
//    nrf_gpio_cfg_output(VCC_POWER_CONTROL_PIN);
}

void config_peripherals(void) {
    config_SGM40561();
    config_spi_master();
    config_twi();
    config_pwm();
}


/** GPIOTE **/

void config_gpio_interrupts(void * handler_function)
{
    ret_code_t err_code;
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t button_in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(TP_INT_PIN,
                                      &button_in_config,
                                      handler_function);
    APP_ERROR_CHECK(err_code);
}


/** SLEEP **/
void sleep_peripherals(void)
{
    // TODO: standardize naming
    twi_disable();
    spi_disable();
}
