//
// Created by Albert on 08.05.2020.
//

#include "i2s.h"

extern I2S_HandleTypeDef i2sHandle;

bool I2S3_Init(void)
{

    __HAL_RCC_SPI3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Switching GPIOD into SPI3 function
	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_12;
    gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOC, &gpio);


	gpio.Pin = GPIO_PIN_4;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	gpio.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOA, &gpio);

	// Filling struct for
	i2sHandle.Instance = SPI3;
	i2sHandle.Init.Mode = I2S_MODE_MASTER_TX;
    i2sHandle.Init.Standard = I2S_STANDARD_PHILIPS;
    i2sHandle.Init.DataFormat = I2S_DATAFORMAT_32B;
    i2sHandle.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    // 8K gives enought time for calculating samples even with few keys pressed
    i2sHandle.Init.AudioFreq = I2S_AUDIOFREQ_8K;
    i2sHandle.Init.CPOL = I2S_CPOL_LOW;
    i2sHandle.Init.ClockSource = I2S_CLOCK_PLL;
    i2sHandle.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

	if(HAL_OK != HAL_I2S_Init(&i2sHandle))
	{
		return false;
	}

	__HAL_I2S_ENABLE(&i2sHandle);
    __HAL_SPI_ENABLE(&i2sHandle);

	return true;
}
