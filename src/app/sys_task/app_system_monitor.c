#include "app_system_monitor.h"

#include "FreeRTOS.h"
#include "task.h"


SystemMonitor_t system_monitor = {
    .free_heap_bytes = 0
};

void app_system_monitor(void)
{
    system_monitor.free_heap_bytes = xPortGetFreeHeapSize();
}
