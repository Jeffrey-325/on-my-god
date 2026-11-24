#ifndef __PID_H
#define __PID_H

//***************结构体定义***************//
typedef struct
{
  float kp;               //比例系数
  float ki;               //积分系数
  float kd;               //微分系数
  float ek;               //当前误差
  float ek1;              //前一次误差e(k-1)
  float last_err_diff;    //两次误差差值
  float location_sum;     //累计积分位置
  float out;              //PID输出值
	float limit_min;        //PID输出最小值
	float limit_max;        //PID输出最大值
}PID_LocTypeDef;


//***************位置式式***************//
float PID_location(float setvalue, float actualvalue, PID_LocTypeDef* PID);
//***************增量式***************//
float PID_increment(float setvalue, float actualvalue, PID_LocTypeDef* PID);

//***************定义PID结构体***************//
extern PID_LocTypeDef kong;
extern PID_LocTypeDef zuo_motor;
extern PID_LocTypeDef you_motor;
extern PID_LocTypeDef Gray_PID;
extern PID_LocTypeDef BMI_PID;


#endif








