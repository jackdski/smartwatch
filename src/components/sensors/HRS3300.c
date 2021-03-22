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

HRS3300_t hrs3300 = {
    .ID         = 0,
    .enable     = false,
    .CH1DATA    = 0,
    .CH0DATA    = 0,
    .HWT        = HRS_CONVERSION_WAIT_TIME_400MS,
    .PDRIVE     = HRS_PDRIVE_12_5MA,
    .ALS_RES    = HRS_ALS_RES_12BITS,
    .HGAIN      = HRS_HGAIN_X2,
    .write_reg  = twi_reg_write,
    .read_reg   = twi_reg_read
};

bool HRS3300_init(void)
{
    if(HRS3300_get_device_id())
    {
        HRS3300_set_conversion_wait_time(hrs3300.HWT);
        HRS3300_set_pdrive_current(hrs3300.PDRIVE);
        HRS3300_set_resolution(hrs3300.ALS_RES);
        HRS3300_set_hgain(hrs3300.HGAIN);
        HRS3300_enable(hrs3300.enable);
        return true;
    }
    return false;
}

void HRS3300_enable(bool enable)
{
    hrs3300.enable = enable;
    uint8_t data;

    // ENABLE
    data = HRS_PACK_ENABLE_REG(hrs3300.enable, hrs3300.HWT, hrs3300.PDRIVE);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);

    // LED DRIVE
    data = HRS_PACK_LED_DRIVE_REG(hrs3300.enable, hrs3300.PDRIVE);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);
}

bool HRS3300_get_device_id(void)
{
    uint8_t dev_id = 0;
    hrs3300.read_reg(HRS_ADDRESS, HRS_DEVICE_ID_REG, &dev_id);
    NRF_LOG_INFO("HRS3300 DEVICE ID: %d", dev_id);
    return (dev_id == HRS_DEVICE_ID);
}

void HRS3300_set_conversion_wait_time(eHRS_HWT wait_time)
{
    hrs3300.HWT = wait_time;
    uint8_t data = HRS_PACK_ENABLE_REG(hrs3300.enable, hrs3300.HWT, hrs3300.PDRIVE);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);
}

uint32_t HRS3300_get_sample(bool channel)
{
    uint8_t high, medium, low;
    if(channel == 0)
    {
        hrs3300.read_reg(HRS_ADDRESS, HRS_REG_C0DATAH, &high);
        hrs3300.read_reg(HRS_ADDRESS, HRS_REG_C0DATAM, &medium);
        hrs3300.read_reg(HRS_ADDRESS, HRS_REG_C0DATAL, &low);
        hrs3300.CH0DATA = HRS_CH0_SET(high, medium, low);
        return hrs3300.CH0DATA;
    }
    else
    {
        hrs3300.read_reg(HRS_ADDRESS, HRS_REG_C1DATAH, &high);
        hrs3300.read_reg(HRS_ADDRESS, HRS_REG_C1DATAM, &medium);
        hrs3300.read_reg(HRS_ADDRESS, HRS_REG_C1DATAL, &low);
        hrs3300.CH1DATA = HRS_CH1_SET(high, medium, low);
        return hrs3300.CH1DATA;
    }
}

void HRS3300_set_pdrive_current(eHRS_PDRIVE pdrive)
{
    hrs3300.PDRIVE = pdrive;
    uint8_t data;

    // ENABLE
    data = HRS_PACK_ENABLE_REG(hrs3300.enable, hrs3300.HWT, hrs3300.PDRIVE);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);

    // LED DRIVE
    data = HRS_PACK_LED_DRIVE_REG(hrs3300.enable, hrs3300.PDRIVE);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);
}

void HRS3300_set_resolution(eHRS_ALS_RES res)
{
    hrs3300.ALS_RES = res;
    uint8_t data = HRS_ALS_RES_PACK(hrs3300.ALS_RES);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);
}

void HRS3300_set_hgain(eHRS_HGAIN hgain)
{
    hrs3300.HGAIN = hgain;
    uint8_t data = HRS_HGAIN_PACK(hrs3300.HGAIN);
    hrs3300.write_reg(HRS_ADDRESS, HRS_REG_ENABLE, data);
}

