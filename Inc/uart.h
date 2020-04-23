//
// Created by Albert on 23.04.2020.
//

#ifndef PROJEKTAMK_UART_H
#define PROJEKTAMK_UART_H

bool UART_PutChar(char c);

size_t UART_WriteData(const void* data, size_t dataSize);

size_t UART_WriteString(const char* string);

bool UART_GetChar(char* c);

size_t UART_ReadData(char* data, size_t maxSize);

// USART Interrupt Service Routine (ISR)
void UART4_IRQHandler(void);

bool UARTInit(void);

#endif //PROJEKTAMK_UART_H
