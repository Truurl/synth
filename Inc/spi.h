//
// Created by Albert on 19.05.2020.
//

#ifndef SYNTH_SPI_H
#define SYNTH_SPI_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"

/*
 * @brief Initializes SPI1 for LCD screem
 * @return True if succeed, otherwise false
 */
bool SPI1_Init(void);

#endif //SYNTH_SPI_H
