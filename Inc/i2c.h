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

#define I2C_READ (0x01)
#define I2C_WRITE (0x00)
#define I2C_REG_INC (0x80)

/**
 * @brief Reads data to other's IC register via I2C
 *
 * @param slaveAddress Address of slave IC
 * @param regAddress Address of slave register
 * @param data Buffer for holding response from IC
 * @param timeout Waiting time for return from IC
 *
 * @return True if response was received before timeout, otherwise false
 */
bool I2C_ReadData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint16_t timeout);

/**
 * @brief Sends data to IC's register
 *
 * @param slaveAddress Address of slave IC
 * @param regAddress Register address of slave IC
 * @param data Buffer with data to be sended
 *
 * @return Bytes sended by function, should return 3 if everything went good
 */
size_t I2C_SendData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data);

/**
 * @brief Function initializing I2C1
 *
 * @return True if succeeded, otherwise false
 */
bool I2C1_Init(void);

#endif //I2C_H
