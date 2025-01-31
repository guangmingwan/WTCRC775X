#include "oled.h"
#include "ui.h"
#include "global.h"
#include "tuner.h"
#include <string.h>
#include "soft_i2c.h"
#include "nv_memory.h"
#include <math.h>
extern TIM_HandleTypeDef htim3;

void OLED_SetBackLight(uint8_t Data)
{
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Data);
}

void LCDEN(void)  // Generate LCD EN pulse
{
	//HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET);
	//Delay(2);
	//HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_RESET);
}

void LCDCmd(uint8_t Cmd)  // Write LCD command
{
	
}

void LCDData(uint8_t Data)  // Write LCD data
{
	
}

void OLED_Clear(void)
{
	LCDCmd(0x01);  // Clear LCD
	HAL_Delay(5);
}

void LCDClr1(void)
{
	OLED_XYStr(0, 1, "                ");  // Clear LCD's 2nd row
}

void OLED_Display_On(void)
{
	if (nBacklightKeep && bLCDOff)
	{
		OLED_SetBackLight(nBacklightAdj);
		bLCDOff = false;
	}
	nBacklightTimer = HAL_GetTick();
}
void OLED_Refresh(void) {
}
void OLED_Init(void)
{
	LCDCmd(0x28);  // 4 bits interface
	HAL_Delay(5);
	OLED_SetBackLight(255);
	LCDCmd(0x28);
	HAL_Delay(5);

	LCDCmd(0x28);
	HAL_Delay(5);

	LCDCmd(0x08);  // LCD off
	Delay(50);
	OLED_Clear();      // Clear LCD
	LCDCmd(0x06);  // Auto increment
	Delay(50);
	LCDCmd(0x0c);  // LCD on
	HAL_Delay(15);
}  // void OLED_Init(void)

void LCDXY(uint8_t x, uint8_t y)  // x:0-15, y:0-1
{
	LCDCmd(0x80 + y * 0x40 + x);
}

void OLED_XYChar(uint8_t x, uint8_t y, const char c)  // Display char at x:0-15, y:0-1
{
	LCDXY(x, y);
	LCDData(c);
}

void OLED_XYStr(uint8_t x, uint8_t y, const char *str)  // Display string at x:0-15, y:0-1
{
	const char *p;
	char c;

	LCDXY(x, y);
	p = str;
	while ((c = *p++) != false)
		LCDData(c);
}

void OLED_FullStr(const char *str)  // Display string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars
{
	const char *p;
	char c;
	uint8_t x, y;
	uint8_t bInStr;

	p = str;
	bInStr = 1;
	for (y = 0; y <= 1; y++)
	{
		for (x = 0; x <= 15; x++)
		{
			if (!x)
				LCDXY(0, y);

			if (bInStr)
			{
				c = *(p++);
				if (!c)
				{
					c = ' ';
					bInStr = 0;
				}
			}
			else
				c = ' ';

			LCDData(c);
		}
	}
}  // void LCDFullStr_P(PGM_P str)  // Display PROGMEM string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars

   // Display string at x:0-15, y:0-1
void OLED_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign)
{
	char *p, c;
	uint8_t i, n;

	LCDXY(x, y);
	p = (char *)str;
	n = nLen - strlen(str);
	if (!bLeftAlign)
	{
		for (i = 0; i < n; i++)
			LCDData(' ');
	}

	while ((c = *p++) != false)
		LCDData(c);

	if (bLeftAlign)
	{
		for (i = 0; i < n; i++)
			LCDData(' ');
	}
}  // void LCDXYStrLen(uint8_t x, uint8_t y, char *str, uint8_t nLen, bool bLeftAlign)

   // Display signed integer at x:0-15, y:0-1. Righ aligned, spaces filled at left. Too long integers are not allowed.
void OLED_XYIntLen(uint8_t x, uint8_t y, int32_t n, uint8_t nLen)
{
	int8_t i;
	int32_t r;

	if (n >= 0)
		r = n;
	else
		r = -n;

	i = nLen - 1;
	do
	{
		OLED_XYChar(x + i--, y, '0' + (r % 10));
		r = r / 10;
	} while (r);

	if (n < 0)
		OLED_XYChar(x + i--, y, '-');

	while (i >= 0)
		OLED_XYChar(x + i--, y, ' ');
}

// Display unsigned integer at x:0-15, y:0-1. Righ aligned, '0' filled at left. Too long unsigned integers are not allowed.
void OLED_XYUIntLenZP(uint8_t x, uint8_t y, uint32_t n, uint8_t nLen)
{
	int8_t i;
	uint32_t r;
	r = n;
	i = nLen - 1;
	do
	{
		OLED_XYChar(x + i--, y, '0' + (r % 10));
		r = r / 10;
	} while (r);

	while (i >= 0)
		OLED_XYChar(x + i--, y, '0');
}
void OLED_Clear1(void)
{
	OLED_XYStr(0, 1, "                ");  // Clear LCD's 2nd row
}
void OLED_Clear2(void)
{
	OLED_XYStr(0, 2, "                ");  // Clear LCD's 2nd row
}
void clear_screen(void) {
	OLED_Clear();
}