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
void twi_tx(uint8_t dev_addr, uint8_t * buffer, uint8_t size);
void twi_reg_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
void twi_rx(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer, uint8_t size);
void twi_reg_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer);

#endif //JDSMARTWATCHPROJECT_SRC_DRIVERS_TWI_DRIVER_H
