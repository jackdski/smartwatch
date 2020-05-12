#define NRF_LOG_BACKEND_RTT_ENABLED     1 // enable rtt
#define NRF_LOG_BACKEND_UART_ENABLED    0 // disable uart
#define NRF_LOG_DEFERRED                0 // flush logs immediately
#define NRF_LOG_ALLOW_OVERFLOW          0 // no overflow
#define SEGGER_RTT_CONFIG_DEFAULT_MODE  2 // block until processed

#define NRF_BLE_GQ_ENABLED                      1
#define NRF_BLE_GQ_DATAPOOL_ELEMENT_COUNT       8
#define NRF_BLE_GQ_DATAPOOL_ELEMENT_SIZE        20
#define NRF_BLE_GQ_QUEUE_SIZE                   5
#define NRF_BLE_GQ_GATTC_WRITE_MAX_DATA_LEN     3
#define NRF_BLE_GQ_GATTS_HVX_MAX_DATA_LEN       16