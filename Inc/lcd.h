//
// Created by Albert on 20.05.2020.
//

#ifndef SYNTH_LCD_H
#define SYNTH_LCD_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "spi.h"
#include "delay.h"
#include "uart.h"

#define LCD_GPIO (GPIOB)

#define COMD_PIN (GPIO_PIN_14)
#define RST_PIN (GPIO_PIN_11)
#define CE_PIN (GPIO_PIN_12)

#define COMMAND (GPIO_PIN_RESET)
#define DATA (GPIO_PIN_SET)

//FUNCTION SET
#define FUNC_SET (0x20)
#define CHIP_ON (0x00)
#define CHIP_OFF (0x04)
#define HORIZONTAL_ADDR (0x00)
#define VERTICAL_ADDR (0x02)
#define BASIC_INSTR (0x00)
#define EXTENDED_INSTR (0x01)

//DISPLAY CONTROL
#define DISPLAY_CONTROL (0x08)
#define DISPLAY_OFF (0x00)
#define DISPLAY_NORM (0x04)
#define DISPLAY_ALL_ON (0x01)
#define DISPLAY_INV (0x05)


#define SET_X_ADDR (0x40)
#define SET_Y_ADDR (0x80)

//TEMPERATURE CONTROL
#define TEMP_CONTROL (0x04)
#define TEMP_COE_0 (0x00)
#define TEMP_COE_1 (0x01)
#define TEMP_COE_2 (0x02)
#define TEMP_COE_3 (0x03)

//BIAS CONTROL
#define BIAS_CONTROL (0x10)
#define BIAS_0 (0x07)
#define BIAS_1 (0x06)
#define BIAS_2 (0x05)
#define BIAS_3 (0x04)
#define BIAS_4 (0x03)
#define BIAS_5 (0x02)
#define BIAS_6 (0x01)
#define BIAS_7 (0x00)

//Vop CONTROL
#define VOP_CONTROL (0x80)

void LCD_SendCommand(uint8_t command);

void LCD_SendData(uint8_t data);

bool LCD_Init(void);

#endif //SYNTH_LCD_H
