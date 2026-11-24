#ifndef VOFA_H
#define VOFA_H
#include "stm32f4xx_hal.h"
#include <stdint.h>

HAL_StatusTypeDef VOFA_Send4(UART_HandleTypeDef *uart, float ch1, float ch2, float ch3, float ch4);
/* Call once after UART and encoder initialization; service from the main loop. */
HAL_StatusTypeDef VOFA_Init(void);
void VOFA_Process(void);
#endif
