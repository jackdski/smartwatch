#include "app_config.h"
#include "XT25F32B.h"

#include "features.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"


typedef enum
{
    FLASH_TASK_STATE_INITIALIZATION,
    FLASH_TASK_STATE_RUNNING,
    FLASH_TASK_STATE_SLEEP
} flash_task_states_E;

typedef struct
{
    flash_task_states_E state;
} flash_task_data_t;

flash_task_data_t flash_data =
{
    .state = FLASH_TASK_STATE_INITIALIZATION
};

XT25F32B_config_t flash_config =
{
    .cs_pin = FLASH_CS_PIN,
};


// FLASH TASK

void flash_task(void * arg)
{
    UNUSED_PARAMETER(arg);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xTaskPeriod = pdMS_TO_TICKS(10);
    nrf_gpio_pin_set(FLASH_CS_PIN);

    while(1)
    {
        switch (flash_data.state)
        {
            case FLASH_TASK_STATE_INITIALIZATION:
            {
                if (XT25F32B_init(&flash_config))
                {
                    // (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
                    if (XT25F32B_getDeviceID() != 0x00)
                    {
                        flash_data.state = FLASH_TASK_STATE_RUNNING;
                    }
                };
            }
            break;

            case FLASH_TASK_STATE_RUNNING:
            {
                // uint8_t status[2] = {0,0};
                XT25F32B_read_status();
                (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5));
            }
            break;

            case FLASH_TASK_STATE_SLEEP:
                break;

            default:
                break;
        }
        vTaskDelayUntil(&xLastWakeTime, xTaskPeriod);
    }
}