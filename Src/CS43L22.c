//
// Created by Albert on 29.04.2020.
//

#include "CS43L22.h"
#include "sampleGeneration.h"
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "delay.h"

static KeyNode *headKey = NULL;

static int16_t SRRC_Filter[480];

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



void FillFilter(int16_t *buffer, size_t n, double alfa, uint16_t delay, uint16_t sustain)
{
    double Ts = 2.0 / n;
    double dt = 1.0 / SAMPLE_RATE;
    char buf[16];
    double temp;
    uint16_t i = 0;

    for(uint16_t index = 0; index < n; ++index)
    {
        temp = 10.0 * (sin(M_PI * (1.0 - alfa) * ((delay - index) * dt / Ts)) +
                4.0 * alfa * ((delay - index) * dt / Ts) * cos(M_PI * (1.0 + alfa) * ((delay - index) * dt / Ts))) /
                (( M_PI * ((delay - index) * dt / Ts)) * ( 1 - pow(4 * alfa *  ((delay - index) * dt / Ts), 2.0) ));
        if ((delay - index) == 0)
        {
            temp = 10.0 * (1.0 - alfa) + (4.0 * alfa) / M_PI;
            *(buffer + index) = (int16_t) round(temp);
            ++index;
            for(i = 0; i < sustain; ++i)
            {
                temp = 10.0 * (1.0 - alfa) + (4.0 * alfa) / M_PI;
                *(buffer + index + i) = (int16_t) round(temp);
            }
        }
        else if (isnan( temp))
        {
            temp = 10.0 * (alfa / sqrt(2.0)) * ( ( 1 + ( 2.0 / M_PI)) * sin( M_PI / ( 4.0 * alfa)) +  ( 1 - ( 2.0 / M_PI)) * cos( M_PI / ( 4.0 * alfa)) );
        }

        *(buffer + index + i) = (int16_t) round(temp);
    }

//    UART_WriteString("[");
//    for(size_t index = 0; index < n; ++index)
//    {
//        sprintf(buf, "%d, ", *(buffer + index));
//        UART_WriteString(buf);
//    }
//    UART_WriteString("\b\b];\n\r");
}


struct KeyNode* FillKey(double frequency, uint32_t gpioPin, GPIO_TypeDef *gpio)
{
//    UART_WriteString("wypelniam klawisz\n\r");
    char buf[16];
//    size_t n = (SAMPLE_RATE/(uint16_t) frequency) * sizeof(int16_t);

    KeyNode *key = (struct KeyNode*) malloc(sizeof(struct KeyNode));
    key->frequency = frequency;
    key->samplesPerCycle = SAMPLE_RATE/(uint16_t) frequency;
//    sprintf(buf, "size of samples=%d\n\r", key->samplesPerCycle);
//    UART_WriteString(buf);
    key->time = 0;
    key->gpiox = gpio;
    key->gpioPin = gpioPin;
    key->nextKey = NULL;

//    sprintf(buf, "size of samples=%d\n\r", key->samplesPerCycle);
//    UART_WriteString(buf);

    for(size_t index = 0; index < key->samplesPerCycle; ++index)
    {
        key->sineSamples[index] = GenerateSineSample(frequency, 0.1, &key->time) +
                                 GenerateSineSample( 2.0 * frequency, 0.05, &key->time) +
                                 GenerateSineSample( 3.0 * frequency, 0.07, &key->time) +
                                 GenerateSineSample( 4.0 * frequency, 0.02, &key->time);
        key->squareSamples[index] = GenerateSquareSample(frequency, 0.1, 0.5, &key->time) +
                                 GenerateSquareSample( 2.0 * frequency, 0.05, 0.5, &key->time) +
                                 GenerateSquareSample( 3.0 * frequency, 0.07, 0.5, &key->time) +
                                 GenerateSquareSample(4.0 * frequency, 0.02, 0.5, &key->time);
        key->triangleSamples[index] = GenerateTriangleSample(frequency, 0.01, &key->time);// +
////                                 GenerateTriangleSample( 2.0 * frequency, 0.005, &key->time) +
////                                 GenerateTriangleSample( 3.0 * frequency, 0.007, &key->time) +
////                                 GenerateTriangleSample( 4.0 * frequency, 0.002, &key->time);
        key->sawtoothSamples[index] = GenerateSawtoothSample(frequency, 0.01, 'p', &key->time);// +
//                                 GenerateSawtoothSample( 2.0 * frequency, 0.01, 'p', &key->time) +
//                                 GenerateSawtoothSample( 3.0 * frequency, 0.007, 'p', &key->time) +
//                                 GenerateSawtoothSample( 4.0 * frequency, 0.002, 'p', &key->time);
//        //        Delay(1);
        (key->time)++;
    }

    Normalize(key->sineSamples, key->samplesPerCycle, 400);
    Normalize(key->squareSamples, key->samplesPerCycle, 400);
    Normalize(key->triangleSamples, key->samplesPerCycle, 400);
    Normalize(key->sawtoothSamples, key->samplesPerCycle, 400);
    key->time = 0;
    UART_WriteString("samples = [");
    for(size_t i = 0; i < key->samplesPerCycle; ++i)
    {
        sprintf(buf, "%d, ", key->squareSamples[i]);
        UART_WriteString(buf);
        Delay(1);
    }
    UART_WriteString("\b\b];\n\r");

    return key;
}

bool AddKeyToList(KeyNode *key)
{

    if(NULL == headKey)
    {
        headKey = key;
        headKey->nextKey = NULL;
        return true;
    }

    KeyNode *temp = headKey;

    while(NULL != temp->nextKey)
    {
        temp = temp->nextKey;
    }

    temp->nextKey = key;
    return true;
}

void KeyBoardInit(void)
{

    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef keypadGPIO;
    keypadGPIO.Mode = GPIO_MODE_INPUT;
    keypadGPIO.Pull = GPIO_PULLUP;
    keypadGPIO.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOE, &keypadGPIO);

    KeyNode *key = FillKey(262.0,  GPIO_PIN_8, GPIOE);
    AddKeyToList(key);
    key = FillKey(293.0, GPIO_PIN_10, GPIOE);
    AddKeyToList(key);
    key = FillKey(329.0, GPIO_PIN_12, GPIOE);
    AddKeyToList(key);
    key = FillKey(349.0, GPIO_PIN_13, GPIOE);
    AddKeyToList(key);
    key = FillKey(392.0, GPIO_PIN_7, GPIOE);
    AddKeyToList(key);
    key = FillKey(440.0, GPIO_PIN_9, GPIOE);
    AddKeyToList(key);
    key = FillKey(493.0, GPIO_PIN_11, GPIOE);
    AddKeyToList(key);
}

int16_t GetSample(uint8_t wave)
{
//    UART_WriteString("zbieram sample\n\r");
    int16_t sample = 0;
    KeyNode *temp = headKey;
    char buf[16];
    while(NULL != temp)
    {
        if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(temp->gpiox, temp->gpioPin))
        {
            if(temp->time < 2 * SAMPLE_RATE && wave == 0)
            {
                sample += (int16_t) (temp->sineSamples[temp->time % temp->samplesPerCycle] * SRRC_Filter[(temp->time / 200) & 480]);
                ++(temp->time);
            }
            else if(temp->time < 2 * SAMPLE_RATE && wave == 1)
            {
                sample += (int16_t) (temp->squareSamples[temp->time % temp->samplesPerCycle] * SRRC_Filter[(temp->time / 200) & 480]);
                ++(temp->time);
            }
            else if(temp->time < 2 * SAMPLE_RATE && wave == 2)
            {
                sample += (int16_t) (temp->triangleSamples[temp->time % temp->samplesPerCycle] * SRRC_Filter[(temp->time / 200) & 480]);
                ++(temp->time);
            }
            else if(temp->time < 2 * SAMPLE_RATE && wave == 3)
            {
                sample += (int16_t) (temp->sawtoothSamples[temp->time % temp->samplesPerCycle] * SRRC_Filter[(temp->time / 200) & 480]);
                ++(temp->time);
            }
        }
        else
        {
            temp->time = 0;
        }
        temp = temp->nextKey;
    }
    return sample;
}


bool CS43L22_Init(void)
{
    KeyBoardInit();
    FillFilter(SRRC_Filter, sizeof(SRRC_Filter)/sizeof(SRRC_Filter[0]), 0.001, 190, 100);

//    char buf[16];

//    for(uint16_t index = 0; index < (sizeof(SRRC_Filter)/sizeof(double )); ++index)
//    {
//        sprintf
//    }

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
//    data = CLK_AUTO_ON;
	I2C_SendData(CS43L22_ADDR, CLK_CONTROL, &data);
	data = INT_MODE_SLAVE | INT_POL_NORM | INT_DSP_DISABLE | INT_FORMAT_I2S | INT_WORD_24BIT;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_1, &data);
	data = INT_MCLK_EQ_SCLK | INT_SW_NORM;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_2, &data);
//	data = BEEP_260 | BEEP_ON_1_50;
//	I2C_SendData(CS43L22_ADDR, BEEP_FRQ_REG, &data);
//	data = BEEP_CONT | BEEP_MIX_ON;
//	I2C_SendData(CS43L22_ADDR, BEEP_TONE_REG, &data);
//	data = 0x06;
//	I2C_SendData(CS43L22_ADDR, BEEP_VOL_REG, &data);
//	data = 0x18;
//	I2C_SendData(CS43L22_ADDR, VOL_MSTA_REG, &data);
//	I2C_SendData(CS43L22_ADDR, VOL_MSTB_REG, &data);
	//Required initialization by manual
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

