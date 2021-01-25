/*          DEBUG          */
//#define PRINT_CONFIG								0
//#define SCREEN_PRINT

/*          RTOS           */
#define TASK_SYSTASK_STACK_SIZE                 256U
#define TASK_DISPLAY_STACK_SIZE                 512U
#define TASK_BLEGENERAL_STACK_SIZE              128U
#define TASK_TIMER_STACK_SIZE                   256U


/*          LOGGER         */
#define NRF_LOG_BACKEND_RTT_ENABLED             1   // enable rtt
#define NRF_LOG_BACKEND_UART_ENABLED            0   // disable uart
#define NRF_LOG_DEFERRED                        0   // flush logs immediately
#define NRF_LOG_ALLOW_OVERFLOW                  1
#define SEGGER_RTT_CONFIG_DEFAULT_MODE          0   // trim
//#define NRF_PWR_MGMT_CONFIG_CPU_USAGE_MONITOR_ENABLED 1 // log CPU usage at 1Hz

#define BLINKY_TEST                             0
#define DEBUG_INFO_ENABLED                      0
#define WATCHDOG_ENABLED                        0

/*          DFU         */
//#define FEATURE_OTA_UPDATE                      1
//#define FEATURE_BUTTONLESS_DFU                  1


/*          BLE         */
#define FREERTOS      // Needed to use FreeRTOS with SoftDevice
#define SOFTDEVICE_PRESENT                      1
#define USE_BLE                                 1
//#define USE_PEER_MANAGER                        1
#define BLE_STACK_SUPPORT_REQD                  1
//#define NRF_BLE_GQ_ENABLED                      1
#define BLE_CTS_C_ENABLED                       1
#define BLE_ANCS_C_ENABLED                      1
#define BLE_DB_DISCOVERY_ENABLED                1
#define NRF_BLE_GATTS_C_ENABLED                 1

//#define NRF_BLE_GQ_ENABLED 1
//#define NRF_BLE_GQ_DATAPOOL_ELEMENT_COUNT       8
//#define NRF_BLE_GQ_DATAPOOL_ELEMENT_SIZE        20
//#define NRF_BLE_GQ_QUEUE_SIZE                   10
//#define NRF_BLE_GQ_GATTC_WRITE_MAX_DATA_LEN     20
//#define NRF_BLE_GQ_GATTS_HVX_MAX_DATA_LEN       16

/*          TWI             */


/*          SPI             */
#define SPIM_BASE                       NRF_SPIM1
#define SPI_MISO_PIN                    NRF_GPIO_PIN_MAP(0, 4)
#define SPI_MOSI_PIN                    NRF_GPIO_PIN_MAP(0, 3)
#define SPI_SCK_PIN                     NRF_GPIO_PIN_MAP(0, 2)


/*          SENSORS         */
#define TWI_INSTANCE_ID                 0
//#define NRFX_TWIM_ENABLED 				1
//#define NRFX_TWIM0_ENABLED				1

#define SENSORS_NRF_TWIM                NRF_TWIM0
#define SENSORS_SDA_PIN                 NRF_GPIO_PIN_MAP(0, 6)
#define SENSORS_SCL_PIN                 NRF_GPIO_PIN_MAP(0, 7)

#define BMA421_INT_PIN                  NRF_GPIO_PIN_MAP(0, 8)
#define TP_INT_PIN                      NRF_GPIO_PIN_MAP(0, 28)
#define TP_RESET_PIN                    NRF_GPIO_PIN_MAP(0, 10)
#define HRS3300_TEST_PIN                NRF_GPIO_PIN_MAP(0, 30)


/*          DISPLAY         */
#define DISPLAY_SPI_INSTANCE            1
#define SPI1_ENABLED 					1
#define SPI1_USE_EASY_DMA				1

#define USING_240X240
#define DISPLAY_HEIGHT                  240U
#define DISPLAY_WIDTH                   240U

#define DISPLAY_BACKLIGHT_LOW           NRF_GPIO_PIN_MAP(0, 14)
#define DISPLAY_BACKLIGHT_MID           NRF_GPIO_PIN_MAP(0, 22)
#define DISPLAY_BACKLIGHT_HIGH          NRF_GPIO_PIN_MAP(0, 23)

#define DISPLAY_CS_PIN                  NRF_GPIO_PIN_MAP(0, 25)
#define DISPLAY_MISO_PIN                SPI_MISO_PIN
#define DISPLAY_MOSI_PIN                SPI_MOSI_PIN
#define DISPLAY_SCK_PIN                 SPI_SCK_PIN
#define DISPLAY_DC_PIN                  NRF_GPIO_PIN_MAP(0, 18)
#define DISPLAY_RESET_PIN               NRF_GPIO_PIN_MAP(0, 26)
#define DISPLAY_DET_PIN                 NRF_GPIO_PIN_MAP(0, 9)


/*          NOR FLASH       */
#define FLASH_SPI_INSTANCE              1
#define FLASH_CS_PIN                    NRF_GPIO_PIN_MAP(0, 5)


/*          BATTERY         */
#define CHARGE_INDICATION_PIN           NRF_GPIO_PIN_MAP(0, 12)
#define POWER_PRESENCE_PIN              NRF_GPIO_PIN_MAP(0, 19)
#define VCC_POWER_CONTROL_PIN           NRF_GPIO_PIN_MAP(0, 24)
#define BATTERY_VOLTAGE_PIN             NRF_GPIO_PIN_MAP(0, 31)
#define BATTERY_VOLTAGE_SAADC_CH        0 // 7


/*          HAPTIC          */
#define HAPTIC_PWM_BASE                 NRF_PWM0
#define HAPTIC_PIN                      NRF_GPIO_PIN_MAP(0, 16)
#define HAPTIC_CLOCK_HZ                 500000
#define HAPTIC_CLOCK_MICROSECONDS       (1000000 / HAPTIC_CLOCK_HZ)
#define HAPTIC_PERIOD_MICROSECONDS      50
#define HAPTIC_PWM_COUNTERTOP           (HAPTIC_PERIOD_MICROSECONDS / HAPTIC_CLOCK_MICROSECONDS)


/*          MISC            */
#define PUSH_BUTTON_IN_PIN              NRF_GPIO_PIN_MAP(0, 13)
#define PUSH_BUTTON_OUT_PIN             NRF_GPIO_PIN_MAP(0, 15)
#define STATUS_LED_PIN                  NRF_GPIO_PIN_MAP(0, 27)
