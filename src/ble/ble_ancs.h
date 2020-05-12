//
// Created by jack on 5/12/20.
//

#ifndef JDSMARTWATCHPROJECT_BLE_ANCS_H
#define JDSMARTWATCHPROJECT_BLE_ANCS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
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


/**@brief String literals for the iOS notification categories. used then printing to UART. */
static char const * lit_catid[BLE_ANCS_NB_OF_CATEGORY_ID] = {
                "Other",
                "Incoming Call",
                "Missed Call",
                "Voice Mail",
                "Social",
                "Schedule",
                "Email",
                "News",
                "Health And Fitness",
                "Business And Finance",
                "Location",
                "Entertainment"
        };

/**@brief String literals for the iOS notification event types. Used then printing to UART. */
char const * lit_eventid[BLE_ANCS_NB_OF_EVT_ID] = {
                "Added",
                "Modified",
                "Removed"
        };

/**@brief String literals for the iOS notification attribute types. Used when printing to UART. */
char const * lit_attrid[BLE_ANCS_NB_OF_NOTIF_ATTR] = {
                "App Identifier",
                "Title",
                "Subtitle",
                "Message",
                "Message Size",
                "Date",
                "Positive Action Label",
                "Negative Action Label"
        };

/**@brief String literals for the iOS notification attribute types. Used When printing to UART. */
char const * lit_appid[BLE_ANCS_NB_OF_APP_ATTR] = {
                "Display Name"
        };

#endif //JDSMARTWATCHPROJECT_BLE_ANCS_H
