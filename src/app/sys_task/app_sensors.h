//
// Created by jack on 11/8/20.
//

#ifndef APP_SENSORS_H_
#define APP_SENSORS_H_

#include <stdbool.h>
#include <stdint.h>

#define BATTERY_SOC_LOW         20U
#define BATTERY_SOC_MED         50U
#define BATTERY_SOC_HIGH        75U
#define BATTERY_SOC_FULL        95U  // not 100% to encourage not overcharging or "topping off"

typedef enum
{
    BATTERY_CHARGING_IDLE,
    BATTERY_CHARGING_ACTIVE,
    BATTERY_CHARGING_COMPLETE
} BatteryChargingState_E;

typedef enum
{
    BATTERY_LEVEL_DEFAULT,
    BATTERY_LEVEL_LOW,
    BATTERY_LEVEL_MEDIUM,
    BATTERY_LEVEL_HIGH,
    BATTERY_LEVEL_FULL
} BatteryLevel_E;

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
    uint8_t                 soc;
    BatteryLevel_E          level;
    BatteryChargingState_E  charging_state;
} BatteryData_t;

typedef struct {
    IMU_t                   imu;
    HRS_t                   hrs;
    BatteryData_t           battery;
} SensorData_t;


void app_accel(void);
void app_battery(void);
void app_heart_rate(void);
void app_sensor_update_display(void);

#endif // APP_SENSORS_H_
