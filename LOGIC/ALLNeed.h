#ifndef _ALLNEED_H
#define _ALLNEED_H

//***************头文件区***************//
/*****库文件*****/
#include "tim.h"
#include "usart.h"
#include "stdio.h"
#include "gpio.h"
/*****C语言库*****/
#include "stdio.h"
/*****自定义函数库*****/ 
//#include "timer.h"
#include "delay.h"
#include "imu_bsp.h"
#include "encoder.h"

//***************重定义区***************//

//***************初始化区***************//

void ALL_Init(void);

//***************变量定义区***************//
/**********串口变量**********/

/**********按键变量**********/

/**********其他变量**********/
extern float angle[3];
extern float a;
extern int aaa;
#endif


