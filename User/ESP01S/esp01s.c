#include "esp01s.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define SSID "CHIX-ZTE-School"     // 路由器或热点账号
#define PASSWD "d812345679" // 路由器或热点密码

#define NTP_1 "ntp1.aliyun.com" // 主 NTP 服务器
#define NTP_2 "ntp2.aliyun.com" // 备用 NTP 服务器

#define MQTT_CLIENTID_ID "<DeviceID>"        // DeviceID
#define MQTT_USERNAME "<Username>"                          // Username
#define MQTT_PASSWD "<Password>" // Password

#define MQTT_DOMAIN "<DOMIAN>" // MQTT 域名
#define MQTT_PORT MQTT                                                         // Set port /MQTT is 1883 /MQTTS is 8883
#define MQTT_RECONN FALSE                                                      // Reconnect or not 是否重新连接

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
    HAL_UART_Transmit(&huart1, (uint8_t *)"Trying connect.\r\n", 16, HAL_MAX_DELAY);
    ESP01S_Clear();
    HAL_Delay(3000);
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
    sprintf(ESP01S_Buf, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",MQTT_CLIENTID_ID,MQTT_USERNAME,MQTT_PASSWD);
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
