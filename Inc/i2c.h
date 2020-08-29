//
// Created by Albert on 27.04.2020.
//

#ifndef I2C_H
#define I2C_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_gpio.h"
#include "ring_buffer.h"
#include "uart.h"
#include "delay.h"

bool I2C_ReadRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint16_t timeout);

bool I2C_ReadData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint16_t timeout);

size_t I2C_WriteRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, size_t bytes);

size_t I2C_SendData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data);

void I2C1_Handler(void);

bool I2C1_Init(void);

#endif //I2C_H
