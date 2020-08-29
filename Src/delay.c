//
// Created by Albert on 23.04.2020.
//

#include "delay.h"

volatile uint32_t ticks = 0;

void SysTick_Handler(void)
{
    ++ticks;
    //HAL_IncTick();
}

void Delay(uint32_t miliseconds)
{
    ticks = 0;
    uint32_t delay = ticks + miliseconds;
    while ( (int32_t)(delay - ticks) >= 0)
    {
        ;
    }
}
