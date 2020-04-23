//
// Created by Albert on 23.04.2020.
//
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "uart.h"

bool UART_PutChar(char c)
{

    __disable_irq();
    if (RingBuffer_PutChar(&USART_RingBuffer_Tx, c))
    {
                __USART_ENABLE_IT(&UartHandle, USART_IT_TXE);
        CORE_ExitCriticalSection();
        return true;
    }
    else
    {
        __enable_irq();
        return false;
    }
}

size_t UART_WriteData(const void* data, size_t dataSize)
{
    size_t i = 0;
    char* temp = (char*) data;
    for (; i<dataSize; i++)
    {
        if (RingBuffer_PutChar(&USART_RingBuffer_Tx, temp[i]))
        {
                    __USART_ENABLE_IT(&UartHandle, USART_IT_TXE);
        }
        else
        {
            return i;
        }
    }
    return i;
}

size_t UART_WriteString(const char* string)
{

    return USART_WriteData(string, strlen(string));

}

bool UART_GetChar(char* c)
{

    __disable_irq();
    CORE_EnterCriticalSection();
    if (!RingBuffer_GetChar(&USART_RingBuffer_Rx, c))
    {
        __enable_irq();
        return false;
    }

    __disable_irq();
    return true;
}

size_t UART_ReadData(char* data, size_t maxSize)
{
    size_t i = 0;

    for (; i<maxSize; ++i)
    {
        if (!RingBuffer_GetChar(&USART_RingBuffer_Rx, &data[i]))
        {
            return i;
        }
    }
}

// USART Interrupt Service Routine (ISR)
void USARTx_IRQHandler(void)
{

    if (__HAL_USART_GET_FLAG(&UartHandle, USART_FLAG_RXNE))
    {
        // the RXNE interrupt has occurred
        if (__HAL_USART_GET_IT_SOURCE(&UartHandle, USART_IT_RXNE))
        {
            // the RXNE interrupt is enabled

            // TODO: read the received character and place it in the receive ring buffer
            char c;
            c = USARTx->DR;
            RingBuffer_PutChar(&USART_RingBuffer_Rx, c);
        }
    }

    if (__HAL_USART_GET_FLAG(&UartHandle, USART_FLAG_TXE))
    {
        // the TXE interrupt has occurred
        if (__HAL_USART_GET_IT_SOURCE(&UartHandle, USART_IT_TXE))
        {
            // the TXE interrupt is enabled

            // TODO: get a character from the transmit ring buffer and send it via UART

            char c;
            if (!RingBuffer_GetChar(&USART_RingBuffer_Tx, &c))
            {
                        __USART_DISABLE_IT(&UartHandle, USART_IT_TXE);
            }
            else
            {
                USARTx->DR = c;
            }
        }
    }
}

bool UARTInit(void)
{
    //inicjalizacja pinow odpowiedzialnych za UART
            __GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpio;
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Alternate = GPIO_AF8_UART4;
    gpio.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(GPIOA, &gpio);

    //inicjalizacjia UART
            __UART4_CLK_ENABLE();

    UART_HandleTypeDef uart;
    uart.Init.BaudRate = 115200;
    uart.Init.WordLength = UART_WORDLENGTH_8B;
    uart.Init.Mode = UART_MODE_TX_RX;
    uart.Init.Parity = UART_PARITY_NONE;
    uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart.Init.OverSampling = UART_OVERSAMPLING_16;
    uart.Init.StopBits = UART_STOPBITS_1;
    uart.Instance = UART4;


    //HAL_UART_MspInit(&uart);
    if (HAL_OK!=HAL_UART_Init(&uart))
    {
        return false;
    }

    //właczenie przerwań od RX
    __HAL_UART_ENABLE_IT(&uart, UART_IT_RXNE);
    HAL_NVIC_SetPriority(UART4_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

    return true;
}
