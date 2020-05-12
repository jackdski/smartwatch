//
// Created by jack on 5/12/20.
//

#ifndef JDSMARTWATCHPROJECT_BLE_ANCS_H
#define JDSMARTWATCHPROJECT_BLE_ANCS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// nRF files
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_err.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "nrf_ble_gatts_c.h"
#include "nrf_ble_ancs_c.h"
#include "ble_conn_state.h"
#include "nrf_ble_gq.h"

#define ATTR_DATA_SIZE                 BLE_ANCS_ATTR_DATA_MAX                 /**< Allocated size for attribute data. */

extern char const * lit_catid[BLE_ANCS_NB_OF_CATEGORY_ID];
extern char const * lit_eventid[BLE_ANCS_NB_OF_EVT_ID];
extern char const * lit_attrid[BLE_ANCS_NB_OF_NOTIF_ATTR];
extern char const * lit_appid[BLE_ANCS_NB_OF_APP_ATTR];

// Functions
void ancs_notif_print(ble_ancs_c_evt_notif_t * p_notif);
void ancs_notif_attr_print(ble_ancs_c_attr_t * p_attr);
void ancs_app_attr_print(ble_ancs_c_attr_t * p_attr);
void ancs_err_code_print(uint16_t err_code_np);

#endif //JDSMARTWATCHPROJECT_BLE_ANCS_H
