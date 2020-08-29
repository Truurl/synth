//
// Created by Albert on 29.04.2020.
//

#ifndef CS43L22_H
#define CS43L22_H

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "uart.h"
#include "i2c.h"
#include "i2s.h"
#include "delay.h"

#define CS43L22_ADDR (0x94)
//Power setting
//*******************************
#define	PWR_CONTROL1 (0x02)
#define PWR_UP (0x9E)
#define PWR_DOWN (0x9F)
//register with power setting for headphones and speakers
#define PWR_CONTROL2 (0X04)
//headphones power settings
#define HEADPHONE_UP (0xA0)
#define HEADPHONE_DOWN (0xF0)
#define HEADPHONE_UP_PIN_6 (0x50)
//speaker power settings
// * * NO CONNECTION ON BOARD
#define SPEAKER_UP (0x0A)
#define SPEAKER_DOWN (0x0F)
#define SPEAKER_UP_PIN_6 (0x00)

//Clock Control for I2S
//**************************
#define CLK_CONTROL (0x05)
#define CLK_AUTO_ON (0x80)
#define CLK_AUTO_OFF (0x00)
//Clock speed
#define CLK_QUARTER_SPEED (0x60)	//4kHz - 12.5kHz Fs
#define CLK_HALF_SPEED (0x40)		//12.5kHz - 25kHz Fs
#define CLK_SINGLE_SPEED (0x20)		//4kHz - 50 kHz Fs
#define CLK_DOUBLE_SPEED (0x00)		//50kHz - 100kHz Fs
//Sample rate 32kHz, 16kHz or 8kHz
#define CLK_32_RATE_YES (0x10)
#define	CLK_32_RATE_NO (0x00)
//27MHz video clk
#define CLK_VIDEO_YES (0x08)
#define CLK_VIDEO_NO (0x00)
//Internal MCLK/LRCK Ratio
#define CLK_IN_RATIO_128 (0x00)
#define CLK_IN_RATIO_125 (0x02)
#define CLK_IN_RATIO_132 (0x04)
#define CLK_IN_RATIO_136 (0x06)
//MCLK divide
#define CLK_NO_DIVIDE (0x00)
#define CLK_DIVIDE2 (0x01)

//Interface Control for I2S
//**************************
#define INT_CONTROL_1 (0x06)
#define INT_CONTROL_2 (0x07)
//Mode
#define INT_MODE_MASTER (0x80)
#define INT_MODE_SLAVE (0x00)
//SCLK polarity
#define INT_POL_INVERTED (0x40)
#define INT_POL_NORM (0x00)
//DSP MODE
#define INT_DSP_ENABLE  (0x01)
#define INT_DSP_DISABLE (0x00)
//Interface Format
#define INT_FORMAT_LEFT (0x00)
#define INT_FORMAT_RIGHT (0x08)
#define INT_FORMAT_I2S (0x04)
//Audio word length for I2S
#define	INT_WORD_24BIT (0x00)
#define	INT_WORD_20BIT (0x01)
#define	INT_WORD_18BIT (0x02)
#define	INT_WORD_16BIT (0x03)
//MCLK = SCLK
#define INT_MCLK_EQ_SCLK (0x00)
#define INT_MCLK_NEQ_SCLK (0x40)
#define INT_SW_INV (0x08)
#define INT_SW_NORM (0x00)

//Passthrough registers
//Beep registers and options
//**************************
#define PASS_A (0x08)
#define PASS_B (0x09)
//Inputs
#define NO_INPUTS (0x00)
#define ANALOG_IN_1 (0x01)
#define ANALOG_IN_2 (0x02)
#define ANALOG_IN_3 (0x04)
#define ANALOG_IN_4 (0x08)

//Beep registers and options
//**************************
#define BEEP_FRQ_REG (0x1c)
#define BEEP_VOL_REG (0x1d)
#define BEEP_TONE_REG (0x1e)
//Beep frequency
#define BEEP_260	(0x00)
#define BEEP_521	(0x10)
#define BEEP_585	(0x20)
#define BEEP_666	(0x30)
#define BEEP_705	(0x40)
#define BEEP_774	(0x50)
#define BEEP_888	(0x60)
#define BEEP_1000	(0x70)
#define BEEP_1043	(0x80)
#define BEEP_1200	(0x90)
#define BEEP_1333	(0xa0)
#define BEEP_1411	(0xb0)
#define BEEP_1600	(0xc0)
#define BEEP_1724	(0xd0)
#define BEEP_2000	(0xe0)
#define BEEP_2182	(0xf0)
//Beep on time
#define BEEP_ON_0_08 (0x00)
#define BEEP_ON_0_43 (0x01)
#define BEEP_ON_0_78 (0x02)
#define BEEP_ON_1_20 (0x03)
#define BEEP_ON_1_50 (0x04)
#define BEEP_ON_1_80 (0x05)
#define BEEP_ON_2_20 (0x06)
#define BEEP_ON_2_50 (0x07)
#define BEEP_ON_2_80 (0x08)
#define BEEP_ON_3_20 (0x09)
#define BEEP_ON_3_50 (0x0a)
#define BEEP_ON_3_80 (0x0b)
#define BEEP_ON_4_20 (0x0c)
#define BEEP_ON_4_50 (0x0d)
#define BEEP_ON_4_80 (0x0e)
#define BEEP_ON_5_20 (0x0f)
//Beep off time
#define BEEP_OFF_1_23 (0x00)
#define BEEP_OFF_2_58 (0x20)
#define BEEP_OFF_3_90 (0x40)
#define BEEP_OFF_5_20 (0x60)
#define BEEP_OFF_6_60 (0x80)
#define BEEP_OFF_8_05 (0x90)
#define BEEP_OFF_9_35 (0xc0)
#define BEEP_OFF_10_80 (0xe0)
//beep configuration
#define BEEP_OFF (0x00)
#define BEEP_SINGLE (0x40)
#define BEEP_MULTIPLE (0x80)
#define BEEP_CONT (0xc0)
#define BEEP_MIX_ON (0x00)
#define BEEP_MIX_OFF (0x20)

//Volume register
//**************************
//Master volume
#define VOL_MSTA_REG (0x20)
#define VOL_MSTB_REG (0x21)
//Headphone volume
#define VOL_HPA_REG (0x22)
#define VOL_HPB_REG (0x23)

//Miscellaneous Controls
//**************************
#define MISC_REG (0x0e)
//Master volume
#define PASSA_ON (0x40)
#define PASSA_OFF (0x00)
#define PASSB_ON (0x80)
#define PASSB_OFF (0x00)
#define PASSA_MUTE_ON (0x10)
#define PASSA_MUTE_OFF (0x00)
#define PASSB_MUTE_ON (0x20)
#define PASSB_MUTE_OFF (0x00)
//Headphone volume
#define VOL_HPA_REG (0x22)
#define VOL_HPB_REG (0x23)

//Status Register
//************************
#define STATUS_REG (0x2e)

typedef struct KeyNode
{
    double frequency;
    uint16_t samplesPerCycle;
    uint64_t time;
    GPIO_TypeDef *gpiox;
    uint16_t gpioPin;
    int16_t sineSamples[400];
    int16_t squareSamples[400];
    int16_t triangleSamples[400];
    int16_t sawtoothSamples[400];
    struct  KeyNode* nextKey;
}KeyNode;

struct KeyNode* FillKey(double frequency, uint32_t gpioPin, GPIO_TypeDef *gpio);

bool AddKeyToList(KeyNode *key);

void KeyBoardInit(void);

int16_t GetSample(uint8_t wave);

bool CS43L22_Init(void);


#endif //CS43L22_H
