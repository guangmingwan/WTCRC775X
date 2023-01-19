#include "soft_i2c.h"
void HAL_Delay_us(uint32_t nus)
{
	//将systic设置为1us中断
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000000);
	//延时nus
	HAL_Delay(nus-1);
	//恢复systic中断为1ms
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
}

static void I2C_Delay(void)
{
	uint8_t i;
	uint8_t delay_type = 1;
	if(delay_type ==0) {
		HAL_Delay_us(1);
	}
	else if(delay_type == 1) {
		uint16_t cnt = 40;
		while (cnt--);
	}
	else if(delay_type == 2) {
    /*　
        下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化

        循环次数为10时，SCL频率 = 205KHz 
        循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
        循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 

        上面的意思是SCL的频率是变化的。因为I2C要求，delay（）延时超过4us。即scl频率低于250Khz。
      上面的处理大抵是符合的。
    */
    for (i = 0; i <= 20; i++);
	}
}

bool I2C_Start(uint8_t addr)
{
	I2C_SDA_LOW;
	I2C_Delay();
	I2C_SCL_LOW;
	return I2C_WriteByte(addr);
}

bool I2C_Restart(uint8_t addr)
{
	I2C_Delay();
	I2C_SDA_HIGH;
	I2C_Delay();
	I2C_SCL_HIGH;
	I2C_Delay();
	return I2C_Start(addr);
}

void I2C_Stop(void)
{
	I2C_SDA_LOW;
	I2C_Delay();
	I2C_SCL_HIGH;
	I2C_Delay();
	I2C_SDA_HIGH;
	I2C_Delay();
}

bool I2C_WriteByte(uint8_t Data)
{
	for (uint8_t m = 0x80; m != 0; m >>= 1)
	{
		// 循环进来的时候SCL必定是LOW，出去的时候也是
		(m & Data) ? I2C_SDA_HIGH : I2C_SDA_LOW;
		I2C_Delay();
		I2C_SCL_HIGH;
		I2C_Delay();
		I2C_SCL_LOW;
	}
	I2C_Delay();
	I2C_SDA_HIGH;// 释放总线，为了接收ACK
	uint8_t rtn = I2C_SDA_READ;
	I2C_SCL_HIGH;
	I2C_Delay();
	I2C_SCL_LOW;
	I2C_SDA_LOW;
	return rtn == 0;
}

uint8_t I2C_ReadByte(bool last)
{
	uint8_t b = 0;
	I2C_SDA_HIGH;// 释放总线，接收数据
	for (uint8_t count = 0; count < 8; count++)
	{
		// 循环进来的时候SCL必定是LOW，出去的时候也是
		b <<= 1;
		I2C_Delay();
		I2C_SCL_HIGH;
		while (!I2C_SCL_READ); // 应对I2C Clock Stretching
		if (I2C_SDA_READ) b |= 0x01;
		I2C_Delay();
		I2C_SCL_LOW;
	}
	(last) ? I2C_SDA_HIGH : I2C_SDA_LOW;
	I2C_Delay();
	I2C_SCL_HIGH;
	I2C_Delay();
	I2C_SCL_LOW;
	I2C_SDA_LOW;
	return b;
}
