//
// Created by Albert on 19.05.2020.
//

#include "spi.h"

extern SPI_HandleTypeDef SPI_Handle;

// Defines for better readability
#define CLK_PIN (GPIO_PIN_5)
#define DATA_PIN (GPIO_PIN_7)

bool SPI1_Init(void)
{
    // Enabling clocks for SPI and GPIOA
    __SPI1_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();

    // Switching pins to spi functions
    GPIO_InitTypeDef gpio;

    gpio.Pin = CLK_PIN | DATA_PIN;
    gpio.Alternate = GPIO_AF5_SPI1;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_LOW;

    // Initialization of gpioa
    HAL_GPIO_Init(GPIOA, &gpio);

    // Filling struct for initialization
    SPI_Handle.Instance = SPI1;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLE;
    SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPI_Handle.Init.CRCPolynomial = 10;

    // Checking if initialization is successfull
    if(HAL_OK != HAL_SPI_Init(&SPI_Handle))
    {
        return false;
    }

    return true;
}
