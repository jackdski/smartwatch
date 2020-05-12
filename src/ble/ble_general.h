//
// Created by jack on 5/10/20.
//

#ifndef JDSMARTWATCHPROJECT_BLE_GENERAL_H
#define JDSMARTWATCHPROJECT_BLE_GENERAL_H

// nRF files
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "boards.h"
#include "fds.h"

// BLE files
#include "ble.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_err.h"
#include "ble_cts_c.h"
#include "nrf_ble_gatts_c.h"
#include "ble_db_discovery.h"
#include "nrf_ble_ancs_c.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "nrf_ble_qwr.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gq.h"

// Application Includes
#include "ble_cus.h"
#include "ble_ancs.h"

// BLE settings
#define DEVICE_NAME                     "jdski_smartwatch"    /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "JD"                    /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */

#define APP_ADV_DURATION                18000                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_OBSERVER_PRIO           3                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                       /**< A tag identifying the SoftDevice BLE configuration. */


#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< Fast advertising interval (in units of 0.625 ms). The default value corresponds to 25 ms. */
#define APP_ADV_SLOW_INTERVAL           0x0C80                                      /**< Slow advertising interval (in units of 0.625 ms). The default value corresponds to 2 seconds. */

#define APP_ADV_FAST_DURATION           3000                                        /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_DURATION           18000                                       /**< The advertising duration of slow advertising in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define FREERTOS
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define NOTIFICATION_INTERVAL           APP_TIMER_TICKS(1000)

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

typedef struct {
    bool connected;
    bool cts_discovered;
    bool cts_request;
    bool ancs_discovered;
    bool gatts_discovered;
} BLE_Manager_t;


// BLE Manager Task
void BLE_Manager_Task(void * arg);
void ble_manager_init(BLE_Manager_t * p_inst);

// init functions
void ble_stack_init(void);
void services_init(void) ;
void gap_params_init(void);
void gatt_init(void);
void db_discovery_init(void);
void advertising_init(void);
void conn_params_init(void);
void peer_manager_init(void);
void apple_notification_setup(void);
void advertising_start(void * p_erase_bonds);

// handler functions
void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);
void gatts_c_evt_handler(nrf_ble_gatts_c_evt_t * p_evt);
void ancs_c_evt_handler(ble_ancs_c_evt_t * p_evt);
void pm_evt_handler(pm_evt_t const * p_evt);
void db_disc_handler(ble_db_discovery_evt_t * p_evt);
void nrf_qwr_error_handler(uint32_t nrf_error);
void current_time_error_handler(uint32_t nrf_error);
void conn_params_error_handler(uint32_t nrf_error);
void apple_notification_error_handler(uint32_t nrf_error);
void notification_timeout_handler(void * p_context);

// on event functions
void on_cus_evt(ble_cus_t * p_cus_service, ble_cus_evt_t * p_evt);
void on_cts_c_evt(ble_cts_c_t * p_cts, ble_cts_c_evt_t * p_evt);
void on_adv_evt(ble_adv_evt_t ble_adv_evt);
void on_conn_params_evt(ble_conn_params_evt_t * p_evt);

// misc
void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size);
void current_time_print(ble_cts_c_evt_t * p_evt);
void delete_bonds(void);

#endif //JDSMARTWATCHPROJECT_BLE_GENERAL_H
