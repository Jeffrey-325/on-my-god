/**
  ******************************************************************************
  * @file    encoder.c
  * @brief   正交编码器软件解码（GPIO 中断 + 4 倍频状态表），STM32F407VET6 版
  *
  *          原理：A/B 两相相差 90°。把 (A,B) 看成一个 2 位状态，共 4 种：
  *                  A B
  *              0 = 0 0
  *              1 = 0 1
  *              2 = 1 0
  *              3 = 1 1
  *          每次任一相跳变就进中断，读出新的状态，查表得到 +1 / -1 / 0：
  *              +1 = 正转一步
  *              -1 = 反转一步
  *               0 = 没动（抖动或非法跳变，忽略）
  *          A/B 各跳变一次算一步，一个完整周期 4 步，所以叫 4 倍频。
  ******************************************************************************
  */

#include "encoder.h"

/* Private variables ---------------------------------------------------------*/
volatile int32_t encoder1_num = 0;      /* 对外暴露的计数值 */
volatile int32_t encoder2_num = 0;      /* PE14/PE15 编码器计数值 */

static uint8_t old_state = 0U;          /* 上一次的 (A,B) 状态，0~3 */
static int32_t last_count = 0;          /* 供 Encoder_GetDelta 用 */
static uint8_t old_state2 = 0U;         /* 编码器2上一次的 (A,B) 状态 */
static int32_t last_count2 = 0;         /* 供 Encoder2_GetDelta 用 */

/* 正交解码状态表：[旧状态][新状态] -> 增量 */
static const int8_t encoder_table[4][4] =
{
    /* new:  0   1   2   3 */
    {        0, -1,  1,  0 },   /* old 0 */
    {        1,  0,  0, -1 },   /* old 1 */
    {       -1,  0,  0,  1 },   /* old 2 */
    {        0,  1, -1,  0 }    /* old 3 */
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  读当前 (A,B) 状态：A 在高位，B 在低位
  */
static uint8_t Encoder_ReadState(void)
{
    uint8_t a = (HAL_GPIO_ReadPin(ENCODER_A_PORT, ENCODER_A_PIN) == GPIO_PIN_SET) ? 1U : 0U;
    uint8_t b = (HAL_GPIO_ReadPin(ENCODER_B_PORT, ENCODER_B_PIN) == GPIO_PIN_SET) ? 1U : 0U;

    return (uint8_t)((a << 1) | b);
}

/**
  * @brief  初始化编码器
  *         1) PE12/PE13 配成上升沿+下降沿都触发的外部中断，内部上拉
  *         2) 读一次初始状态，避免上电第一个边沿算错方向
  *         3) 初始化 PE14/PE15 编码器并打开共用的 EXTI15_10 中断
  */
void Encoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    ENCODER_GPIO_CLK_ENABLE();

    /* 先清掉可能残留的中断标志，避免一开中断就误进一次 */
    __HAL_GPIO_EXTI_CLEAR_IT(ENCODER_A_PIN);
    __HAL_GPIO_EXTI_CLEAR_IT(ENCODER_B_PIN);

    GPIO_InitStruct.Pin   = ENCODER_A_PIN | ENCODER_B_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;   /* 双边沿 */
    GPIO_InitStruct.Pull  = GPIO_PULLUP;                   /* 编码器多为开集输出，上拉必须 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ENCODER_A_PORT, &GPIO_InitStruct);

    /* 记住上电时的相位 */
    old_state  = Encoder_ReadState();
    encoder1_num = 0;
    last_count   = 0;

    Encoder2_Init();
}

/**
  * @brief  读编码器2的 (A,B) 状态：A 在高位，B 在低位
  */
static uint8_t Encoder2_ReadState(void)
{
    uint8_t a = (HAL_GPIO_ReadPin(ENCODER2_A_PORT, ENCODER2_A_PIN) == GPIO_PIN_SET) ? 1U : 0U;
    uint8_t b = (HAL_GPIO_ReadPin(ENCODER2_B_PORT, ENCODER2_B_PIN) == GPIO_PIN_SET) ? 1U : 0U;

    return (uint8_t)((a << 1) | b);
}

/**
  * @brief  初始化编码器2：PE14 = A 相，PE15 = B 相，双边沿中断、内部上拉
  */
void Encoder2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    ENCODER_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin   = ENCODER2_A_PIN | ENCODER2_B_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ENCODER2_A_PORT, &GPIO_InitStruct);

    old_state2   = Encoder2_ReadState();
    encoder2_num = 0;
    last_count2  = 0;

    __HAL_GPIO_EXTI_CLEAR_IT(ENCODER2_A_PIN);
    __HAL_GPIO_EXTI_CLEAR_IT(ENCODER2_B_PIN);

    HAL_NVIC_SetPriority(ENCODER_IRQn, ENCODER_IRQ_PRIORITY, 0U);
    HAL_NVIC_EnableIRQ(ENCODER_IRQn);
}

/**
  * @brief  中断里调用的解码函数
  * @param  GPIO_Pin 触发中断的引脚（HAL 已经帮我们清过标志位）
  */
void Encoder_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint8_t new_state;

    if ((GPIO_Pin != ENCODER_A_PIN) && (GPIO_Pin != ENCODER_B_PIN))
    {
        Encoder2_EXTI_Callback(GPIO_Pin);
        return;
    }

    new_state = Encoder_ReadState();

    encoder1_num += encoder_table[old_state][new_state];
    old_state = new_state;
}

/**
  * @brief  读计数值
  */
int32_t Encoder_GetCount(void)
{
    return encoder1_num;
}

/**
  * @brief  清零
  */
void Encoder_Reset(void)
{
    encoder1_num = 0;
    last_count   = 0;
}

/**
  * @brief  取增量（用于测速）
  *         建议每 5~20 ms 调用一次，返回值 = 这段时间内的脉冲数，
  *         转速 ∝ 返回值 / 时间。
  */
int32_t Encoder_GetDelta(void)
{
    int32_t now = encoder1_num;
    int32_t delta = now - last_count;

    last_count = now;

    return delta;
}

/**
  * @brief  编码器2的中断解码，独立保存相位和计数
  */
void Encoder2_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint8_t new_state;

    if ((GPIO_Pin != ENCODER2_A_PIN) && (GPIO_Pin != ENCODER2_B_PIN))
    {
        return;
    }

    new_state = Encoder2_ReadState();
    encoder2_num += encoder_table[old_state2][new_state];
    old_state2 = new_state;
}

/**
  * @brief  读编码器2的累计计数值
  */
int32_t Encoder2_GetCount(void)
{
    return encoder2_num;
}

/**
  * @brief  编码器2清零，同时清除增量基准
  */
void Encoder2_Reset(void)
{
    encoder2_num = 0;
    last_count2  = 0;
}

/**
  * @brief  取编码器2距上次调用以来的增量，建议每 5~20 ms 调用一次
  */
int32_t Encoder2_GetDelta(void)
{
    int32_t now = encoder2_num;
    int32_t delta = now - last_count2;

    last_count2 = now;

    return delta;
}

#if (ENCODER_OWN_EXTI_CALLBACK == 1)
/**
  * @brief  HAL 的 EXTI 回调（HAL 里是 __weak，这里覆盖它）
  *         如果你的工程已经有自己的 HAL_GPIO_EXTI_Callback，
  *         把 ENCODER_OWN_EXTI_CALLBACK 设为 0，并在你自己的回调里调用
  *         Encoder_EXTI_Callback(GPIO_Pin);
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    Encoder_EXTI_Callback(GPIO_Pin);
}
#endif

#if (ENCODER_OWN_IRQ_HANDLER == 1)
/**
  * @brief  EXTI12 / EXTI13 / EXTI14 / EXTI15 共用的中断入口
  */
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(ENCODER_A_PIN);
    HAL_GPIO_EXTI_IRQHandler(ENCODER_B_PIN);
    HAL_GPIO_EXTI_IRQHandler(ENCODER2_A_PIN);
    HAL_GPIO_EXTI_IRQHandler(ENCODER2_B_PIN);
}
#endif
