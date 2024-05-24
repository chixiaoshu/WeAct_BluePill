#include "dht11.h"

void Delay_us(uint8_t us)
{
    HAL_TIM_Base_Start(&htim3);
    __HAL_TIM_SetCounter(&htim3, us);
    while (__HAL_TIM_GetCounter(&htim3) > 0);
    HAL_TIM_Base_Stop(&htim3);
}

void GPIO_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO pin : PA8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void GPIO_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO pin : PA8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DHT11_Rst(void)   //主机开始信号
{
    GPIO_Output();
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    Delay_us(30);
}

uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    GPIO_Input();
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //等待回应拉位低电平
    {
        retry++;
        Delay_us(1);
    }

    if (retry >= 100)return 1;else retry = 0;				//当变量值大于100 返回1 说明无响应  返回 0 则为正确响应

    while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //等待变为高电平
    {
        retry++;
        Delay_us(1);
    }
    if (retry >= 100)return 1;
    return 0;
}

uint8_t DHT11_Init(void)
{
    DHT11_Rst();
    return DHT11_Check();
}

uint8_t DHT11_ReadBit(void)   //读取一个位
{
    uint8_t retry = 0;
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //等待变为低电平
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //等待变为高电平
    {
        retry++;
        Delay_us(1);
    }
    Delay_us(40);  //40us 后如果为低电平 数据为0   高电平数据为1
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))return 1;else return 0;
}

uint8_t DHT11_ReadByte(void)    //读取一个字节  返回值位采集值
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;									//数据左移一位
        dat |= DHT11_ReadBit();			//每读取到一个位 放到dat的最后一位
    }
    return dat;
}

uint8_t DHT11_ReadData(uint8_t *h)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() == 0) {
        for (i = 0; i < 5; i++)
        {
            buf[i] = DHT11_ReadByte();
        }
        if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4])
        {
            *h = buf[0];
            h++;
            *h = buf[2];
        }
    } else return 1;
    return 0;
}
