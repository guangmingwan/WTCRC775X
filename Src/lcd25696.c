/****************************************************************/
// JLX19264G-925-PN-S
// 串行接口
// 驱动IC : ST7525
// 水平显示点阵192x64点阵
// 垂直64行 8点为一个page 共有8个page 写入页地址时从上到下写入 数据位对应纵向位置
// 水平192列 共192column 写入顺序从左到右 写完最后一列不会自动换行
//
// 取图取模使用img2lcd 软件 水平取模 字节垂直 数据倒序 不带图片头
// 取字模使用Pctolcd2002 阴码 逐行式 逆向
/****************************************************************/
#ifdef LCD25696
#include "main.h"
#include "tuner.h"
#include "ui.h"
#include "oledfont.h"
#include "lcd25696.h"
#include "font.h"
#include "stdint.h"
#include "string.h"
#ifdef STM32F405xx
#include "stm32f4xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif
#include "spi.h"
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
#define LCD_WR_Byte LCD_HAL_WR_Byte
static char SPI2_ReadWriteByte(uint8_t txdata)
{
	uint8_t rxdata=00;
	HAL_SPI_TransmitReceive(&hspi2,&txdata,&rxdata,1,3);
	return rxdata;
}
void LCD_HAL_WR_Byte(u8 dat,u8 cmd)
{
	u8 i;
	if(cmd)
	  OLED_DC_Set();
	else
	  OLED_DC_Clr();
	OLED_CS_Clr();

	SPI2_ReadWriteByte(dat);

	OLED_CS_Set();
	OLED_DC_Set();
}
void LCD_SOFT_WR_Byte(u8 dat,u8 cmd)
{
	u8 i;
	if(cmd)
	  OLED_DC_Set();
	else
	  OLED_DC_Clr();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;
	}
	OLED_CS_Set();
	OLED_DC_Set();
}



void LCD_XYChar(uint8_t x, uint8_t y, const char c)  // Display char at x:0-15, y:0-2
{

	LCD_ShowChar(x,y,c);
}
//在指定位置显示一个字符,包括部分字符
//x:0~255
//y:0~95
//mode:0,反白显示;1,正常显示
//size:选择字体 16/12
void LCD_ShowChar(u8 x,u8 y,u8 chr)
{
	int ry = (Font32.Height)*y;
	int rx = x * Font32.Width;
  uint8_t page = (ry / 8)+1;
	DispChar(page,rx,chr,&Font32);
}
void LCD_XYStr(uint8_t x, uint8_t y, const char *str)  // Display string at x:0-15, y:0-1
{

	LCD_ShowString(x,y,(u8*)str);
}
//显示一个字符号串
void LCD_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{
		LCD_ShowChar(x,y,chr[j]);
		x++;
		if(x>15){
			x=0;
			y++;
		}
		j++;
	}
}
void LCD_FullStr(const char *str)  // Display string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars
{
	const char *p;
	char c;
	uint8_t x, y;
	uint8_t bInStr;

	p = str;
	bInStr = 1;
	for (y = 0; y <= 2; y++)
	{
		for (x = 0; x <= 15; x++)
		{
			//if (!x)
			//	LCD_XY(0, y);

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

			//LCD_Data(c);
			LCD_XYChar(x, y, c);
		}
	}
}  // void LCDFullStr_P(PGM_P str)  // Display PROGMEM string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars

void LCD_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign)
{
	char *p, c;
	uint8_t i, n, pc;

	//LCD_XY(x, y);
	p = (char *)str;
	n = nLen - strlen(str);
	if (!bLeftAlign)
	{
		for (i = 0; i < n; i++) {
			//LCD_Data(' ');
			LCD_XYChar(x+i,y,' ');
		}
	}
	pc = 0;
	while ((c = *p++)) {
		//LCD_Data(c);
		LCD_XYChar(x+pc,y,c);
		pc++;
	}

	if (bLeftAlign)
	{
		for (i = 0; i < n; i++) {
			//LCD_Data(' ');
			LCD_XYChar(x+pc+i,y,' ');
		}
	}
}  // void LCDXYStrLen(uint8_t x, uint8_t y, char *str, uint8_t nLen, bool bLeftAlign)

   // Display signed integer at x:0-15, y:0-1. Righ aligned, spaces filled at left. Too long integers are not allowed.
void LCD_XYIntLen(uint8_t x, uint8_t y, int32_t n, uint8_t nLen)
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
		LCD_XYChar(x + i--, y, '0' + (r % 10));
		r = r / 10;
	} while (r);

	if (n < 0)
		LCD_XYChar(x + i--, y, '-');

	while (i >= 0)
		LCD_XYChar(x + i--, y, ' ');
}

// Display unsigned integer at x:0-15, y:0-1. Righ aligned, '0' filled at left. Too long unsigned integers are not allowed.
void LCD_XYUIntLenZP(uint8_t x, uint8_t y, uint32_t n, uint8_t nLen)
{
	int8_t i;
	uint32_t r;
	r = n;
	i = nLen - 1;
	do
	{
		LCD_XYChar(x + i--, y, '0' + (r % 10));
		r = r / 10;
	} while (r);

	while (i >= 0)
		LCD_XYChar(x + i--, y, '0');
}
void LCD_Clear(void) {
	LCD_Clear0();
	LCD_Clear1();
	LCD_Clear2();
}

void LCD_Clear0(void) {
	LCD_XYStr(0, 0, "                ");
}
void LCD_Clear1(void)
{
	LCD_XYStr(0, 1, "                ");  // Clear LCD's 1nd row
}
void LCD_Clear2(void)
{
	LCD_XYStr(0, 2, "                ");  // Clear LCD's 2nd row
}

/****************************************************************/

void lcd_delay(uint16_t time)
{
   uint16_t i=0;
   while(time--)
   {
      i=15;
      while(i--) ;
   }
}

/****************************************************************/



void transfer_command(uint8_t cmd)
{
	CS_PIN_LOW();
	DC_PIN_LOW();
	//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
	//spi_i2s_data_transmit(SPI1, cmd);
	HAL_SPI_Transmit(&hspi2,&cmd, 1, 1000);
	lcd_delay(1);
	CS_PIN_HIGH();
}


void transfer_data(uint8_t data)
{
	CS_PIN_LOW();
	DC_PIN_HIGH();
	//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
	//spi_i2s_data_transmit(SPI1, data);
	HAL_SPI_Transmit(&hspi2,&data, 1, 1000);
	lcd_delay(1);
	CS_PIN_HIGH();
}




void transfer_muilt_data(uint8_t * data, uint16_t length)
{
	uint16_t i = 0;
	CS_PIN_LOW();
	DC_PIN_HIGH();
	for(i = 0;i<length;i++)
	{
		//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
		//spi_i2s_data_transmit(SPI1, data[i]);
		HAL_SPI_Transmit(&hspi2,&data[i], 1, 1000);
	}
	lcd_delay(2);
	CS_PIN_HIGH();

}


/* 写LCD显示地址 X为起始列地址 Y为起始页地址 x_total y_total分别为列地址和页地址从起点到终点的差值 */
void lcd_address(int x,int y,int x_total, int y_total)
{
	x=x-1;
	y=y+7;

	transfer_command(0x15);	//Set Column Address
	transfer_data(x);
	transfer_data(x+x_total-1);

	transfer_command(0x75);	//Set Page Address
	transfer_data(y);
	transfer_data(y+y_total-1);
	transfer_command(0x30);
	transfer_command(0x5c);

}


/* 清屏 */
void clear_screen()
{
	int i,j;
	lcd_address(1,1,256,17);
	for(i=0;i<17;i++)
	{
		for(j=0;j<256;j++)
		{
			transfer_data(0x00);
		}
	}
}





/**
 * @brief  显示一个ASCII字符
 * @param  page 页
 * @param  column 列
 * @param  ascii 字符
 * @param  font 字体
 */
void DispChar(uint8_t page, uint8_t column, char ascii, sFONT* font)
{
	uint8_t i, j, ch;
	uint16_t char_offset;
	char stdch = ascii;
	const unsigned char* ptr;
	if(stdch < 32 || stdch > 126)
		stdch = '?';
	char_offset = (stdch - 32) * ((font->Height * font->Width) / 8);
	ptr = &(font->table[char_offset]);

	for(i=0;i<(font->Height/8);i++)
	{
		lcd_address(column,page+i,font->Width,1);
		for(j=0; j<font->Width; j++) {
			transfer_data(font->revert ? reverse_bit(*(ptr+j)) : *(ptr+j) );
		}
		//transfer_muilt_data((uint8_t *)ptr, font->Width);
		ptr = ptr + font->Width;
	}
}



/****************************************************************/

/**
 * @brief  LCD背光模式
 * @param  lumi 0-100 背光强度
 */
void DispBacklight(uint8_t lumi)
{
	//DAC_OutVol(2, lumi);	//背光开关
}

/**
 * @brief  LCD对比度
 * @param  contrast 0-255/norm:121
 */
void DispContrast(uint8_t contrast)
{
	//transfer_command(0x81); //设置对比度
	//transfer_command(contrast); //设置对比度的值 可设置范围0x00到0xFF
}

/**
 * @brief  LCD偏压
 * @param  bias	0->1/6, 1->1/7, 2->1/8, 3->1/9
 */
void DispBias(uint8_t bias)
{
	uint8_t temp = 0xE8+bias;
	if(bias<=3)
	{
		transfer_command(temp);
	}
}

/**
 * @brief  屏幕扫描速度
 * @param  fps 0->76fps, 1->95fps, 2->132fps, 3->168fps
 */
void DispFrame(uint8_t fps)
{
	uint8_t temp = 0xA0+fps;
	if(fps<=3)
	{
		transfer_command(temp);
	}
}

/**
 * @brief  屏幕反显
 * @param  inv true->Inverse display
 */
void DispInverse(bool inv)
{
	uint8_t temp = 0xA6;
	if(inv)
		temp+=1;
	transfer_command(temp);
}

/**
 * @brief  LCD电源模式
 * @param  mode 0:正常显示 1:关闭模式
 */
void DispPower(uint8_t mode)
{
	if(mode == 0)
		transfer_command(0xAF);	//PD = 1 退出省电模式
	else if (mode ==1)
	{
		transfer_command(0xAE);	//PD = 0 省电模式
		DispBacklight(0);		//关闭背光
	}
	HAL_Delay(300);	//最小250ms以关闭LCD升压电路
}
/**
 * @brief  填充屏幕 区域高度应为8的倍数
 * @param  page 起始页
 * @param  column 起始列
 * @param  x 宽度
 * @param  y 高度
 * @param  color 颜色 0:黑色
 */
void DispFill(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color)

{
	uint8_t i,j;
	uint8_t height = (uint8_t)(y/8);
	lcd_address(column,page,x,y);
	for(i=0;i<height;i++)
	{

		for(j=0;j<x;j++)
		{
			transfer_data(color);
		}
	}
}
unsigned int reverse_bit(unsigned char n)
{
	int i = 0;
	unsigned char num = 0;
	for (i = 0; i < 8; i++)
	{
		num += ((n >> i) & 1) << (8 - 1 - i);
		//将数据右移i位并取最低位
		//再把该位移动到反转后的位置
	}
	return num;
}

/**
 * @brief  写入指定大小的图像 高度应为8的倍数
 * @param  page 页
 * @param  column 列
 * @param  x 宽度
 * @param  y 高度
 * @param  *dp 图像指针
 */
void DispPic(uint8_t page, uint8_t x, uint8_t w ,uint8_t h ,const unsigned char *dp)
{

	uint8_t  height;
	uint8_t i,j;

	height = (uint8_t)(h/8);

	lcd_address(x, page, w, height);

	for(i=0;i<height;i++)
	{
		for(j=0;j<w;j++)
		{
			transfer_data(reverse_bit(*dp));
			dp++;
		}
		//transfer_muilt_data((uint8_t *)dp, w);
		//dp = dp + w;
	}
}
/* 显示256*96点阵图片 */
void disp_256x96(int x,int y,char *dp)
{
	int i,j;
	lcd_address(x,y,256,12);
	for(i=0;i<12;i++)
	{
		for(j=0;j<256;j++)
		{
			transfer_data(*dp);
			dp++;
		}
	}
}
void LCD_SetBackLight(uint8_t Data)
{
	if(Data<=0) {
		HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_RESET);
	}
}

void LCD_On(void)
{
	if (nBacklightKeep && bLCDOff)
	{
		//LCDSetBackLight(nBacklightAdj);
		HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_SET);

		bLCDOff = false;
	}
	nBacklightTimer = HAL_GetTick();
}



/**** 发送命令到晶讯字库IC ***/
void send_command_to_ROM( uchar datu )
{

}

/**** 从晶讯字库IC读取汉字或字符数据 1个字节 ***/
static uchar get_data_from_ROM( )
{

}

//把指定地址数据写入液晶指定位置 page column
void get_and_write_16x16(ulong fontaddr,uchar column,uchar page,uchar reverse)
{

}

//把指定地址数据写入液晶指定位置 page column
void get_and_write_8x16(ulong fontaddr,uchar column,uchar page,uchar reverse)
{

}

//把指定地址数据写入液晶指定位置 page column
void get_and_write_5x7(ulong fontaddr,uchar column,uchar page,uchar reverse)
{

}

//****************************************************************

ulong  fontaddr=0;
void display_GB2312_string(uchar column,uchar page,uchar reverse,uchar *text)
{
	uchar i= 0;
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{
			//国标汉字GB2312在晶讯字库IC中的地址按如下公式计算
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//由于单片机进行乘法运算较慢 所以分步取得地址
			fontaddr = (text[i]- 0xb0)*94;
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (ulong)(fontaddr*32);

			get_and_write_16x16(fontaddr,column,page,reverse);	 //把指定地址数据写入液晶指定位置 page column
			i+=2;
			column+=16;
		}

		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{
			//国标符号GB2312的15x16点阵字符在晶讯字库IC中的地址按如下公式计算
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//由于单片机进行乘法运算较慢 所以分步取得地址
			fontaddr = (text[i]- 0xa1)*94;
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (ulong)(fontaddr*32);

			get_and_write_16x16(fontaddr,column,page,reverse);	 //把指定地址数据写入液晶指定位置 page column
			i+=2;
			column+=16;
			if(column>256)
			{
			  page=page+2;
			}
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))
		{
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);

			get_and_write_8x16(fontaddr,column,page,reverse);	 //把指定地址数据写入液晶指定位置 page column
			i+=1;
			column+=8;
		}
		else
			i++;
	}
}

void display_string_5x7(uchar column,uchar page,uchar reverse,uchar *text)
{
	unsigned char i= 0;
	while((text[i]>0x00))
	{
		if((text[i]>=0x20) &&(text[i]<=0x7e))
		{
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*8);
			fontaddr = (unsigned long)(fontaddr+0x3bfc0);

			get_and_write_5x7(fontaddr,column,page,reverse);/* 显示5x7的ASCII字到LCD上 y为页地址 x为列地址 fontbuf为数据 */
			i+=1;
			column+=6;
		}
		else
		i++;
	}
}

/**
 * @brief  LCD初始化
 */
void LCD_Init()
{

	//spi_enable(SPI1);


	RST_PIN_LOW();
	HAL_Delay(100);
	RST_PIN_HIGH();
	HAL_Delay(100);

	transfer_command(0x30);   //EXT=0
	transfer_command(0x94);   //Sleep out
	transfer_command(0x31);   //EXT=1
	transfer_command(0xD7);   //Autoread disable
	transfer_data(0X9F);      //
	transfer_command(0x32);   //Analog SET
	transfer_data(0x00);  		//OSC Frequency adjustment
	transfer_data(0x01);  		//Frequency on booster capacitors->6KHz
	transfer_data(0x03);  		//Bias=1/11
	transfer_command(0x20);   // Gray Level

	transfer_command(0x31);   //Analog SET
	transfer_command(0xf2);   //温度补偿
	transfer_data(0x1e);  	  //OSC Frequency adjustment
	transfer_data(0x28);  	  //Frequency on booster capacitors->6KHz
	transfer_data(0x32);  	  //

	transfer_data(0x01);
	transfer_data(0x03);
	transfer_data(0x05);
	transfer_data(0x07);
	transfer_data(0x09);
	transfer_data(0x0b);
	transfer_data(0x0d);
	transfer_data(0x10);
	transfer_data(0x11);
	transfer_data(0x13);
	transfer_data(0x15);
	transfer_data(0x17);
	transfer_data(0x19);
	transfer_data(0x1b);
	transfer_data(0x1d);
	transfer_data(0x1f);
	transfer_command(0x30);   //EXT=0
	transfer_command(0x75);   //Page Address setting
	transfer_data(0X00);      // XS=0
	transfer_data(0X14);      // XE=159 0x28
	transfer_command(0x15);   //Clumn Address setting
	transfer_data(0X00);      // XS=0
	transfer_data(0Xff);      // XE=256
	transfer_command(0xBC);    //Data scan direction
	transfer_data(0x00);  		 //MX.MY=Normal
	transfer_data(0xA6);
	transfer_command(0xCA);    //Display Control
	transfer_data(0X00);  		 //
	transfer_data(0X9F);  		 //Duty=160
	transfer_data(0X20);  		 //Nline=off
	transfer_command(0xF0);    //Display Mode
	transfer_data(0X10);  		 //10=Monochrome Mode,11=4Gray
	transfer_command(0x81);    //EV control
	transfer_data(0x0a);   		 //VPR[5-0]  //设置范围0x00~0x3f 每格电压约0.04V
	transfer_data(0x04);  		 //VPR[8-6]	 //设置范围0x00~0x07
	transfer_command(0x20);    //Power control
	transfer_data(0x0B);   		 //D0=regulator ; D1=follower ; D3=booste,  on:1 off:0
	//delay_us(100);
	HAL_Delay(1);
	DispFill(1,1,255,96,0);	//清除显存
	transfer_command(0xAF);    //Display on
	//DAC_Start(2);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_SET);
}
void LCDOn(void) {
}
#endif
