//
// Created by jack on 7/23/20.
//

#ifndef JDSMARTWATCHPROJECT_SRC_DRIVERS_SPI_DRIVER_H
#define JDSMARTWATCHPROJECT_SRC_DRIVERS_SPI_DRIVER_H

#include "FreeRTOS.h"
#include "semphr.h"

#include "nrf52.h"
#include "nrf_spim.h"
#include "nrfx_spim.h"
#include "nrf_gpio.h"

void spim_evt_handler(nrfx_spim_evt_t const * p_event, void * p_context);
void config_spi_master(void);
void spi_end_event(void);
bool spi_write(uint32_t cs_pin, uint8_t * const data, uint32_t size);
bool spi_read(uint32_t cs_pin, uint8_t * reg_buffer, uint8_t * rx_buffer);

void spi_wakeup(void);
void spi_disable(void);
void setup_workaround_spim(uint32_t ppi_channel, uint32_t gpiote_channel);
void disable_workaround_spim(uint32_t ppi_channel, uint32_t gpiote_channel);


#endif //JDSMARTWATCHPROJECT_SRC_DRIVERS_SPI_DRIVER_H
