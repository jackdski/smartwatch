#include "XT25F32B.h"
#include <string.h>

#include "spi_driver.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

XT25F32B_t flash_dev;


// PRIVATE FUNCTIONS
static bool XT25F32B_write_enable(void)
{
    bool ret = true;
    if (flash_dev.config != NULL)
    {
        uint8_t regBuffer = SPI_FLASH_WREN;
        spi_write(flash_dev.config->cs_pin, &regBuffer, 1);
    }
    else
    {
        ret = false;
    }
    return ret;
}


// PUBLIC FUNCTIONS

bool XT25F32B_init(XT25F32B_config_t * config)
{
    bool ret = true;
    
    memset(&flash_dev, 0, sizeof(flash_dev));
    flash_dev.config = config;

    if(flash_dev.config != NULL)
    {
        // uint8_t regBuffer[4] = {SPI_FLASH_RDID}; //, 0x00, 0x00, 0x00};
        uint8_t regBuffer = SPI_FLASH_RDID;
        // uint8_t rxBuffer[3] = {0};  // last byte is repeat
        uint8_t rxBuffer = 0;
        // spi_write(flash_dev.config->cs_pin, regBuffer, 4);
        // (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5));
        spi_read(flash_dev.config->cs_pin, &regBuffer, &rxBuffer);
        (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5));
        // flash_dev.manufacturerID = rxBuffer[0];
        // flash_dev.deviceID = rxBuffer[1];
        flash_dev.deviceID = rxBuffer;
    }
    else
    {
        ret = false;
    }
    return ret;
}

uint8_t XT25F32B_read_status(void)
{
    uint8_t regBuffer = SPI_FLASH_RDSR;
    return spi_read(flash_dev.config->cs_pin, &regBuffer, flash_dev.status);
}

uint8_t XT25F32B_read_block(uint32_t block, void * buffer, uint32_t size)
{
    return 1;
}


uint8_t XT25F32B_getManufacturerID(void)
{
    return flash_dev.manufacturerID;
}

uint8_t XT25F32B_getDeviceID(void)
{
    return flash_dev.deviceID;
}