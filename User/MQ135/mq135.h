#ifndef __MQ135_H__
#define __MQ135_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "adc.h"
#include "main.h"

#define MAX_CONVERTED_VALUE   4095.0f    /* Max converted value */
#define VREF                  3300.0f

void MQ135_Init(void);
uint32_t MQ135_GetValue(void);
float MQ135_GetQuality(void);


#ifdef __cplusplus
}
#endif

#endif /* __MQ135_H__ */