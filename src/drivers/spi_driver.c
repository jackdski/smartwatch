//
// Created by jack on 7/23/20.
//

#include "spi_driver.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "nrf52.h"
#include "nrf_gpio.h"
#include "nrfx_spim.h"

extern SemaphoreHandle_t spi_mutex;

#define USE_NRF_SPIM_DRIVER_PRIVATE

#ifdef USE_NRF_SPIM_DRIVER_PRIVATE

uint32_t current_cs_pin = 0;
volatile bool spi_xfer_done = false;

nrfx_spim_t m_spim = {
    .p_reg = SPIM_BASE,
    .drv_inst_idx = NRFX_SPIM1_INST_IDX
};

nrfx_spim_config_t spim_config = {
    .sck_pin = SPI_SCK_PIN,
    .mosi_pin = SPI_MOSI_PIN,
    .miso_pin = SPI_MISO_PIN,
    .ss_pin = NRFX_SPIM_PIN_NOT_USED,
    .ss_active_high = false,
    .irq_priority = NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0xFF,
    .frequency = NRF_SPIM_FREQ_8M,
    .mode = NRF_SPIM_MODE_0,
    .bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST
};

void spim_evt_handler(nrfx_spim_evt_t const * p_event, void * p_context)
{
    if(p_event->type == NRFX_SPIM_EVENT_DONE)
    {
        spi_xfer_done = true;
    }
}

void config_spi_master(void)
{
    nrfx_spim_init(&m_spim, &spim_config, spim_evt_handler, NULL);
}

bool spi_write(uint32_t cs_pin, uint8_t * data, uint32_t size)
{
    if(xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(500)) == pdTRUE)
    {
        // Package SPIM xfer
    //    uint8_t spi_data[size];
    //    memcpy(spi_data, data, size);
        nrfx_spim_xfer_desc_t xfer = {
            .p_tx_buffer = data,
            .tx_length = size,
            .p_rx_buffer = NULL,
            .rx_length = 0
        };
        current_cs_pin = cs_pin;
        nrf_gpio_pin_clear(current_cs_pin);
        spi_xfer_done = false;
        nrfx_spim_xfer(&m_spim, &xfer, 0);
        while(!spi_xfer_done);
        nrf_gpio_pin_set(current_cs_pin);
        xSemaphoreGive(spi_mutex);
//        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//        xSemaphoreGiveFromISR(spi_mutex, &xHigherPriorityTaskWoken);
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        return false;
    }
    return true;
}


#else /* USE_NRF_SPI_DRIVER */
#include "nrf_spim.h"

static uint32_t tx_buffer_size = 0;
static uint8_t * tx_buffer_addr = 0;
static uint32_t current_cs_pin = 0;

void config_spi_master(void)
{
    // Configure SPI frequency to 8 Mbps
    nrf_spim_frequency_set(SPIM_BASE, NRF_SPIM_FREQ_8M);

    // Clear EVT flags
    nrf_spim_event_clear(SPIM_BASE, NRF_SPIM_EVENT_STOPPED);
    nrf_spim_event_clear(SPIM_BASE, NRF_SPIM_EVENT_ENDRX);
    nrf_spim_event_clear(SPIM_BASE, NRF_SPIM_EVENT_END);
    nrf_spim_event_clear(SPIM_BASE, NRF_SPIM_EVENT_ENDTX);
    nrf_spim_event_clear(SPIM_BASE, NRF_SPIM_EVENT_STARTED);

    // Set interrupts
    nrf_spim_int_enable(SPIM_BASE, NRF_SPIM_INT_END_MASK);

    // Enable peripheral
    nrf_spim_enable(SPIM_BASE);

    // Enable IRQ and Set Priority
    NRFX_IRQ_PRIORITY_SET(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, 3);
    NRFX_IRQ_ENABLE(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
}

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    if(nrf_spim_event_check(SPIM_BASE, NRF_SPIM_EVENT_END) && nrf_spim_int_enable_check(SPIM_BASE, NRF_SPIM_INT_END_MASK)) {
        nrf_spim_event_clear(SPIM_BASE, NRF_SPIM_EVENT_END);
        spi_end_event();
    }
}

void spi_end_event(void)
{
//    NRF_LOG_INFO("SPIM TX Buf: %d", tx_buffer_size);
    // Send more if there is more
    if(tx_buffer_size > 0) {
        nrf_spim_tx_buffer_set(SPIM_BASE, tx_buffer_addr, tx_buffer_size);

        // Update buffer
        uint32_t size_update = (tx_buffer_size < 255) ? 0 : (tx_buffer_size - 255);
        tx_buffer_size -= size_update;
        tx_buffer_addr += size_update;

        nrf_spim_task_trigger(SPIM_BASE, NRF_SPIM_TASK_START);
    }
    else {  // End communication sequence
        nrf_gpio_pin_set(current_cs_pin);
        tx_buffer_addr = 0;
        tx_buffer_size = 0;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spi_mutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

bool spi_write(uint32_t cs_pin, uint8_t * data, uint32_t size)
{
    if(data == NULL)
    {
        return false;
    }

    // Send Data
    if(xSemaphoreTake(spi_mutex, portMAX_DELAY) == pdTRUE)
    {
        current_cs_pin = cs_pin;
        tx_buffer_size = size;
        tx_buffer_addr = data;

        if(size == 1)
        {
            setup_workaround_spim(0,0);
        } else {
            disable_workaround_spim(0, 0);
        }

        // Select Chip, Set TX Buffer
        nrf_gpio_pin_clear(current_cs_pin);
        nrf_spim_tx_buffer_set(SPIM_BASE, tx_buffer_addr, tx_buffer_size);

        // Update buffer
        uint32_t size_update = (tx_buffer_size < 255) ? tx_buffer_size : (tx_buffer_size - 255);
        tx_buffer_size -= size_update;
        tx_buffer_addr += size_update;

        // Start
        nrf_spim_task_trigger(SPIM_BASE, NRF_SPIM_TASK_START);

        if(size == 1)
        {
            while(SPIM_BASE->EVENTS_END == 0);
            nrf_gpio_pin_set(current_cs_pin);
            tx_buffer_addr = 0;
            xSemaphoreGive(spi_mutex);
        }
    }
    else {  // Failed to take mutex
        return false;
    }

    return true;
}

void spi_wakeup(void)
{
    xSemaphoreTake(spi_mutex, 0);
    nrf_spim_enable(SPIM_BASE);
    xSemaphoreGive(spi_mutex);
}

void spi_disable(void)
{
    xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(100));
    nrf_spim_disable(SPIM_BASE);
    xSemaphoreGive(spi_mutex);
}

void setup_workaround_spim(uint32_t ppi_channel, uint32_t gpiote_channel)
{
    // Create an event when SCK toggles.
    NRF_GPIOTE->CONFIG[gpiote_channel] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
        (SPIM_BASE->PSEL.SCK << GPIOTE_CONFIG_PSEL_Pos) |
        (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);

    NRF_PPI->CH[ppi_channel].EEP = (uint32_t) &NRF_GPIOTE->EVENTS_IN[gpiote_channel];
    NRF_PPI->CH[ppi_channel].TEP = (uint32_t) &SPIM_BASE->TASKS_STOP;
    NRF_PPI->CHENSET = 1U << ppi_channel;
    SPIM_BASE->EVENTS_END = 0;
    SPIM_BASE->INTENCLR = (1<<6);
    SPIM_BASE->INTENCLR = (1<<1);
    SPIM_BASE->INTENCLR = (1<<19);
}

void disable_workaround_spim(uint32_t ppi_channel, uint32_t gpiote_channel) {
    NRF_GPIOTE->CONFIG[gpiote_channel] = 0;
    NRF_PPI->CH[ppi_channel].EEP = 0;
    NRF_PPI->CH[ppi_channel].TEP = 0;
    NRF_PPI->CHENSET = ppi_channel;
    SPIM_BASE->EVENTS_END = 0;
    SPIM_BASE->INTENSET = (1<<6);
    SPIM_BASE->INTENSET = (1<<1);
    SPIM_BASE->INTENSET = (1<<19);
}

#endif

void spi_wakeup(void)
{
    xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(100));
    nrf_spim_enable(SPIM_BASE);
    xSemaphoreGive(spi_mutex);
}

void spi_disable(void)
{
    xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(100));
    nrf_spim_disable(SPIM_BASE);
    xSemaphoreGive(spi_mutex);
}