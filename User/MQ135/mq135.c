#include "mq135.h"

extern uint32_t ADC_originValue[];
// Initialize the ADC_DMA to convert the internal Temperature and ADC_IN_1 sensor channels
void MQ135_Init(void)
{
  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_originValue, 2) != HAL_OK)
  {
    Error_Handler();
  }
}

// Calculate the average ADC_IN_1 Origin Value
uint32_t MQ135_GetValue(void)
{
    uint32_t buffer = 0;
    for (uint8_t t = 0; t < 5; t++)
    {
        buffer += ADC_originValue[1];
        HAL_Delay(5);
    }
    return buffer / 5;
}

// Use the measured value / 1000 to get the x.xxx quality
float MQ135_GetQuality(void)
{
    float value = MQ135_GetValue();
    float quality = value * VREF / MAX_CONVERTED_VALUE;
    return quality;
}
