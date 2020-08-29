//
// Created by Albert on 08.05.2020.
//

#ifndef I2S_H
#define I2S_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2s.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"
#include "sampleGeneration.h"
#include "uart.h"

bool I2S3_Init(void);

void SPI3_Handler(void);

#endif //I2S_H

