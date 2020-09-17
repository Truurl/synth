//
// Created by Albert on 27.04.2020.
//

#include "i2c.h"

static I2C_HandleTypeDef i2cHandle;

bool I2C_ReadData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint16_t timeout)
{
	__HAL_I2C_ENABLE(&i2cHandle);
	//checking if I2C is busy
	while(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_BUSY))
	{
		;
	}
	//generatign start condition
	I2C1->CR1 |= I2C_CR1_START;
	//celareing SB bit
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_SB))
	{
		;
	}
	//sending slave address with write bit
	I2C1->DR = (slaveAddress | I2C_WRITE);
	//waiting for completing sending salve addr
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_ADDR))
	{
		;
	}

	I2C1->SR2;
	//sengind map byte
	I2C1->DR = (regAddress);
	I2C1->CR1 |= I2C_CR1_STOP;

	I2C1->CR1 |= I2C_CR1_START;

	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_SB))
	{
		;
	}
	//sending slave address with write bit
	I2C1->DR = (slaveAddress | I2C_READ);

	//waiting for completing sending salve addr
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_ADDR))
	{
		;
	}
	I2C1->CR1 &= !I2C_CR1_ACK;
	I2C1->SR2;

	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_RXNE))
	{

	}
	uint16_t temp = I2C1->DR & 0x00ff;
	*data = temp;

	I2C1->CR1 |= I2C_CR1_STOP;

	__HAL_I2C_DISABLE(&i2cHandle);

	return true;
}


size_t I2C_SendData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data)
{
	size_t bytesSended = 0;

	__HAL_I2C_ENABLE(&i2cHandle);
	//checking if I2C is busy
	while(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_BUSY))
	{
		;
	}
	//generatign start condition
	I2C1->CR1 |= I2C_CR1_START;
	//celareing SB bit
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_SB))
	{
		;
	}
	//sending slave address with write bit
	I2C1->DR = (slaveAddress | I2C_WRITE);
	//waiting for completing sending salve addr
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_ADDR))
	{
		;
	}
	++bytesSended;

	I2C1->SR2;
	//sengind map byte
	I2C1->DR = (regAddress);
	//waiting for empty DR register
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_TXE))
	{
		;
	}
	++bytesSended;
	//sending data to be writen into register
	I2C1->DR = (*data) & 0x00ff;
	//setting stop condition
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_TXE))
	{
		;
	}
	I2C1->CR1 |= I2C_CR1_STOP;
	while(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_MSL))
	{
		;
	}

	__HAL_I2C_DISABLE(&i2cHandle);

	++bytesSended;
	return bytesSended;
}


bool I2C1_Init(void)
{

	__GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_9;
	gpio.Pull = GPIO_NOPULL;
	gpio.Alternate = GPIO_AF4_I2C1;
	gpio.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOB, &gpio);

	// Filling struct for initialization
	__I2C1_CLK_ENABLE();
	i2cHandle.Instance = I2C1;
	i2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2cHandle.Init.ClockSpeed = 100000;
	i2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	i2cHandle.Init.OwnAddress1 = 0xff;
	i2cHandle.Init.OwnAddress2 = 0xff;
	i2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;

	if( HAL_OK != HAL_I2C_Init(&i2cHandle))
	{
		return false;
	}

	return true;
}
