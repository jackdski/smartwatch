//
// Created by jack on 11/8/20.
//

#ifndef JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_SENSORS_H
#define JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_SENSORS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool          interrupt_active:1;
  uint8_t       interrupt_source;
  uint32_t      steps;
} IMU_t;

struct {
  uint32_t steps;
  uint32_t heart_rate;
} SensorData_t;

void run_accel_app(void);
void run_heart_rate_app(void);
void run_sensor_update_display(void);

#endif //JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_SENSORS_H
