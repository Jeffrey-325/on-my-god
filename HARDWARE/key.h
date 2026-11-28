#ifndef CAR_KEY_H
#define CAR_KEY_H

#include "stm32f4xx_hal.h"

#define key1 (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)  == GPIO_PIN_RESET)
#define key2 (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == GPIO_PIN_RESET)
#define key3 (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11) == GPIO_PIN_RESET)
#define key4 (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)  == GPIO_PIN_RESET)

/* Call after GPIO initialization, on every pass through the main loop. */
void Key_Poll(void);

#endif
