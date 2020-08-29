//
// Created by Albert on 27.04.2020.
//

#include "i2c.h"

#define I2C_READ (0x01)
#define I2C_WRITE (0x00)
#define I2C_REG_INC (0x80)

static char TX_Buffer[128];
static RingBuffer I2C_Tx;

static I2C_HandleTypeDef i2cHandle;

bool I2C_ReadRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint16_t timeout)
{

	if( !RingBuffer_PutChar(&I2C_Tx, (char) (slaveAddress & I2C_READ)))
	{
		UART_WriteString("I2C_Tx buffer error\n\r");
		return false;
	}

	__HAL_I2C_ENABLE_IT(&i2cHandle, I2C_IT_BUF);

	if( !RingBuffer_PutChar(&I2C_Tx, (char) regAddress))
	{
		UART_WriteString("I2C_Tx buffer error\n\r");
		return false;
	}

	__HAL_I2C_ENABLE_IT(&i2cHandle, I2C_IT_BUF);
	uint16_t msCounts = 1;
	while(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_RXNE) && (msCounts <= timeout))
	{
		Delay(1);
		msCounts++;
	}

	if(msCounts >= timeout)
	{
		UART_WriteString("I2C read operation error\n\r");
		return false;
	}

	*data = (uint8_t) I2C1->DR;
	return true;
}

bool I2C_ReadData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, uint16_t timeout)
{
	char buf[16];

//	UART_WriteString("czytam\n\r");

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
//	sprintf(buf,"I2C1->DR: 0x%x\n\r", I2C1->DR);
//	UART_WriteString(buf);

	I2C1->CR1 |= I2C_CR1_START;

	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_SB))
	{
		;
	}
	//sending slave address with write bit
	I2C1->DR = (slaveAddress | I2C_READ);

//	sprintf(buf,"I2C1->DR: 0x%x\n\r", I2C1->DR);
//	UART_WriteString(buf);
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

//	sprintf(buf,"I2C1->DR: 0x%x\n\r", *data);
//	UART_WriteString(buf);

	__HAL_I2C_DISABLE(&i2cHandle);

	return true;
}


size_t I2C_WriteRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data, size_t bytes)
{
	UART_WriteString("wysylam\n\r");
	size_t bytesSended = 0;
	if( !RingBuffer_PutChar(&I2C_Tx, (char) (slaveAddress | I2C_WRITE)) )
	{
		UART_WriteString("I2C_Tx buffer error\n\r");
		UART_WriteString("war 1\n\r");
		return bytesSended;
	}

	__HAL_I2C_ENABLE_IT(&i2cHandle, I2C_IT_EVT);
	++bytesSended;

	if( !RingBuffer_PutChar(&I2C_Tx, (char) (regAddress)))
	{
		UART_WriteString("I2C_Tx buffer error\n\r");
		UART_WriteString("war 2\n\r");
		return bytesSended;
	}

	__HAL_I2C_ENABLE_IT(&i2cHandle, I2C_IT_EVT);
	++bytesSended;

	for(size_t i = 0; i < bytes; ++i){
		if( !RingBuffer_PutChar(&I2C_Tx, (char) *data))
		{
			UART_WriteString("I2C_Tx buffer error\n\r");
			UART_WriteString("war 3\n\r");
			return bytesSended;
		}
		__HAL_I2C_ENABLE_IT(&i2cHandle, I2C_IT_EVT);
		++bytesSended;
	}
	char buf[5];
	sprintf(buf, "%d", bytesSended);
	UART_WriteString("Bytes sended: ");
	UART_WriteString(buf);
	UART_WriteString(" ");
	sprintf(buf, "%d", RingBuffer_GetLen(&I2C_Tx));
	UART_WriteString("I2C_Tx length: ");
	UART_WriteString(buf);
	UART_WriteString("\n\r");
	UART_WriteString("fin\n\r");
	return bytesSended;
}

size_t I2C_SendData(uint8_t slaveAddress, uint8_t regAddress, uint8_t *data)
{
//	UART_WriteString("wysylam\n\r");
	size_t bytesSended = 0;

	__HAL_I2C_ENABLE(&i2cHandle);
	//checking if I2C is busy
	while(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_BUSY))
	{
//		UART_WriteString("Busy\n\r");
		;
	}
	//generatign start condition
	I2C1->CR1 |= I2C_CR1_START;
	//celareing SB bit
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_SB))
	{
//		UART_WriteString("MSL still wating\n\r");
		;
	}
	//sending slave address with write bit
	I2C1->DR = (slaveAddress | I2C_WRITE);
	//waiting for completing sending salve addr
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_ADDR))
	{
//		UART_WriteString("MSL still wating\n\r");
		;
	}
	++bytesSended;

	I2C1->SR2;
	//sengind map byte
	I2C1->DR = (regAddress);
	//waiting for empty DR register
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_TXE))
	{
//		UART_WriteString("MSL still wating\n\r");
		;
	}
	++bytesSended;
	//sending data to be writen into register
	I2C1->DR = (*data) & 0x00ff;
	//setting stop condition
	while(!__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_TXE))
	{
//		UART_WriteString("TX not empty\n\r");
		;
	}
	I2C1->CR1 |= I2C_CR1_STOP;
	while(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_MSL))
	{
//		UART_WriteString("MSL still wating\n\r");
		;
	}

	__HAL_I2C_DISABLE(&i2cHandle);

	++bytesSended;
	return bytesSended;
}


void I2C1_EV_IRQHandler(void)
{
	UART_WriteString("jestem w przerwaniu i2c ev\n\r");
	if(__HAL_I2C_GET_FLAG(&i2cHandle, I2C_FLAG_TXE))
	{
		if(__HAL_I2C_GET_IT_SOURCE(&i2cHandle, I2C_IT_EVT))
		{
			char c;
			if (!RingBuffer_GetChar(&I2C_Tx, &c))
			{
				__HAL_I2C_DISABLE_IT(&i2cHandle, I2C_IT_EVT);
			}
			else
			{
				I2C1->DR = c;
			}
		}
	}
}

void I2C1_ER_IRQHandler(void)
{
	UART_WriteString("jestem w przerwaniu i2c er\n\r");

}


bool I2C1_Init(void)
{

	RingBuffer_Init(&I2C_Tx, TX_Buffer, sizeof(TX_Buffer));

	__GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_9;
	gpio.Pull = GPIO_NOPULL;
	gpio.Alternate = GPIO_AF4_I2C1;
	gpio.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOB, &gpio);

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
		UART_WriteString("I2C init error\n\r");
		return false;
	}

	HAL_NVIC_SetPriority(I2C1_EV_IRQn,1,2);
	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

	return true;
}
