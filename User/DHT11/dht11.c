#include "dht11.h"

void Delay_us(uint8_t us)
{
    uint16_t differ = 0xffff - us - 5;
    __HAL_TIM_SET_COUNTER(&DHT11_TIMER, differ); // 设定TIM计数器起始值
    HAL_TIM_Base_Start(&DHT11_TIMER);            // 启动定时器

    while (differ < 0xffff - 5)
    {                                                 // 判断
        differ = __HAL_TIM_GET_COUNTER(&DHT11_TIMER); // 查询计数器的计数值
    }
    HAL_TIM_Base_Stop(&DHT11_TIMER);
}

void GPIO_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /*Configure GPIO pin : PA8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void GPIO_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /*Configure GPIO pin : PA8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DHT11_Rst(void) // 主机开始信号
{
    GPIO_Output();
    DHT11_OUT_LOW;
    HAL_Delay(20);
    DHT11_OUT_HIGH;
    Delay_us(30);
}

uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    GPIO_Input();
    while (DHT11_IN_Read && retry < 100) // 等待回应拉位低电平
    {
        retry++;
        Delay_us(1);
    }

    if (retry >= 100)
        return 1;
    else
        retry = 0; // 当变量值大于100 返回1 说明无响应  返回 0 则为正确响应

    while (!DHT11_IN_Read && retry < 100) // 等待变为高电平
    {
        retry++;
        Delay_us(1);
    }
    if (retry >= 100)
        return 1;
    return 0;
}

uint8_t DHT11_Init(void)
{
    DHT11_Rst();
    return DHT11_Check();
}

uint8_t DHT11_ReadBit(void) // 读取一个位
{
    uint8_t retry = 0;
    while (DHT11_IN_Read && retry < 100) // 等待变为低电平
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    while (!DHT11_IN_Read && retry < 100) // 等待变为高电平
    {
        retry++;
        Delay_us(1);
    }
    Delay_us(40); // 40us 后如果为低电平 数据为0   高电平数据为1
    if (DHT11_IN_Read)
        return 1;
    else
        return 0;
}

uint8_t DHT11_ReadByte(void) // 读取一个字节  返回值位采集值
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;              // 数据左移一位
        dat |= DHT11_ReadBit(); // 每读取到一个位 放到dat的最后一位
    }
    return dat;
}

uint8_t DHT11_ReadData(uint16_t *temp, uint16_t *hum)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++)
        {
            buf[i] = DHT11_ReadByte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *hum = (buf[0] << 8) + buf[1];
            *temp = (buf[2] << 8) + buf[3];
        }
    }
    else
        return 1;
    return 0;
}
