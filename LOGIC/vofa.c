#include "vofa.h"
#include "usart.h"
#include "motor.h"
#include <string.h>

/* Target speed, unit: encoder counts per 10 ms (4x decoding), not rpm.
   Change these two numbers to change the target speed.
   They are sent on CH0 / CH2 so the plot always shows the real setpoint. */
#define SPEED_SET_ZUO     30.0f
#define SPEED_SET_YOU     30.0f

/* VOFA+ JustFloat: N floats + tail {0, 0, 0x80, 0x7f}.
   Frame layout (4 channels):
     CH0 = left  target  SPEED_SET_ZUO
     CH1 = left  actual  speed_now_zuo   (measured in the TIM6 10 ms interrupt)
     CH2 = right target  SPEED_SET_YOU
     CH3 = right actual  speed_now_you   (measured in the TIM6 10 ms interrupt)
   The actual speeds are only read here, the encoder deltas stay owned by the
   interrupt so this file never disturbs the speed loop. */
#define VOFA_PORT         (&huart2)
#define VOFA_PERIOD_MS    5U          /* 50 Hz, enough to see a 10 ms control loop */

static uint32_t last_send;

HAL_StatusTypeDef VOFA_Send4(UART_HandleTypeDef *uart, float a, float b, float c, float d)
{
    float values[4]; uint8_t frame[20];
    static const uint8_t tail[4] = {0, 0, 0x80, 0x7f};
    if (uart == NULL || sizeof(float) != 4U) return HAL_ERROR;
    values[0]=a; values[1]=b; values[2]=c; values[3]=d;
    memcpy(frame,values,16); memcpy(frame+16,tail,4);
    return HAL_UART_Transmit(uart,frame,sizeof(frame),10);
}

/* Call once before the main loop. The speed loop owns PID and PWM, so this
   function does not touch PID state or the motor. */
HAL_StatusTypeDef VOFA_Init(void)
{
    last_send = HAL_GetTick();
    return HAL_OK;
}

/* Call from the main loop only: HAL_UART_Transmit blocks for ~1.8 ms at 115200. */
void VOFA_Process(void)
{
    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - last_send) < VOFA_PERIOD_MS) return;
    last_send = now;

    (void)VOFA_Send4(VOFA_PORT, SPEED_SET_ZUO, speed_now_zuo,
                                SPEED_SET_YOU, speed_now_you);
}

