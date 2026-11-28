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

void Test4(void)
{
    enum { AC = 10, CB, BD, DA, DONE, FAULT };
    /* Set both to the measured absolute counts for a forward 1 m roll. */
    float right_counts_per_m = 0.0f;
    float left_counts_per_m = 0.0f;
    const uint8_t distance_enabled = right_counts_per_m > 0.0f &&
                                     left_counts_per_m > 0.0f;
    static uint8_t laps, armed, last_black;
    static uint32_t segment_since, sample_at, color_since;
    static int32_t right_origin, left_origin;
    extern float start;
    uint32_t now = HAL_GetTick();
    uint8_t black, on_arc, near_exit;
    float cm = 0.0f;
    float error;
    int pwm;

    /* Key_Poll calls all_init(), which puts part back at zero. */
    if (part == 0) {
        laps = 0;
        part = AC;
        armed = 0;
        segment_since = color_since = now;
        sample_at = now - 10U;
        last_black = Gray_Read_Filter();
        right_origin = Encoder_GetCount();
        left_origin = Encoder2_GetCount();
    }
    if (part == DONE || part == FAULT) return;
    if ((uint32_t)(now - sample_at) < 10U) return;
    sample_at = now;

    black = Gray_Read_Filter();
    if (black != last_black) {
        last_black = black;
        color_since = now;
    }
    on_arc = (part == CB || part == DA);
    if (distance_enabled) {
        float right = (float)((int64_t)Encoder_GetCount() - right_origin);
        float left = (float)((int64_t)Encoder2_GetCount() - left_origin);
        if (right < 0.0f) right = -right;
        if (left < 0.0f) left = -left;
        cm = 50.0f * (right / right_counts_per_m + left / left_counts_per_m);
    }
    if ((uint32_t)(now - segment_since) >= 15000U ||
        (distance_enabled && cm > (on_arc ? 150.0f : 155.0f))) {
        goto fault;
    }

    if (!on_arc) {
        /* Ignore the departure endpoint until the sensors see clear floor. */
        if (!black && (uint32_t)(now - color_since) >= 30U) armed = 1;
        if (armed && black && (uint32_t)(now - color_since) >= 20U &&
            (!distance_enabled || cm >= 100.0f)) {
            part = (part == AC) ? CB : DA;
            aaa = 5;
            xunji_1(1500, 1500);
            goto next_segment;
        }
        aaa = (part == AC) ? 3 : 4;
        return;
    }

    /* The tangent toward the infield is west at B and east at A. */
    error = angle[0] - start - ((part == CB) ? 180.0f : 0.0f);
    while (error > 180.0f) error -= 360.0f;
    while (error < -180.0f) error += 360.0f;
    near_exit = (error >= -30.0f && error <= 30.0f) &&
                (!distance_enabled || cm >= 100.0f);
    if (!black) {
        if (near_exit && (uint32_t)(now - color_since) >= 30U) {
            if (part == DA) {
                if (++laps == 4U) {
                    part = DONE;
                    aaa = 0;
                    return;
                }
                part = AC;
                aaa = 3;
            } else {
                part = BD;
                aaa = 4;
            }
            goto next_segment;
        }
        if ((uint32_t)(now - color_since) >= 150U) goto fault;
    }
    pwm = (part == DA && laps == 3U && near_exit) ? 1250 : 1500;
    xunji_1(pwm, pwm);
    return;

next_segment:
    segment_since = color_since = now;
    right_origin = Encoder_GetCount();
    left_origin = Encoder2_GetCount();
    armed = 0;
    return;

fault:
    part = FAULT;
    aaa = 0;
}





