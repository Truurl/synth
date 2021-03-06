//
// Created by Albert on 29.04.2020.
//

#ifndef CS43L22_H
#define CS43L22_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "i2c.h"

/*
 * Few defines for better readability
 */

//DAC's address
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

/**
 * @brief Function for initializing DAC
 *
 * @return True if initializing is successful otherwise false
 */
bool CS43L22_Init(void);

#endif //CS43L22_H
