//
// Created by jack on 9/2/20.
//

#include "HRS3300.h"
#include "drivers/twi_driver.h"

#include <stdint.h>
#include <stdbool.h>

bool HRS3300_init(void)
{
    bool stop = true;
    HRS3300_enable();

    {
        uint8_t dev_id = 0;
        twi_read_reg(HRS_ADDRESS, HRS_DEVICE_ID_REG, &dev_id, 1);
        if(dev_id != HRS_DEVICE_ID)
        {
            return false;
        }
    }

    {
        uint8_t data[] = {HRS_RES_REG, HRS_RES_16BIT};
        twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
    }

    {
        uint8_t data[] = {HRS_HGAIN_REG, HRS_HGAIN_DOUBLE};
        twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
    }

    {
        uint8_t data[] = {HRS_PDRIVER_REG, (HRS_PDRIVER_PON | (1 << 6))};
        twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
    }
    return true;
}

void HRS3300_enable(void)
{
    bool stop = true;
    uint8_t data[] = {HRS_ENABLE_REG, (HRS_ENABLE_HEN | HRS_ENABLE_DEFAULT)};
    twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
}

void HRS3300_low_power(void)
{
    bool stop = true;
    uint8_t data[] = {HRS_PDRIVER_REG, HRS_PDRIVER_PON};
    twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
}

void HRS3300_change_sample_rate(uint8_t rate)
{

}

void HRS3300_set_pdrive_current(ePDriveCurrent current)
{
    bool stop = true;
    uint8_t data[] = {HRS_PDRIVER_REG, (HRS_TWELVE_MILLIAMP_LED_DRIVE << HRS_PDRIVER_CURRENT_OFF) | HRS_PDRIVER_PON};
    twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
}
