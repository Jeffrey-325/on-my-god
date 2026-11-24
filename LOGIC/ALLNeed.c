#include "ALLNeed.h"
//#include "logic.h"
#include "motor.h"
#include "timer.h"

float a;
//***************初始化区***************//
void ALL_Init(void)
{
	DWT_Init();
	IMU_INIT(BMI2_SPI_INTF);
	Encoder_Init();
	timer_Init();
}

//***************printf重定向区域***************//

//int fputc(int ch,FILE *p)
//{
//	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xffff);
//	return ch;
//}

//***************变量定义区***************//
/**********串口变量**********/

/**********定时器变量**********/         

/**********OLED变量**********/

/**********按键变量**********/

/**********编码器变量**********/

/**********其他变量**********/
float angle[3];
//***************中断回调函数***************//


int aaa=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/**********10ms定时器**********/
	if(htim->Instance==TIM6)
	{
		
		/* Disabled during VOFA speed tuning: avoid UART mixing and long ISR. */
		IMU_Update(angle);
		speed_now_zuo = (float)Encoder2_GetDelta();   /* 左轮 */
    speed_now_you = (float)Encoder_GetDelta();    /* 右轮 */
		if(aaa == 0)
		{
			SET_Motor_PWM(0,0);
		}
			/*第一题无线走直线*/
		if(aaa == 1)
		{
			Angle_Control(0,2000,2000);
		}
		else if (aaa == 2) //第二题C->D
		{
			Angle_Control(180,2000,2000);
		}
		else if (aaa == 3) //第三题A->C
		{
			Angle_Control(45,2000,2000);
		}
		else if (aaa == 4) //第三题B->D
		{
			Angle_Control(135,2000,2000);
		}
	}
	
	/**********ICM45686**********/
	if(htim->Instance==TIM7)
	{
		nowtime++;
	}
//	//
//	if(htim->Instance == TIM10)  
//    {	


//			
//       // xunji_2(90,2000,2000);			
//			    Read_Encoder_ZUO();
//					Read_Encoder_YOU();
//				  a=you_bianma*0.025;  
//    }

}


//***************逻辑实现区***************//
 




/***************已完成记录区**************



*************************************************/





/***************注意事项，问题收集区**************









*************************************************/














