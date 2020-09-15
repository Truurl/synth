//
// Created by Albert on 19.05.2020.
//

#include "spi.h"
#include <stdio.h>

extern SPI_HandleTypeDef SPI_Handle;

static DMA_HandleTypeDef SPI_DMA_Handle;

#define CLK_PIN (GPIO_PIN_5)
#define DATA_PIN (GPIO_PIN_7)

//uint8_t bytemap[];


size_t SPI1_SendData(uint8_t *data, uint16_t timeout)
{

    uint16_t timeTaken = 0;

    char buf[64];

    SPI1->CR1 |= SPI_CR1_SPE;

    sprintf(buf, "SPI->SR = %x | ", SPI1->SR);
    UART_WriteString(buf);

    if(__HAL_SPI_GET_FLAG(&SPI_Handle, SPI_FLAG_TXE))
    {
        SPI1->DR = (*data);
    }


    sprintf(buf, "data = %x | SPI->DR = %x\n\r", *data);
    UART_WriteString(buf);




//    while(__HAL_SPI_GET_FLAG(&SPI_Handle, SPI_FLAG_TXE) )
//    {
////            Delay(1);
//        timeTaken++;
//
//        if(timeTaken >= timeout)
//        {
//            UART_WriteString("SPI timeout\n\r");
//            Delay(5);
//            SPI1->CR1 &= ~SPI_CR1_SPE;
//        }
//    }


    while(__HAL_SPI_GET_FLAG(&SPI_Handle, SPI_FLAG_BSY))
    {
        ;
    }

    SPI1->CR1 &= ~SPI_CR1_SPE;

}

void SPI2_IRQHandler(void)
{
    if(__HAL_SPI_GET_FLAG(&SPI_Handle, SPI_FLAG_TXE))
    {
        if(__HAL_SPI_GET_IT_SOURCE(&SPI_Handle, SPI_IT_TXE))
        {
//            SPI1->DR = frame.frame[frame.row++][frame.column++];

        }
    }
}


bool SPI1_Init(void)
{

    __SPI1_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio;

    gpio.Pin = CLK_PIN | DATA_PIN;
    gpio.Alternate = GPIO_AF5_SPI1;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init(GPIOA, &gpio);

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

    if(HAL_OK != HAL_SPI_Init(&SPI_Handle))
    {
        UART_WriteString("SPI1 init failed\n\r");
        return false;
    }

//    SPI1->CR1 |= SPI_CR1_BIDIOE;
//    SPI1->CR1 &= ~SPI_CR1_CPOL;
//    HAL_NVIC_SetPriority(SPI1_IRQn,1,3);
//    HAL_NVIC_EnableIRQ(SPI1_IRQn);
//    __HAL_SPI_ENABLE_IT(&SPI_Handle, SPI_IT_TXE);

//    __HAL_SPI_ENABLE(&SPI_Handle);
//
//    if(!SPI_DMA_Init())
//    {
//        UART_WriteString("DMA init  fials\n\r");
//        return false;
//    }
//
//    __HAL_LINKDMA(&SPI_Handle, hdmatx, SPI_DMA_Handle);
    return true;
}

bool SPI_DMA_Init(void)
{
    __DMA2_CLK_ENABLE();
    SPI_DMA_Handle.Instance = DMA2_Stream3;
    SPI_DMA_Handle.Init.Channel = DMA_CHANNEL_3;
    SPI_DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    SPI_DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    SPI_DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
    SPI_DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    SPI_DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    SPI_DMA_Handle.Init.Mode = DMA_NORMAL;
    SPI_DMA_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    SPI_DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
//    SPI_DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
//    SPI_DMA_Handle.Init.MemBurst = DMA_MBURST_INC16;
//    SPI_DMA_Handle.Init.PeriphBurst = DMA_PBURST_INC16;

    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

    if(HAL_OK != HAL_DMA_Init(&SPI_DMA_Handle))
    {
        return false;
    }

    return true;
}


void DMA2_Stream3_IRQHandler(void)
{
    UART_WriteString("DMA handler\n\r");
    Delay(1);
}

