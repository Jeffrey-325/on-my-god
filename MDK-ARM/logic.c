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

void all_init(void)
{
	encoder1_num = 0;
	encoder2_num = 0;
}

static int part = 0;

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
		
void Test2(void)
{

	
	switch(part)
	{
		case 0:         //A->B 直线
			aaa = 1;
		if(Gray_Read_Filter())   
		{
			aaa = 0;
		}
		break;
		
		case 1:      //B->C 环
			all_init();
			
			xunji_1(10,10);
		break;
		
		case 2:   // C->D 直线
			aaa = 2;
		if(Gray_Read_Filter())
		{
			aaa = 0;
		}
		break;
		
		case 3:   //D->A  环
			all_init();
		if(Gray_Read_Filter())
		{
			aaa = 0;
		}
		break;
		
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
	
		switch(part)
	{
		case 0:      //A->C 斜45直线
			aaa = 3;
		if(Gray_Read_Filter())   
		{
			aaa = 0;
		}
		break;			
			
		case 1:      //C->B 环
			all_init();
			
			xunji_1(10,10);
		break;		
			
	}
		
}





