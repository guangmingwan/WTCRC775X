#ifndef __LCD19264_H
#define __LCD19264_H
/****************************************************************/
#ifdef LCD25696
#include "main.h"
#include "font.h"
#ifdef STM32F405xx
#include "stm32f4xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif
#include <stdbool.h>
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	256
#define Y_WIDTH 	96	 
 

#define CS_PIN_LOW() 	HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_RESET)
#define CS_PIN_HIGH() 	HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_SET)
#define DC_PIN_LOW() 	HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_RESET)
#define DC_PIN_HIGH() 	HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_SET)

#define RST_PIN_LOW() 	HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_RESET)
#define RST_PIN_HIGH() 	HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_SET)
#define SCK_PIN_LOW() 	HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, GPIO_PIN_RESET)
#define SCK_PIN_HIGH()	HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, GPIO_PIN_SET)
#define SDA_PIN_LOW() 	HAL_GPIO_WritePin(OLED_SDOUT_GPIO_Port, OLED_SDOUT_Pin, GPIO_PIN_RESET)
#define SDA_PIN_HIGH() 	HAL_GPIO_WritePin(OLED_SDOUT_GPIO_Port, OLED_SDOUT_Pin, GPIO_PIN_SET)

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, GPIO_PIN_RESET)//CLK 
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDOUT_GPIO_Port, OLED_SDOUT_Pin, GPIO_PIN_RESET)//SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDOUT_GPIO_Port, OLED_SDOUT_Pin, GPIO_PIN_SET)

#define OLED_RES_Clr()  HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_RESET)
#define OLED_RES_Set()  HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_SET)

//****************数据/命令***************

#define OLED_DC_Clr()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_Set()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_RESET)

//****************片选*********************

#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_SET)


#define LCD_CMD  0	//写命令
#define LCD_DATA 1	//写数据
//OLED控制用函数
void clear_screen();
void LCD_WR_Byte(u8 dat,u8 cmd);	    
void LCD_Display_On(void);
void LCD_Display_Off(void);

void LCD_XY(uint8_t x, uint8_t y);
void LCD_XYChar(uint8_t x, uint8_t y, const char c);
void LCD_XYStr(uint8_t x, uint8_t y, const char *str);
void LCD_FullStr(const char *str);
void LCD_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign);
void LCD_XYIntLen(uint8_t x, uint8_t y, int32_t n, uint8_t nLen);
void LCD_XYUIntLenZP(uint8_t x, uint8_t y, uint32_t n, uint8_t nLen);

void LCD_Clear(void);
void LCD_Clear0(void);
void LCD_Clear1(void);
void LCD_Clear2(void);
void LCD_DrawPoint(u8 x,u8 y,u8 t);
void LCD_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void LCD_ShowChar(u8 x,u8 y,u8 chr);
void LCD_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void LCD_ShowString(u8 x,u8 y, u8 *p);	 

void LCD_ShowCHinese(u8 x,u8 y,u8 no);
void LCD_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void LCD_SetBackLight(uint8_t Data);
void LCD_FullStr(const char *str);
void LCD_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign);


void LCD_Init(void);
void DispFill(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color);
void DispPic(uint8_t page, uint8_t column, uint8_t x ,uint8_t y ,const unsigned char *dp);
void DispChar(uint8_t page, uint8_t column, char ascii, sFONT* font);
 

void DispBacklight(uint8_t lumi);
void DispContrast(uint8_t contrast);
void DispBias(uint8_t bias);
void DispFrame(uint8_t fps);
void DispInverse(bool inv);
void DispPower(uint8_t mode);
unsigned int reverse_bit(unsigned char n);
void disp_256x96(int x,int y,char *dp);
void lcd_address_old(uint8_t page,uint8_t column);
unsigned int reverse_bit(unsigned char n);
void LCDOn(void);
/****************************************************************/

#endif
#endif
