//
// Created by Albert on 20.05.2020.
//

#ifndef SYNTH_LCD_H
#define SYNTH_LCD_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "spi.h"
#include "delay.h"
#include "uart.h"

#define LCD_GPIO (GPIOA)
#define COMD_PIN (GPIO_PIN_3)
#define CE_PIN (GPIO_PIN_2)
#define RST_PIN (GPIO_PIN_6)

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

#define COLUMNS (uint8_t) (84)
#define ROWS (uint8_t) (6)

#define SET_Y_ADDR (uint8_t) (0x40)
#define SET_X_ADDR (uint8_t) (0x80)

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

// Positions of symbols on LCD
#define SINE_UPPER_ROW (uint8_t) (1)
#define SINE_LOWER_ROW (uint8_t) (2)
#define SINE_START_COL (uint8_t) (13)
#define SINE_BYTEMAP_SIZE (uint8_t) (44)

#define SQUARE_UPPER_ROW (uint8_t) (1)
#define SQUARE_LOWER_ROW (uint8_t) (2)
#define SQUARE_START_COL (uint8_t) (49)
#define SQUARE_BYTEMAP_SIZE (uint8_t) (42)

#define SAWTOOTH_UPPER_ROW (uint8_t) (3)
#define SAWTOOTH_LOWER_ROW (uint8_t) (4)
#define SAWTOOTH_START_COL (uint8_t) (13)
#define SAWTOOTH_BYTEMAP_SIZE (uint8_t) (42)

#define TRIANGLE_UPPER_ROW (uint8_t) (3)
#define TRIANGLE_LOWER_ROW (uint8_t) (4)
#define TRIANGLE_START_COL (uint8_t) (49)
#define TRIANGLE_BYTEMAP_SIZE (uint8_t) (42)

/**
 * @typedef Pointer to function drawing something on LCD screen,
 * Its array of 4, because there are four bytemaps, each for wave
 */
typedef void (*LCD_FuncPtr[4]) (void);

// Used by keyboard.h to draw normal wave symbol
extern LCD_FuncPtr lcd_draw_array;
// Used by keyboard.h to draw inverse wave symbol
extern LCD_FuncPtr lcd_inverse_array;

/**
 * @brief Sends command to LCD screen
 * @param command Command for LCD screen
 * @retval nothing
 */
void LCD_SendCommand(uint8_t command);

/**
 * @brief Sends data to LCD screen
 * @param data Data to be writen on LCD screen
 * @retval nothing
 */
void LCD_SendData(uint8_t data);

/**
 * @brief Sets cursor in specific position
 * @param row Position Y of cursor, from 0 to 5, counted from top
 * @param column Position X of cursor, from 0 to 83, counted from left
 */
void LCD_SetCursor(uint8_t row, uint8_t column);

/**
 * @brief Draw sine symbol on LCD
 * @retval nothing
 */
void LCD_DrawSine(void);

/**
 * @brief Draw inverse sine symbol on LCD
 * @retval nothing
 */
void LCD_InverseSine(void);

/**
 * @brief Draw square symbol on LCD
 * @retval nothing
 */
void LCD_DrawSquare(void);

/**
 * @brief Draws inverse square symbol on LCD
 * @retval nothing
 */
void LCD_InverseSquare(void);

/**
 * @brief Draws sawtooth symbol on LCD
 * @retval nothing
 */
void LCD_DrawSawtooth(void);

/**
 * @brief Draws inverse sawtooth symbol on LCD
 * @retval nothing
 */
void LCD_InverseSawtooth(void);

/**
 * @brief Draws triangle symbol on LCD
 * @retval nothing
 */
void LCD_DrawTriangle(void);

/**
 * @brief Draws inverse triangle symbol on LCD
 * @retval nothing
 */
void LCD_InverseTriangle(void);

/**
 * @brief Sends default bytemap to LCD
 * @retval nothing
 */
void LCD_SendFrame();

/**
 * @brief Initializes LCD screen
 * @retval True if succeeded, otherwise falls
 */
bool LCD_Init(void);

#endif //SYNTH_LCD_H
