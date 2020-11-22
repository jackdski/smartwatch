//
// Created by jack on 9/2/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_HRS3300_H
#define BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_HRS3300_H

#include <stdint.h>
#include <stdbool.h>

#define HRS_ADDRESS             0x44

#define HRS_DEVICE_ID_REG       0x00
#define HRS_DEVICE_ID           0x21

#define HRS_ENABLE_REG          0x01
#define HRS_ENABLE_DEFAULT      0x68
#define HRS_ENABLE_HEN          (1 << 7)

#define HRS_ENABLE_HWT_800MS    (0x0 << 4)
#define HRS_ENABLE_HWT_400MS    (0x1 << 4)
#define HRS_ENABLE_HWT_200MS    (0x2 << 4)
#define HRS_ENABLE_HWT_100MS    (0x3 << 4)
#define HRS_ENABLE_HWT_75MS     (0x4 << 4)
#define HRS_ENABLE_HWT_50MS     (0x5 << 4)
#define HRS_ENABLE_HWT_12_5MS   (0x6 << 4)
#define HRS_ENABLE_HWT_0MS      (0x7 << 4)

#define HRS_PDRIVE_HIGH_CURRENT (1 << 3)

#define HRS_C1DATAH_REG         0x0D
#define HRS_C1DATAM_REG         0x08
#define HRS_C1DATAL_REG         0x0E

#define HRS_C0DATAH_REG         0x09
#define HRS_C0DATAM_REG         0x0A
#define HRS_C0DATAL_REG         0x0F

#define HRS_PDRIVER_REG         0x0C
#define HRS_PDRIVER_CURRENT_OFF (1 << 6)
#define HRS_PDRIVER_PON         (1 << 5)

#define HRS_RES_REG             0x16
#define HRS_RES_12BIT           0x04
#define HRS_RES_14BIT           0x06
#define HRS_RES_16BIT           0x08

#define HRS_HGAIN_REG           0x17
#define HRS_HGAIN_SINGLE        (0x00 << 2)
#define HRS_HGAIN_DOUBLE        (0x01 << 2)
#define HRS_HGAIN_QUADRUPLE     (0x02 << 2)
#define HRS_HGAIN_EIGHTX        (0x03 << 2)

typedef enum {
  HRS_TWELVE_MILLIAMP_LED_DRIVE = 0b00,
  HRS_TWENTY_MILLIAMP_LED_DRIVE = 0b01,
  HRS_THIRTY_MILLIAMP_LED_DRIVE = 0b10,
  HRS_FORTY_MILLIAMP_LED_DRIVE = 0b11,
} ePDriveCurrent;

bool HRS3300_init(void);
void HRS3300_enable(void);
bool HRS3300_get_device_id(void);
void HRS3300_low_power(void);
void HRS3300_change_sample_rate(uint8_t rate);
uint32_t HRS3300_get_sample(bool channel);
void HRS3300_set_pdrive_current(ePDriveCurrent current);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_HRS3300_H
