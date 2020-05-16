//
// Created by Albert on 08.05.2020.
//

#ifndef I2S_H
#define I2S_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2s.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"
#include "waves.h"
#include "uart.h"

bool I2S3_Init(void);

void SPI3_Handler(void);

#ifdef __cplusplus
}
#endif

#endif //I2S_H

