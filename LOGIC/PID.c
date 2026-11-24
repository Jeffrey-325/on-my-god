#include "pid.h"
extern float motion[6];
/************************定义PID结构体************************************/
PID_LocTypeDef kong = //空PID
{
        .kp = 0,
        .ki = 0,
        .kd = 0,
  	    .ek = 0,
        .ek1 = 0,
        .last_err_diff = 0,
        .location_sum = 0,
        .out = 0,
	      .limit_min = 0,
	      .limit_max = 0,
};

//PID_LocTypeDef zuo_motor = //左轮速度环PID
//{
//        .kp = 300,
//        .ki = 50,         
//        .kd = 0,
//        .ek = 0,
//        .ek1 = 0,
//        .last_err_diff = 0,
//        .location_sum = 0,
//        .out = 0,
//	      .limit_min = -8000,
//	      .limit_max = 8000,
//};
PID_LocTypeDef zuo_motor = //左轮速度环PID
{
        .kp = 85,
        .ki = 0,
        .kd = 0,
        .ek = 0,
        .ek1 = 0,
        .last_err_diff = 0,
        .location_sum = 0,
        .out = 0,
	      .limit_min = -6000,
	      .limit_max = 6000,
};

PID_LocTypeDef you_motor = //右轮速度环PID
{
        .kp = 85,
        .ki = 0,         
        .kd = 0,
        .ek = 0,
        .ek1 = 0,
        .last_err_diff = 0,
        .location_sum = 0,
        .out = 0,
	      .limit_min = -6000,
	      .limit_max = 6000,
};

PID_LocTypeDef Gray_PID = //位置环PID
{
        .kp = 420,
        .ki = 0,
        .kd = 2,
        .ek = 0,
        .ek1 = 0,
        .last_err_diff = 0,
        .location_sum = 0,
        .out = 0,
	      .limit_min = -6000,
	      .limit_max = 6000,
};
//PID_LocTypeDef BMI_PID = //角度PID
//{
//        .kp = 120,
//        .ki = 0,
//        .kd = 0,
//        .ek = 0,
//        .ek1 = 0,
//        .last_err_diff = 0,
//        .location_sum = 0,
//        .out = 0,
//	      .limit_min = -12000,
//	      .limit_max = 15000,
//};


PID_LocTypeDef BMI_PID = //角度PID
{
        .kp = 50,
        .ki = 0,
        .kd = 4,
        .ek = 0,
        .ek1 = 0,
        .last_err_diff = 0,
        .location_sum = 0,
        .out = 0,
	      .limit_min = -4000,
	      .limit_max = 4000,
};
/************************采样周期未知且不变************************************/
//***************位置式PID***************//
//pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
//setvalue : 设置值（期望值）
//actualvalue: 实际值
//由于全量输出，每次输出均与过去状态有关，计算时要对ek累加，计算量大
float PID_location(float setvalue, float actualvalue, PID_LocTypeDef* PID)
{
    // 1. 计算当前误差
    PID->ek = setvalue - actualvalue;
    // 2. 误差累加
    PID->location_sum += PID->ek;
    // 3. 计算 P、I、D 分项的输出
    float P_out = PID->kp * PID->ek;
    float I_out = PID->ki * PID->location_sum;
//    float D_out = PID->kd * (PID->ek - PID->ek1);
	float D_out = PID->kd *motion[5] ;
    // 4. 积分抗饱和处理
    if (PID->ki != 0)
    {
        if (I_out > PID->limit_max)
        {
            I_out = PID->limit_max; // 限制积分项输出
            PID->location_sum = PID->limit_max / PID->ki;
        }
        else if (I_out < PID->limit_min)
        {
            I_out = PID->limit_min;
            PID->location_sum = PID->limit_min / PID->ki;
        }
    }
    // 5. 计算总输出
    PID->out = P_out + I_out - D_out;
    // 6. 更新历史状态
    PID->ek1 = PID->ek;
    // 7. 总输出限幅
    if (PID->out < PID->limit_min)  
        PID->out = PID->limit_min;
    else if (PID->out > PID->limit_max)
        PID->out = PID->limit_max;
        
    return PID->out;
}

//***************增量式PID***************//
//pidout+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
//setvalue : 设置值（期望值）
//actualvalue: 实际值
float PID_increment(float setvalue, float actualvalue, PID_LocTypeDef* PID)
{
    // 1. 计算当前误差
    PID->ek = setvalue - actualvalue;
    // 2. 算出本次误差变化量
    float err_diff = PID->ek - PID->ek1;
    // 3. 计算 P、I、D 三个维度的增量
    float P_out = PID->kp * err_diff;
    float I_out = PID->ki * PID->ek;
    float D_out = PID->kd * (err_diff - PID->last_err_diff);
    // 4. 累加到最终输出
    PID->out += (P_out + I_out + D_out);
    // 5. 更新历史状态：把你这次算出来的 err_diff 存入结构体，留给下一次用
    PID->last_err_diff = err_diff; 
    PID->ek1 = PID->ek;
    // 6. 输出限幅
    if (PID->out < PID->limit_min)  
        PID->out = PID->limit_min;
    else if (PID->out > PID->limit_max)  
        PID->out = PID->limit_max;
        
    return PID->out;
}










