/**
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include "imu_bsp.h"

/******************************************************************************/
// 宏定义

#define GRAVITY_EARTH  (9.80665f)
#define GRAVITY_MG     (1000.0f)

/*******************************************************************************/
// 全局变量定义

struct bmm150_dev bmm;
struct bmi2_dev bmi;
struct bmi2_sens_data sens_data = {{0}};

/*******************************************************************************/
/*!               Static  Functions Definitions                                */

static void bmi2_accel_gyro_conf(struct bmi2_dev *bmi)
{
    int8_t rslt;
    struct bmi2_sens_config config[2];

    config[0].type = BMI2_ACCEL;
    config[1].type = BMI2_GYRO;

    rslt = bmi2_get_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);

    config[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;
    config[0].cfg.acc.bwp = BMI2_ACC_OSR2_AVG2;
    config[0].cfg.acc.odr = BMI2_ACC_ODR_100HZ;
    config[0].cfg.acc.range = BMI2_ACC_RANGE_4G;

    config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
    config[1].cfg.gyr.noise_perf = BMI2_GYR_RANGE_1000;
    config[1].cfg.gyr.bwp = BMI2_GYR_OSR2_MODE;
    config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
    config[1].cfg.gyr.range = BMI2_GYR_RANGE_1000;
    config[1].cfg.gyr.ois_range = BMI2_GYR_OIS_2000;

    rslt = bmi2_set_sensor_config(config, 2, bmi);
    bmi2_error_codes_print_result(rslt);
}

static void bmi2_aux_conf(uint8_t mag_odr, struct bmm150_dev *bmm, struct bmi2_dev *bmi)
{
    int8_t rslt;
    struct bmi2_sens_config bmi2_conf;
    struct bmm150_settings bmm1_sett;
    uint8_t bmi2_sens = BMI2_AUX;

    switch (mag_odr)
    {
        case BMI2_AUX_ODR_0_78HZ:
        case BMI2_AUX_ODR_1_56HZ:
        case BMI2_AUX_ODR_3_12HZ:
        case BMI2_AUX_ODR_6_25HZ:
        case BMI2_AUX_ODR_12_5HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_HIGHACCURACY;
            break;

        case BMI2_AUX_ODR_25HZ:
        case BMI2_AUX_ODR_50HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_ENHANCED;
            break;

        case BMI2_AUX_ODR_100HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_REGULAR;
            break;

        case BMI2_AUX_ODR_200HZ:
        case BMI2_AUX_ODR_400HZ:
        case BMI2_AUX_ODR_800HZ:
            bmm1_sett.preset_mode = BMM150_PRESETMODE_LOWPOWER;
            break;

        default:
            mag_odr = 0;
            printf("Error : Unsupported Auxiliary Magnetometer ODR : %u\n", mag_odr);
            break;
    }

    bmi2_conf.type = BMI2_AUX;
    bmi2_conf.cfg.aux.odr = mag_odr;
    bmi2_conf.cfg.aux.aux_en = BMI2_ENABLE;
    bmi2_conf.cfg.aux.i2c_device_addr = BMM150_DEFAULT_I2C_ADDRESS;
    bmi2_conf.cfg.aux.fcu_write_en = BMI2_ENABLE;
    bmi2_conf.cfg.aux.manual_en = BMI2_ENABLE;
    bmi2_conf.cfg.aux.aux_rd_burst = BMI2_AUX_READ_LEN_3;
    bmi2_conf.cfg.aux.man_rd_burst = BMI2_AUX_READ_LEN_3;
    bmi2_conf.cfg.aux.read_addr = BMM150_REG_DATA_X_LSB;

    rslt = bmi2_set_sensor_config(&bmi2_conf, 1, bmi);
    bmi2_error_codes_print_result(rslt);

    bmm1_sett.pwr_mode = BMM150_POWERMODE_FORCED;

    if (mag_odr != 0)
    {
        rslt = bmm150_init(bmm);
        bmm150_error_codes_print_result("bmm150_init", rslt);

        rslt = bmm150_set_presetmode(&bmm1_sett, bmm);
        bmm150_error_codes_print_result("bmm150_set_presetmode", rslt);

        rslt = bmm150_set_op_mode(&bmm1_sett, bmm);
        bmm150_error_codes_print_result("bmm150_set_op_mode", rslt);

        bmi2_conf.cfg.aux.manual_en = BMI2_DISABLE;
        rslt = bmi2_set_sensor_config(&bmi2_conf, 1, bmi);
        bmi2_error_codes_print_result(rslt);
    }
    else
    {
        bmi2_conf.cfg.aux.aux_en = BMI2_DISABLE;
        bmi2_conf.cfg.aux.fcu_write_en = BMI2_DISABLE;
        bmi2_conf.cfg.aux.manual_en = BMI2_ENABLE;

        rslt = bmi2_set_sensor_config(&bmi2_conf, 1, bmi);
        bmi2_error_codes_print_result(rslt);

        rslt = bmi2_sensor_disable(&bmi2_sens, 1, bmi);
        bmi2_error_codes_print_result(rslt);
    }
}

static float lsb_to_mg(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;
    float half_scale = (float)(pow((double)power, (double)bit_width) / 2.0f);

    return (GRAVITY_MG * val * g_range) / half_scale;
}

//static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width) 
//{ 
//	 double power = 2; 
//	 float half_scale = (float)(pow((double)power, (double)bit_width) / 2.0f); 
//	 return (GRAVITY_EARTH * val * g_range) / half_scale; 
//}

static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;
    float half_scale = (float)(pow((double)power, (double)bit_width) / 2.0f);

    return (dps / half_scale) * val;
}

static int8_t bmi2_aux_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int8_t rslt;
    struct bmi2_dev *dev = (struct bmi2_dev *)intf_ptr;

    rslt = bmi2_read_aux_man_mode(reg_addr, reg_data, (uint16_t)length, dev);

    return rslt;
}

static int8_t bmi2_aux_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int8_t rslt;
    struct bmi2_dev *dev = (struct bmi2_dev *)intf_ptr;

    rslt = bmi2_write_aux_man_mode(reg_addr, reg_data, (uint16_t)length, dev);

    return rslt;
}

static void bmi2_aux_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    delay_us(period);
}

/*******************************************************************************/
/*!                             Public Functions                               */

int8_t bmi2_aux_interface_init(struct bmm150_dev *bmm, struct bmi2_dev *bmi)
{
    int8_t rslt = BMI2_OK;

    if (bmm != NULL)
    {
        bmm->intf_ptr = bmi;
        bmm->read = bmi2_aux_i2c_read;
        bmm->write = bmi2_aux_i2c_write;
        bmm->delay_us = bmi2_aux_delay_us;
        bmm->intf = BMM150_I2C_INTF;
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

void bmm150_error_codes_print_result(const char api_name[], int8_t rslt)
{
    if (rslt != BMM150_OK)
    {
        printf("%s\t", api_name);

        switch (rslt)
        {
            case BMM150_E_NULL_PTR:
                printf("Error [%d] : Null pointer error.", rslt);
                printf("It occurs when the user tries to assign value (not address) to a pointer, "
                       "which has been initialized to NULL.\r\n");
                break;

            case BMM150_E_COM_FAIL:
                printf("Error [%d] : Communication failure error.", rslt);
                printf("It occurs due to read/write operation failure and also due to power failure during communication\r\n");
                break;

            case BMM150_E_DEV_NOT_FOUND:
                printf("Error [%d] : Device not found error. It occurs when the device chip id is incorrectly read\r\n",
                       rslt);
                break;

            case BMM150_E_INVALID_CONFIG:
                printf("Error [%d] : Invalid sensor configuration.", rslt);
                printf(" It occurs when there is a mismatch in the requested feature with the available one\r\n");
                break;

            default:
                printf("Error [%d] : Unknown error code\r\n", rslt);
                break;
        }
    }
}

int8_t bsp_BmiGetRawData(float accel_mg[3], float gyro_dps[3], float *temp_degc)
{
    int8_t rslt;

    accel_mg[0] = accel_mg[1] = accel_mg[2] = 0.0f;
    gyro_dps[0] = gyro_dps[1] = gyro_dps[2] = 0.0f;

    if (temp_degc)
    {
        *temp_degc = 0.0f;
    }

    rslt = bmi2_get_sensor_data(&sens_data, &bmi);
    bmi2_error_codes_print_result(rslt);

    if ((rslt == BMI2_OK) &&
        (sens_data.status & BMI2_DRDY_ACC) &&
        (sens_data.status & BMI2_DRDY_GYR))
    {
        accel_mg[0] = lsb_to_mg(sens_data.acc.x, 4.0f, bmi.resolution);
        accel_mg[1] = lsb_to_mg(sens_data.acc.y, 4.0f, bmi.resolution);
        accel_mg[2] = lsb_to_mg(sens_data.acc.z, 4.0f, bmi.resolution);

        gyro_dps[0] = lsb_to_dps(sens_data.gyr.x, 1000.0f, bmi.resolution);
        gyro_dps[1] = lsb_to_dps(sens_data.gyr.y, 1000.0f, bmi.resolution);
        gyro_dps[2] = lsb_to_dps(sens_data.gyr.z, 1000.0f, bmi.resolution);

        return 0;
    }

    return -1;
}

void IMU_INIT(uint8_t intf)     //bmi270_ 为 BMI2_SPI_INTF 时使用SPI通信，为 BMI2_I2C_INTF 时为I2C通信
{
    int8_t rslt;
    uint8_t regdata;
    uint8_t sensor_list[3] = {BMI2_ACCEL, BMI2_GYRO, BMI2_AUX};

    rslt = bmi2_interface_init(&bmi, intf);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_aux_interface_init(&bmm, &bmi);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi270_init(&bmi);
    bmi2_error_codes_print_result(rslt);

    regdata = BMI2_ASDA_PUPSEL_10K;
    rslt = bmi2_set_regs(BMI2_AUX_IF_TRIM, &regdata, 1, &bmi);
    bmi2_error_codes_print_result(rslt);

    bmi2_accel_gyro_conf(&bmi);
    bmi2_aux_conf(BMI2_AUX_ODR_100HZ, &bmm, &bmi);

    rslt = bmi2_sensor_enable(sensor_list, 3, &bmi);
    bmi2_error_codes_print_result(rslt);

    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, &bmi);
    bmi2_error_codes_print_result(rslt);

    /* Initialization banners disabled; retain sensor initialization. */

    IMU_init();


}

//    float ypr[3];

void IMU_Update(float* ypr)
{
    int8_t rslt;
    struct bmm150_mag_data mag_data;
    IMU_getYawPitchRoll(ypr);
//    printf("\tangle:%.2f\t%.2f\t%.2f\r\n", ypr[0], ypr[1], ypr[2]);

    if (bmm.chip_id == BMM150_CHIP_ID)
    {
        rslt = bmm150_aux_mag_data(sens_data.aux_data, &mag_data, &bmm);
        bmm150_error_codes_print_result("bmm150_aux_mag_data", rslt);
    }
}
























