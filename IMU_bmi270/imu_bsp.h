#ifndef __IMU_BSP_H
#define __IMU_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "delay.h"
#include "common.h"
#include "bmi270.h"
#include "bmm150.h"
#include "imu.h"
#include "IMU.h"

/* 传感器全局对象 */
extern struct bmm150_dev bmm;
extern struct bmi2_dev bmi;
extern struct bmi2_sens_data sens_data;

/* 对外接口 */
int8_t bsp_BmiGetRawData(float accel_mg[3], float gyro_dps[3], float *temp_degc);

int8_t bmi2_aux_interface_init(struct bmm150_dev *bmm, struct bmi2_dev *bmi);
void bmm150_error_codes_print_result(const char api_name[], int8_t rslt);

void IMU_INIT(uint8_t intf);
void IMU_Update(float* ypr);

#ifdef __cplusplus
}
#endif

#endif
