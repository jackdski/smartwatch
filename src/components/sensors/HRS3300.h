#ifndef HRS3300_H_
#define HRS3300_H_

#include <stdint.h>
#include <stdbool.h>

#define HRS_ADDRESS             0x44

#define HRS_DEVICE_ID_REG       0x00
#define HRS_DEVICE_ID           0x21

enum
{
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

typedef enum
{
	HRS_CONVERSION_WAIT_TIME_800MS,
	HRS_CONVERSION_WAIT_TIME_400MS,
	HRS_CONVERSION_WAIT_TIME_200MS,
	HRS_CONVERSION_WAIT_TIME_100MS,
	HRS_CONVERSION_WAIT_TIME_75MS,
	HRS_CONVERSION_WAIT_TIME_50MS,
	HRS_CONVERSION_WAIT_TIME_12_5MS,
	HRS_CONVERSION_WAIT_TIME_0MS
} HRS_HWT_E;

#define HRS_SHIFT_ENABLE_HWT(X)         (X << 4)

typedef enum {
	HRS_PDRIVE_12_5MA,
	HRS_PDRIVE_20_MA,
	HRS_PDRIVE_30_MA,
	HRS_PDRIVE_40_MA,
} HRS_PDRIVE_E;

#define HRS_SHIFT_ENABLE_PDRIVE(X)      ((X & 0b10) << 3)
#define HRS_SHIFT_LED_DRIVE_PDRIVE(X)     ((X & 0b01) << 6)

#define HRS_PACK_ENABLE_REG(EN, HWT, PD)    (HRS_SHIFT_ENABLE_HEN(EN) | HRS_SHIFT_ENABLE_HWT(HWT) | HRS_SHIFT_ENABLE_PDRIVE(PD))
#define HRS_PACK_LED_DRIVE_REG(EN, PD)      (HRS_SHIFT_LED_DRIVE_PON(EN) | HRS_SHIFT_LED_DRIVE_PDRIVE(PD))

#define HRS_CH1_SET(H, M, L)            (((0x3F & H) << 11) | (M << 3) | ((0x07) & L))
//#define HRS_CH0_SET(H, M, L)            ((((0x0F & H) << 4) | (M << 8) | ((0x30) & L) << 16) | (0x0F & L))
#define HRS_CH0_SET(H, M, L)            ((((0x78 & H) << 4) | (M     << 8) | ((0x30) & L) << 16) | (0x0F & L))

typedef enum
{
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
} HRS_ALS_RES_E;

#define HRS_ALS_RES_PACK(X)         (X)

typedef enum
{
	HRS_HGAIN_X1,
	HRS_HGAIN_X2,
	HRS_HGAIN_X4,
	HRS_HGAIN_X8,
	HRS_HGAIN_X64
} HRS_HGAIN_E;

#define HRS_HGAIN_PACK(X)           (X << 2)

typedef void (*i2c_read_reg)(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer);
typedef void (*i2c_write_reg)(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

typedef struct
{
	bool			enable;
	uint8_t         ID;
	uint16_t        CH1DATA;
	uint16_t        CH0DATA;
	HRS_HWT_E       HWT;
	HRS_PDRIVE_E    PDRIVE;
	HRS_ALS_RES_E   ALS_RES;
	HRS_HGAIN_E     HGAIN;
	i2c_write_reg   write_reg;
	i2c_read_reg    read_reg;
} HRS3300_t;

bool init_HRS3300(void);
void HRS3300_enable(bool enable);
bool HRS3300_get_device_id(void);
void HRS3300_update(void);
void HRS3300_set_conversion_wait_time(HRS_HWT_E wait_time);
uint32_t HRS3300_get_sample(bool channel);
void HRS3300_set_pdrive_current(HRS_PDRIVE_E pdrive);
void HRS3300_set_resolution(HRS_ALS_RES_E res);
void HRS3300_set_hgain(HRS_HGAIN_E hgain);

#endif /* HRS3300_H */
