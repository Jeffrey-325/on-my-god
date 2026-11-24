#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f4xx_hal.h"

// 独立按键读取宏定义
#define KEY0    HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)  // 读取按键0
#define KEY1    HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)  // 读取按键1
#define WK_UP   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)  // 读取按键2 

// 列线输入引脚配置（上拉）
#define L1      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) // PD7
#define L2      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_5) // PD5
#define L3      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) // PD3
#define L4      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) // PD1

// 行线输出引脚配置
#define H1(x)   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, (x))   // PE1
#define H2(x)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, (x))   // PB9
#define H3(x)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (x))  // PB7
#define H4(x)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (x))  // PB5

// 按键值定义
#define S1      '1'    
#define S2      '2'    
#define S3      '3'
#define S4      'A'    
#define S5      '4'    
#define S6      '5'    
#define S7      '6'
#define S8      'B'    
#define S9      '7'
#define S10     '8'    
#define S11     '9'
#define S12     'C'    
#define S13     '*'    
#define S14     '-'    
#define S15     '#'    
#define S16     'D'

// 函数声明
void KEY1_Init(void);    // IO初始化
uint8_t KEY1_Scan(void); // 矩阵键盘扫描函数


//delay

void Delay_Init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif
