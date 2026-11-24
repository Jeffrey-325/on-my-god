#include "bmi270_iic.h"
#include "delay.h"

// IIC初始化
void IIC_Init(void)
{
    IIC_SDA_H();
    IIC_SCL_H();
}

// 产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA_H();
    IIC_SCL_H();
    delay_us(4);
    IIC_SDA_L();
    delay_us(4);
    IIC_SCL_L();
}

// 产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL_L();
    IIC_SDA_L();
    delay_us(4);
    IIC_SCL_H();
    IIC_SDA_H();
    delay_us(4);
}

// 等待应答信号到来
// 返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;

    SDA_IN();
    delay_us(1);
    IIC_SCL_H();
    delay_us(1);

    while (READ_SDA() == GPIO_PIN_SET)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
        delay_us(1);
    }

    IIC_SCL_L();
    return 0;
}

// 产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL_L();
    SDA_OUT();
    IIC_SDA_L();
    delay_us(2);
    IIC_SCL_H();
    delay_us(2);
    IIC_SCL_L();
}

// 不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL_L();
    SDA_OUT();
    IIC_SDA_H();
    delay_us(2);
    IIC_SCL_H();
    delay_us(2);
    IIC_SCL_L();
}

// IIC发送一个字节
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;

    SDA_OUT();
    IIC_SCL_L();

    for (t = 0; t < 8; t++)
    {
        if (txd & 0x80)
            IIC_SDA_H();
        else
            IIC_SDA_L();

        txd <<= 1;

        delay_us(2);
        IIC_SCL_H();
        delay_us(2);
        IIC_SCL_L();
        delay_us(2);
    }
}

// 读1个字节，ack=1时发送ACK，ack=0时发送NACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
    uint8_t i, receive = 0;

    SDA_IN();   // SDA设置为输入

    for (i = 0; i < 8; i++)
    {
        IIC_SCL_L();
        delay_us(2);
        IIC_SCL_H();

        receive <<= 1;
        if (READ_SDA() == GPIO_PIN_SET)
            receive++;

        delay_us(1);
    }

    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();

    return receive;
}

uint8_t IIC_Write_1Byte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)
{
    IIC_Start();

    IIC_Send_Byte((SlaveAddress << 1) | 0);   // 发送器件地址+写命令
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(REG_Address);               // 写寄存器地址
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(REG_data);                  // 发送数据
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    IIC_Stop();
    return 0;
}

uint8_t IIC_Read_1Byte(uint8_t SlaveAddress, uint8_t REG_Address)
{
    uint8_t data;

    IIC_Start();
    IIC_Send_Byte((SlaveAddress << 1) | 0);   // 发送器件地址+写命令
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 0;
    }

    IIC_Send_Byte(REG_Address);               // 写寄存器地址
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 0;
    }

    IIC_Start();
    IIC_Send_Byte((SlaveAddress << 1) | 1);   // 发送器件地址+读命令
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 0;
    }

    data = IIC_Read_Byte(0);                  // 读取数据，发送NACK
    IIC_Stop();
    return data;
}

// IIC写n字节数据
uint8_t IIC_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint16_t len, const uint8_t *buf)
{
    IIC_Start();

    IIC_Send_Byte(SlaveAddress << 1);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(REG_Address);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    while (len--)
    {
        IIC_Send_Byte(*buf++);
        if (IIC_Wait_Ack())
        {
            IIC_Stop();
            return 1;
        }
    }

    IIC_Stop();
    return 0;
}

// IIC读n字节数据
uint8_t IIC_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint16_t len, uint8_t *buf)
{
    IIC_Start();

    IIC_Send_Byte(SlaveAddress << 1);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(REG_Address);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    IIC_Start();

    IIC_Send_Byte((SlaveAddress << 1) | 0x01);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }

    while (len)
    {
        if (len == 1)
            *buf = IIC_Read_Byte(0);   // 最后一个字节发送NACK
        else
            *buf = IIC_Read_Byte(1);   // 前面的字节发送ACK

        buf++;
        len--;
    }

    IIC_Stop();
    return 0;
}




