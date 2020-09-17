//
// Created by Albert on 29.04.2020.
//

#include "CS43L22.h"

bool CS43L22_Init(void)
{
    // Initializing PD4 got resetting DAC
	__GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = GPIO_PIN_4;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOD, &gpio);

	// Quick reset of DAC
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	uint8_t data = 0x00;

	// My setting for DAC
	data = HEADPHONE_UP | SPEAKER_DOWN;
	I2C_SendData(CS43L22_ADDR, PWR_CONTROL2, &data);
	data = CLK_AUTO_OFF | CLK_SINGLE_SPEED | CLK_32_RATE_NO | CLK_VIDEO_NO | CLK_IN_RATIO_128 | CLK_NO_DIVIDE;
	I2C_SendData(CS43L22_ADDR, CLK_CONTROL, &data);
	data = INT_MODE_SLAVE | INT_POL_NORM | INT_DSP_DISABLE | INT_FORMAT_I2S | INT_WORD_16BIT;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_1, &data);
	data = INT_MCLK_EQ_SCLK | INT_SW_NORM;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_2, &data);
    // Required initialization by DAC documentation
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

	return true;
}

