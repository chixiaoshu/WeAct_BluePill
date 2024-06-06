#include "esp01s.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define SSID "CHIX-14"     // 路由器或热点账号
#define PASSWD "12345678" // 路由器或热点密码

#define NTP_1 "ntp1.aliyun.com" // 主 NTP 服务器
#define NTP_2 "ntp2.aliyun.com" // 备用 NTP 服务器

#define MQTT_CLIENTID_ID "<>"        // DeviceID
#define MQTT_USERNAME "<>"                          // Username
#define MQTT_PASSWD "<>" // Password

#define MQTT_DOMAIN "7ce86aad4b.st1.iotda-device.cn-north-4.myhuaweicloud.com" // MQTT 域名
#define MQTT_PORT MQTT                                                         // Set port /MQTT is 1883 /MQTTS is 8883
#define MQTT_RECONN FALSE                                                      // Reconnect or not 是否重新连接

ESP01S_DataTypeDef receive_data_usart1;
ESP01S_DataTypeDef receive_data_usart2;

SensorData Device5;

char ESP01S_Buf[255];

void ESP01S_Send(const uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&ESP01S_UART_PORT, data, size, HAL_MAX_DELAY);
}

void ESP01S_Clear(void)
{
    memset(ESP01S_Buf, 0, sizeof(ESP01S_Buf));
    HAL_Delay(10);
}

void ESP01S_ProcessData(const char *data)
{
    // 处理接收到的数据，根据需要进行解析和处理
    // 例如，可以在这里判断是否收到预期的回复，并做相应的处理
    if (strstr(data, "OK") != NULL)
    {
        // 处理收到的 "OK" 回复
        HAL_UART_Transmit(&huart1, (uint8_t *)"OK.\r\n", 4, HAL_MAX_DELAY);
    }
    else if (strstr(data, "ERROR") != NULL)
    {
        // 处理收到的 "ERROR" 回复
    }
    else
    {
        // 处理其他情况
    }
}

// 初始化ESP8266-01s
// 默认波特率115200
void ESP01S_Init(void)
{
    sprintf(ESP01S_Buf, "AT+CWMODE=1\r\n");
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    ESP01S_Clear();

    sprintf(ESP01S_Buf, "AT+CIPSNTPCFG=1,8,\"%s\",\"%s\"\r\n", NTP_1, NTP_2);
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    ESP01S_Clear();

    sprintf(ESP01S_Buf, "AT+CWDHCP=1,1\r\n");
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    ESP01S_Clear();

    sprintf(ESP01S_Buf, "AT+RST\r\n");
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    ESP01S_Clear();
    HAL_Delay(1000);
    HAL_UART_Transmit(&huart1, (uint8_t *)"ESP01S is initialized.\r\n", 24, HAL_MAX_DELAY);
}

// WiFi连接函数
void ESP01S_ConnectWiFi(const char *ssid, const char *password)
{
    char cmd[100];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    ESP01S_Send((uint8_t *)cmd, strlen(cmd));
    HAL_UART_Transmit(&huart1, (uint8_t *)"Trying connect.\r\n", 16, HAL_MAX_DELAY);
    HAL_Delay(3000);
}

void ESP01S_ConnectWiFi_Macro(void)
{
    sprintf(ESP01S_Buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    HAL_UART_Transmit(&huart2, (uint8_t *)"Trying connect.\r\n", 16, HAL_MAX_DELAY);
    ESP01S_Clear();
    HAL_Delay(3000);
}
// 连接 TCP-Server // ESP01S作为TCP-Client
void ESP01S_ConnectTCP_Server(const char *ip, uint32_t port)
{
    char cmd[100];
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    ESP01S_Send((uint8_t *)cmd, strlen(cmd));
    HAL_Delay(500);
    //    HAL_UART_Transmit(&huart1, (uint8_t *)"Trying connect TCP.\r\n", 20, HAL_MAX_DELAY);
    sprintf(cmd, "AT+CIPMODE=1\r\n");
    ESP01S_Send((uint8_t *)cmd, strlen(cmd));
}

// 连接 MQTT 初始化
void ESP01S_InitMQTT(void)
{
    ESP01S_MQTTUserConfig();
    ESP01S_ConnectMQTT();
}

void ESP01S_MQTTUserConfig(void)
{
    HAL_Delay(1000);
    sprintf(ESP01S_Buf, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", MQTT_CLIENTID_ID, MQTT_USERNAME, MQTT_PASSWD);
    // sprintf(ESP01S_Buf, "AT+MQTTUSERCFG=%d,%d,\"%s\",\"%s\",\"%s\",%d,%d,\"\"\r\n",0,1,MQTT_CLIENTID_ID,MQTT_USERNAME,MQTT_PASSWD,0,0);
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    HAL_UART_Transmit(&huart1, (uint8_t *)"Set MQTT Config.\r\n", 17, HAL_MAX_DELAY);
    ESP01S_Clear();
}

void ESP01S_ConnectMQTT(void)
{
    HAL_Delay(1000);
    sprintf(ESP01S_Buf, "AT+MQTTCONN=0,\"%s\",%d,%d\r\n", MQTT_DOMAIN, MQTT_PORT, MQTT_RECONN);
    // sprintf(ESP01S_Buf, "AT+MQTTCONN=%d,\"%s\",%d,%d\r\n", 0, MQTT_DOMAIN, 1883, 0);
    ESP01S_Send((uint8_t *)ESP01S_Buf, strlen(ESP01S_Buf));
    ESP01S_Clear();
    HAL_UART_Transmit(&huart1, (uint8_t *)"Connecting MQTT.\r\n", 17, HAL_MAX_DELAY);
}

void ESP01S_ProcessReceivedData_MQTT(void)
{
    receive_data_usart1.header_h = DATA_CR;
    receive_data_usart1.header_l = DATA_LF;
    receive_data_usart1.ender_h = DATA_CR;
    receive_data_usart1.ender_l = DATA_LF;
}

void ESP01S_ProcessReceivedData_TCP(void)
{
    receive_data_usart1.header_h = DATA_CR;
    receive_data_usart1.header_l = DATA_LF;
    receive_data_usart1.ender_h = DATA_CR;
    receive_data_usart1.ender_l = DATA_LF;

#ifdef PRMODE

    if ((receive_data_usart1.data[receive_data_usart1.len - 2] == DATA_CR) &&
        (receive_data_usart1.data[receive_data_usart1.len - 1] == DATA_LF))
    {
        if ((receive_data_usart1.data[0] == DATA_CR) && (receive_data_usart1.data[1] == DATA_LF))
        {
            uint8_t data_size = receive_data_usart1.len - 4;
            uint8_t split_count = 0;
            for (uint8_t i = 2; i < receive_data_usart1.len - 2; i++)
            {
                if (receive_data_usart1.data[i] == '=')
                {
                    split_count++;
                }
            }

            // 根据 split_count 验证格式是否正确（例如split_count == 5）
            if (split_count == 5)
            {
                // 定义一个临时缓冲区来存储数据
                char temp_buffer[REC_BUF_LEN] = {0};
                char temp_send_buffer[REC_BUF_LEN] = {0};
                memcpy(temp_buffer, &receive_data_usart1.data[2], data_size);

                // 提取并解析每个数据字段
                char *token = strtok(temp_buffer, ",");
                while (token != NULL)
                {
                    if (strncmp(token, "DeviceId=", 9) == 0)
                    {
                        Device5.DeviceId = atoi(&token[9]);
                    }
                    else if (strncmp(token, "Temp=", 5) == 0)
                    {
                        Device5.Temperature = atoi(&token[5]);
                    }
                    else if (strncmp(token, "Hum=", 4) == 0)
                    {
                        Device5.Humidity = atoi(&token[4]);
                    }
                    else if (strncmp(token, "Lux=", 4) == 0)
                    {
                        Device5.Lux = atoi(&token[4]);
                    }
                    else if (strncmp(token, "Volt=", 5) == 0)
                    {
                        Device5.Volt = atoi(&token[5]);
                    }
                    token = strtok(NULL, ",");
                }
                // 清空接收数据数组
                memset(receive_data_usart1.data, 0, REC_BUF_LEN);
                receive_data_usart1.len = 0;
            }
        }
    }
#else
// 如果未定义 PRMODE，可以在此处理其他情况
#endif
}

#ifdef ONLY_ESP01S // If only the ESP01S module is used, and no other UART modules are present
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        receive_data_usart1.len = Size;
    }

    if (huart->Instance == USART2)
    {
        receive_data_usart2.len = Size;
    }
}

#endif
