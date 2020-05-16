//
// Created by Albert on 08.05.2020.
//

#include "i2s.h"

I2S_HandleTypeDef i2sHandle;

static uint16_t time;
static  int16_t sample;

void SPI3_IRQHandler(void)
{

	UART_WriteString("i2s przerwanie\n\r");
	if(__HAL_I2S_GET_FLAG(&i2sHandle, I2S_FLAG_TXE))
	{
		sample = GenerateSquareSample(220.0, 1, 0.5, &time);
		SPI3->DR = 10000;
	}
}


bool I2S3_Init(void)
{
	__GPIOC_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_12;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	gpio.Alternate = GPIO_AF6_SPI3;
	gpio.Mode = GPIO_MODE_AF_PP;

	HAL_GPIO_Init(GPIOC, &gpio);

	gpio.Pin = GPIO_PIN_4;
	gpio.Alternate = GPIO_AF6_SPI3;

	HAL_GPIO_Init(GPIOA, &gpio);

	__SPI3_CLK_ENABLE();
	i2sHandle.Instance = SPI3;
	i2sHandle.Init.Mode = I2S_MODE_MASTER_TX;
	i2sHandle.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	i2sHandle.Init.DataFormat = I2S_DATAFORMAT_16B;
	i2sHandle.Init.Standard = I2S_STANDARD_PHILIPS;
	i2sHandle.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	i2sHandle.Init.ClockSource = I2S_CLOCK_PLL;
	i2sHandle.Init.CPOL = I2S_CPOL_LOW;

	if(HAL_OK != HAL_I2S_Init(&i2sHandle))
	{
		return false;
	}

	SPI3->I2SCFGR |= SPI_I2SCFGR_I2SE;

	__HAL_SPI_ENABLE_IT(&i2sHandle, SPI_IT_TXE);
	__HAL_I2S_ENABLE_IT(&i2sHandle, I2S_IT_TXE);
	HAL_NVIC_SetPriority(SPI3_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(SPI3_IRQn);

	SPI3->DR = 0x0000;

	return true;
}
