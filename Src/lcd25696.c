/****************************************************************/
// JLX19264G-925-PN-S
// ??????
// ????IC : ST7525
// ???????192x64????
// ????6 4???8???h??page??????8??page????????????????????????????????????????????????
// ????192?????192column?????????????????????h?????????????
//
// ????g'??img2lcd???????????????????????????????????????????
// ???g'??Pctolcd2002?????????????????
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
//?SSD1106???????
//dat:??????/??
//cmd:??/???? 0,????;1,????;
#define LCD_WR_Byte LCD_HAL_WR_Byte
static char SPI2_ReadWriteByte(uint8_t txdata)
{
	uint8_t rxdata=00;
	HAL_SPI_TransmitReceive(&hspi2,&txdata,&rxdata,1,3);
	return rxdata;
}
void OLED_HAL_WR_Byte(u8 dat,u8 cmd)
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
void OLED_SOFT_WR_Byte(u8 dat,u8 cmd)
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



void OLED_XYChar(uint8_t x, uint8_t y, const char c)  // Display char at x:0-15, y:0-2
{
	
	OLED_ShowChar(x,y,c);
}
//???????????,??????
//x:0~255
//y:0~95
//mode:0,????;1,????				 
//size:???? 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{ 
	int ry = (Font32.Height)*y;
	int rx = x * Font32.Width;
  uint8_t page = (ry / 8)+1;	
	DispChar(page,rx,chr,&Font32);
}
void OLED_XYStr(uint8_t x, uint8_t y, const char *str)  // Display string at x:0-15, y:0-1
{

	OLED_ShowString(x,y,(u8*)str);
}
//????????
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j]);
		x++;
		if(x>15){
			x=0;
			y++;
		}
		j++;
	}
}
void OLED_FullStr(const char *str)  // Display string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars
{
	const char *p;
	char c;
	uint8_t x, y;
	uint8_t bInStr;

	p = str;
	bInStr = 1;
	for (y = 1; y <= 2; y++)
	{
		for (x = 0; x <= 15; x++)
		{
			//if (!x)
			//	OLED_XY(0, y);

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

			//OLED_Data(c);
			OLED_XYChar(x, y, c);
		}
	}
}  // void LCDFullStr_P(PGM_P str)  // Display PROGMEM string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars

void OLED_XYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign)
{
	char *p, c;
	uint8_t i, n, pc;

	//OLED_XY(x, y);
	p = (char *)str;
	n = nLen - strlen(str);
	if (!bLeftAlign)
	{
		for (i = 0; i < n; i++) {
			//OLED_Data(' ');
			OLED_XYChar(x+i,y,' ');
		}
	}
	pc = 0;
	while ((c = *p++)) {
		//OLED_Data(c);
		OLED_XYChar(x+pc,y,c);
		pc++;
	}

	if (bLeftAlign)
	{
		for (i = 0; i < n; i++) {
			//OLED_Data(' ');
			OLED_XYChar(x+pc+i,y,' ');
		}
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
void OLED_Clear(void) {
	OLED_Clear0();
	OLED_Clear1();
	OLED_Clear2();
}

void OLED_Clear0(void) {
	OLED_XYStr(0, 0, "                ");
}
void OLED_Clear1(void)
{
	OLED_XYStr(0, 1, "                ");  // Clear LCD's 1nd row
}
void OLED_Clear2(void)
{
	OLED_XYStr(0, 2, "                ");  // Clear LCD's 2nd row
}

/****************************************************************/

void OLED_delay(uint16_t time)
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
	OLED_delay(1);
	CS_PIN_HIGH();
}


void transfer_data(uint8_t data)
{
	CS_PIN_LOW();
	DC_PIN_HIGH();
	//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
	//spi_i2s_data_transmit(SPI1, data);
	HAL_SPI_Transmit(&hspi2,&data, 1, 1000);
	OLED_delay(1);
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
	OLED_delay(2);
	CS_PIN_HIGH();
	
}


/*?LCD ????????X???'????????Y???'????????x_total,y_total????????????????????????? */
void OLED_address(int x,int y,int x_total, int y_total)
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


/*????*/
void clear_screen()
{
	int i,j;	
	OLED_address(1,1,256,17);
	for(i=0;i<17;i++)
	{
		for(j=0;j<256;j++)
		{
			transfer_data(0x00);
		}
	}
}





/**
 * @brief  ???h??ASCII???
 * @param  page ?
 * @param  column ??
 * @param  ascii ???
 * @param  font ????
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
		OLED_address(column,page+i,font->Width,1);
		for(j=0; j<font->Width; j++) {
			transfer_data(font->revert ? reverse_bit(*(ptr+j)) : *(ptr+j) );
		}
		//transfer_muilt_data((uint8_t *)ptr, font->Width);
		ptr = ptr + font->Width;
	}
}
 


/****************************************************************/

/**
 * @brief  LCD????g?
 * @param  lumi 0-100 ???????
 */
void DispBacklight(uint8_t lumi)
{
	//DAC_OutVol(2, lumi);	//??????
}

/**
 * @brief  LCD????
 * @param  contrast 0-255/norm:121
 */
void DispContrast(uint8_t contrast)
{
	//transfer_command(0x81); //??????
	//transfer_command(contrast); //???????????????�??0x00??0xFF
}

/**
 * @brief  LCDt?
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
 * @brief  ??L??????
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
 * @brief  ??L????
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
 * @brief  LCD???g?
 * @param  mode 0:??????? 1:???g?
 */
void DispPower(uint8_t mode)
{
	if(mode == 0)
		transfer_command(0xAF);	//PD = 1 ??????g?
	else if (mode ==1)
	{
		transfer_command(0xAE);	//PD = 0 ???g?
		DispBacklight(0);		//??????
	}
	HAL_Delay(300);	//???250ms???????LCD?????�
}
/**
 * @brief  ?????L????????8?i?????
 * @param  page ??'?
 * @param  column ??'??
 * @param  x ????
 * @param  y ???
 * @param  color ??? 0:???
 */
void DispFill(uint8_t page ,uint8_t column ,uint8_t x ,uint8_t y ,uint8_t color)
	
{
	uint8_t i,j;
	uint8_t height = (uint8_t)(y/8);
	OLED_address(column,page,x,y);
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
		//?????z?i??????1?????�???i?
		//??i??????z?32-1-i???????????????????		
	}
	return num;
}

/**
 * @brief  ????????????????????8?i?????
 * @param  page ?
 * @param  column ??
 * @param  x ????
 * @param  y ???
 * @param  *dp ??????
 */
void DispPic(uint8_t page, uint8_t x, uint8_t w ,uint8_t h ,const unsigned char *dp)
{
	
	uint8_t  height;
	uint8_t i,j;
	
	height = (uint8_t)(h/8);
	
	OLED_address(x, page, w, height);
	
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
/*???256*96????????*/
void disp_256x96(int x,int y,char *dp)
{
	int i,j;
	OLED_address(x,y,256,12);
	for(i=0;i<12;i++)	  
	{
		for(j=0;j<256;j++)  
		{
			transfer_data(*dp);
			dp++;
		}
	}
}
void OLED_SetBackLight(uint8_t Data)
{
	if(Data<=0) {
		HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_RESET);
	}
}
	
void OLED_On(void)
{
	if (nBacklightKeep && bLCDOff)
	{
		//LCDSetBackLight(nBacklightAdj);
		HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_SET);
		
		bLCDOff = false;
	}
	nBacklightTimer = HAL_GetTick();
}



/****?????????????IC***/
void send_command_to_ROM( uchar datu )
{
	 
}

/****?????????IC????????????????1??????***/
static uchar get_data_from_ROM( )
{
	 
}

//?????????????????????????????page,column)??????
void get_and_write_16x16(ulong fontaddr,uchar column,uchar page,uchar reverse)
{
	 
}

//?????????????????????????????page,column)??????
void get_and_write_8x16(ulong fontaddr,uchar column,uchar page,uchar reverse)
{
 
}

//?????????????????????????????page,column)??????
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
			//???????GB2312???????????????IC??j???????�????????
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//???????8??????????????????????????????
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (ulong)(fontaddr*32);

			get_and_write_16x16(fontaddr,column,page,reverse);	 //?????????????????????????????page,column)??????
			i+=2;
			column+=16;			
		}

		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			//???????GB2312??15x16???????????????IC??j???????�????????
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//???????8??????????????????????????????
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (ulong)(fontaddr*32);
	
			get_and_write_16x16(fontaddr,column,page,reverse);	 //?????????????????????????????page,column)??????
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

			get_and_write_8x16(fontaddr,column,page,reverse);	 //?????????????????????????????page,column)??????
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
			
			get_and_write_5x7(fontaddr,column,page,reverse);/*???5x7??ASCII???LCD???y???????x???????fontbuf[]?????*/
			i+=1;
			column+=6;
		}
		else
		i++;	
	}	
}

/**
 * @brief  LCD??'??
 */
void OLED_Init()
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
	transfer_command(0xf2);   //?�????
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
	transfer_data(0x0a);   		 //VPR[5-0]  //?????�??0x00~0x3f,�??????0.04V
	transfer_data(0x04);  		 //VPR[8-6]	 //?????�??0x00~0x07
	transfer_command(0x20);    //Power control
	transfer_data(0x0B);   		 //D0=regulator ; D1=follower ; D3=booste,  on:1 off:0
	//delay_us(100);
	HAL_Delay(1);
	DispFill(1,1,255,96,0);	//??????	
	transfer_command(0xAF);    //Display on	
	//DAC_Start(2);
	HAL_Delay(5);
	
}
void LCDOn(void) {
	if (nBacklightKeep && bLCDOff)
	{
		//LCDSetBackLight(nBacklightAdj);
		HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_SET);
		
		bLCDOff = false;
	}
	nBacklightTimer = HAL_GetTick();
}
void BackLightOn(void) {
	HAL_GPIO_WritePin(OLED_BL_GPIO_Port, OLED_BL_Pin, GPIO_PIN_SET);
}
void OLED_Refresh(void)
{
}
void OLED_Display_On(void)
{
	if (nBacklightKeep && bLCDOff)
	{
		//LCDSetBackLight(nBacklightAdj);
		HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_SET);
		
		bLCDOff = false;
	}
	nBacklightTimer = HAL_GetTick();
}
#endif