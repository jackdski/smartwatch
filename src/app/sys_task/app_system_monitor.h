#ifndef SYSTEM_MONITOR_H_
#define SYSTEM_MONITOR_H_

typedef struct {
    uint16_t free_heap_bytes;
} SystemMonitor_t;


void app_system_monitor(void);

#endif /* SYSTEM_MONITOR_H_ */