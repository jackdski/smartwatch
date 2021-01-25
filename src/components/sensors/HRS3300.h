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

enum {
  HRS_REG_ID        = 0x00,
  HRS_REG_ENABLE    = 0x01,
  HRS_REG_C1DATAM   = 0x08,
  HRS_REG_C0DATAM   = 0x09,
  HRS_REG_C0DATAH   = 0X0A,
  HRS_REG_PDRIVER   = 0x0C,
  HRS_REG_C1DATAH   = 0x0D,
  HRS_REG_C1DATAL   = 0x0E,
  HRS_REG_C0DATAL   = 0x0F,
  HRS_REG_RES       = 0x16,
  HRS_REG_HGAIN     = 0x17
};

#define HRS_SHIFT_ENABLE_HEN(X)             (X << 7)
#define HRS_SHIFT_LED_DRIVE_PON(X)          (X << 5)

typedef enum {
  HRS_CONVERSION_WAIT_TIME_800MS,
  HRS_CONVERSION_WAIT_TIME_400MS,
  HRS_CONVERSION_WAIT_TIME_200MS,
  HRS_CONVERSION_WAIT_TIME_100MS,
  HRS_CONVERSION_WAIT_TIME_75MS,
  HRS_CONVERSION_WAIT_TIME_50MS,
  HRS_CONVERSION_WAIT_TIME_12_5MS,
  HRS_CONVERSION_WAIT_TIME_0MS
} eHRS_HWT;

#define HRS_SHIFT_ENABLE_HWT(X)         (X << 4)

typedef enum {
  HRS_PDRIVE_12_5MA,
  HRS_PDRIVE_20_MA,
  HRS_PDRIVE_30_MA,
  HRS_PDRIVE_40_MA,
} eHRS_PDRIVE;

#define HRS_SHIFT_ENABLE_PDRIVE(X)      ((X & 0b10) << 3)
#define HRS_SHIFT_LED_DRIVE_PDRIVE(X)     ((X & 0b01) << 6)

#define HRS_PACK_ENABLE_REG(EN, HWT, PD)    (HRS_SHIFT_ENABLE_HEN(EN) | HRS_SHIFT_ENABLE_HWT(HWT) | HRS_SHIFT_ENABLE_PDRIVE(PD))
#define HRS_PACK_LED_DRIVE_REG(EN, PD)      (HRS_SHIFT_LED_DRIVE_PON(EN) | HRS_SHIFT_LED_DRIVE_PDRIVE(PD))

#define HRS_CH1_SET(H, M, L)            (((0x3F & H) << 11) | (M << 3) | ((0x07) & L))
//#define HRS_CH0_SET(H, M, L)            ((((0x0F & H) << 4) | (M << 8) | ((0x30) & L) << 16) | (0x0F & L))
#define HRS_CH0_SET(H, M, L)            ((((0x78 & H) << 4) | (M     << 8) | ((0x30) & L) << 16) | (0x0F & L))

typedef enum {
  HRS_ALS_RES_8BITS,
  HRS_ALS_RES_9BITS,
  HRS_ALS_RES_10BITS,
  HRS_ALS_RES_11BITS,
  HRS_ALS_RES_12BITS,
  HRS_ALS_RES_13BITS,
  HRS_ALS_RES_14BITS,
  HRS_ALS_RES_15BITS,
  HRS_ALS_RES_16BITS,
  HRS_ALS_RES_17BITS,
  HRS_ALS_RES_18BITS
} eHRS_ALS_RES;

#define HRS_ALS_RES_PACK(X)         (X)

typedef enum {
  HRS_HGAIN_X1,
  HRS_HGAIN_X2,
  HRS_HGAIN_X4,
  HRS_HGAIN_X8,
  HRS_HGAIN_X64
} eHRS_HGAIN;

#define HRS_HGAIN_PACK(X)           (X << 2)

typedef struct {
  uint8_t       ID;
  bool          enable;
  uint16_t      CH1DATA;
  uint16_t      CH0DATA;
  eHRS_HWT      HWT;
  eHRS_PDRIVE   PDRIVE;
  eHRS_ALS_RES  ALS_RES;
  eHRS_HGAIN    HGAIN;
} HRS3300_t;

bool HRS3300_init(void);
void HRS3300_enable(bool enable);
bool HRS3300_get_device_id(void);
void HRS3300_set_conversion_wait_time(eHRS_HWT wait_time);
uint32_t HRS3300_get_sample(bool channel);
void HRS3300_set_pdrive_current(eHRS_PDRIVE pdrive);
void HRS3300_set_resolution(eHRS_ALS_RES res);
void HRS3300_set_hgain(eHRS_HGAIN hgain);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_SENSORS_HRS3300_H
