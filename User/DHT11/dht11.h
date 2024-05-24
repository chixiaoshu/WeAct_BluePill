#ifndef __DHT11_H__
#define __DHT11_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

extern TIM_HandleTypeDef htim3;  	//�������

//����ԭ��
void Delay_us(uint8_t);   //΢����ʱ������������һ����ʱ������ΪDHT11ͨѶ�����漰΢����ʱ
void GPIO_Input(void);	  //GPIO ״̬ת��ĺ��� CUBEMXĬ�ϵ�GPIO��ʼ����ֻ������������ߵ�ʹ��
//��GPIO״̬������ �������װ������������
void GPIO_Output(void);
void DHT11_Rst(void);		//������ʼ�ɼ����ź�
uint8_t DHT11_Check(void);  //���DHT�Ƿ��Ӧ
uint8_t DHT11_Init(void);   //��ʼ������
uint8_t DHT11_ReadBit(void);   //��ȡһλ
uint8_t DHT11_ReadByte(void);  //��ȡһ���ֽ�
uint8_t DHT11_ReadData(uint8_t *);   //��ȡ���ݣ�40��λ�� 

#ifdef __cplusplus
}
#endif

#endif /* __DHT11_H__ */
