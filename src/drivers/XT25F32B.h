#include "app_config.h"
#include <stdint.h>


/** Page 15 **/
#define SPI_FLASH_WREN              0x06
#define SPI_FLASH_VOLATILE_WREN     0x50
#define SPI_FLASH_WRDI              0x04
#define SPI_FLASH_RDSR              0x35  // or 0x05 - returns 2 bytes
#define SPI_FLASH_WRSR              0x01  // write 2 bytes
#define SPI_FLASH_READ              0x03  // followed by a 3-byte address
#define SPI_FLASH_FAST_READ         0x0B  // followed by a 3-byte address and a dummy byte
#define SPI_FLASH_PAGE_PROGRAM      0x02  // followed by 3-byte address and at least 1 data byte, max 256 data bytes
#define SPI_FLASH_SECTOR_ERASE      0x20  // followed by 3-byte address
#define SPI_FLASH_BLOCK_ERASE       0x52
#define SPI_FLASH_CHIP_ERASE        0x60  // or 0xC7 erase all data on the chip
#define SPI_FLASH_POWER_DOWN        0xB9
#define SPI_FLASH_RDI               0xAB  // release from Power-Down and Read Device ID, if not powered down send 3 dummy bytes
#define SPI_FLASH_RDID              0x9F  // read manufacture ID and device ID
#define SPI_FLASH_REMS              0x90  // read manufacture ID/Device ID
#define SPI_FLASH_ENABLE_RESET      0x66
#define SPI_FLASH_RESET             0x99
#define SPI_FLASH_RUID              0x5A  // follow wwith 0x00 - 0x01 - 0x94 - dummy byte - 128bit unique ID


#define SPI_FLASH_STATUS_WIP        0x01
#define SPI_FLASH_STATUS_WEL        (0x01 << 1)
#define SPI_FLASH_STATUS_BP         (0x1F << 2)
#define SPI_FLASH_STATUS_SRP0       (0x01 << 7)
#define SPI_FLASH_STATUS_SRP1       0x01
#define SPI_FLASH_STATUS_QE         0x01 << 1
#define SPI_FLASH_STATUS_LB         0x01 << 2
#define SPI_FLASH_STATUS_CMP        0x01 << 6

typedef struct
{
    uint32_t    cs_pin;

    // bool (* spi_write)(uint8_t * const data, uint32_t size);
    // bool (* spi_read)(uint8_t * const buffer, uint32_t size);
} XT25F32B_config_t;


typedef struct
{
    XT25F32B_config_t  *config;
    uint8_t             status[2];
    uint8_t             manufacturerID;
    uint8_t             deviceID;
} XT25F32B_t;


bool XT25F32B_init(XT25F32B_config_t * config);

uint8_t XT25F32B_read_status(void);
uint8_t XT25F32B_read_block(uint32_t block, void * buffer, uint32_t size);


uint8_t XT25F32B_getManufacturerID(void);
uint8_t XT25F32B_getDeviceID(void);