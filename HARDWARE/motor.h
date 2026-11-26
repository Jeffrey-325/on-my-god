#ifndef  _MOTOR_H
#define _MOTOR_H
#include <stdint.h>

void SET_Motor_PWM(int zuo,int you);
void xunji_1(int zuo_basic,int you_basic);
uint8_t Gray_Read_Filter(void);
void Angle_Control(float set_angle,int zuo_basic,int you_basic);
void xunji_3(float set_angle,int zuo_basic,int you_basic);
int Read_Encoder_ZUO(void);
int Read_Encoder_YOU(void);
void bai(void);
#define zuo_IN1_GPIO_Port GPIOD
#define zuo_IN1_Pin GPIO_PIN_1
#define zuo_IN2_GPIO_Port GPIOD
#define zuo_IN2_Pin GPIO_PIN_2
#define you_IN1_GPIO_Port GPIOD
#define you_IN1_Pin GPIO_PIN_3
#define you_IN2_GPIO_Port GPIOD
#define you_IN2_Pin GPIO_PIN_4
extern float speed_now_zuo, speed_now_you;
extern float speed_set_zuo, speed_set_you;
void Speed_Control(float target_zuo, float target_you);
//#define TIM_Encoder_Zuo TIM2
//#define TIM_Encoder_You TIM1
//#define Encoder_zuo htim2
//#define Encoder_you htim1
//extern int zuo_bianma;
//extern int you_bianma;


#endif


