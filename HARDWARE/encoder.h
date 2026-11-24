/**
  ******************************************************************************
  * @file    encoder.h
  * @brief   正交编码器软件解码（GPIO 中断 + 状态表），STM32F407VET6 版
  *
  *          原工程是 TI MSPM0（dl_gpio / DL_GPIO_readPins / DL_INTERRUPT_GROUP_1），
  *          这里全部换成 STM32 HAL 的写法，解码算法（4 倍频状态表）保持不变。
  *
  *          编码器1：PE13 = A 相，PE12 = B 相
  *          编码器2：PE14 = A 相，PE15 = B 相
  *
  *          注意：PE12 在 F407 上是 TIM1_CH3N、PE13 是 TIM1_CH3，
  *                **不是** TIM1 的 CH1/CH2，所以无法使用硬件"编码器接口模式"，
  *                只能用 GPIO 中断软件解码（就是本文件的做法）。
  ******************************************************************************
  */

#ifndef __ENCODER_H
#define __ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* ==========================================================================
 * 1. 引脚定义（换引脚只改这里，同时要保证两个脚在同一个 EXTI 分组里，
 *    PE12/PE13 和 PE14/PE15 都属于 EXTI15_10，共用中断入口）
 * ========================================================================== */
#define ENCODER_A_PORT          GPIOE
#define ENCODER_A_PIN           GPIO_PIN_15     /* A 相 / TI1 */
#define ENCODER_B_PORT          GPIOE
#define ENCODER_B_PIN           GPIO_PIN_14      /* B 相 / TI2 */

#define ENCODER2_A_PORT         GPIOE
#define ENCODER2_A_PIN          GPIO_PIN_13      /* A 相 */
#define ENCODER2_B_PORT         GPIOE
#define ENCODER2_B_PIN          GPIO_PIN_12      /* B 相 */

#define ENCODER_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOE_CLK_ENABLE()
#define ENCODER_IRQn                EXTI15_10_IRQn
#define ENCODER_IRQ_PRIORITY        5U

/* ==========================================================================
 * 2. 开关（按你的工程情况选）
 *
 *  ENCODER_OWN_IRQ_HANDLER
 *      1 = 本文件自己实现 EXTI15_10_IRQHandler（默认，什么都不用配）
 *      0 = 你的工程里已经有了 EXTI15_10_IRQHandler（比如 CubeMX 配了别的 EXTI 脚），
 *          这时把它设 0，并在你自己的中断里调用：
 *              HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
 *              HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
 *              HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
 *              HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
 *
 *  ENCODER_OWN_EXTI_CALLBACK
 *      1 = 本文件实现 HAL_GPIO_EXTI_Callback()（默认）
 *      0 = 你工程里已经有自己的 HAL_GPIO_EXTI_Callback()，
 *          这时把它设 0，并在你的回调里加一行：
 *              Encoder_EXTI_Callback(GPIO_Pin);
 * ========================================================================== */
#ifndef ENCODER_OWN_IRQ_HANDLER
#define ENCODER_OWN_IRQ_HANDLER     1
#endif

#ifndef ENCODER_OWN_EXTI_CALLBACK
#define ENCODER_OWN_EXTI_CALLBACK   1
#endif

/* ==========================================================================
 * 3. 对外接口
 * ========================================================================== */

/* 编码器计数值（4 倍频：A/B 每个边沿 ±1）
   正转加、反转减；用 volatile 因为中断里会改 */
extern volatile int32_t encoder1_num;
extern volatile int32_t encoder2_num;

/* 初始化两路：配 GPIO 为双边沿中断、读初始相位、开中断。main 里调用一次即可。 */
void Encoder_Init(void);

/* 读当前计数值 */
int32_t Encoder_GetCount(void);

/* 清零 */
void Encoder_Reset(void);

/* 取"距上次调用以来"的增量，用来算速度。
   用法：每 10 ms 调一次，返回值就是这 10 ms 内的脉冲数。 */
int32_t Encoder_GetDelta(void);

/* 中断里调用（给自定义 HAL_GPIO_EXTI_Callback 用） */
void Encoder_EXTI_Callback(uint16_t GPIO_Pin);

/* 编码器2（PE14/PE15）的独立接口；Encoder_Init 已包含它的初始化。 */
void Encoder2_Init(void);
int32_t Encoder2_GetCount(void);
void Encoder2_Reset(void);
int32_t Encoder2_GetDelta(void);

/* Encoder_EXTI_Callback 会按引脚分发，也可单独调用此回调。 */
void Encoder2_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __ENCODER_H */
