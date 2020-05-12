//
// Created by jack on 5/11/20.
//
//  Modified version of SparkFun's DRV2605L Arduino Library
//  https://cdn.sparkfun.com/assets/learn_tutorials/5/9/5/Haptic_Motor_Driver.zip
//

#ifndef JDSMARTWATCHPROJECT_DRV2605L_H
#define JDSMARTWATCHPROJECT_DRV2605L_H

#include <stdint.h>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_twi_sensor.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_pwm.h"

#define     DRV_PERIOD_US       10
#define     DRV_EN_PIN          NRF_GPIO_PIN_MAP(0, 3)
#define     DRV_PWM_PIN         NRF_GPIO_PIN_MAP(0, 4)

#define     DRV_2605L_ADDR      0x5A

//The Status Register (0x00): The Device ID is bits 7-5. For DRV2605L it should be 7 or 111.
//bits 4 and 2 are reserved. Bit 3 is the diagnostic result. You want to see 0.
//bit 1 is the over temp flag, you want this to be 0
//bit 0 is  over current flag, you want this to be zero.
// Ideally the register will read 0xE0.
#define     DRV_STATUS_REG      0x00
#define     DRV_STATUS_DEV_ID   0xE0
#define     DRV_STATUS_DIAG_BIT 0x08

//The Mode Register (0x01):
//Default 010000000 -- Need to get it out of Standby
//Set to 0000 0000=0x00 to use Internal Trigger
//Set to 0000 0001=0x01 to use External Trigger (edge mode)(like a switch on the IN pin)
//Set to 0000 0010=0x02 to use External Trigger (level mode)
//Set to 0000 0011=0x03 to use PWM input and analog output
//Set to 0000 0100=0x04 to use Audio to Vibe
//Set to 0000 0101=0x05 to use Real-Time Playback
//Set to 0000 0110=0x06 to perform a diagnostic test - result stored in Diagnostic bit in register 0x00
//Set to 0000 0111 =0x07 to run auto calibration
#define     DRV_MODE_REG        0x01
#define     DRV_MODE_STANDBY_BIT 0x40

//The Feedback Control Register (0x1A)
//bit 7: 0 for ERM, 1 for LRA -- Default is 0
//Bits 6-4 control brake factor
//bits 3-2 control the Loop gain
//bit 1-0 control the BEMF gain
#define     DRV_FEEDBACK_REG    0x1A

//The Real-Time Playback Register (0x02)
//There are 6 ERM libraries.
#define     DRV_RTP_REG         0x02

//The Library Selection Register (0x03)
//See table 1 in Data Sheet for
#define     DRV_LIB_REG         0x03

//The waveform Sequencer Register (0X04 to 0x0B)
#define     DRV_WAVESEQ1            0x04 //Bit 7: set this include a wait time between playback
#define     DRV_WAVESEQ2            0x05
#define     DRV_WAVESEQ3            0x06
#define     DRV_WAVESEQ4            0x07
#define     DRV_WAVESEQ5            0x08
#define     DRV_WAVESEQ6            0x09
#define     DRV_WAVESEQ7            0x0A
#define     DRV_WAVESEQ8            0x0B

//The Go register (0x0C)
//Set to 0000 0001=0x01 to set the go bit
#define     DRV_GO_REG              0x0C

//The Overdrive Time Offset Register (0x0D)
//Only useful in open loop mode
#define     DRV_OVERDRIVE_REG       0x0D

//The Sustain Time Offset, Positive Register (0x0E)
#define     DRV_SUSTAINOFFSETPOS_REG 0x0E

//The Sustain Time Offset, Negative Register (0x0F)
#define     DRV_SUSTAINOFFSETNEG_REG 0x0F


//The Break Time Offset Register (0x10)
#define     DRV_BREAKTIME_REG       0x10

//The Audio to Vibe control Register (0x11)
#define     DRV_AUDIOCTRL_REG       0x11

//The Audio to vibe minimum input level Register (0x12)
#define     DRV_AUDMINLVL_REG       0x12

//The Audio to Vibe maximum input level Register (0x13)
#define     DRV_AUDMAXLVL_REG       0x13

// Audio to Vibe minimum output Drive Register (0x14)
#define     DRV_AUDMINDRIVE_REG     0x14

//Audio to Vibe maximum output Drive Register (0x15)
#define     DRV_AUDMAXDRIVE_REG     0X15

//The rated Voltage Register (0x16)
#define     DRV_RATEDVOLT_REG       0x16
#define     DRV_RATEDVOLT_LRA_VAL   (127)
#define     DRV_RATEDVOLT_ERM_VAL   (156)

//The Overdive clamp Voltage (0x17)
#define     DRV_OVERDRIVECLAMP_REG  0x17
#define     DRV_OD_CLAMP_LRA_VAL    (166)
#define     DRV_OD_CLAMP_ERM_VAL    (153)

//The Auto-Calibration Compensation - Result Register (0x18)
#define     DRV_COMPRESULT_REG      0x18

//The Auto-Calibration Back-EMF Result Register (0x19)
#define     DRV_BACKEMF_REG         0x19

//The Control1 Register (0x1B)
//For AC coupling analog inputs and Controlling Drive time
#define     DRV_CONTROL1_REG        0x1B

//The Control2 Register (0x1C)
#define     DRV_CONTROL2_REG        0x1C

//The Control3 Register (0x1D)
#define     DRV_CONTROL3_REG        0x1D

//The Control4 Register (0x1E)
#define     DRV_CONTROL4_REG        0x1E

//The Control5 Register (0x1F)
#define     DRV_CONTROL5_REG        0X1F

//The LRA Open Loop Period Register (0x20)
//This register sets the period to be used for driving an LRA when
//Open Loop mode is selected: see data sheet page 50.
#define     DRV_OLP_REG             0x20

//The V(Batt) Voltage Monitor Register (0x21)
//This bit provides a real-time reading of the supply voltage
//at the VDD pin. The Device must be actively sending a waveform to take
//reading Vdd=Vbatt[7:0]*5.6V/255
#define     DRV_VBATMONITOR_REG     0x21

//The LRA Resonance-Period Register
//This bit reports the measurement of the LRA resonance period
#define     DRV_LRARESPERIOD_REG    0x22

typedef enum {
    DRV_Init,
    DRV_Idle,
    DRV_Get_Voltage,
    DRV_Pulse,
//    DRV_Double_Pulse,
    DRV_Status_Check
} eDRV_State;

typedef enum {
    DRV_MODE_INTERNAL_TRIGGER       = 0x00,
    DRV_MODE_EXTERNAL_TRIGGER_EDGE  = 0x01,
    DRV_MODE_EXTERNAL_TRIGGER_LEVEL = 0x02,
    DRV_MODE_PWM_INPUT              = 0x03,
    DRV_MODE_AUDIO_TO_VIBE          = 0x04,
    DRV_MODE_REAL_TIME_PLAYBACK     = 0x05,
    DRV_MODE_DIAGNOSTIC_TEST        = 0x06,
    DRV_MODE_AUTO_CALIBRATION       = 0x07,
}eDRV_Mode;

typedef enum {
    ERM_MODE = 0,
    LRA_MODE = 1
} eDRV_Motor_Type;

typedef enum {
    LOOP_GAIN_LOW       = (0 << 2),
    LOOP_GAIN_MEDIUM    = (1 << 2),  // default
    LOOP_GAIN_HIGH      = (2 << 2),
    LOOP_GAIN_VERY_HIGH = (3 << 2)
} eDRV_Loop_Gain;

/*
 * @brief Struct to abstract DRV2605L settings
 */
typedef struct {
    uint8_t const   sensor_addr;
    uint8_t         vbatt_reading;
    float           vbatt_voltage;
    uint32_t        en_pin;
    uint32_t        pwm_pin;
    uint32_t        period_us;
    eDRV_State      state;
    eDRV_Motor_Type motor_type;
    eDRV_Loop_Gain  loop_gain;
    bool            enable;
    bool            go_bit;
    bool            initialized;
} DRV2605L_t;

#define DRV2605L_READ(p_reg_addr, p_buffer, byte_cnt) \
    NRF_TWI_MNGR_WRITE(DRV_2605L_ADDR, p_reg_addr, 1, NRF_TWI_MNGR_NO_STOP), \
    NRF_TWI_MNGR_READ (DRV_2605L_ADDR, p_buffer,   byte_cnt, 0)

// init transfer
#define DRV2605L_INIT_TRANSFER_COUNT        6
extern nrf_twi_mngr_transfer_t const drv2605l_init_transfers_lra[DRV2605L_INIT_TRANSFER_COUNT];
extern nrf_twi_mngr_transfer_t const drv2605l_init_transfers_erm[DRV2605L_INIT_TRANSFER_COUNT];
extern nrf_twi_mngr_transfer_t const drv2605l_go_on_transfers[1];
extern nrf_twi_mngr_transfer_t const drv2605l_go_off_transfers[1];
extern nrf_twi_mngr_transfer_t const drv2605l_set_active_transfers[2];
extern nrf_twi_mngr_transfer_t const drv2605l_set_standby_transfers[2];

extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_status_reg_addr;
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_mode_reg_addr;
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_go_reg_addr;
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605_pwm_mode[];
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND drv2605l_vbatt_reg_addr;

// Functions
ret_code_t drv2605l_config(DRV2605L_t * p_inst);
void drv2605l_pwm_config(DRV2605L_t * p_inst);
void drv2605l_general_callback(ret_code_t result, void * p_user_data);
void drv2605l_vbatt_callback(ret_code_t result, void * p_user_data);
void drv2605l_vbatt_voltage_conversion(ret_code_t result, void * p_user_data);
void set_drv_vbatt(DRV2605L_t * p_inst);
bool get_vbatt_data_available(void);
bool set_vbatt_data_available(bool status);

void drv2605l_pwm_config(DRV2605L_t * p_inst);
ret_code_t drv2605l_set_pwm_duty_cycle(uint8_t duty_cycle);

#endif //JDSMARTWATCHPROJECT_DRV2605L_H
