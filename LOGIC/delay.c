#include "delay.h"
#include "stdint.h"

// DWT寄存器定义
#define DWT_CTRL   (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT (*(volatile uint32_t*)0xE0001004)
#define DEM_CR     (*(volatile uint32_t*)0xE000EDFC)

// 系统时钟频率（需要根据实际情况设置）
extern uint32_t SystemCoreClock;

//***************  使用 delay_us 与 delay_ms 前一定使用初始化！！！！！！！！！！  ***************//
// 启用DWT周期计数器
void DWT_Init(void)
{
    // 启用跟踪功能
    DEM_CR |= (1 << 24);
    
    // 清零周期计数器并启用它
    DWT_CYCCNT = 0;
    DWT_CTRL |= (1 << 0);
}

//***************us级延迟函数（上限为 0xFFFFFFFF/(SystemCoreClock/1000000)）***************//
void delay_us(uint32_t us)     
{
    uint32_t start = DWT_CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    
    // 显式处理计数器溢出
    while(1)
    {
        uint32_t current = DWT_CYCCNT;
        uint32_t elapsed;
        
        if(current >= start)
        {
            elapsed = current - start;
        }
        else
        {
            // 计数器溢出情况
            elapsed = (0xFFFFFFFF - start) + current + 1;
        }
        
        if(elapsed >= cycles)
            break;
    }
}

//***************ms级延迟函数（上限为 0xFFFFFFFF-1）***************//
void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_us(1000);
    }
}









































