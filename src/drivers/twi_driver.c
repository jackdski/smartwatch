//
// Created by jack on 8/2/20.
//

#include "twi_driver.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "nrf52.h"
#include "nrf_twim.h"
#include "nrf_drv_twi.h"
#include "nrfx_twim.h"
#include "nrf_gpio.h"

extern SemaphoreHandle_t twi_mutex;

#define USE_NRF_TWI_DRIVER
#ifdef USE_NRF_TWI_DRIVER

//NRFX_TWIM_INSTANCE(0);
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

nrfx_twim_t m_twim = {
    .p_twim = SENSORS_NRF_TWIM,
    .drv_inst_idx = NRFX_TWIM0_INST_IDX
};

//nrfx_twim_config_t twim_config = {
//    .scl = SENSORS_SCL_PIN,
//    .sda = SENSORS_SDA_PIN,
//    .frequency = NRF_TWIM_FREQ_400K,
//    .interrupt_priority = NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY,
//    .hold_bus_uninit = true
//};

nrf_drv_twi_config_t twim_config = {
    .scl = SENSORS_SCL_PIN,
    .sda = SENSORS_SDA_PIN,
    .frequency = NRF_DRV_TWI_FREQ_400K,
    .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
    .clear_bus_init = true,
    .hold_bus_uninit = true
};

//void twi_callback(nrfx_twim_evt_t const * p_event, void * p_context)
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    switch (p_event->type)
    {
//        case NRFX_TWIM_EVT_DONE:
        case NRF_DRV_TWI_EVT_DONE:
            if(p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xSemaphoreGiveFromISR(twi_mutex, &xHigherPriorityTaskWoken);
                // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
            break;

//        case NRFX_TWIM_EVT_BUS_ERROR:
//            nrfx_twim_bus_recover(SENSORS_SCL_PIN, SENSORS_SDA_PIN);
//            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//            xSemaphoreGiveFromISR(twi_mutex, &xHigherPriorityTaskWoken);
    //        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            break;
        default:
            break;
    }
}

void config_twi(void)
{
//    nrfx_twim_init(&m_twim, &twim_config, twi_callback, NULL);
    ret_code_t err_code;
    err_code = nrf_drv_twi_init(&m_twi, &twim_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

void twi_disable(void)
{
    nrf_twim_disable(SENSORS_NRF_TWIM);
}

void twi_tx(uint8_t devAddr, uint8_t * buffer, uint8_t size)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
//        nrfx_twim_tx(&m_twim, devAddr, buffer, size, intf_ptr);
        nrf_drv_twi_tx(&m_twi, devAddr, buffer, size, false);
//        xSemaphoreGive(twi_mutex);
    }
}

void twi_rx(uint8_t devAddr, uint8_t * buffer, uint32_t size)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
//        nrfx_twim_rx(&m_twim, devAddr, buffer, size);
//        xSemaphoreGive(twi_mutex);
        nrf_drv_twi_rx(&m_twi, devAddr, buffer, size);
    }
}

void twi_read_reg(uint8_t devAddr, uint8_t reg, uint8_t * buffer, uint8_t size)
{
    if(xSemaphoreTake(twi_mutex, portMAX_DELAY) == pdPASS) {
        const uint8_t read_reg = reg;
        nrf_drv_twi_tx(&m_twi, devAddr, &read_reg, 1, true);
        nrf_drv_twi_rx(&m_twi, devAddr, buffer, size);
    }
//    twi_tx(devAddr, &reg, 1, (void *)1);
//    twi_rx(devAddr, buffer, size, NULL);
}

#else /* USE_NRF_TWI_DRIVER */
void config_twi(void)
{
    nrf_twim_frequency_set(SENSORS_NRF_TWIM, NRF_TWIM_FREQ_400K);

    // Clear all event flags
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_STOPPED);
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_ERROR);
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_SUSPENDED);
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_RXSTARTED);
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_TXSTARTED);
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_LASTRX);
    nrf_twim_event_clear(SENSORS_NRF_TWIM,NRF_TWIM_EVENT_LASTTX);

    nrf_twim_enable(SENSORS_NRF_TWIM);
}

void twi_read_reg(uint8_t devAddr, uint8_t reg, uint8_t * buffer, uint8_t size)
{
    xSemaphoreTake(twi_mutex, portMAX_DELAY);
    bool stop = true;
    twi_tx(devAddr, &reg, 1, &stop);
    twi_rx(devAddr, buffer, size, &stop);
    xSemaphoreGive(twi_mutex);
}

void twi_rx(uint8_t devAddr, uint8_t * buffer, uint32_t size, void * intf_ptr)
{
    xSemaphoreTake(twi_mutex, portMAX_DELAY);
    nrf_twim_address_set(SENSORS_NRF_TWIM, devAddr);
    nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_RESUME);
    nrf_twim_rx_buffer_set(SENSORS_NRF_TWIM, buffer, size);
    nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_STARTRX);

    while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_RXSTARTED) && !nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_ERROR));
    nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_RXSTARTED);

    while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_LASTRX) && !nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_ERROR));
    nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_LASTRX);

    if(((bool)intf_ptr == true) || SENSORS_NRF_TWIM->EVENTS_ERROR)
    {
        nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_STOP);
        while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_STOPPED));
        nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_STOPPED);
    }
    else
    {
        nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_SUSPEND);
        while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_SUSPENDED));
        nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_SUSPENDED);
    }

    if(SENSORS_NRF_TWIM->EVENTS_ERROR)
    {
        SENSORS_NRF_TWIM->EVENTS_ERROR = 0;
    }
}

void twi_tx(uint8_t devAddr, uint8_t * buffer, uint8_t size, void * intf_ptr)
{
    xSemaphoreTake(twi_mutex, portMAX_DELAY);
    nrf_twim_address_set(SENSORS_NRF_TWIM, devAddr);
    nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_RESUME);
    nrf_twim_tx_buffer_set(SENSORS_NRF_TWIM, buffer, size);
    nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_STARTTX);

    while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_TXSTARTED) && !nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_ERROR));
    nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_TXSTARTED);

    while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_LASTTX) && !nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_ERROR));
    nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_LASTTX);

    if(((bool)intf_ptr == true) || SENSORS_NRF_TWIM->EVENTS_ERROR)
    {
        nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_STOP);
        while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_STOPPED));
        nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_STOPPED);
    }
    else
    {
        nrf_twim_task_trigger(SENSORS_NRF_TWIM, NRF_TWIM_TASK_SUSPEND);
        while(!nrf_twim_event_check(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_SUSPENDED));
        nrf_twim_event_clear(SENSORS_NRF_TWIM, NRF_TWIM_EVENT_SUSPENDED);
    }

    if(SENSORS_NRF_TWIM->EVENTS_ERROR)
    {
        SENSORS_NRF_TWIM->EVENTS_ERROR = 0;
    }
}
#endif /* USE_NRF_TWI_DRIVER */
