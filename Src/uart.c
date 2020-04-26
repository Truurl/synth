//
// Created by Albert on 23.04.2020.
//
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "uart.h"
#include "ring_buffer.h"
#include "delay.h"

// UART transmit buffer descriptor
static RingBuffer UART_RingBuffer_Tx;
// UART transmit buffer memory pool
static char RingBufferData_Tx[2048];

// UART receive buffer descriptor
static RingBuffer UART_RingBuffer_Rx;
// UART receive buffer memory pool
static char RingBufferData_Rx[2048];


static UART_HandleTypeDef uart;

bool UART_PutChar(char c)
{

    //__disable_irq();
    if (RingBuffer_PutChar(&UART_RingBuffer_Tx, c))
    {
        __HAL_UART_ENABLE_IT(&uart, UART_IT_TXE);
        //__enable_irq();
        return true;
    }
    else
    {
        //__enable_irq();
        return false;
    }
}

size_t UART_WriteData(const void* data, size_t dataSize)
{
//    size_t i = 0;
//    for (; i<dataSize; i++)
//    {
//        if (RingBuffer_PutChar(&UART_RingBuffer_Tx, ((char *) data)[i]))
//        {
//
//        }__HAL_UART_ENABLE_IT(&uart, UART_IT_TXE);
//        else
//        {
//            return i;
//        }
//    }
//    return i;

	size_t i = 0;
	for (; i<dataSize; i++)
	{
		if (!RingBuffer_PutChar(&UART_RingBuffer_Tx, ((char *) data)[i]))
		{
			return i;
		}
	}
	__HAL_UART_ENABLE_IT(&uart, UART_IT_TXE);
	return i;
}

size_t UART_WriteString(const char* string)
{

    return UART_WriteData(string, strlen(string));

}

bool UART_GetChar(char* c)
{

    //__disable_irq();
    if (!RingBuffer_GetChar(&UART_RingBuffer_Rx, c))
    {
        //__enable_irq();
        return false;
    }

    //__disable_irq();
    return true;
}

size_t UART_ReadData(char* data, size_t maxSize)
{
    size_t i = 0;

    for (; i<maxSize; ++i)
    {
        if (!RingBuffer_GetChar(&UART_RingBuffer_Rx, &data[i]))
        {
            return i;
        }
    }
}

// UART Interrupt Service Routine (ISR)
void UART4_IRQHandler(void)
{

    if (__HAL_UART_GET_FLAG(&uart, UART_FLAG_RXNE))
    {
        // the RXNE interrupt has occurred
        if (__HAL_UART_GET_IT_SOURCE(&uart, UART_IT_RXNE))
        {
            // the RXNE interrupt is enabled
            char c;
            c = UART4->DR;
            RingBuffer_PutChar(&UART_RingBuffer_Rx, c);
        }
    }

    if (__HAL_UART_GET_FLAG(&uart, UART_FLAG_TXE))
    {
        // the TXE interrupt has occurred
        if (__HAL_UART_GET_IT_SOURCE(&uart, UART_IT_TXE))
        {
            // the TXE interrupt is enabled
            char c;
            if (!RingBuffer_GetChar(&UART_RingBuffer_Tx, &c))
            {
                __HAL_UART_DISABLE_IT(&uart,UART_IT_TXE);
            }
            else
            {
                UART4->DR = c;
            }
        }
    }
    HAL_NVIC_ClearPendingIRQ(UART4_IRQn);
}

bool UARTInit(void)
{
    // initialize ring buffers
    RingBuffer_Init(&UART_RingBuffer_Tx, RingBufferData_Tx, sizeof(RingBufferData_Tx));
    RingBuffer_Init(&UART_RingBuffer_Rx, RingBufferData_Rx, sizeof(RingBufferData_Rx));

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

    uart.Init.BaudRate = 115200;
    uart.Init.WordLength = UART_WORDLENGTH_8B;
    uart.Init.Mode = UART_MODE_TX_RX;
    uart.Init.Parity = UART_PARITY_NONE;
    uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart.Init.OverSampling = UART_OVERSAMPLING_16;
    uart.Init.StopBits = UART_STOPBITS_1;
    uart.Instance = UART4;


    //HAL_UART_MspInit(&uart);
    if(HAL_OK != HAL_UART_Init(&uart))
    {
        return false;
    }

    //właczenie przerwań od RX
    __HAL_UART_ENABLE_IT(&uart, UART_IT_RXNE);
    HAL_NVIC_SetPriority(UART4_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

    return true;
}
