#include "alerts.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

extern QueueHandle_t alerts_queue;


void set_alert(Alerts_E new_alert)
{
    const Alerts_E active_alert = new_alert;
    xQueueSend(alerts_queue, &active_alert, pdMS_TO_TICKS(0));
}
