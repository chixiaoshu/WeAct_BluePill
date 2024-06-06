#ifndef _ESP01S_H_
#define _ESP01S_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* choose a Serial port to use. */
#define ESP01S_UART_PORT huart1
// #define ESP01S_UART_PORT huart2
extern UART_HandleTypeDef ESP01S_UART_PORT;
// #define ONLY_ESP01S
// #define NTMODE // 普通传输模式
#define PRMODE // 透传数据模式

#define MQTT    1883
#define MQTTS   8883
#define TRUE    1
#define FALSE   0

typedef struct {
	uint16_t DeviceId;
	uint16_t Temperature;
	uint16_t Humidity;
	uint16_t Lux;
	uint16_t Volt;
}SensorData;

// 定义 ESP01S 接收结构体
#define DATA_CR 0x0D				// 换行符: /r
#define DATA_LF 0x0A				// 换行符: /n
typedef struct {
  uint8_t header_h; 	// 起始位_高
  uint8_t header_l; 	// 起始位_低
  uint8_t ender_h;  	// 结束位_高
  uint8_t ender_l;  	// 结束位_低
  uint8_t len;  			// 数据长度
  uint8_t data[128]; 	// 实际数据
}ESP01S_DataTypeDef;

// 声明外部变量
extern ESP01S_DataTypeDef receive_data_usart1;
extern ESP01S_DataTypeDef receive_data_usart2;
extern SensorData Device5; 


void ESP01S_Send(const uint8_t *data, uint16_t size);
// void ESP01S_Init(UART_HandleTypeDef *huart);
void ESP01S_Init(void); 
void ESP01S_Clear(void);
void ESP01S_ConnectWiFi(const char *ssid, const char *password); // 手动输入SSID和PASSWORD进行连接WiFi
void ESP01S_ConnectWiFi_Macro(void); // 密码将在esp01.c的SSID和PASSWD宏中定义
void ESP01S_ConnectTCP_Server(const char *ip, uint32_t port);
void ESP01S_InitMQTT(void);
void ESP01S_MQTTUserConfig(void);
void ESP01S_ConnectMQTT(void);
void ESP01S_ProcessReceivedData_MQTT(void);
void ESP01S_ProcessReceivedData_TCP(void);

uint16_t string_to_uint16(char *str);

#endif
