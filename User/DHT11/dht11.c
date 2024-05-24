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

void DHT11_Rst(void)   //������ʼ�ź�
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
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //�ȴ���Ӧ��λ�͵�ƽ
    {
        retry++;
        Delay_us(1);
    }

    if (retry >= 100)return 1;else retry = 0;				//������ֵ����100 ����1 ˵������Ӧ  ���� 0 ��Ϊ��ȷ��Ӧ

    while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //�ȴ���Ϊ�ߵ�ƽ
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

uint8_t DHT11_ReadBit(void)   //��ȡһ��λ
{
    uint8_t retry = 0;
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //�ȴ���Ϊ�͵�ƽ
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) && retry < 100)      //�ȴ���Ϊ�ߵ�ƽ
    {
        retry++;
        Delay_us(1);
    }
    Delay_us(40);  //40us �����Ϊ�͵�ƽ ����Ϊ0   �ߵ�ƽ����Ϊ1
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))return 1;else return 0;
}

uint8_t DHT11_ReadByte(void)    //��ȡһ���ֽ�  ����ֵλ�ɼ�ֵ
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;									//��������һλ
        dat |= DHT11_ReadBit();			//ÿ��ȡ��һ��λ �ŵ�dat�����һλ
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
