#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_adc_ex.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_i2s.h"
#include "i2c.h"
#include "uart.h"
#include "delay.h"
#include "waves.h"
#include "CS43L22.h"
#include "spi.h"
#include "lcd.h"
#include "stm32f4xx_hal_tim.h"
//#include "arm_math.h"

/*
 * TODO:
 * (1) osluga ekranu i domysla bitmapa
 * (2) przycisk do zmianny fali
 * (3) komentarze i ogranizacja projektu
 * */

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

I2S_HandleTypeDef i2sHandle;

uint8_t Wave;

void LED_Init()
{
	__GPIOD_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOD,  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static TIM_HandleTypeDef TIM2_InitStruct;

bool TIM2_Init(uint16_t period)
{
    __TIM2_CLK_ENABLE();

//    TIM2->CR1 |= TIM_CR1_DIR;
//    TIM2->PSC = 1000;
//    TIM2->ARR = 420000;
//    TIM2->DIER |= TIM_DIER_UIE;
//    TIM2->EGR |= TIM_EGR_UG;

    TIM2_InitStruct.Instance = TIM2;
    TIM2_InitStruct.Init.Prescaler = 42000;
    TIM2_InitStruct.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM2_InitStruct.Init.Period = 2 * period;
    TIM2_InitStruct.Init.ClockDivision = 0;

    if(HAL_OK != HAL_TIM_Base_Init(&TIM2_InitStruct))
    {
        return false;
    }


//    __HAL_TIM_ENABLE_IT(&TIM2_InitStruct, TIM2_IRQn);
    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
//    HAL_NVIC_EnableIRQ(TIM2_IRQn);

//    HAL_TIM_Base_Start_IT(&TIM2_InitStruct);

    return true;
}


void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM2_InitStruct);
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
////    LCD_SendFrame();
//}

int main(void)
{

	SPI_HandleTypeDef spi;
	spi.Instance = SPI3;
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();

	LED_Init();

	UARTInit();

    TIM2_Init(1000);

    if(false == CS43L22_Init())
	{
		UART_WriteString("CS43L22 init error\n\r");
	}

	int16_t sample = 0;
	char buffer[32];
    uint8_t data;

	if(I2C_ReadData(CS43L22_ADDR, PWR_CONTROL1, &data, 500))
    {
	    sprintf(buffer,"PWR_CONTROL1: 0x%x\n\r", data);
	    UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, PWR_CONTROL2, &data, 500))
    {
        sprintf(buffer,"PWR_CONTROL2: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }


    if(I2C_ReadData(CS43L22_ADDR, CLK_CONTROL, &data, 500))
    {
        sprintf(buffer,"CLK_CONTROL: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, INT_CONTROL_1, &data, 500))
    {
        sprintf(buffer,"INT_CONTROL_1: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, INT_CONTROL_2, &data, 500))
    {
        sprintf(buffer,"INT_CONTROL_2: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, BEEP_FRQ_REG, &data, 500))
    {
        sprintf(buffer,"BEEP_FRQ_REG: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, BEEP_TONE_REG, &data, 500))
    {
        sprintf(buffer,"BEEP_TONE_REG: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, MISC_REG, &data, 500))
    {
        sprintf(buffer,"MISC_REG: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, STATUS_REG, &data, 500))
    {
        sprintf(buffer,"STATUS_REG: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(I2C_ReadData(CS43L22_ADDR, 0x31, &data, 500))
    {
        sprintf(buffer,"SPEAKER_SATTUS: 0x%x\n\r", data);
        UART_WriteString(buffer);
    }

    if(false == LCD_Init())
    {
        UART_WriteString("LCD init error\n\r");
    }

//    for(size_t index = 0; index < sizeof(bytemap); ++index)
//    {
//        sprintf(buffer, "%d, ", bytemap[index]);
//        UART_WriteString(buffer);
//    }


    i2sHandle.Instance = SPI3;
    i2sHandle.Init.Mode = I2S_MODE_MASTER_TX;
    i2sHandle.Init.Standard = I2S_STANDARD_PHILIPS;
    i2sHandle.Init.DataFormat = I2S_DATAFORMAT_16B;
    i2sHandle.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    i2sHandle.Init.AudioFreq = I2S_AUDIOFREQ_48K;
    i2sHandle.Init.CPOL = I2S_CPOL_LOW;
    i2sHandle.Init.ClockSource = I2S_CLOCK_PLL;
    i2sHandle.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

    char Sbuf[16];

	while(1)
	{
//	    Delay(1);
        sample = GetSample();
//

        if(HAL_OK == HAL_I2S_Transmit(&i2sHandle, &sample, 2, 0))
		{
			UART_WriteString("left channel\n\r");
		}
        if(HAL_OK == HAL_I2S_Transmit(&i2sHandle, &sample, 3, 0))
        {
            UART_WriteString("right channel\n\r");
        }

        GetWave();

        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);

	}

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void
SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void
MX_GPIO_Init(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();

}

void
Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
