//
// Created by jack on 11/8/20.
//

#ifndef JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_BATTERY_H
#define JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_BATTERY_H

#define BATTERY_SOC_LOW         20U
#define BATTERY_SOC_MED         50U
#define BATTERY_SOC_HIGH        75U
#define BATTERY_SOC_FULL        95U  // not 100% to encourage not overcharging or "topping off"

void run_battery_app(void);

#endif //JD_SMARTWATCH_SRC_APP_SYS_TASK_APP_BATTERY_H
