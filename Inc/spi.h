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
#include "stm32f4xx_hal_tim.h"
#include "uart.h"
#include "delay.h"

size_t SPI2_SendData(uint8_t *data, size_t bytes, uint16_t timeout);

void SPI2_IRQHandler(void);

bool SPI2_Init(void);


#endif //SYNTH_SPI_H
