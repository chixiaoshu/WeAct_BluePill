#ifndef __OLED_H
#define __OLED_H

#include "main.h"

#define OLED_I2C_PORT hi2c1
extern I2C_HandleTypeDef OLED_I2C_PORT;

#define OLED_I2C_ADDRESS 0x78

#define X_WIDTH 128
#define Y_WIDTH 64

#define CHAR_SIZE_WIDTH 6
#define CHAR_SIZE_HIGHT 12

typedef enum {
    PEN_CLEAR = 0,
    PEN_WRITE = 1,
    PEN_INVERSION = 2
} pen_typedef;

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);

uint32_t OLED_Power(uint8_t base, uint8_t exponent);
uint8_t OLED_CheckNumLength(uint32_t num);
void OLED_ShowNumber(uint8_t x, uint8_t y, uint32_t num, uint8_t mode, uint8_t length);
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t mode);

void OLED_Init(void);
void OLED_OperateFrameBuffer(pen_typedef pen);
void OLED_SetCursor(uint8_t x, uint8_t y);
void OLED_DrawPixel(uint8_t x, uint8_t y, pen_typedef pen);
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, pen_typedef pen);
void OLED_ShowChar(uint8_t row, uint8_t col, uint8_t chr);
void OLED_ShowString(uint8_t row, uint8_t col, uint8_t *str);
void OLED_Refresh(void);

#endif
