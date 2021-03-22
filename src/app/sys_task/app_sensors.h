//
// Created by jack on 11/8/20.
//

#ifndef JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_SENSORS_H
#define JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_SENSORS_H

#include <stdbool.h>
#include <stdint.h>

#define BATTERY_SOC_LOW         20U
#define BATTERY_SOC_MED         50U
#define BATTERY_SOC_HIGH        75U
#define BATTERY_SOC_FULL        95U  // not 100% to encourage not overcharging or "topping off"

typedef uint32_t BatteryEventBits;

typedef struct {
  bool          interrupt_active:1;
  uint8_t       interrupt_source;
  uint32_t      steps;
} IMU_t;

typedef struct {
  uint16_t      heart_rate;
  bool          channel;
} HRS_t;

typedef struct {
  uint32_t          steps;
  uint32_t          heart_rate;
  BatteryEventBits  battery_events;
  uint8_t           battery_soc;
} SensorData_t;


void run_accel_app(void);
void run_battery_app(void);
void run_heart_rate_app(void);
void run_sensor_update_display(void);

#endif //JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_SENSORS_H
