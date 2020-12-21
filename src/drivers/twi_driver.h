//
// Created by jack on 8/2/20.
//

#ifndef JDSMARTWATCHPROJECT_SRC_DRIVERS_TWI_DRIVER_H
#define JDSMARTWATCHPROJECT_SRC_DRIVERS_TWI_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "nrf_drv_twi.h"

void config_twi(void);
void twi_disable(void);
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);
//void twi_rx(uint8_t devAddr, uint8_t * buffer, uint32_t size, void * intf_ptr);
void twi_read_reg(uint8_t devAddr, uint8_t reg, uint8_t * buffer, uint8_t size);
//void twi_tx(uint8_t devAddr, uint8_t * buffer, uint8_t size, void * intf_ptr);
void twi_tx(uint8_t devAddr, uint8_t * buffer, uint8_t size);
void twi_rx(uint8_t devAddr, uint8_t * buffer, uint32_t size);

#endif //JDSMARTWATCHPROJECT_SRC_DRIVERS_TWI_DRIVER_H
