#ifndef __OLED_H
#define __OLED_H 

#include "main.h"
#include "stdlib.h"	
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
#define X_WIDTH 	128
#define Y_WIDTH 	64	 

//-----------------OLED??????---------------- 

//#define OLED_SCL_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_0)//SCL
//#define OLED_SCL_Set() GPIO_SetBits(GPIOA,GPIO_Pin_0)

//#define OLED_SDA_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_1)//SDA
//#define OLED_SDA_Set() GPIO_SetBits(GPIOA,GPIO_Pin_1)

//#define OLED_RES_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_2)//RES
//#define OLED_RES_Set() GPIO_SetBits(GPIOA,GPIO_Pin_2)

//#define OLED_DC_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_3)//DC
//#define OLED_DC_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_3)
// 		     
//#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)//CS
//#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_4)
//****************???*********************

#define OLED_SCL_Clr() HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, RESET)//CLK
#define OLED_SCL_Set() HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, SET)

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, RESET)//CLK
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_GPIO_Port, OLED_SCLK_Pin, SET)

#define OLED_SDA_Clr() HAL_GPIO_WritePin(OLED_SDOUT_GPIO_Port, OLED_SDOUT_Pin, RESET)//SDA
#define OLED_SDA_Set() HAL_GPIO_WritePin(OLED_SDOUT_GPIO_Port, OLED_SDOUT_Pin, SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_GPIO_Port, OLED_SDIN_Pin, RESET)//DIN
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_GPIO_Port, OLED_SDIN_Pin, SET)

#define OLED_RES_Clr()  HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_RESET)
#define OLED_RES_Set()  HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin, GPIO_PIN_SET)

//****************????/????***************

#define OLED_DC_Clr()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_Set()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin, GPIO_PIN_SET)

//****************??*********************

#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_CMD  0	//?????
#define OLED_DATA 1	//?????


//OLED?????ú???
void OLED_Refresh();	   
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);


void OLED_XYChar(uint8_t x, uint8_t y, const char c);
void OLED_XYStr(uint8_t col, uint8_t line, const char *str);
void OLED_FullStr(const char *str);
void OLED_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign);
void OLED_XYIntLen(uint8_t x, uint8_t y, int32_t n, uint8_t nLen);
void OLED_XYUIntLenZP(uint8_t x, uint8_t y, uint32_t n, uint8_t nLen);

void clear_screen();
u32 OLED_Pow(u8 m,u8 n);
void OLED_SOFT_WR_Byte(u8 dat,u8 cmd);
void OLED_HAL_WR_Byte(u8 dat,u8 cmd);
void OLED_Clear(void);
void OLED_Clear0(void);
void OLED_Clear1(void);
void OLED_Clear2(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowChar2(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowNum2(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowString(u8 x,u8 y, u8 *p);
void OLED_ShowString2(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_SetBackLight(uint8_t Data);
void OLED_FullStr(const char *str);
void OLED_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign);
void LCDOn(void);
#endif

