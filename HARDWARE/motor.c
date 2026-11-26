#include "main.h"
#include "delay.h"
#include "tim.h"
#include "gpio.h"
#include "pid.h"
#include "motor.h"
#include "ALLNeed.h"

extern float start;
float speed_now_zuo = 0, speed_now_you = 0;   /* 实际速度，TIM6 中断每 10ms 更新 */
float speed_set_zuo = 0, speed_set_you = 0;   /* 目标速度，主循环随时改 */




void SET_Motor_PWM(int zuo,int you)
{
	
//    HAL_TIM_Encoder_Start(&Encoder_zuo, TIM_CHANNEL_ALL); // ???????????????
//    HAL_TIM_Encoder_Start(&Encoder_you, TIM_CHANNEL_ALL); // ???????????????
//    __HAL_TIM_ENABLE_IT(&Encoder_zuo, TIM_IT_UPDATE);        // ????????????????????ж?,?????????
//    __HAL_TIM_ENABLE_IT(&Encoder_you, TIM_IT_UPDATE);        // ????????????????????ж?,?????????
//    
//    __HAL_TIM_SetCounter(&Encoder_zuo, 0);                // ???????????????
//    __HAL_TIM_SetCounter(&Encoder_you, 0);                // ???????????????    


	
		if (zuo >= 0)							
	{
    HAL_GPIO_WritePin(zuo_IN1_GPIO_Port, zuo_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(zuo_IN2_GPIO_Port, zuo_IN2_Pin, GPIO_PIN_RESET);
	}
	else								
	{
    HAL_GPIO_WritePin(zuo_IN1_GPIO_Port,zuo_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(zuo_IN2_GPIO_Port,zuo_IN2_Pin, GPIO_PIN_SET);
		zuo = -zuo;			
	}
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, zuo);
//	htim3.Instance ->CCR1 =zuo;
    
	
		if (you >= 0)							
	{
    HAL_GPIO_WritePin(you_IN1_GPIO_Port, you_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(you_IN2_GPIO_Port, you_IN2_Pin, GPIO_PIN_RESET);
	}
	else								
	{
    HAL_GPIO_WritePin(you_IN1_GPIO_Port,you_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(you_IN2_GPIO_Port,you_IN2_Pin, GPIO_PIN_SET);
		you = -you;			
	}
//	htim3.Instance ->CCR2 =you;
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, you);
}


int zuo_bianma = 0,you_bianma = 0;

//int Read_Encoder_ZUO(void)
//{
//    int Encoder_TIM;
//    Encoder_TIM = TIM_Encoder_Zuo->CNT;     // 读取计数
//    if(Encoder_TIM > 0xefff)                // 转化计数值为有方向的值，大于0正转，小于0反转。
//      Encoder_TIM = Encoder_TIM - 0xffff;   // TIM4->CNT范围为0-0xffff，初值为0。
//    zuo_bianma+=Encoder_TIM;                   // 左轮总编码数
//    TIM_Encoder_Zuo->CNT = 0;               // 读取完后计数清零
//    return Encoder_TIM;                     // 返回值
//}

//int Read_Encoder_YOU(void)
//{
//    int Encoder_TIM;
//    Encoder_TIM = TIM_Encoder_You->CNT;     // 读取计数
//    if(Encoder_TIM > 0xefff)                // 转化计数值为有方向的值，大于0正转，小于0反转。
//      Encoder_TIM = Encoder_TIM - 0xffff;   // TIM4->CNT范围为0-0xffff，初值为0。
//    you_bianma+=Encoder_TIM;                   // 右轮总编码数
//    TIM_Encoder_You->CNT = 0;               // 读取完后计数清零
//    return Encoder_TIM;                     // 返回值
//}

/*位置环*/

 void xunji_1(int zuo_basic,int you_basic)
{
//	int xunji_p=200;
	float err=0.0f;
	float out=0.0f;
	float count = 0.0f;
	
 if(Out_1==1){err +=9; count ++;}
 if(Out_2==1){err +=8; count ++;}
 if(Out_3==1){err +=3; count ++;}
 if(Out_4==1){err +=1; count ++;}
	
 if(Out_8==1){err -=9; count ++;}
 if(Out_7==1){err -=8; count ++;}
 if(Out_6==1){err -=3; count ++;}
 if(Out_5==1){err -=1; count ++;}
 
 //防止除零
	if(count != 0.0f)
        out = err / count;
    else
        out = 0.0f;
	
 PID_location(0, out,&Gray_PID);
	

	SET_Motor_PWM(zuo_basic + Gray_PID.out,you_basic - Gray_PID.out);


	
}

/*灰度输入防抖：周期调用，连续两次采样相同才更新结果 */

uint8_t Gray_Read_Filter(void)
{
    static uint8_t last = 0, value = 0;

    uint8_t now = (Out_1 == 0 || Out_2 == 0 || Out_3 == 0 || Out_4 == 0 ||
                   Out_5 == 0 || Out_6 == 0 || Out_7 == 0 || Out_8 == 0) ? 1 : 0;

    if (now == last) value = now;
    last = now;

    return value;
}

/*速度环*/

void Speed_Control(float target_zuo, float target_you)
{
    int pwm_zuo = (int)PID_increment(target_zuo, speed_now_zuo, &zuo_motor);
    int pwm_you = (int)PID_increment(target_you, speed_now_you, &you_motor);

    SET_Motor_PWM(pwm_zuo, pwm_you);
}
	

/*角度环*/
void Angle_Control(float set_angle,int zuo_basic,int you_basic)
{
    PID_location(set_angle,angle[0]-start,&BMI_PID);
    SET_Motor_PWM(zuo_basic - BMI_PID.out,you_basic + BMI_PID.out);         
}


//void xunji_3(float set_angle,int zuo_basic,int you_basic)
//{
//	PID_location(set_angle,angle[0]-start+180,&BMI_PID);
//	SET_Motor_PWM(zuo_basic + BMI_PID.out,you_basic - BMI_PID.out);	     
//}



//void bai(void)
//{
//	SET_Motor_PWM(2000,0);
//	delay_ms(50);
//	SET_Motor_PWM(0,2000);
//	delay_ms(50);
//}





