//
// Created by Albert on 23.04.2020.
//

#ifndef PROJEKTAMK_DELAY_H
#define PROJEKTAMK_DELAY_H

#include <stdint.h>

extern volatile uint32_t ticks;

void SysTick_Handler(void);

void Delay(uint32_t miliseconds);

#endif //PROJEKTAMK_DELAY_H
