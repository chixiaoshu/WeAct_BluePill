#ifndef __DHT11_H__
#define __DHT11_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

#define DHT11_TIMER         htim3
extern TIM_HandleTypeDef    DHT11_TIMER;  	//引入变量

#define DHT11_OUT_HIGH  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)
#define DHT11_OUT_LOW   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)
#define DHT11_IN_Read   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)

//函数原型
//void Delay_us(uint8_t us);   //微妙延时函数，启用了一个定时器。因为DHT11通讯过程涉及微妙延时
void GPIO_Input(void);	  //GPIO 状态转变的函数 CUBEMX默认的GPIO初始化我只开启了相关总线的使能
//把GPIO状态（输入 输出）封装成了两个函数
void GPIO_Output(void);
void DHT11_Rst(void);		//主机开始采集的信号
uint8_t DHT11_Check(void);  //检查DHT是否回应
uint8_t DHT11_Init(void);   //初始化函数
uint8_t DHT11_ReadBit(void);   //读取一位
uint8_t DHT11_ReadByte(void);  //读取一个字节
uint8_t DHT11_ReadData(float *temp, float *hum);   //读取数据（40个位） 

#ifdef __cplusplus
}
#endif

#endif /* __DHT11_H__ */
