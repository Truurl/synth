//
// Created by Albert on 29.04.2020.
//

#include "CS43L22.h"
#include "sampleGeneration.h"
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "delay.h"

#define FILTER_SIZE (SAMPLE_RATE / 100)
#define KEYS (size_t) (12)


static KeyNode *headKey = NULL;

static int16_t SRRC_Filter[FILTER_SIZE];

static KeyNode keyboardArray[KEYS];

static uint8_t Wave;

LCD_FuncPtr lcd_draw_array = { LCD_DrawSine, LCD_DrawSquare, LCD_DrawSawtooth, LCD_DrawTriangle};

LCD_FuncPtr lcd_inverse_array = { LCD_InverseSine, LCD_InverseSquare, LCD_InverseSawtooth, LCD_InverseTriangle};

int16_t Max(int16_t *buffer, size_t n)
{
    int16_t max = *buffer;

    for(size_t index = 0; index < n; ++index)
    {
        if(max <= *(buffer + index))
        {
            max = *(buffer + index);
        }
    }
    return max;
}

void Normalize(int16_t *buffer, size_t n, int16_t maxValue)
{
    int16_t maximum = Max(buffer, n);

    for(uint16_t index = 0; index < n; ++index)
    {
        *(buffer + index) =  (int16_t)  ((( (double) *(buffer + index) )  / maximum) * maxValue);
    }
}



void FillFilter(int16_t *buffer, size_t n, double amplitude, double alfa, uint16_t delay, uint16_t sustain)
{
    double Ts = 2.0 / n;
    double dt = 1.0 / SAMPLE_RATE;
    char buf[32];
    uint16_t i = 0;
    uint16_t t = 0;

    for(uint16_t index = 0; index < n; ++index)
    {

        if ((delay - index) == 0)
        {
            *(buffer + index) = (int16_t) round(amplitude * (1.0 - alfa) + (4.0 * alfa) / M_PI);
            for(t; t < sustain; ++t, ++index)
            {
                *(buffer + index) = (int16_t) round(amplitude * (1.0 - alfa) + (4.0 * alfa) / M_PI);
            }
        }
        else if (isnan( amplitude * (alfa / sqrt(2.0)) * ( ( 1 + ( 2.0 / M_PI)) * sin( M_PI / ( 4.0 * alfa)) +  ( 1 - ( 2.0 / M_PI)) * cos( M_PI / ( 4.0 * alfa)) )))
        {
            *(buffer + index) = round(amplitude * (alfa / sqrt(2.0)) * ( ( 1 + ( 2.0 / M_PI)) * sin( M_PI / ( 4.0 * alfa)) +  ( 1 - ( 2.0 / M_PI)) * cos( M_PI / ( 4.0 * alfa)) ));
        }
        else
        {
            *(buffer + index) = round(amplitude * (sin(M_PI * (1.0 - alfa) * ((delay - index - i + t) * dt / Ts)) +
                    4.0 * alfa * ((delay - index - i + t) * dt / Ts) * cos(M_PI * (1.0 + alfa) * ((delay - index - i + t) * dt / Ts))) /
                    (( M_PI * ((delay - index - i + t) * dt / Ts)) * ( 1 - pow(4 * alfa *  ((delay - index - i + t) * dt / Ts), 2.0) )));
        }
        if(*(buffer + index) <= 0)
        {
            *(buffer + index) = 0;
        }
    }

    UART_WriteString("samples = [");
    for(size_t index = 0; index < n; ++index)
    {
        sprintf(buf, "%d, ", *(buffer + index));
        UART_WriteString(buf);
    }
    UART_WriteString("\b\b]; plot(samples)\n\r");
}


struct KeyNode* FillKey(double frequency, uint32_t gpioPin, GPIO_TypeDef *gpio, uint8_t column, char keyAlign)
{
    char buf[16];

    KeyNode *key = (struct KeyNode*) malloc(sizeof(struct KeyNode));
    key->frequency = frequency;
    key->samplesPerCycle = SAMPLE_RATE/(uint16_t) frequency;

    key->time = 0;
    key->gpiox = gpio;
    key->gpioPin = gpioPin;
    key->column = column;
//    key->nextKey = NULL;

    for(size_t index = 0; index < key->samplesPerCycle; ++index)
    {
        key->sineSamples[index] = GenerateSineSample(frequency, 0.1, &key->time) +
                                 GenerateSineSample( 2.0 * frequency, 0.03, &key->time) +
                                 GenerateSineSample( 4.0 * frequency, 0.07, &key->time) +
                                 GenerateSineSample( 0.5 * frequency, 0.07, &key->time)+
                                 GenerateSineSample( 0.25 * frequency, 0.03, &key->time);
        key->squareSamples[index] = GenerateSquareSample(frequency, 0.1, 0.5, &key->time);// +
//                                 GenerateSquareSample( 2.0 * frequency, 0.05, 0.5, &key->time) +
//                                 GenerateSquareSample( 3.0 * frequency, 0.07, 0.5, &key->time) +
//                                 GenerateSquareSample(4.0 * frequency, 0.02, 0.5, &key->time);
        key->triangleSamples[index] = GenerateTriangleSample(frequency, 0.01, &key->time);// +
////                                 GenerateTriangleSample( 2.0 * frequency, 0.005, &key->time) +
////                                 GenerateTriangleSample( 3.0 * frequency, 0.007, &key->time) +
////                                 GenerateTriangleSample( 4.0 * frequency, 0.002, &key->time);
        key->sawtoothSamples[index] = GenerateSawtoothSample(frequency, 0.01, 'p', &key->time);// +
//                                 GenerateSawtoothSample( 2.0 * frequency, 0.01, 'p', &key->time) +
//                                 GenerateSawtoothSample( 3.0 * frequency, 0.007, 'p', &key->time) +
//                                 GenerateSawtoothSample( 4.0 * frequency, 0.002, 'p', &key->time);
        (key->time)++;
    }
    Normalize(key->sineSamples, key->samplesPerCycle, 400);
    Normalize(key->squareSamples, key->samplesPerCycle, 400);
    Normalize(key->triangleSamples, key->samplesPerCycle, 400);
    Normalize(key->sawtoothSamples, key->samplesPerCycle, 400);
    key->time = 0;

    return key;
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

    Wave = 0;

    keyboardArray[0] = *FillKey(130.81,  GPIO_PIN_7, GPIOE, C3_COL, 'l'); // C3
    keyboardArray[1] = *FillKey(138.59, GPIO_PIN_9, GPIOE, CIS_COL, 'h'); // C3#
    keyboardArray[2] = *FillKey(146.83, GPIO_PIN_11, GPIOE, D3_COL, 'm'); // D3
    keyboardArray[3] = *FillKey(155.56, GPIO_PIN_13, GPIOE, DIS_COL, 'h'); // D3#
    keyboardArray[4] = *FillKey(164.81, GPIO_PIN_8, GPIOE, E3_COL, 'r'); // E3
    keyboardArray[5] = *FillKey(174.61, GPIO_PIN_10, GPIOE, F3_COL, 'l'); // F3
    keyboardArray[6] = *FillKey(185.00, GPIO_PIN_12, GPIOE, FIS_COL, 'h'); // F3#
    keyboardArray[7] = *FillKey(196.00, GPIO_PIN_14, GPIOE, G3_COL, 'm'); // G3
    keyboardArray[8] = *FillKey(207.65, GPIO_PIN_10, GPIOB, GIS_COL, 'h'); // G3#
    keyboardArray[9] = *FillKey(220.0, GPIO_PIN_15, GPIOE, A3_COL, 'm'); // A3
    keyboardArray[10] = *FillKey(233.08, GPIO_PIN_12, GPIOB, AIS_COL, 'h'); // A3#
    keyboardArray[11] = *FillKey(246.94, GPIO_PIN_11, GPIOB, B3_COL, 'r'); // B3

}

int16_t GetSample()
{
    int16_t sample = 0;
    for(size_t index = 0; index < KEYS; ++index)
    {
        if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(keyboardArray[index].gpiox, keyboardArray[index].gpioPin))
        {
            if(keyboardArray[index].time < 3 * SAMPLE_RATE && Wave == 0)
            {
                sample += (int16_t) (keyboardArray[index].sineSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * SRRC_Filter[(keyboardArray[index].time / 300) & (SAMPLE_RATE / 100)]);
                ++(keyboardArray[index].time);
            }
            else if(keyboardArray[index].time< 2 * SAMPLE_RATE && Wave == 1)
            {
                sample += (int16_t) (keyboardArray[index].squareSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * SRRC_Filter[(keyboardArray[index].time / 200) & 480]);
                ++(keyboardArray[index].time);
            }
            else if(keyboardArray[index].time < 2 * SAMPLE_RATE && Wave == 2)
            {
                sample += (int16_t) (keyboardArray[index].sawtoothSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * SRRC_Filter[(keyboardArray[index].time / 200) & 480]);
                ++(keyboardArray[index].time);
            }
            else if(keyboardArray[index].time < 2 * SAMPLE_RATE && Wave == 3)
            {
                sample += (int16_t) (keyboardArray[index].triangleSamples[keyboardArray[index].time % keyboardArray[index].samplesPerCycle]
                        * SRRC_Filter[(keyboardArray[index].time / 200) & 480]);
                ++(keyboardArray[index].time);
            }

        }
        else
        {
            keyboardArray[index].time = 0;
        }
    }


    return sample;
}

void GetWave()
{
    if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4))
    {
        while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4))
        {
            ;
        }
        lcd_draw_array[Wave]();
//        LCD_DrawSine();
        Wave = ++Wave % 4;
        lcd_inverse_array[Wave]();
//        LCD_InverseSawtooth();
    }
}


bool CS43L22_Init(void)
{
    KeyBoardInit();
    FillFilter(SRRC_Filter, sizeof(SRRC_Filter)/sizeof(SRRC_Filter[0]), 10.0, 0.0001, 100, 40);

	__GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = GPIO_PIN_4;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOD, &gpio);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);

	if(false == I2C1_Init())
	{
		UART_WriteString("I2C1 init error\n\r");
		return false;
	}

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	uint8_t data = 0x00;

	char buf[32];

	data = HEADPHONE_UP | SPEAKER_DOWN;
	I2C_SendData(CS43L22_ADDR, PWR_CONTROL2, &data);
	data = CLK_AUTO_OFF | CLK_SINGLE_SPEED | CLK_32_RATE_NO | CLK_VIDEO_NO | CLK_IN_RATIO_128 | CLK_NO_DIVIDE;
	I2C_SendData(CS43L22_ADDR, CLK_CONTROL, &data);
	data = INT_MODE_SLAVE | INT_POL_NORM | INT_DSP_DISABLE | INT_FORMAT_I2S | INT_WORD_16BIT;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_1, &data);
	data = INT_MCLK_EQ_SCLK | INT_SW_NORM;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_2, &data);
	data = 0x99;
	I2C_SendData(CS43L22_ADDR, 0x00, &data);
	data = 0x80;
	I2C_SendData(CS43L22_ADDR, 0x47, &data);
	data = 0xff;
	I2C_SendData(CS43L22_ADDR, 0x32, &data);
	data = 0x7f;
	I2C_SendData(CS43L22_ADDR, 0x32, &data);
	data = 0x00;
	I2C_SendData(CS43L22_ADDR, 0x00, &data);
	data = 0x9e;
	I2C_SendData(CS43L22_ADDR, PWR_CONTROL1, &data);

	if(false == I2S3_Init())
	{
		UART_WriteString("I2S3 init error\n\r");
		return false;
	}

	return true;
}

