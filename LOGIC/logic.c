#include "main.h"
#include "motor.h"
#include "pid.h"
#include "imu_bsp.h"
#include "bsp_key.h"
#include "delay.h"
#include "ALLNeed.h"


int dates_clear(int x)     //数据清零，跳到下一个case
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_RESET);//光
	you_bianma = 0;
	zuo_bianma = 0;
	return x;
	
}


void xunxian(int x)					//滤波巡线
{
	static int lost_count = 0;    //滤波变量
			if(  Out_3 == 0 && Out_4 == 0 && Out_5 == 0 && Out_6 == 0  )//识别到一次丢线
			{ 
				lost_count++;    
			}
			else
			{
				lost_count = 0;
			}
			
			
			if(lost_count<50)     //按原来继续巡线
				{
					xunji_1(2400,2400);	   
				}
			else									//确实出线  停车
				{ SET_Motor_PWM(0,0);
					lost_count = 0;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);//声
					delay_ms(500);
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
					delay_ms(1500);
					way = dates_clear(x);}
}






//第一题
void test_1(void)
{
//	static int lost_count = 0;
	static int way = 0;
switch (way)
{
	case 0:
		
//	xunji_2(0,11700,11900);
//	if(Out_3 == 1||Out_4 == 1||Out_5 == 1||Out_6 == 1)
	aaa=1;
//	if(a >= 27)
	
	if(  Out_3 == 1 || Out_4 == 1 || Out_5 == 1 || Out_6 == 1  )
				{ aaa = 3;
					SET_Motor_PWM(0,0);
					way = dates_clear(1);}
	break;
	
	case 1:
		
	SET_Motor_PWM(0,0);
	aaa = 3;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);//声
	delay_ms(500);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	
	way = dates_clear(2);
	
	break;}
	
}




//第二题
void test_2(void)
{
	
	switch (way)
	{		
		case 0:		//走直线
			aaa=1;
//	xunji_2(0,11700,11900);
	if(Out_3 == 1||Out_4 == 1||Out_5 == 1||Out_6 == 1)
//	if(a >= 25)				//走到距离了
	{	way = dates_clear(1);
	 aaa=3;}
	
	break;
	
	case 1:					//停会儿车，亮灯，发声
	SET_Motor_PWM(0,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(500);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	way = dates_clear(2);
	break;
		
		case 2:					//巡弧线
			xunji_1(2400,2400);			
			xunxian(3);
		break;
	
	
		case 3:					//
     aaa=2;
//	xunji_2(180,11900,11600);
//	if(Out_3 == 1||Out_4 == 1||Out_5 == 1||Out_6 == 1)
//	if(a >= 29)
		if(  Out_3 == 1 || Out_4 == 1 || Out_5 == 1 || Out_6 == 1  )
	{way = dates_clear(4);
	 aaa=3;}
	
	break;
	
	case 4:
	SET_Motor_PWM(0,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声	
	delay_ms(500);
HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	way = dates_clear(5);
	break;
		
		
		case 5:					//巡弧线
			xunji_1(2400,2400);			
			xunxian(6);
				
		break;
			
}			
		
}			


//第三题
void test_3(void)
{
	
	switch (way)
		{
		case 0:
//			SET_Motor_PWM(15000,10000);
		  aaa=4;
		
//			xunji_2(28.37,11900,11600);//a点到c点
		if(a >= 33){	way = dates_clear(1);  aaa=3;}
	break;
		
		case 1:
		SET_Motor_PWM(0,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(500);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	way = dates_clear(2);
	break;
		
			case 2:
			xunji_1(2400,2400);
			xunxian(3);
		break;
		
		case 3:
//			SET_Motor_PWM(10000,15000);//原地转弯
//			xunji_2(28.37,11900,11600);//b点到d点
		aaa=6;
		if(angle[0]-start>=77){aaa=6;}
		if(a >= 33){	way = dates_clear(4);aaa=3;}
		
		break;
		
		case 4:
		SET_Motor_PWM(0,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(500);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	way = dates_clear(5);
	break;
		
		case 5:
			xunji_1(2400,2400);
			xunxian(6);
		break;
		
	}
}



//第四题
void test_4(void)
{
	
	static int i=0;
	switch (way)
		{
		case 0:
			i++;
//			SET_Motor_PWM(15000,10000);//原地转弯
//			xunji_2(28.37,11900,11600);//a点到c点
		aaa=4;
		if(a >= 30){	way = dates_clear(1);}
	break;
		
		case 1:
		SET_Motor_PWM(0,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(500);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	way = dates_clear(2);
	break;
		
			case 2:
			xunji_1(2400,2400);
			xunxian(3);
		break;
		
		case 3:
//			SET_Motor_PWM(10000,15000);//原地转弯
//			xunji_2(28.37,11900,11600);//b点到d点
		aaa=5;
		if(a >= 30){	way = dates_clear(4);}
		
		break;
		
		case 4:
		SET_Motor_PWM(0,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//光
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(500);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);//声
	delay_ms(1500);
	way = dates_clear(5);
	break;
		
		case 5:
			xunji_1(2400,2400);
			xunxian(6);
		break;
		
		case 6:
		if(i>=4){ way = dates_clear(7);}
		break;
	}
}






