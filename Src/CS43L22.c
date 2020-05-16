//
// Created by Albert on 29.04.2020.
//

#include "CS43L22.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "uart.h"
#include "i2c.h"
#include "i2s.h"
#include "delay.h"

#define CS43L22_ADDR (0x94)
//Power setting
//*******************************
#define	PWR_CONTROL1 (0x02)
#define PWR_UP (0x9E)
#define PWR_DOWN (0x9F)
//register with power setting for headphones and speakers
#define PWR_CONTROL2 (0X04)
//headphones power settings
#define HEADPHONE_UP (0xA0)
#define HEADPHONE_DOWN (0xF0)
#define HEADPHONE_UP_PIN_6 (0x50)
//speaker power settings
/*
 * * NO CONNECTION ON BOARD
 * #define SPEAKER_UP (0x0A)
 * #define SPEAKER_DOWN (0x0F)
 * #define SPEAKER_UP_PIN_6 (0x00)
 */

//Clock Control for I2S
//**************************
#define CLK_CONTROL (0x05)
#define CLK_AUTO_ON (0x80)
#define CLK_AUTO_OFF (0x00)
//Clock speed
#define CLK_QUARTER_SPEED (0x60)	//4kHz - 12.5kHz Fs
#define CLK_HALF_SPEED (0x40)		//12.5kHz - 25kHz Fs
#define CLK_SINGLE_SPEED (0x20)		//4kHz - 50 kHz Fs
#define CLK_DOUBLE_SPEED (0x00)		//50kHz - 100kHz Fs
//Sample rate 32kHz, 16kHz or 8kHz
#define CLK_32_RATE_YES (0x10)
#define	CLK_32_RATE_NO (0x00)
//27MHz video clk
#define CLK_VIDEO_YES (0x08)
#define CLK_VIDEO_NO (0x00)
//Internal MCLK/LRCK Ratio
#define CLK_IN_RATIO_128 (0x00)
#define CLK_IN_RATIO_125 (0x02)
#define CLK_IN_RATIO_132 (0x04)
#define CLK_IN_RATIO_136 (0x06)
//MCLK divide
#define CLK_NO_DIVIDE (0x00)
#define CLK_DIVIDE2 (0x01)

//Interface Control for I2S
//**************************
#define INT_CONTROL_1 (0x06)
#define INT_CONTROL_2 (0x07)
//Mode
#define INT_MODE_MASTER (0x80)
#define INT_MODE_SLAVE (0x00)
//SCLK polarity
#define INT_POL_INVERTED (0x60)
#define INT_POL_NORM (0x00)
//DSP MODE
#define INT_DSP_ENABLE  (0x01)
#define INT_DSP_DISABLE (0x00)
//Interface Format
#define INT_FORMAT_LEFT (0x00)
#define INT_FORMAT_RIGHT (0x08)
#define INT_FORMAT_I2S (0x0C)
//Audio word length for I2S
#define	INT_WORD_24BIT (0x00)
#define	INT_WORD_20BIT (0x01)
#define	INT_WORD_18BIT (0x02)
#define	INT_WORD_16BIT (0x03)
//MCLK = SCLK
#define INT_MCLK_EQ_SCLK (0x00)
#define INT_MCLK_NEQ_SCLK (0x40)
#define INT_SW_INT (0x08)
#define INT_SW_NORM (0x00)

//Beep registers and options
//**************************
#define BEEP_FRQ_REG (0x1c)
#define BEEP_VOL_REG (0x1d)
#define BEEP_TONE_REG (0x1e)
//Beep frequency
#define BEEP_260	(0x00)
#define BEEP_521	(0x10)
#define BEEP_585	(0x20)
#define BEEP_666	(0x30)
#define BEEP_705	(0x40)
#define BEEP_774	(0x50)
#define BEEP_888	(0x60)
#define BEEP_1000	(0x70)
#define BEEP_1043	(0x80)
#define BEEP_1200	(0x90)
#define BEEP_1333	(0xa0)
#define BEEP_1411	(0xb0)
#define BEEP_1600	(0xc0)
#define BEEP_1724	(0xd0)
#define BEEP_2000	(0xe0)
#define BEEP_2182	(0xf0)
//Beep on time
#define BEEP_ON_0_08 (0x00)
#define BEEP_ON_0_43 (0x01)
#define BEEP_ON_0_78 (0x02)
#define BEEP_ON_1_20 (0x03)
#define BEEP_ON_1_50 (0x04)
#define BEEP_ON_1_80 (0x05)
#define BEEP_ON_2_20 (0x06)
#define BEEP_ON_2_50 (0x07)
#define BEEP_ON_2_80 (0x08)
#define BEEP_ON_3_20 (0x09)
#define BEEP_ON_3_50 (0x0a)
#define BEEP_ON_3_80 (0x0b)
#define BEEP_ON_4_20 (0x0c)
#define BEEP_ON_4_50 (0x0d)
#define BEEP_ON_4_80 (0x0e)
#define BEEP_ON_5_20 (0x0f)
//Beep off time
#define BEEP_OFF_1_23 (0x00)
#define BEEP_OFF_2_58 (0x20)
#define BEEP_OFF_3_90 (0x40)
#define BEEP_OFF_5_20 (0x60)
#define BEEP_OFF_6_60 (0x80)
#define BEEP_OFF_8_05 (0x90)
#define BEEP_OFF_9_35 (0xc0)
#define BEEP_OFF_10_80 (0xe0)


//Volume register
//**************************
//Master volume
#define VOL_MSTA_REG (0x20)
#define VOL_MSTB_REG (0x21)
//Headphone volume
#define VOL_HPA_REG (0x22)
#define VOL_HPB_REG (0x23)


bool CS43L22_Init(void)
{
	__GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = GPIO_PIN_4;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOD, &gpio);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
	Delay(1);

	if(false == I2C1_Init())
	{
		UART_WriteString("I2C1 init error\n\r");
		return false;
	}

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	uint8_t data = 0x00;

	I2C_HandleTypeDef hi2c;
	hi2c.Instance = I2C1;

	char buf[16];

	data = HEADPHONE_UP;
	I2C_SendData(CS43L22_ADDR, PWR_CONTROL2, &data);

	data = 0x00;
	if(I2C_ReadData(CS43L22_ADDR, PWR_CONTROL2, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "PWR_CONTROL2: 0x%x\n\r", data);
		UART_WriteString(buf);
	}

	data = CLK_AUTO_OFF | CLK_SINGLE_SPEED | CLK_32_RATE_NO | CLK_VIDEO_NO | CLK_IN_RATIO_128 | CLK_NO_DIVIDE;
	I2C_SendData(CS43L22_ADDR, CLK_CONTROL, &data);
	if(I2C_ReadData(CS43L22_ADDR, CLK_CONTROL, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "CLK_CONTROL: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, CLK_CONTROL, 1, &data, 1, 500);
	data = INT_MODE_SLAVE | INT_POL_NORM | INT_DSP_DISABLE | INT_FORMAT_I2S | INT_WORD_16BIT;
	I2C_SendData(CS43L22_ADDR, INT_CONTROL_1, &data);
	if(I2C_ReadData(CS43L22_ADDR, INT_CONTROL_1, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "PWR_CONTROL2: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, INT_CONTROL_1, 1, &data, 1, 500);
	data = BEEP_260 | BEEP_ON_1_50;
	I2C_SendData(CS43L22_ADDR, BEEP_FRQ_REG, &data);
	if(I2C_ReadData(CS43L22_ADDR, BEEP_FRQ_REG, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "BEEP_FRQ_REG: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, BEEP_FRQ_REG, 1, &data, 1, 500);
	data = 0xc0;
	I2C_SendData(CS43L22_ADDR, BEEP_TONE_REG, &data);
	if(I2C_ReadData(CS43L22_ADDR, BEEP_TONE_REG, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "BEEP_TONE_REG: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, BEEP_TONE_REG, 1, &data, 1, 500);

	//Required initialization by manual
	data = 0x99;
	I2C_SendData(CS43L22_ADDR, 0x00, &data);
	if(I2C_ReadData(CS43L22_ADDR, 0x00, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "Reg 0x00: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, 0x00, 1, &data, 1, 500);
	data = 0x80;
	I2C_SendData(CS43L22_ADDR, 0x47, &data);
	if(I2C_ReadData(CS43L22_ADDR, 0x47, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "Reg 0x47: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, 0x47, 1, &data, 1, 500);
	data = 0x80;
	I2C_SendData(CS43L22_ADDR, 0x32, &data);
	if(I2C_ReadData(CS43L22_ADDR, 0x32, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "Reg 0x32: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, 0x32, 1, &data, 1, 500);
	data = 0x00;
	I2C_SendData(CS43L22_ADDR, 0x32, &data);
	if(I2C_ReadData(CS43L22_ADDR, 0x32, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "Reg 0x32: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, 0x32, 1, &data, 1, 500);
	data = 0x00;
	I2C_SendData(CS43L22_ADDR, 0x00, &data);
	if(I2C_ReadData(CS43L22_ADDR, 0x00, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "Reg 0x00: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, 0x00, 1, &data, 1, 500);

	data = 0x9e;
	I2C_SendData(CS43L22_ADDR, PWR_CONTROL1, &data);
	if(I2C_ReadData(CS43L22_ADDR, PWR_CONTROL1, &data, 1000))
	{
		sprintf(buf, "data writen: 0x%x\n\r", data);
		UART_WriteString(buf);
		sprintf(buf, "PWR_CONTROL1: 0x%x\n\r", data);
		UART_WriteString(buf);
	}
//	HAL_I2C_Mem_Write(&hi2c, CS43L22_ADDR, PWR_CONTROL1, 1, &data, 1, 500);

	data = 0x00;
//
//	if(I2C_ReadData(CS43L22_ADDR, PWR_CONTROL2, &data, 1000))
//	{
//		char buf[16];
//		sprintf(buf, "PWR_CONTROL2: 0x%x\n\r", data);
//		UART_WriteString(buf);
//	}

	if(false == I2S3_Init())
	{
		UART_WriteString("I2S3 init error\n\r");
		return false;
	}

	return true;
}

