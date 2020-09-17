//
// Created by Albert on 15.09.2020.
//

#include "keyboard.h"

#define FILTER_SIZE (size_t)(SAMPLE_RATE / 100)
#define KEYS (size_t) (12)

// Shaper of envelope
static int16_t Filter[FILTER_SIZE];

static KeyNode keyboardArray[KEYS];

static uint8_t Wave;

// Filling array with functions
LCD_FuncPtr lcd_draw_array = { LCD_DrawSine, LCD_DrawSquare, LCD_DrawSawtooth, LCD_DrawTriangle};

LCD_FuncPtr lcd_inverse_array = { LCD_InverseSine, LCD_InverseSquare, LCD_InverseSawtooth, LCD_InverseTriangle};

struct KeyNode FillKey(double frequency, uint32_t gpioPin, GPIO_TypeDef *gpio)
{
    // Filling struct with data
    KeyNode key;
    key.frequency = frequency;
    key.samplesPerCycle = SAMPLE_RATE/(uint16_t) frequency;
    key.time = 0;
    key.gpiox = gpio;
    key.gpioPin = gpioPin;

    for(size_t index = 0; index < key.samplesPerCycle; ++index)
    {
        // Filling all waves arrays
        key.sineSamples[index] = GenerateSineSample(frequency, 0.1, &key.time) +
                GenerateSineSample( 2.0 * frequency, 0.06, &key.time) +
                GenerateSineSample( 4.0 * frequency, 0.03, &key.time) +
                GenerateSineSample( 0.5 * frequency, 0.07, &key.time) +
                GenerateSineSample( 0.25 * frequency, 0.03, &key.time);
        key.squareSamples[index] = GenerateSquareSample(frequency, 0.1, 0.5, &key.time) +
                GenerateSquareSample( 2.0 * frequency, 0.05, 0.5, &key.time) +
                GenerateSquareSample(0.25 * frequency, 0.02, 0.5, &key.time);
        key.triangleSamples[index] = GenerateTriangleSample(frequency, 0.1, &key.time) ;
        key.sawtoothSamples[index] = GenerateSawtoothSample(frequency, 0.1, 'p', &key.time);
        (key.time)++;
    }

    // Normalzing all samples to one level, so when feq keys are pressed there is no overflow
    Normalize(key.sineSamples, key.samplesPerCycle, 400);
    Normalize(key.squareSamples, key.samplesPerCycle, 400);
    Normalize(key.triangleSamples, key.samplesPerCycle, 400);
    Normalize(key.sawtoothSamples, key.samplesPerCycle, 400);

    key.time = 0;

    return key;
}

int16_t GetSample(void)
{
    int16_t sample = 0;
    for(size_t index = 0; index < KEYS; ++index)
    {
        // Checking if key is pressed
        if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(keyboardArray[index].gpiox, keyboardArray[index].gpioPin))
        {
            // Picking proper sample and giving shape to wave
            if(keyboardArray[index].time < 3 * SAMPLE_RATE && Wave == 0)
            {
                sample += (int16_t) (keyboardArray[index].sineSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * Filter[(keyboardArray[index].time / 300) & (SAMPLE_RATE / 100)]);
                ++(keyboardArray[index].time);
            }
            else if(keyboardArray[index].time< 2 * SAMPLE_RATE && Wave == 1)
            {
                sample += (int16_t) (keyboardArray[index].squareSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * Filter[(keyboardArray[index].time / 200) & 480]);
                ++(keyboardArray[index].time);
            }
            else if(keyboardArray[index].time < 2 * SAMPLE_RATE && Wave == 2)
            {
                sample += (int16_t) (keyboardArray[index].sawtoothSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * Filter[(keyboardArray[index].time / 200) & 480]);
                ++(keyboardArray[index].time);
            }
            else if(keyboardArray[index].time < 2 * SAMPLE_RATE && Wave == 3)
            {
                sample += (int16_t) (keyboardArray[index].triangleSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * Filter[(keyboardArray[index].time / 200) & 480]);
                ++(keyboardArray[index].time);
            }

        }
        // If not pressed, wave should start from beginning
        else
        {
            keyboardArray[index].time = 0;
        }
    }

    return sample;

}

void GetWave(void)
{
    if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4))
    {
        while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4))
        {
            ;
        }
        // Drawing normal symbol, than drawing inverse for chosen wave
        lcd_draw_array[Wave]();
        Wave = ++Wave % 4;
        lcd_inverse_array[Wave]();
    }
}

void KeyBoardInit(void)
{

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef keypadGPIO;
    keypadGPIO.Mode = GPIO_MODE_INPUT;
    keypadGPIO.Pull = GPIO_PULLUP;
    keypadGPIO.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &keypadGPIO);

    keypadGPIO.Mode = GPIO_MODE_INPUT;
    keypadGPIO.Pull = GPIO_PULLUP;
    keypadGPIO.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOB, &keypadGPIO);

    keypadGPIO.Mode = GPIO_MODE_INPUT;
    keypadGPIO.Pull = GPIO_PULLUP;
    keypadGPIO.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOC, &keypadGPIO);

    // Filling filter with samples, used only to shape envelope so signal rise and falls like real instrument
    FillFilter(Filter, sizeof(Filter)/sizeof(Filter[0]), 10.0, 0.0001, 110, 40);

    Wave = 0;
    // Inversing first wave symbol
    lcd_inverse_array[Wave]();

    // Filling all key structures
    keyboardArray[0] = FillKey(130.81,  GPIO_PIN_7, GPIOE); // C3
    keyboardArray[1] = FillKey(138.59, GPIO_PIN_9, GPIOE); // C3#
    keyboardArray[2] = FillKey(146.83, GPIO_PIN_11, GPIOE); // D3
    keyboardArray[3] = FillKey(155.56, GPIO_PIN_13, GPIOE); // D3#
    keyboardArray[4] = FillKey(164.81, GPIO_PIN_8, GPIOE); // E3
    keyboardArray[5] = FillKey(174.61, GPIO_PIN_10, GPIOE); // F3
    keyboardArray[6] = FillKey(185.00, GPIO_PIN_12, GPIOE); // F3#
    keyboardArray[7] = FillKey(196.00, GPIO_PIN_14, GPIOE); // G3
    keyboardArray[8] = FillKey(207.65, GPIO_PIN_10, GPIOB); // G3#
    keyboardArray[9] = FillKey(220.0, GPIO_PIN_15, GPIOE); // A3
    keyboardArray[10] = FillKey(233.08, GPIO_PIN_12, GPIOB); // A3#
    keyboardArray[11] = FillKey(246.94, GPIO_PIN_11, GPIOB); // B3

}