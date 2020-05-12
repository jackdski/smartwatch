//
// Created by jack on 5/10/20.
//

#include "ble_general.h"

#include "FreeRTOS.h"
#include "task.h"

NRF_BLE_GATTS_C_DEF(m_gatts_c);                         /**< GATT Service client instance. Handles Service Changed indications from the peer. */
BLE_ANCS_C_DEF(m_ancs_c);                               /**< Apple Notification Service Client instance. */
BLE_CTS_C_DEF(m_cts_c);                                 /**< Current Time service instance. */
NRF_BLE_GATT_DEF(m_gatt);
NRF_BLE_QWR_DEF(m_qwr);                                 /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);
BLE_DB_DISCOVERY_DEF(m_ble_db_discovery);               /**< DB discovery module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                        /**< BLE GATT Queue instance. */
                NRF_SDH_BLE_PERIPHERAL_LINK_COUNT,
                NRF_BLE_GQ_QUEUE_SIZE);                 /**< Advertising module instance. */

BLE_CUS_DEF(m_cus);                                     /**< Context for the Queued Write module.*/

static pm_peer_id_t m_peer_id;                                                      /**< Device reference handle to the current bonded central. */
static uint16_t     m_conn_handle = BLE_CONN_HANDLE_INVALID;

static pm_peer_id_t m_whitelist_peers[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
static uint32_t     m_whitelist_peer_cnt;
static ble_gatt_db_srv_t m_peer_srv_buf[2] = {0};                             /**< Array of services with room to store both GATT Service and ANCS. */

static ble_ancs_c_evt_notif_t m_notification_latest;                          /**< Local copy to keep track of the newest arriving notifications. */
static ble_ancs_c_attr_t      m_notif_attr_latest;                            /**< Local copy of the newest notification attribute. */
static ble_ancs_c_attr_t      m_notif_attr_app_id_latest;                     /**< Local copy of the newest app attribute. */

static uint8_t m_attr_appid[ATTR_DATA_SIZE];                                  /**< Buffer to store attribute data. */
static uint8_t m_attr_title[ATTR_DATA_SIZE];                                  /**< Buffer to store attribute data. */
static uint8_t m_attr_subtitle[ATTR_DATA_SIZE];                               /**< Buffer to store attribute data. */
static uint8_t m_attr_message[ATTR_DATA_SIZE];                                /**< Buffer to store attribute data. */
static uint8_t m_attr_message_size[ATTR_DATA_SIZE];                           /**< Buffer to store attribute data. */
static uint8_t m_attr_date[ATTR_DATA_SIZE];                                   /**< Buffer to store attribute data. */
static uint8_t m_attr_posaction[ATTR_DATA_SIZE];                              /**< Buffer to store attribute data. */
static uint8_t m_attr_negaction[ATTR_DATA_SIZE];                              /**< Buffer to store attribute data. */
static uint8_t m_attr_disp_name[ATTR_DATA_SIZE];                              /**< Buffer to store attribute data. */


static uint8_t      m_custom_value = 0;
static ble_uuid_t   m_adv_uuids[] = {
//        { CUSTOM_SERVICE_UUID, BLE_UUID_TYPE_BLE},
//        { BLE_UUID_CURRENT_TIME_SERVICE, BLE_UUID_TYPE_BLE }
        { ANCS_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN }
    };

static BLE_Manager_t ble_manager;

static char const * day_of_week[] = {
        "Unknown",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"
};

static char const * month_of_year[] = {
        "Unknown",
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
};


void BLE_Manager_Task(void * arg) {
     ble_manager_init(&ble_manager);

    while(1) {
        // Requesting Read
        if(ble_cts_c_is_cts_discovered(&m_cts_c)) {
            ble_manager.cts_discovered = true;
            ble_manager.cts_request = true;
            ble_cts_c_current_time_read(&m_cts_c);
        }
        vTaskDelay(5000);
    }
}

void ble_manager_init(BLE_Manager_t * p_inst) {
    p_inst->connected       = false;
    p_inst->cts_discovered  = false;
    p_inst->cts_request     = false;
    p_inst->ancs_discovered = false;
    p_inst->gatts_discovered = false;
}

/*
 * init functions
 */

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void) {
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void) {
    ret_code_t          err_code;
    ble_ancs_c_init_t      ancs_c_init;
    nrf_ble_gatts_c_init_t gatts_c_init;
    ble_cus_init_t      cus_init = {0};
    ble_cts_c_init_t    cts_init = {0};
    nrf_ble_qwr_init_t  qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;
    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Init the GATTS client module.
    memset(&gatts_c_init, 0, sizeof(gatts_c_init));

    gatts_c_init.evt_handler  = gatts_c_evt_handler;
    gatts_c_init.p_gatt_queue = &m_ble_gatt_queue;

    err_code = nrf_ble_gatts_c_init(&m_gatts_c, &gatts_c_init);
    APP_ERROR_CHECK(err_code);

    // Init the Apple Notification Center Service client module.
    memset(&ancs_c_init, 0, sizeof(ancs_c_init));

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER,
                                  m_attr_appid,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_app_attr_add(&m_ancs_c,
                                      BLE_ANCS_APP_ATTR_ID_DISPLAY_NAME,
                                      m_attr_disp_name,
                                      sizeof(m_attr_disp_name));
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_TITLE,
                                  m_attr_title,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_MESSAGE,
                                  m_attr_message,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_SUBTITLE,
                                  m_attr_subtitle,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,
                                  m_attr_message_size,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_DATE,
                                  m_attr_date,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,
                                  m_attr_posaction,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL,
                                  m_attr_negaction,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(err_code);

    ancs_c_init.evt_handler   = ancs_c_evt_handler;
    ancs_c_init.error_handler = apple_notification_error_handler;
    ancs_c_init.p_gatt_queue  = &m_ble_gatt_queue;

    err_code = ble_ancs_c_init(&m_ancs_c, &ancs_c_init);
    APP_ERROR_CHECK(err_code);

    // Initialize CUS Service init structure to zero.
    cus_init.evt_handler                = on_cus_evt;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cus_init.custom_value_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cus_init.custom_value_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cus_init.custom_value_char_attr_md.write_perm);

    err_code = ble_cus_init(&m_cus, &cus_init);
    APP_ERROR_CHECK(err_code);

    // Initialize CTS.
    cts_init.evt_handler   = on_cts_c_evt;
    cts_init.error_handler = current_time_error_handler;
    cts_init.p_gatt_queue  = &m_ble_gatt_queue;
    err_code               = ble_cts_c_init(&m_cts_c, &cts_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void) {
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
void gatt_init(void) {
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Database discovery collector initialization.
 */
void db_discovery_init(void) {
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void) {
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_whitelist_enabled = true;
    init.config.ble_adv_fast_enabled      = true;
    init.config.ble_adv_fast_interval     = APP_ADV_FAST_INTERVAL;
    init.config.ble_adv_fast_timeout      = APP_ADV_FAST_DURATION;
    init.config.ble_adv_slow_enabled      = true;
    init.config.ble_adv_slow_interval     = APP_ADV_SLOW_INTERVAL;
    init.config.ble_adv_slow_timeout      = APP_ADV_SLOW_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void) {
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Peer Manager initialization.
 */
void peer_manager_init(void) {
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for setting up GATTC notifications from the Notification Provider.
 *
 * @details This function is called when a successful connection has been established.
 */
void apple_notification_setup(void) {
    ret_code_t ret;

    nrf_delay_ms(100); // Delay because we cannot add a CCCD to close to starting encryption. iOS specific.

    ret = ble_ancs_c_notif_source_notif_enable(&m_ancs_c);
    APP_ERROR_CHECK(ret);

    ret = ble_ancs_c_data_source_notif_enable(&m_ancs_c);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEBUG("Notifications Enabled.");
}

/**@brief Function for starting advertising.
 */
void advertising_start(void * p_erase_bonds) {
    bool erase_bonds = *(bool*)p_erase_bonds;

    if (erase_bonds) {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else {
        ret_code_t ret;

        memset(m_whitelist_peers, PM_PEER_ID_INVALID, sizeof(m_whitelist_peers));
        m_whitelist_peer_cnt = (sizeof(m_whitelist_peers) / sizeof(pm_peer_id_t));

        peer_list_get(m_whitelist_peers, &m_whitelist_peer_cnt);

        ret = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
        APP_ERROR_CHECK(ret);

        ret = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
        if(ret != NRF_ERROR_NOT_SUPPORTED) {
            APP_ERROR_CHECK(ret);
        }

        ret = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(ret);
    }
}

/*
 * handler functions
 */

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {
    ret_code_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED: {
            NRF_LOG_INFO("Connected.");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            ble_manager.connected = true;
            break;
        }
        case BLE_GAP_EVT_DISCONNECTED: {
            NRF_LOG_INFO("Disconnected.");
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            if (p_ble_evt->evt.gap_evt.conn_handle == m_cts_c.conn_handle) {
                m_cts_c.conn_handle = BLE_CONN_HANDLE_INVALID;
            }
            ble_manager.connected = false;
            break;
        }
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys = {
                    .rx_phys = BLE_GAP_PHY_AUTO,
                    .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
            break;
        }
        case BLE_GATTC_EVT_TIMEOUT: {
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;
            ble_manager.connected = false;
        }
        case BLE_GATTS_EVT_TIMEOUT: {
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;
            ble_manager.connected = false;
        }
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling events from the GATT Servive client module.

   @param[in] p_evt GATT Service event.
*/
void gatts_c_evt_handler(nrf_ble_gatts_c_evt_t * p_evt) {
    ret_code_t ret = NRF_SUCCESS;

    switch (p_evt->evt_type)
    {
        case NRF_BLE_GATTS_C_EVT_DISCOVERY_COMPLETE:
        {
            NRF_LOG_DEBUG("GATT Service and Service Changed characteristic found on server.");

            ret = nrf_ble_gatts_c_handles_assign(&m_gatts_c,
                                                 p_evt->conn_handle,
                                                 &p_evt->params.srv_changed_char);
            APP_ERROR_CHECK(ret);

            pm_peer_id_t peer_id;
            ret = pm_peer_id_get(p_evt->conn_handle, &peer_id);
            APP_ERROR_CHECK(ret);

            memset(&m_peer_srv_buf[0], 0, sizeof(m_peer_srv_buf[0]));
            m_peer_srv_buf[0].charateristics[0] = p_evt->params.srv_changed_char;

            ble_manager.gatts_discovered = true;
            if(ble_manager.gatts_discovered && ble_manager.ancs_discovered)
            {
                ret = pm_peer_data_remote_db_store(peer_id,
                                                   (ble_gatt_db_srv_t *)m_peer_srv_buf,
                                                   sizeof(m_peer_srv_buf),
                                                   NULL);
                if (ret == NRF_ERROR_STORAGE_FULL)
                {
                    ret = fds_gc();
                }
                APP_ERROR_CHECK(ret);
            }
            ret = nrf_ble_gatts_c_enable_indication(&m_gatts_c, true);
            APP_ERROR_CHECK(ret);
        } break;

        case NRF_BLE_GATTS_C_EVT_DISCOVERY_FAILED:
            NRF_LOG_DEBUG("GATT Service or Service Changed characteristic not found on server.");
            break;

        case NRF_BLE_GATTS_C_EVT_DISCONN_COMPLETE:
            NRF_LOG_DEBUG("GATTS Service client disconnected connection handle %i.",
                          p_evt->conn_handle);
            break;

        case NRF_BLE_GATTS_C_EVT_SRV_CHANGED:
            NRF_LOG_DEBUG("Service Changed indication received.");

            // Discover peer's services.
            ble_manager.ancs_discovered  = false;
            ble_manager.gatts_discovered = false;
            ret = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);
            APP_ERROR_CHECK(ret);
            break;

        default:
            break;
    }
}

void ancs_c_evt_handler(ble_ancs_c_evt_t * p_evt) {
    switch (p_evt->evt_type)     {
        case BLE_ANCS_C_EVT_DISCOVERY_COMPLETE: {
            ret_code_t ret = NRF_SUCCESS;
            NRF_LOG_DEBUG("Apple Notification Center Service discovered on the server.");
            ret = nrf_ble_ancs_c_handles_assign(&m_ancs_c, p_evt->conn_handle, &p_evt->service);
            APP_ERROR_CHECK(ret);

            pm_peer_id_t peer_id;
            ret = pm_peer_id_get(p_evt->conn_handle, &peer_id);
            APP_ERROR_CHECK(ret);

            // Copy the needed ANCS handles into a ble_gatt_db_srv_t struct that will be stored in
            // flash.
            ble_gatt_db_char_t * p_char = m_peer_srv_buf[1].charateristics;
            memset(&m_peer_srv_buf[1], 0, sizeof(m_peer_srv_buf[1]));

            p_char[0].characteristic = p_evt->service.control_point_char;
            p_char[1].characteristic = p_evt->service.notif_source_char;
            p_char[1].cccd_handle    = p_evt->service.notif_source_cccd.handle;
            p_char[2].characteristic = p_evt->service.data_source_char;
            p_char[2].cccd_handle    = p_evt->service.data_source_cccd.handle;

            ble_manager.ancs_discovered = true;

            if (ble_manager.gatts_discovered && ble_manager.ancs_discovered)
            {
                ret = pm_peer_data_remote_db_store(peer_id,
                                                   (ble_gatt_db_srv_t *)m_peer_srv_buf,
                                                   sizeof(m_peer_srv_buf),
                                                   NULL);
                if (ret == NRF_ERROR_STORAGE_FULL)
                {
                    ret = fds_gc();
                }
                APP_ERROR_CHECK(ret);
            }
            apple_notification_setup();
        } break;

        case BLE_ANCS_C_EVT_NOTIF:
            m_notification_latest = p_evt->notif;
            ancs_notif_print(&m_notification_latest);
            break;

        case BLE_ANCS_C_EVT_NOTIF_ATTRIBUTE:
            m_notif_attr_latest = p_evt->attr;
            ancs_notif_attr_print(&m_notif_attr_latest);
            if (p_evt->attr.attr_id == BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER)
            {
                m_notif_attr_app_id_latest = p_evt->attr;
            }
            break;
        case BLE_ANCS_C_EVT_DISCOVERY_FAILED:
            NRF_LOG_DEBUG("Apple Notification Center Service not discovered on the server.");
            break;

        case BLE_ANCS_C_EVT_APP_ATTRIBUTE:
            ancs_app_attr_print(&p_evt->attr);
            break;
        case BLE_ANCS_C_EVT_NP_ERROR:
            ancs_err_code_print(p_evt->err_code_np);
            break;
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
void pm_evt_handler(pm_evt_t const * p_evt) {
    ret_code_t err_code, ret;

    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id) {
        case PM_EVT_BONDED_PEER_CONNECTED: {
            NRF_LOG_INFO("Connected to a previously bonded device.");
            if (p_evt->peer_id != PM_PEER_ID_INVALID) {
                uint32_t data_len = sizeof(m_peer_srv_buf);
                ret = pm_peer_data_remote_db_load(p_evt->peer_id, m_peer_srv_buf, &data_len);
                if (ret == NRF_ERROR_NOT_FOUND) {
                    NRF_LOG_DEBUG("Could not find the remote database in flash.");
                    ret = nrf_ble_gatts_c_handles_assign(&m_gatts_c, p_evt->conn_handle, NULL);
                    APP_ERROR_CHECK(ret);

                    // Discover peer's services.
                    ble_manager.ancs_discovered  = false;
                    ble_manager.gatts_discovered = false;
                    memset(&m_ble_db_discovery, 0x00, sizeof(m_ble_db_discovery));
                    ret = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);
                    APP_ERROR_CHECK(ret);
                }
                else {
                    // Check if the load was successful.
                    ASSERT(data_len == sizeof(m_peer_srv_buf));
                    APP_ERROR_CHECK(ret);
                    NRF_LOG_INFO("Remote Database loaded from flash.");

                    // Assign the loaded handles to the GATT Service client module.
                    ble_gatt_db_char_t srv_changed_handles = m_peer_srv_buf[0].charateristics[0];
                    ret = nrf_ble_gatts_c_handles_assign(&m_gatts_c,
                                                         p_evt->conn_handle,
                                                         &srv_changed_handles);
                    APP_ERROR_CHECK(ret);

                    // Enable indications.
                    ret = nrf_ble_gatts_c_enable_indication(&m_gatts_c, true);
                    APP_ERROR_CHECK(ret);

                    //Load the relevant handles into a ble_ancs_c_service_t struct that can be
                    // assigned to the ANCS module.
                    ble_ancs_c_service_t ancs_handles;
                    ble_gatt_db_char_t * p_char           = m_peer_srv_buf[1].charateristics;
                    ancs_handles.control_point_char       = p_char[0].characteristic;
                    ancs_handles.notif_source_char        = p_char[1].characteristic;
                    ancs_handles.notif_source_cccd.handle = p_char[1].cccd_handle;
                    ancs_handles.data_source_char         = p_char[2].characteristic;
                    ancs_handles.data_source_cccd.handle  = p_char[2].cccd_handle;

                    ret = nrf_ble_ancs_c_handles_assign(&m_ancs_c, p_evt->conn_handle,
                                                        &ancs_handles);
                    APP_ERROR_CHECK(ret);
                }
            }
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED: {
            // Check it the Service Changed characteristic handle exists in our client instance.
            // If it is invalid, we know service discovery is needed.
            // (No database was loaded during @ref PM_EVT_BONDED_PEER_CONNECTED)
            if (m_gatts_c.srv_changed_char.characteristic.handle_value == BLE_GATT_HANDLE_INVALID) {
                ret = nrf_ble_gatts_c_handles_assign(&m_gatts_c, p_evt->conn_handle, NULL);
                APP_ERROR_CHECK(ret);

                // Discover peer's services.
                ble_manager.ancs_discovered  = false;
                ble_manager.gatts_discovered = false;
                memset(&m_ble_db_discovery, 0x00, sizeof(m_ble_db_discovery));
                ret = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);
                APP_ERROR_CHECK(ret);
            }
//            m_peer_id = p_evt->peer_id;
//
//            // Discover peer's services.
//            err_code  = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);
//            APP_ERROR_CHECK(err_code);
        } break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED: {
            // Note: You should check on what kind of white list policy your application should use.
            if (     p_evt->params.peer_data_update_succeeded.flash_changed
                     && (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING))
            {
                NRF_LOG_DEBUG("New Bond, add the peer to the whitelist if possible");
                NRF_LOG_DEBUG("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d",
                              m_whitelist_peer_cnt + 1,
                              BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

                if (m_whitelist_peer_cnt < BLE_GAP_WHITELIST_ADDR_MAX_COUNT)
                {
                    // Bonded to a new peer, add it to the whitelist.
                    m_whitelist_peers[m_whitelist_peer_cnt++] = m_peer_id;

                    // The whitelist has been modified, update it in the Peer Manager.
                    err_code = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
                    if (err_code != NRF_ERROR_NOT_SUPPORTED)
                    {
                        APP_ERROR_CHECK(err_code);
                    }

                    err_code = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break;

        case PM_EVT_CONN_SEC_FAILED: {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ: {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL: {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED: {
            advertising_start(false);
        } break;
        case PM_EVT_PEER_DATA_UPDATE_FAILED: {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED: {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED: {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED: {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
            // This can happen when the local DB has changed.
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}

/**@brief Function for handling Database Discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective service instances.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
void db_disc_handler(ble_db_discovery_evt_t * p_evt) {
    ble_ancs_c_on_db_disc_evt(&m_ancs_c, p_evt);
    nrf_ble_gatts_c_on_db_disc_evt(&m_gatts_c, p_evt);
    ble_cts_c_on_db_disc_evt(&m_cts_c, p_evt);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void nrf_qwr_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the Current Time Service errors.
 *
 * @param[in]  nrf_error  Error code containing information about what went wrong.
 */
void current_time_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the Apple Notification Service client errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
void apple_notification_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
void notification_timeout_handler(void * p_context) {
    UNUSED_PARAMETER(p_context);
    ret_code_t err_code;

    // Increment the value of m_custom_value before nortifing it.
    m_custom_value++;

    err_code = ble_cus_custom_value_update(&m_cus, m_custom_value);
    APP_ERROR_CHECK(err_code);
}

/*
 * on event functions
 */

/**@brief Function for handling the Custom Service Service events.
 *
 * @details This function will be called for all Custom Service events which are passed to
 *          the application.
 *
 * @param[in]   p_cus_service  Custom Service structure.
 * @param[in]   p_evt          Event received from the Custom Service.
 *
 */
void on_cus_evt(ble_cus_t * p_cus_service, ble_cus_evt_t * p_evt) {
//    ret_code_t err_code;

    switch (p_evt->evt_type) {
        case BLE_CUS_EVT_NOTIFICATION_ENABLED:
//            err_code = app_timer_start(m_notification_timer_id, NOTIFICATION_INTERVAL, NULL);
//            APP_ERROR_CHECK(err_code);
            break;
        case BLE_CUS_EVT_NOTIFICATION_DISABLED:
//            err_code = app_timer_stop(m_notification_timer_id);
//            APP_ERROR_CHECK(err_code);
            break;
        case BLE_CUS_EVT_CONNECTED:
        case BLE_CUS_EVT_DISCONNECTED:
            break;
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling the Current Time Service client events.
 *
 * @details This function will be called for all events in the Current Time Service client that
 *          are passed to the application.
 *
 * @param[in] p_evt Event received from the Current Time Service client.
 */
void on_cts_c_evt(ble_cts_c_t * p_cts, ble_cts_c_evt_t * p_evt) {
    ret_code_t err_code;

    switch (p_evt->evt_type)
    {
        case BLE_CTS_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_INFO("Current Time Service discovered on server.");
            err_code = ble_cts_c_handles_assign(&m_cts_c,
                                                p_evt->conn_handle,
                                                &p_evt->params.char_handles);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_CTS_C_EVT_DISCOVERY_FAILED:
            NRF_LOG_INFO("Current Time Service not found on server. ");
            // CTS not found in this case we just disconnect. There is no reason to stay
            // in the connection for this simple app since it all wants is to interact with CT
            if (p_evt->conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                err_code = sd_ble_gap_disconnect(p_evt->conn_handle,
                                                 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                APP_ERROR_CHECK(err_code);
            }
            ble_manager.cts_discovered = false;
            break;

        case BLE_CTS_C_EVT_DISCONN_COMPLETE:
            NRF_LOG_INFO("Disconnect Complete.");
            ble_manager.cts_discovered = false;
            break;

        case BLE_CTS_C_EVT_CURRENT_TIME:
            NRF_LOG_INFO("Current Time received.");
            current_time_print(p_evt);
            ble_manager.cts_request = false;
            break;

        case BLE_CTS_C_EVT_INVALID_TIME:
            NRF_LOG_INFO("Invalid Time received.");
            ble_manager.cts_request = false;
            break;

        default:
            break;
    }
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
    ret_code_t err_code;

    switch (ble_adv_evt) {
        case BLE_ADV_EVT_FAST: {
            NRF_LOG_INFO("Fast advertising.");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        }
        case BLE_ADV_EVT_SLOW: {
            NRF_LOG_INFO("SLow Advertising.");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
            APP_ERROR_CHECK(err_code);
        }
        case BLE_ADV_EVT_FAST_WHITELIST: {
            NRF_LOG_INFO("Fast advertising with WhiteList");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
            APP_ERROR_CHECK(err_code);
            break;
        }
        case BLE_ADV_EVT_SLOW_WHITELIST: {
            NRF_LOG_INFO("Slow advertising with WhiteList");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
            APP_ERROR_CHECK(err_code);
            err_code = ble_advertising_restart_without_whitelist(&m_advertising);
            APP_ERROR_CHECK(err_code);
            break;
        }
        case BLE_ADV_EVT_WHITELIST_REQUEST: {
            ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            ble_gap_irk_t whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            uint32_t addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
            uint32_t irk_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

            err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt,
                                        whitelist_irks, &irk_cnt);
            APP_ERROR_CHECK(err_code);
            NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist",
                          addr_cnt,
                          irk_cnt);

            // Apply the whitelist.
            err_code = ble_advertising_whitelist_reply(&m_advertising,
                                                       whitelist_addrs,
                                                       addr_cnt,
                                                       whitelist_irks,
                                                       irk_cnt);
            APP_ERROR_CHECK(err_code);
            break;
        }
        case BLE_ADV_EVT_IDLE: {
//            sleep_mode_enter();
            break;
        }
        default:
            break;
    }
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
void on_conn_params_evt(ble_conn_params_evt_t * p_evt) {
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/*
 * misc
 */

/**@brief Fetch the list of peer manager peer IDs.
 *
 * @param[inout] p_peers   The buffer where to store the list of peer IDs.
 * @param[inout] p_size    In: The size of the @p p_peers buffer.
 *                         Out: The number of peers copied in the buffer.
 */
void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size) {
    pm_peer_id_t peer_id;
    uint32_t     peers_to_copy;

    peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
                     *p_size : BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

    peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    *p_size = 0;

    while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--))
    {
        p_peers[(*p_size)++] = peer_id;
        peer_id = pm_next_peer_id_get(peer_id);
    }
}

/**@brief Function for handling the Current Time Service errors.
 *
 * @param[in] p_evt  Event received from the Current Time Service client.
 */
void current_time_print(ble_cts_c_evt_t * p_evt) {
    NRF_LOG_INFO("\r\nCurrent Time:");
    NRF_LOG_INFO("\r\nDate:");

    NRF_LOG_INFO("\tDay of week   %s", (uint32_t)day_of_week[p_evt->
            params.
            current_time.
            exact_time_256.
            day_date_time.
            day_of_week]);

    if (p_evt->params.current_time.exact_time_256.day_date_time.date_time.day == 0)
    {
        NRF_LOG_INFO("\tDay of month  Unknown");
    }
    else
    {
        NRF_LOG_INFO("\tDay of month  %i",
                     p_evt->params.current_time.exact_time_256.day_date_time.date_time.day);
    }

    NRF_LOG_INFO("\tMonth of year %s",
                 (uint32_t)month_of_year[p_evt->params.current_time.exact_time_256.day_date_time.date_time.month]);
    if (p_evt->params.current_time.exact_time_256.day_date_time.date_time.year == 0)
    {
        NRF_LOG_INFO("\tYear          Unknown");
    }
    else
    {
        NRF_LOG_INFO("\tYear          %i",
                     p_evt->params.current_time.exact_time_256.day_date_time.date_time.year);
    }
    NRF_LOG_INFO("\r\nTime:");
    NRF_LOG_INFO("\tHours     %i",
                 p_evt->params.current_time.exact_time_256.day_date_time.date_time.hours);
    NRF_LOG_INFO("\tMinutes   %i",
                 p_evt->params.current_time.exact_time_256.day_date_time.date_time.minutes);
    NRF_LOG_INFO("\tSeconds   %i",
                 p_evt->params.current_time.exact_time_256.day_date_time.date_time.seconds);
    NRF_LOG_INFO("\tFractions %i/256 of a second",
                 p_evt->params.current_time.exact_time_256.fractions256);

    NRF_LOG_INFO("\r\nAdjust reason:\r");
    NRF_LOG_INFO("\tDaylight savings %x",
                 p_evt->params.current_time.adjust_reason.change_of_daylight_savings_time);
    NRF_LOG_INFO("\tTime zone        %x",
                 p_evt->params.current_time.adjust_reason.change_of_time_zone);
    NRF_LOG_INFO("\tExternal update  %x",
                 p_evt->params.current_time.adjust_reason.external_reference_time_update);
    NRF_LOG_INFO("\tManual update    %x",
                 p_evt->params.current_time.adjust_reason.manual_time_update);
}

/**@brief Clear bond information from persistent storage.
 */
void delete_bonds(void) {
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}