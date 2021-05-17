//
// Created by jack on 8/2/20.
//

#include "twi_driver.h"
#include "app_config.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "nrf52.h"
//#include "nrf_twim.h"
//#include "nrf_drv_twi.h"
#include "nrf_twi_mngr.h"
//#include "nrfx_twim.h"
#include "nrf_gpio.h"

extern SemaphoreHandle_t twi_mutex;


NRF_TWI_MNGR_DEF(twi_mngr_instance, 4, 0);

void config_twi(void)
{
    nrf_drv_twi_config_t twi_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    twi_config.scl = SENSORS_SCL_PIN;
    twi_config.sda = SENSORS_SDA_PIN;
    twi_config.frequency = NRF_DRV_TWI_FREQ_400K;
    nrf_twi_mngr_init(&twi_mngr_instance, &twi_config);
}

void twi_disable(void)
{
    nrf_twim_disable(SENSORS_NRF_TWIM);
}

void twi_tx(uint8_t dev_addr, uint8_t * buffer, uint8_t size)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
        nrf_twi_mngr_transfer_t const write_transfer[] = {
            NRF_TWI_MNGR_WRITE(dev_addr, buffer, size, 0)
        };
        ret_code_t error_code = nrf_twi_mngr_perform(&twi_mngr_instance, NULL, write_transfer, 1, NULL);
        APP_ERROR_CHECK(error_code);
        xSemaphoreGive(twi_mutex);
    }
}

void twi_reg_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
        uint8_t buffer[2] = {reg_addr, data};
        nrf_twi_mngr_transfer_t const write_transfer[] = {
            NRF_TWI_MNGR_WRITE(dev_addr, buffer, 2, 0),
        };
        ret_code_t error_code = nrf_twi_mngr_perform(&twi_mngr_instance, NULL, write_transfer, 1, NULL);
        APP_ERROR_CHECK(error_code);
        xSemaphoreGive(twi_mutex);
    }
}

void twi_rx(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer, uint8_t size)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
        nrf_twi_mngr_transfer_t const read_transfer[] = {
            NRF_TWI_MNGR_WRITE(dev_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
            NRF_TWI_MNGR_READ(dev_addr, buffer, 1, 0)
        };
        ret_code_t error_code = nrf_twi_mngr_perform(&twi_mngr_instance, NULL, read_transfer, 2, NULL);
        APP_ERROR_CHECK(error_code);
        xSemaphoreGive(twi_mutex);
    }
}

void twi_reg_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
        nrf_twi_mngr_transfer_t const read_transfer[] = {
            NRF_TWI_MNGR_WRITE(dev_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
            NRF_TWI_MNGR_READ(dev_addr, buffer, 1, 0)
        };
        ret_code_t error_code = nrf_twi_mngr_perform(&twi_mngr_instance, NULL, read_transfer, 2, NULL);
        APP_ERROR_CHECK(error_code);
        xSemaphoreGive(twi_mutex);
    }
}
