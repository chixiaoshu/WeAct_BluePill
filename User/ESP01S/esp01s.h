#ifndef _ESP01S_H_
#define _ESP01S_H_


#include "stm32f1xx_hal.h"

/* choose a Serial port to use. */
// #define ESP01S_UART_PORT huart1
#define ESP01S_UART_PORT huart2
extern UART_HandleTypeDef ESP01S_UART_PORT;

#define MQTT    1883
#define MQTTS   8883
#define TRUE    1
#define FALSE   0

void ESP01S_Send(const uint8_t *data, uint16_t size);
// void ESP01S_Init(UART_HandleTypeDef *huart);
void ESP01S_Init(void); 
void ESP01S_Clear(void);
void ESP01S_ConnectWiFi(const char *ssid, const char *password); // 手动输入SSID和PASSWORD进行连接WiFi
void ESP01S_ConnectWiFi_Macro(void); // 密码将在esp01.c的SSID和PASSWD宏中定义
void ESP01S_InitMQTT(void);
void ESP01S_MQTTUserConfig(void);
void ESP01S_ConnectMQTT(void);


#endif
