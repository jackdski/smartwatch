//
// Created by jack on 5/12/20.
//

#include "ble_ancs.h"

/**@brief String literals for the iOS notification categories. used then printing to UART. */
char const * lit_catid[BLE_ANCS_NB_OF_CATEGORY_ID] = {
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


/**@brief Function for printing an iOS notification.
 *
 * @param[in] p_notif  Pointer to the iOS notification.
 */
void ancs_notif_print(ble_ancs_c_evt_notif_t * p_notif) {
    NRF_LOG_INFO("\r\nNotification");
    NRF_LOG_INFO("Event:       %s", (uint32_t)lit_eventid[p_notif->evt_id]);
    NRF_LOG_INFO("Category ID: %s", (uint32_t)lit_catid[p_notif->category_id]);
    NRF_LOG_INFO("Category Cnt:%u", (unsigned int) p_notif->category_count);
    NRF_LOG_INFO("UID:         %u", (unsigned int) p_notif->notif_uid);

    NRF_LOG_INFO("Flags:");
    if (p_notif->evt_flags.silent == 1)
    {
        NRF_LOG_INFO(" Silent");
    }
    if (p_notif->evt_flags.important == 1)
    {
        NRF_LOG_INFO(" Important");
    }
    if (p_notif->evt_flags.pre_existing == 1)
    {
        NRF_LOG_INFO(" Pre-existing");
    }
    if (p_notif->evt_flags.positive_action == 1)
    {
        NRF_LOG_INFO(" Positive Action");
    }
    if (p_notif->evt_flags.negative_action == 1)
    {
        NRF_LOG_INFO(" Negative Action");
    }
}

/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] p_attr Pointer to an iOS notification attribute.
 */
void ancs_notif_attr_print(ble_ancs_c_attr_t * p_attr) {
    if (p_attr->attr_len != 0)
    {
        NRF_LOG_INFO("%s: %s", (uint32_t)lit_attrid[p_attr->attr_id],
                     nrf_log_push((char *)p_attr->p_attr_data));
    }
    else if (p_attr->attr_len == 0)
    {
        NRF_LOG_INFO("%s: (N/A)", (uint32_t)lit_attrid[p_attr->attr_id]);
    }
}

/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] p_attr Pointer to an iOS App attribute.
 */
void ancs_app_attr_print(ble_ancs_c_attr_t * p_attr) {
    if (p_attr->attr_len != 0)
    {
        NRF_LOG_INFO("%s: %s", (uint32_t)lit_appid[p_attr->attr_id], (uint32_t)p_attr->p_attr_data);
    }
    else if (p_attr->attr_len == 0)
    {
        NRF_LOG_INFO("%s: (N/A)", (uint32_t) lit_appid[p_attr->attr_id]);
    }
}

/**@brief Function for printing out errors that originated from the Notification Provider (iOS).
 *
 * @param[in] err_code_np Error code received from NP.
 */
void ancs_err_code_print(uint16_t err_code_np) {
    switch (err_code_np)
    {
        case BLE_ANCS_NP_UNKNOWN_COMMAND:
            NRF_LOG_INFO("Error: Command ID was not recognized by the Notification Provider. ");
            break;

        case BLE_ANCS_NP_INVALID_COMMAND:
            NRF_LOG_INFO("Error: Command failed to be parsed on the Notification Provider. ");
            break;

        case BLE_ANCS_NP_INVALID_PARAMETER:
            NRF_LOG_INFO("Error: Parameter does not refer to an existing object on the Notification Provider. ");
            break;

        case BLE_ANCS_NP_ACTION_FAILED:
            NRF_LOG_INFO("Error: Perform Notification Action Failed on the Notification Provider. ");
            break;

        default:
            break;
    }
}