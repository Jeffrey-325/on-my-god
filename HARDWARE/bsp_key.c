#include "bsp_key.h"
#include "delay.h"



///////////*延时函数*/
//void Delay_Init(void)
//{
//    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
//        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
//        DWT->CYCCNT = 0;
//        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//    }
//}

//// 微秒延时
//void delay_us(uint32_t us)
//{
//    uint32_t start = DWT->CYCCNT;
//    // 系统时钟频率，例如168MHz
//    uint32_t cycles = us * (SystemCoreClock / 1000000);
//    
//    while ((DWT->CYCCNT - start) < cycles) {
//        // 等待
//    }
//}

//void delay_ms(uint32_t ms)
//{
//    for (uint16_t i = 0; i < ms; i++)
//    {
//        delay_us(1000);
//    }
//}



/////////
void KEY1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIO时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*---- 配置列线 L1~L4（输入上拉）----*/
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*---- 配置行线 H1~H4（推挽输出）----*/
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    // H1 → PE1
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // H2/H3/H4 → PB9/PB7/PB5
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 行线默认输出高电平
    H1(GPIO_PIN_SET);
    H2(GPIO_PIN_SET);
    H3(GPIO_PIN_SET);
    H4(GPIO_PIN_SET);
}
// 矩阵键盘扫描函数
uint8_t KEY1_Scan(void) 
{
    // 扫描第1行 (H1=0)
    H1(GPIO_PIN_RESET); 
    H2(GPIO_PIN_SET); 
    H3(GPIO_PIN_SET); 
    H4(GPIO_PIN_SET);
    delay_ms(2); // 添加短暂延时确保电平稳定
    if(L1 == GPIO_PIN_RESET) return S1;
    if(L2 == GPIO_PIN_RESET) return S2;
    if(L3 == GPIO_PIN_RESET) return S3;
    if(L4 == GPIO_PIN_RESET) return S4;
    
    // 扫描第2行 (H2=0)
    H1(GPIO_PIN_SET); 
    H2(GPIO_PIN_RESET); 
    H3(GPIO_PIN_SET); 
    H4(GPIO_PIN_SET);
    delay_ms(2);
    if(L1 == GPIO_PIN_RESET) return S5;
    if(L2 == GPIO_PIN_RESET) return S6;
    if(L3 == GPIO_PIN_RESET) return S7;
    if(L4 == GPIO_PIN_RESET) return S8;
    
    // 扫描第3行 (H3=0)
    H1(GPIO_PIN_SET); 
    H2(GPIO_PIN_SET); 
    H3(GPIO_PIN_RESET); 
    H4(GPIO_PIN_SET);
    delay_ms(2);
    if(L1 == GPIO_PIN_RESET) return S9;
    if(L2 == GPIO_PIN_RESET) return S10;
    if(L3 == GPIO_PIN_RESET) return S11;
    if(L4 == GPIO_PIN_RESET) return S12;
    
    // 扫描第4行 (H4=0)
    H1(GPIO_PIN_SET); 
    H2(GPIO_PIN_SET); 
    H3(GPIO_PIN_SET); 
    H4(GPIO_PIN_RESET);
    delay_ms(2);
    if(L1 == GPIO_PIN_RESET) return S13;
    if(L2 == GPIO_PIN_RESET) return S14;
    if(L3 == GPIO_PIN_RESET) return S15;
    if(L4 == GPIO_PIN_RESET) return S16;
    
    return 0; // 无按键按下
}
