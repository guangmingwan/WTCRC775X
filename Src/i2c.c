#include "i2c.h"

static void I2C_Delay(void)
{
	uint16_t cnt = 40;
	while (cnt--);
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
		// ѭ��������ʱ��SCL�ض���LOW����ȥ��ʱ��Ҳ��
		(m & Data) ? I2C_SDA_HIGH : I2C_SDA_LOW;
		I2C_Delay();
		I2C_SCL_HIGH;
		I2C_Delay();
		I2C_SCL_LOW;
	}
	I2C_Delay();
	I2C_SDA_HIGH;// �ͷ����ߣ�Ϊ�˽���ACK
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
	I2C_SDA_HIGH;// �ͷ����ߣ���������
	for (uint8_t count = 0; count < 8; count++)
	{
		// ѭ��������ʱ��SCL�ض���LOW����ȥ��ʱ��Ҳ��
		b <<= 1;
		I2C_Delay();
		I2C_SCL_HIGH;
		while (!I2C_SCL_READ); // Ӧ��I2C Clock Stretching
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
