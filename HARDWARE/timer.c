#include "timer.h"
#include "imu_bsp.h"
//#include "VOFA.h"
//*************************重定义区*************************//

//*************************变量区*************************//
//float pitch,roll,yaw;
//*************************函数区*************************//

void timer_Init(void)
{
  HAL_TIM_Base_Start_IT(&htim6);          // 开启10ms定时器中断
  HAL_TIM_Base_Start_IT(&htim7);          // 开启10ms定时器中断
}

//*************************中断函数区*************************//
/**********定时器中断回调函数**********/
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	/**********10ms定时器**********/
//	if(htim->Instance==TIM6)
//	{
//		

//	}
//	
//	/**********ICM45686**********/
//	if(htim->Instance==TIM7)
//	{
//		nowtime++;
//	}
//	
//}

/**********定时器输入捕获中断回调函数**********/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	
}

/**********定时器输出比较中断回调函数**********/
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	
}
































