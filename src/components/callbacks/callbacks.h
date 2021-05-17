#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

void init_gpio_interrupts(void);
void gpio_irq_callback(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif /* CALLBACKS_H_ */