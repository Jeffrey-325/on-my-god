#ifndef _MYIIC_H
#define _MYIIC_H
#include "main.h"
	
static inline void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin  = BMI270_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BMI270_SDA_GPIO_Port, &GPIO_InitStruct);
}
static inline void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = BMI270_SDA_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BMI270_SDA_GPIO_Port, &GPIO_InitStruct);
}
#define IIC_SCL_H()   HAL_GPIO_WritePin(BMI270_SCL_GPIO_Port, BMI270_SCL_Pin, GPIO_PIN_SET)
#define IIC_SCL_L()   HAL_GPIO_WritePin(BMI270_SCL_GPIO_Port, BMI270_SCL_Pin, GPIO_PIN_RESET)
#define IIC_SDA_H()   HAL_GPIO_WritePin(BMI270_SDA_GPIO_Port, BMI270_SDA_Pin, GPIO_PIN_SET)
#define IIC_SDA_L()   HAL_GPIO_WritePin(BMI270_SDA_GPIO_Port, BMI270_SDA_Pin, GPIO_PIN_RESET)
#define READ_SDA()    HAL_GPIO_ReadPin(BMI270_SDA_GPIO_Port, BMI270_SDA_Pin)

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号



uint8_t IIC_Write_1Byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
uint8_t IIC_Read_1Byte(uint8_t SlaveAddress,uint8_t REG_Address);
uint8_t IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint16_t len, const uint8_t *buf);
uint8_t IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint16_t len, uint8_t *buf);

#endif

