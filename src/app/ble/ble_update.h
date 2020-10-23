//
// Created by jack on 7/20/20.
//

#ifndef JDSMARTWATCHPROJECT_SRC_BLE_BLE_DFU_H
#define JDSMARTWATCHPROJECT_SRC_BLE_BLE_DFU_H

#include "ble_general.h"

void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context);
void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event);

#endif //JDSMARTWATCHPROJECT_SRC_BLE_BLE_DFU_H
