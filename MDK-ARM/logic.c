#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "motor.h"
#include "timer.h"
#include "encoder.h"
#include "imu_bsp.h"
#include "IMU.h"
#include "common.h"
#include "vofa.h"
#include "logic.h"

static int part = 0;

void all_init(void)
{
	encoder1_num = 0;
	encoder2_num = 0;
	part = 0;
}



/**********题1******/
void Test1(void)
	{
		static uint8_t stopped = 0;      /* 0=行驶，1=已停车（只变一次） */

    if (stopped) return;             /* 停住后彻底不管 aaa */

    if (Gray_Read_Filter())
    {
        stopped = 1;
        aaa = 0;                     /* 1 → 0，只变这一次 */
											/* 立刻刹，不等中断 */
    }
    else
    {
        aaa = 1;                     /* 一直是 1，值不变，中断什么时刻读都一样 */
    }
	}	
		///// aaa = 0 -> 停车   1 -> 直线    2 -> 180   3 ->  45  4 -> 135
void Test2(void)
{

	static uint8_t arc_seen = 0;
	static uint32_t last_line_ms = 0;
	


	
	switch(part)
	{
		case 0:         //A->B 直线
			aaa = 1;
		if(Gray_Read_Filter())   
		{
			aaa = 0;
			arc_seen = 0;
			aaa = 5;  // 循迹模式：当前 TIM6 对这个值不写 PWM
			part = 1;
		}
		break;
		
	case 1: {  // B->C 循迹
    uint32_t now = HAL_GetTick();
    uint8_t on_line = Gray_Read_Filter();

    if (on_line) {
        arc_seen = 1;
        last_line_ms = now;
    } else if (arc_seen && (uint32_t)(now - last_line_ms) >= 30U) {
        aaa = 2;
        part = 2;
        break;
    }

    xunji_1(1500, 1500);
    break;
}


		
		case 2:   // C->D 直线
			aaa = 2;

			if(Gray_Read_Filter())   
		{
			aaa = 0;
			arc_seen = 0;
			aaa = 5;  // 循迹模式：当前 TIM6 对这个值不写 PWM
			part = 3;
		}
			break;
		
		case 3: {  // B->C 循迹
    uint32_t now = HAL_GetTick();
    uint8_t on_line = Gray_Read_Filter();

    if (on_line) {
        arc_seen = 1;
        last_line_ms = now;
    } else if (arc_seen && (uint32_t)(now - last_line_ms) >= 30U) {
        aaa = 0;
        part = 4;
        break;
    }

    xunji_1(1500, 1500);
    break;
}
//		case 3:   //D->A  环

//			aaa = 0;
//		xunji_1(1500,1500);
//		if (Out_1 == 1 && Out_2 == 1 && Out_3 == 1 && Out_4 == 1 &&
//       Out_5 == 1 && Out_6 == 1 && Out_7 == 1 && Out_8 == 1){ aaa = 0;}
//			 
//		break;
		
	}
	
}		



void Test3(void)
{
//	Go_Straight_1();
//	xunji_1(10,10);
//	//加个检测
//	Go_Straight_2();
//	xunji_1(10,10);
//	
	
	static uint8_t arc_seen = 0;
	static uint32_t last_line_ms = 0;
		switch(part)
	{
		case 0:      //A->C 斜45直线
			aaa = 3;
		if(Gray_Read_Filter())   
		{
			aaa = 0;
			arc_seen = 0;
			aaa = 5;  // 循迹模式：当前 TIM6 对这个值不写 PWM
			part = 1;
		}
		break;
			
		
		case 1: {      //C->B 环
			 
    uint32_t now = HAL_GetTick();
    uint8_t on_line = Gray_Read_Filter();

    if (on_line) {
        arc_seen = 1;
        last_line_ms = now;
    } else if (arc_seen && (uint32_t)(now - last_line_ms) >= 30U) {
        aaa = 2;
        part = 2;
        break;
    }

    xunji_1(1500, 1500);
    break;}
		
		case 2:
			aaa = 4;
		if(Gray_Read_Filter())   
		{
			aaa = 0;
			arc_seen = 0;
			aaa = 5;  // 循迹模式：当前 TIM6 对这个值不写 PWM
			part = 3;
		}
		break;
		
		case 3:
			{  // B->C 循迹
    uint32_t now = HAL_GetTick();
    uint8_t on_line = Gray_Read_Filter();

    if (on_line) {
        arc_seen = 1;
        last_line_ms = now;
    } else if (arc_seen && (uint32_t)(now - last_line_ms) >= 30U) {
        aaa = 0;
        part = 4;
        break;
    }
		    xunji_1(1500, 1500);
    break;
	}
		
	}
}





