//
// Created by Albert on 20.05.2020.
//

#include "lcd.h"
#include "stm32f4xx_hal_spi.h"

SPI_HandleTypeDef spiHandle;

void LCD_SendCommand(uint8_t command)
{

    HAL_GPIO_WritePin(LCD_GPIO, CE_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_GPIO, COMD_PIN, COMMAND);

    SPI2_SendData(&command, sizeof(command), 500);

    HAL_GPIO_WritePin(LCD_GPIO, CE_PIN, GPIO_PIN_SET);

    HAL_GPIO_WritePin(LCD_GPIO, COMD_PIN, DATA);


}

void LCD_SendData(uint8_t data)
{

    HAL_GPIO_WritePin(LCD_GPIO, CE_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_GPIO, COMD_PIN, DATA);

    SPI2_SendData(&data, sizeof(data), 500);

    HAL_GPIO_WritePin(LCD_GPIO, CE_PIN, GPIO_PIN_SET);

}


bool LCD_Init(void)
{
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio;

    HAL_GPIO_WritePin(LCD_GPIO, CE_PIN | COMD_PIN | RST_PIN, GPIO_PIN_SET);

    gpio.Pin =  CE_PIN | COMD_PIN | RST_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOB, &gpio);

    if(false == SPI2_Init())
    {
        UART_WriteString("LCD init failed\n\r");
        return false;
    }

    HAL_GPIO_WritePin(GPIOB, RST_PIN, GPIO_PIN_RESET);
    Delay(30);
    HAL_GPIO_WritePin(GPIOB, RST_PIN, GPIO_PIN_SET);

    LCD_SendCommand(FUNC_SET | CHIP_ON | EXTENDED_INSTR);

    LCD_SendCommand(TEMP_CONTROL | TEMP_COE_2);

    LCD_SendCommand(BIAS_CONTROL | BIAS_4);

    LCD_SendCommand(VOP_CONTROL | 0x3f);

    LCD_SendCommand(FUNC_SET | BASIC_INSTR | CHIP_ON | HORIZONTAL_ADDR);

    LCD_SendCommand(DISPLAY_CONTROL | DISPLAY_INV );

    for(uint16_t i = 0; i < (84 * 42); ++i)
    {
        LCD_SendData(0x00);
    }

    return true;

}