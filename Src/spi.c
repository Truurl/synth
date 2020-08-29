//
// Created by Albert on 19.05.2020.
//

#include "spi.h"
#include <stdio.h>

extern SPI_HandleTypeDef spiHandle;

struct Frame{
    bool change;
    uint8_t frame[48][84];
    uint8_t row;
    uint8_t column;
}frame;


size_t SPI2_SendData(uint8_t *data, size_t bytes, uint16_t timeout)
{

    uint16_t timeTaken = 0;
    size_t bytesSended = 0;

//    SPI2->CR1 |= SPI_CR1_BIDIOE | SPI_CR1_SPE;

    char buf[24];
    uint16_t temp;
    __HAL_SPI_ENABLE(&spiHandle);
    for(uint16_t i = 0; i < bytes; ++i)
    {

        temp = data[i];
        SPI2->DR = data[i];
//        sprintf(buf, "SPI2->DR: 0x%x\n\r", SPI2->DR);
//        UART_WriteString(buf);

        while(!__HAL_SPI_GET_FLAG(&spiHandle, SPI_FLAG_TXE) )
        {
            Delay(1);
            timeTaken++;

            if(timeTaken >= timeout)
            {
                SPI2->CR1 &= ~SPI_CR1_SPE;
                return bytesSended;
            }
        }

        bytesSended++;

    }

    while(__HAL_SPI_GET_FLAG(&spiHandle, SPI_FLAG_BSY))
    {
        ;
    }

    __HAL_SPI_DISABLE(&spiHandle);
    return bytesSended;

}

void SPI2_IRQHandler(void)
{
    if(__HAL_SPI_GET_FLAG(&spiHandle, SPI_FLAG_TXE))
    {
        if(__HAL_SPI_GET_IT_SOURCE(&spiHandle, SPI_IT_TXE))
        {
            SPI2->DR = frame.frame[frame.row++][frame.column++];

            if(frame.row == sizeof(frame.frame) && frame.column == sizeof(frame.frame[0]))
            {

                frame.row = 0;
                frame.column = 0;
                __HAL_SPI_DISABLE_IT(&spiHandle, SPI_IT_TXE);
            }

            frame.row %= sizeof(frame.frame);
            frame.column %= sizeof(frame.frame[0]);

        }
    }
}


bool SPI2_Init(void)
{

    __SPI2_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio;

    gpio.Pin = GPIO_PIN_13 | GPIO_PIN_15;
    gpio.Alternate = GPIO_AF5_SPI2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOB, &gpio);

    spiHandle.Instance = SPI2;
    spiHandle.Init.Mode = SPI_MODE_MASTER;
    spiHandle.Init.Direction = SPI_DIRECTION_1LINE;
    spiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    spiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    spiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    spiHandle.Init.NSS = SPI_NSS_SOFT;
    spiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spiHandle.Init.CRCPolynomial = 10;

    if(HAL_OK != HAL_SPI_Init(&spiHandle))
    {
        UART_WriteString("SPI2 init failed\n\r");
        return false;
    }

    SPI2->CR1 |= SPI_CR1_BIDIOE;
    SPI2->CR1 &= ~SPI_CR1_CPOL;
    HAL_NVIC_SetPriority(SPI2_IRQn,1,3);
    HAL_NVIC_EnableIRQ(SPI2_IRQn);
//    __HAL_SPI_ENABLE_IT(&spiHandle, SPI_IT_TXE);

    return true;
}


