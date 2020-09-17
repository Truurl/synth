//
// Created by Albert on 15.09.2020.
//

#ifndef SYNTH_KEYBOARD_H
#define SYNTH_KEYBOARD_H

#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "sampleGeneration.h"
#include "lcd.h"

/**
 * @typedef struct describing keys able to make sound
 */
typedef struct KeyNode
{
    /*{*/
    double frequency; /**< frequency of wave */
    uint16_t samplesPerCycle; /**< how many samples is in one cycle of wave */
    uint64_t time; /**< tracks which sample should be added or played*/
    GPIO_TypeDef *gpiox; /**< gpio which tact switch is connected */
    uint16_t gpioPin; /**< pin on which tact switch is connected */
    int16_t sineSamples[400]; /**< samples for sine wave */
    int16_t squareSamples[400]; /**< samples for square wave */
    int16_t triangleSamples[400]; /**< samples for triangle wave */
    int16_t sawtoothSamples[400]; /**< samples for sawtooth wave */
    /*}*/
}KeyNode;

/**
 * @brief Fill KeyNode struct with data
 * @param frequency frequency of wave
 * @param gpioPin pin on which tact switch is connected
 * @param gpio port on which tact switch is connected
 * @return filled KeyNode struct
 */
struct KeyNode FillKey(double frequency, uint32_t gpioPin, GPIO_TypeDef *gpio);

/**
 * @brief Gets samples from all tact switches and adds them to one variable
 * @return samples from all keys added together
 */
int16_t GetSample(void);

/**
 * @brief Change wave if button is pressed
 * @return nothing
 */
void GetWave(void);

/**
 * @brief Initializes whole keyboard
 * @return nothing
 */
void KeyBoardInit(void);

#endif //SYNTH_KEYBOARD_H
