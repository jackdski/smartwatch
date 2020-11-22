//
// Created by jack on 9/2/20.
//

#include "HRS3300.h"
#include "drivers/twi_driver.h"

#include <stdint.h>
#include <stdbool.h>

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

bool HRS3300_init(void)
{
    bool stop = true;
    HRS3300_enable();

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

bool HRS3300_get_device_id(void)
{
    uint8_t dev_id = 0;
    twi_read_reg(HRS_ADDRESS, HRS_DEVICE_ID_REG, &dev_id, 1);
    NRF_LOG_INFO("HRS3300 DEVICE ID: %d", dev_id);

    bool ret = false;
    if (dev_id == HRS_DEVICE_ID) {
        ret = true;
    }
    return ret;
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

uint32_t HRS3300_get_sample(bool channel)
{
    uint8_t rxdata[8] = {0};
    if(channel == 0)
    {
        twi_read_reg(HRS_ADDRESS, HRS_C0DATAH_REG, &rxdata[0], 1);
        twi_read_reg(HRS_ADDRESS, HRS_C0DATAM_REG, &rxdata[1], 1);
        twi_read_reg(HRS_ADDRESS, HRS_C0DATAL_REG, &rxdata[2], 1);
        return ((rxdata[2] & 0x30) << 16) | (rxdata[0] << 8) | ((rxdata[1] & 0x0F) << 4) | (rxdata[2] & 0x0F);
    }
    else
    {
        twi_read_reg(HRS_ADDRESS, HRS_C1DATAH_REG, &rxdata[0], 1);
        twi_read_reg(HRS_ADDRESS, HRS_C1DATAM_REG, &rxdata[1], 1);
        twi_read_reg(HRS_ADDRESS, HRS_C1DATAL_REG, &rxdata[2], 1);
        return ((rxdata[0] & 0x3F) << 11) | (rxdata[1] << 3) | (rxdata[2] & 0x07);
    }
}

void HRS3300_set_pdrive_current(ePDriveCurrent current)
{
    bool stop = true;
    uint8_t data[] = {HRS_PDRIVER_REG, (HRS_TWELVE_MILLIAMP_LED_DRIVE << HRS_PDRIVER_CURRENT_OFF) | HRS_PDRIVER_PON};
    twi_tx(HRS_ADDRESS, data, sizeof(data), &stop);
}
