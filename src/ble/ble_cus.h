//
// Created by jack on 5/8/20.
//

#ifndef BLINKYEXAMPLEPROJECT_BLE_CUS_H
#define BLINKYEXAMPLEPROJECT_BLE_CUS_H

#include <string.h>
#include "sdk_common.h"
#include "boards.h"
#include "ble_srv_common.h"
#include "nrf_gpio.h"
#include "nrf_log.h"

// 46 17 56 56 - dd 85 - 47 28 - 87 64 - 19 7a 04 a3 44 7a
#define CUSTOM_SERVICE_UUID_BASE    {0x7A, 0x44, 0xA3, 0x04, 0x7A, 0x19, 0x64, 0x87, \
                                     0x28, 0x47, 0x85, 0xDD, 0x56, 0x56, 0x17, 0x46}

#define CUSTOM_SERVICE_UUID         0x1400
#define CUSTOM_VALUE_CHAR_UUID      0x1401

typedef struct ble_cus_s ble_cus_t;

#define BLE_CUS_DEF(_name)                        \
static ble_cus_t _name;                           \
NRF_SDH_BLE_OBSERVER(_name ## _obs,               \
                     BLE_HRS_BLE_OBSERVER_PRIO,   \
                     ble_cus_on_ble_evt, &_name)
typedef enum {
    BLE_CUS_EVT_NOTIFICATION_ENABLED,
    BLE_CUS_EVT_NOTIFICATION_DISABLED,
    BLE_CUS_EVT_DISCONNECTED,
    BLE_CUS_EVT_CONNECTED,
} ble_cus_evt_type_t;

typedef struct {
    ble_cus_evt_type_t evt_type; /**< Type of event. */
    uint8_t duty_cycle;
} ble_cus_evt_t;

typedef void (*ble_cus_evt_handler_t) (ble_cus_t * p_cus, ble_cus_evt_t * p_evt);

typedef struct {
    ble_cus_evt_handler_t        evt_handler;
    uint8_t                      initial_custom_value;
    ble_srv_cccd_security_mode_t custom_value_char_attr_md;
} ble_cus_init_t;

struct ble_cus_s {
    ble_cus_evt_handler_t   evt_handler;
    uint16_t                service_handle;
    ble_gatts_char_handles_t custom_value_handles;
    uint16_t                conn_handle;
    uint8_t                 uuid_type;
};

// functions

uint32_t ble_cus_init(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init);
uint32_t custom_value_char_add(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init);
void ble_cus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
void on_connect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt);
void on_disconnect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt);
void on_write(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt);
uint32_t ble_cus_custom_value_update(ble_cus_t * p_cus, uint8_t custom_value);

#endif //BLINKYEXAMPLEPROJECT_BLE_CUS_H
