#include "watchdog.h"
#include "nrf_drv_wdt.h"


nrf_drv_wdt_channel_id m_channel_id;                    //  watchdog channel

void init_watchdog(void)
{
    ret_code_t err_code;
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    err_code = nrf_drv_wdt_init(&config, watchdog_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
    nrf_drv_wdt_channel_feed(m_channel_id);
}

void watchdog_kick(void)
{
    nrf_drv_wdt_feed();
}


/**
 * @brief WDT events handler.
 */
void watchdog_handler(void)
{
    // TODO
}