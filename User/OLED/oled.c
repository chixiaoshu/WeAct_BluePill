#include "oled.h" 
#include "oledfont.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

uint8_t OLED_FrameBuffer[128][8];
uint8_t OLED_Buffer[8][128];
uint8_t OLED_CommandBuffer[8][4] = {0};
uint8_t I2C_TransmitCompleteFlag = 1;
uint8_t LineCounter = 0; 
uint8_t BufferReadyFlag = 0; 

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == OLED_I2C_PORT.Instance)
	{
		if(BufferReadyFlag)
    {
      HAL_I2C_Mem_Write_DMA(&OLED_I2C_PORT, OLED_I2C_ADDRESS, 0x40, I2C_MEMADD_SIZE_8BIT, OLED_Buffer[LineCounter], 128);
    }
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == OLED_I2C_PORT.Instance)
	{
    if(LineCounter == 7)
    {
      BufferReadyFlag = 0;
      LineCounter = 0;
    }
    if(BufferReadyFlag)
    {
			LineCounter++;
			HAL_I2C_Master_Transmit_DMA(&OLED_I2C_PORT, OLED_I2C_ADDRESS, OLED_CommandBuffer[LineCounter], 4);
    }
	}
}

uint32_t OLED_Power(uint8_t base, uint8_t exponent)
{
    uint32_t result = 1;
    while (exponent--)
    {
        result *= base;
    }
    return result;
}

uint8_t OLED_CheckNumLength(uint32_t num)
{
    uint32_t temp;
    uint8_t length;
    for(length = 1; length < 10; length++)
    {
        temp = OLED_Power(10, length);
        if(num < temp)
        {
            return length;
        }
    }
    return 0;
}

void OLED_ShowNumber(uint8_t x, uint8_t y, uint32_t num, uint8_t mode, uint8_t length)
{
    uint8_t i, digit;
    uint8_t isNonZero = 0;

    for (i = 0; i < length; i++)
    {
        digit = (num / OLED_Power(10, length - i - 1)) % 10;

        if (isNonZero == 0 && i < (length - 1))
        {
            if (digit == 0)
            {
                if (mode == 0)
                {
                    OLED_ShowChar(x, y + i, ' ');
                }
                else
                {
                    OLED_ShowChar(x, y + i, '0');
                }
                continue;
            }
            else
            {
                isNonZero = 1;
            }
        }

        OLED_ShowChar(x, y + i, digit + '0');
    }
}

void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t mode)
{
    int32_t integerPart, fractionalPart;
    char buffer[10]; // 足够存放负号、小数点和数字

    integerPart = (int32_t)num;
    fractionalPart = (int32_t)((num - integerPart) * 100); // 两位小数

    if (fractionalPart < 0)
        fractionalPart = -fractionalPart; // 处理负数情况

    if (fractionalPart >= 100) {
        fractionalPart -= 100;
        integerPart += 1;
    }

    // 将数字转换成字符串
    snprintf(buffer, sizeof(buffer), "%d.%02d", integerPart, fractionalPart);

    // 显示字符串
    OLED_ShowString(x, y, (uint8_t *)buffer);
}

uint8_t OLED_InitCommands[] =
{
    0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8,
    0x3F, 0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1,
    0xDA, 0x12, 0xDB, 0x30, 0x8D, 0x14, 0xAF, 0x20, 0x00
};

void OLED_Init(void)
{
    HAL_I2C_Mem_Write_DMA(&OLED_I2C_PORT, OLED_I2C_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, OLED_InitCommands, sizeof(OLED_InitCommands));
}

void OLED_OperateFrameBuffer(pen_typedef pen)
{
    if (pen == PEN_WRITE)
    {
        memset(OLED_FrameBuffer, 0xFF, sizeof(OLED_FrameBuffer));
    }
    else if(pen == PEN_CLEAR)
    {
        memset(OLED_FrameBuffer, 0x00, sizeof(OLED_FrameBuffer));
    }
}

void OLED_SetCursor(uint8_t x, uint8_t y)
{
    OLED_CommandBuffer[y][0] = 0x00;
    OLED_CommandBuffer[y][1] = 0xB0 + y;
    OLED_CommandBuffer[y][2] = 0x10;
    OLED_CommandBuffer[y][3] = 0x02;
}

void OLED_DrawPixel(uint8_t x, uint8_t y, pen_typedef pen)
{
    uint8_t page = 0, row = 0;

    if ((x < 0) || (x >= X_WIDTH) || (y < 0) || (y >= Y_WIDTH))
    {
        return;
    }

    page = y / 8;
    row = y % 8;

    if (pen == PEN_WRITE)
    {
        OLED_FrameBuffer[x][page] |= 1 << row;
    }
    else if (pen == PEN_INVERSION)
    {
        OLED_FrameBuffer[x][page] ^= 1 << row;
    }
    else
    {
        OLED_FrameBuffer[x][page] &= ~(1 << row);
    }
}

void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, pen_typedef pen)
{
    uint8_t col = 0, row = 0;
    uint8_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;
    float slope = 0.0f, intercept = 0.0f;

    if (y1 == y2)
    {
        (x1 <= x2) ? (x_start = x1) : (x_start = x2);
        (x1 <= x2) ? (x_end = x2) : (x_end = x1);

        for (col = x_start; col <= x_end; col++)
        {
            OLED_DrawPixel(col, y1, pen);
        }
    }
    else if (x1 == x2)
    {
        (y1 <= y2) ? (y_start = y1) : (y_start = y2);
        (y1 <= y2) ? (y_end = y2) : (y_end = y1);

        for (row = y_start; row <= y_end; row++)
        {
            OLED_DrawPixel(x1, row, pen);
        }
    }
    else
    {
        slope = ((float)(y2 - y1)) / (x2 - x1);
        intercept = (float)y1 - slope * x1;

        (x1 <= x2) ? (x_start = x1) : (x_start = x2);
        (x1 <= x2) ? (x_end = x2) : (x_end = x2);

        for (col = x_start; col <= x_end; col++)
        {
            OLED_DrawPixel(col, (uint8_t)(col * slope + intercept), pen);
        }
    }
}

void OLED_ShowChar(uint8_t row, uint8_t col, uint8_t chr)
{
    uint8_t x = col * CHAR_SIZE_WIDTH;
    uint8_t y = row * CHAR_SIZE_HIGHT;
    uint8_t temp, i, j;
    uint8_t y_origin = y;
    chr = chr - ' ';

    for (i = 0; i < CHAR_SIZE_HIGHT; i++)
    {
        temp = asc2_1206[chr][i];

        for (j = 0; j < 8; j++)
        {
            if (temp & 0x80)
            {
                OLED_DrawPixel(x, y, PEN_WRITE);
            }
            else
            {
                OLED_DrawPixel(x, y, PEN_CLEAR);
            }

            temp <<= 1;
            y++;
            if ((y - y_origin) == CHAR_SIZE_HIGHT)
            {
                y = y_origin;
                x++;
                break;
            }
        }
    }
}

void OLED_ShowString(uint8_t row, uint8_t col, uint8_t *str)
{
    uint8_t n = 0;

    while (str[n] != '\0')
    {
        OLED_ShowChar(row, col, str[n]);
        col++;

        if (col > 20)
        {
            col = 0;
            row++;
        }

        n++;
    }
}

void OLED_Refresh(void)
{
    uint8_t i, j;

    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(0, j);

        for (i = 0; i < 128; i++)
        {
            OLED_Buffer[j][i] = OLED_FrameBuffer[i][j];
        }
    }

    BufferReadyFlag = 1;
    HAL_I2C_Master_Transmit_DMA(&OLED_I2C_PORT, OLED_I2C_ADDRESS, OLED_CommandBuffer[LineCounter], 4);
}
