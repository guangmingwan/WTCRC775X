/****************************************************************/
// JLX19264G-925-PN-S
// ���нӿ�
// ����IC : ST7525
// ˮƽ��ʾ��192x64����
// ����6 4�У�8��Ϊһ��page������8��page��д��ҳ����ʱ�����ϵ���д�룬������������ض�Ӧ�������λ��
// ����192�У���192column��д��˳�������ң�д�����һ�в����Զ����С�
//
// ȡͼ��ģʹ��img2lcd������ˮƽ���ֽڴ�ֱ���ֽ������ݵ��򣬲�����ͼ��ͷ����
// ȡ��ģʹ��Pctolcd2002�����룬����ʽ������
/****************************************************************/
#ifdef LCD25696
#include "main.h"
#include "tuner.h"
#include "ui.h"
#include "oledfont.h"
#include "lcd25696.h"
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
void LCD_WR_Byte(u8 dat,u8 cmd)
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

void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
	LCD_WR_Byte(0xb0+y,LCD_CMD);
	LCD_WR_Byte(((x&0xf0)>>4)|0x10,LCD_CMD);
	LCD_WR_Byte((x&0x0f)|0x01,LCD_CMD); 
}  
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void LCD_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
			LCD_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			LCD_WR_Byte(F8X16[c*16+i],LCD_DATA);
			LCD_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			LCD_WR_Byte(F8X16[c*16+i+8],LCD_DATA);
			}
			else {	
				LCD_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				LCD_WR_Byte(F6x8[c][i],LCD_DATA);
				
			}
}
//显示一个字符号串
void LCD_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		LCD_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
void LCD_XYStr(uint8_t x, uint8_t y, const char *str)  // Display string at x:0-15, y:0-1
{
	int ry = 3+ 3*y;
	int rx = x * 8;
	LCD_ShowString(rx,ry,(u8*)str);
}
void LCD_FullStr(const char *str)  // Display string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars
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
void LCD_XYChar(uint8_t x, uint8_t y, const char c)  // Display char at x:0-15, y:0-1
{
	int ry = 3+ 3*y;
	int rx = x * 8;
	
	//LCD_XY(rx, ry);
	LCD_ShowChar(rx,ry,c);
}

void LCD_ClearT(void) {
	LCD_ShowString(0,0, (u8*)"                ");
}
void LCD_Clear0(void)
{
	LCD_XYStr(0, 0, "                ");  // Clear LCD's 1nd row
}
void LCD_Clear1(void)
{
	LCD_XYStr(0, 1, "                ");  // Clear LCD's 2nd row
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


/**
 * @brief  дָ��
 * @param  cmd ָ��
 */
void transfer_command(uint8_t cmd)
{
	char i;
	CS_PIN_LOW();
	DC_PIN_LOW();
	for(i=0;i<8;i++)
	{
		SCK_PIN_LOW();
		if(cmd&0x80) { 
			SDA_PIN_HIGH();
		}
		else {
			SDA_PIN_LOW();
		}
		SCK_PIN_HIGH();
//    delay_us(1);
		cmd=cmd<<=1;
	}
	CS_PIN_HIGH();
}
void transfer_command1(uint8_t cmd)
{
	CS_PIN_LOW();
	DC_PIN_LOW();
	//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
	//spi_i2s_data_transmit(SPI1, cmd);
	HAL_SPI_Transmit(&hspi1,&cmd, 1, 1000);
	lcd_delay(1);
	CS_PIN_HIGH();
}


/**
 * @brief  д����
 * @param  data ����
 */
void transfer_data(uint8_t data)
{
	char i;
	CS_PIN_LOW();
	DC_PIN_HIGH();
	for(i=0;i<8;i++)
	{
		SCK_PIN_LOW();
		if(data&0x80) SDA_PIN_HIGH();
		else SDA_PIN_LOW();
		SCK_PIN_HIGH();
//        delay_us(1);
		data=data<<=1;
	}
	CS_PIN_HIGH();
}

void transfer_data1(uint8_t data)
{
	CS_PIN_LOW();
	DC_PIN_HIGH();
	//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
	//spi_i2s_data_transmit(SPI1, data);
	HAL_SPI_Transmit(&hspi1,&data, 1, 1000);
	lcd_delay(1);
	CS_PIN_HIGH();
}


/**
 * @brief  д�������
 * @param  data ����ָ��
 * @param  length ���ݳ���
 */
void transfer_muilt_data(uint8_t * data, uint16_t length)
{
	uint16_t i = 0;
	uint16_t v = 0;
	CS_PIN_LOW();
	DC_PIN_HIGH();
	for(i = 0;i<length;i++)
	{
		//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
		//spi_i2s_data_transmit(SPI1, data[i]);
		HAL_SPI_Transmit(&hspi1,&data[i], 1, 1000);
			for(v=0;v<8;v++)
		{
			SCK_PIN_LOW();
			if(data[v]&0x80) SDA_PIN_HIGH();
			else SDA_PIN_LOW();
			SCK_PIN_HIGH();
	//        delay_us(1);
			data[v]=data[v]<<=1;
		}
	}
	lcd_delay(2);
	CS_PIN_HIGH();
	
}

void transfer_muilt_data1(uint8_t * data, uint16_t length)
{
	uint16_t i = 0;
	CS_PIN_LOW();
	DC_PIN_HIGH();
	for(i = 0;i<length;i++)
	{
		//while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
		//spi_i2s_data_transmit(SPI1, data[i]);
		HAL_SPI_Transmit(&hspi1,&data[i], 1, 1000);
	}
	lcd_delay(2);
	CS_PIN_HIGH();
	
}
/**
 * @brief  ����LCD�Դ�ָ��
 * @param  page ҳ��[1-8]
 * @param  column ����[1-192]
 */
void lcd_address_old(uint8_t page,uint8_t column)
{
	column=column-1; //��1�У���LCD����IC���ǵ�0�У������������ȥ1
	page=page-1;		 //��1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1
	transfer_command(0xB0+page); //����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ
	transfer_command(((column>>4)&0x0f)+0x10); //�����е�ַ�ĸ�4λ
	transfer_command(column&0x0f); 						 //�����е�ַ�ĵ�4λ
}


/*дLCD ���е�ַ��XΪ��ʼ���е�ַ��YΪ��ʼ���е�ַ��x_total,y_total�ֱ�Ϊ�е�ַ���е�ַ����㵽�յ�Ĳ�ֵ */
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


/*����*/
void clear_screen()
{
	int i,j;	
	lcd_address(1,1,256,17);
	for(i=0;i<17;i++)
	{
		for(j=0;j<256;j++)
		{
			transfer_data(0xff);
		}
	}
}



/****��ָ�����Ѷ�ֿ�IC***/
void send_command_to_ROM( uchar datu )
{
	 
}

/****�Ӿ���Ѷ�ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�***/
static uchar get_data_from_ROM( )
{
	 
}

//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_16x16(ulong fontaddr,uchar column,uchar page,uchar reverse)
{
	 
}

//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_8x16(ulong fontaddr,uchar column,uchar page,uchar reverse)
{
 
}

//��ָ����ַ��������д��Һ����ָ����page,column)������
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
			//������壨GB2312�������ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (ulong)(fontaddr*32);

			get_and_write_16x16(fontaddr,column,page,reverse);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=2;
			column+=16;			
		}

		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			//������壨GB2312��15x16����ַ��ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (ulong)(fontaddr*32);
	
			get_and_write_16x16(fontaddr,column,page,reverse);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
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

			get_and_write_8x16(fontaddr,column,page,reverse);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
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
			
			get_and_write_5x7(fontaddr,column,page,reverse);/*��ʾ5x7��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=1;
			column+=6;
		}
		else
		i++;	
	}	
}





/**
 * @brief  ��ʾһ��ASCII�ַ�
 * @param  page ҳ
 * @param  column ��
 * @param  ascii �ַ�
 * @param  font ����
 */
void DispChar(uint8_t page, uint8_t column, char ascii, sFONT* font)
{
	uint8_t i, j, ch;
	uint16_t char_offset;
	char stdch = ascii;
	const unsigned char* ptr;
	if(stdch < 32 || stdch > 126)
		stdch = '?';
	char_offset = (stdch - ' ') * font->Height * font->Width / 8;
	ptr = &font->table[char_offset];

	for(i=0;i<(font->Height/8);i++)
	{
		lcd_address(column,page+i,font->Width,1);
		for(j=0; j<font->Width; j++) {
			transfer_data(reverse_bit(*(ptr+j)));
		}
		//transfer_muilt_data((uint8_t *)ptr, font->Width);
		ptr = ptr + font->Width;
	}
}


/**
 * @brief  ��ʾASCII�ַ���
 * @param  page ҳ
 * @param  column ��
 * @param  text �ַ���
 * @param  font ����
 */
void DispString(uint8_t page, uint8_t column, const char* text, sFONT* font)
{
	const char* p_text = text;
	uint8_t x = column;
	
	while(*p_text != 0)
	{
		if(*p_text < 32 || *p_text > 126)
			DispFill(page, x, font->Width, font->Height, 0x5A);
		else
			DispChar(page,x,*p_text,font);
		x = x + font->Width;
		if(x+font->Width > 193)  // Wrap
		{
			x=column;
			page += font->Height/8;
			if(page > 8)
				break;
		}
		p_text++;
	}
}


/**
 * @brief  ��ʾASCII�ַ�����һ����
 * @param  page ҳ
 * @param  column ��
 * @param  text �ַ���
 * @param  font ����
 * @param  start ��ʼ�ַ�λ��
 * @param  length ��ʾ����
 */
void DispStringPart(uint8_t page, uint8_t column, const char* text, sFONT* font, uint16_t start, uint16_t length)
{
	const char* p_text = text;
	uint8_t x = column;
	uint16_t count = length;
	p_text += start;
	
	while(*p_text != 0)
	{
		if(*p_text < 32 || *p_text > 126)
			DispFill(page, x, font->Width, font->Height, 0x5A);
		else
			DispChar(page,x,*p_text,font);
		
		count--;
		if(count == 0)
			break;
		
		x = x + font->Width;
		if(x+font->Width > 193)  // Wrap
		{
			x=column;
			page += font->Height/8;
			if(page > 8)
				break;
		}
		p_text++;
	}
}


void DispNumber(uint8_t page, uint8_t column, int32_t number, sFONT* font, uint8_t length)
{
	int32_t num = number;
	uint8_t x = column;
}

/****************************************************************/

/**
 * @brief  LCD����ģʽ
 * @param  lumi 0-100 ����ǿ��
 */
void DispBacklight(uint8_t lumi)
{
	//DAC_OutVol(2, lumi);	//���⿪��
}

/**
 * @brief  LCD�Աȶ�
 * @param  contrast 0-255/norm:121
 */
void DispContrast(uint8_t contrast)
{
	//transfer_command(0x81); //���Աȶ�
	//transfer_command(contrast); //���Աȶȵ�ֵ�������÷�Χ0x00��0xFF
}

/**
 * @brief  LCDƫѹ
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
 * @brief  ��Ļɨ���ٶ�
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
 * @brief  ��Ļ����
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
 * @brief  LCD��Դģʽ
 * @param  mode 0:������ʾ 1:�ر�ģʽ
 */
void DispPower(uint8_t mode)
{
	if(mode == 0)
		transfer_command(0xAF);	//PD = 1 �˳�ʡ��ģʽ
	else if (mode ==1)
	{
		transfer_command(0xAE);	//PD = 0 ʡ��ģʽ
		DispBacklight(0);		//�رձ���
	}
	HAL_Delay(300);	//��С250ms�Թرջ���LCD��ѹ��·
}
/**
 * @brief  �����Ļ���߶���Ϊ8�ı�����
 * @param  page ��ʼҳ
 * @param  column ��ʼ��
 * @param  x ����
 * @param  y �߶�
 * @param  color ��ɫ 0:��ɫ
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
		//�����ƶ�iλ��λ��1�����õ���iλ
		//��iλ�����ƶ�32-1-iλ����������ת֮���λ��		
	}
	return num;
}

/**
 * @brief  д�������С��ͼ�񣨸߶���Ϊ8�ı�����
 * @param  page ҳ
 * @param  column ��
 * @param  x ����
 * @param  y �߶�
 * @param  *dp ͼ��ָ��
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
/*��ʾ256*96�����ͼ��*/
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

/**
 * @brief  LCD��ʼ��
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
	transfer_command(0xf2);   //�¶Ȳ���
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
	transfer_data(0x0a);   		 //VPR[5-0]  //�����÷�Χ0x00~0x3f,ÿ���ѹ��0.04V
	transfer_data(0x04);  		 //VPR[8-6]	 //�����÷�Χ0x00~0x07
	transfer_command(0x20);    //Power control
	transfer_data(0x0B);   		 //D0=regulator ; D1=follower ; D3=booste,  on:1 off:0
	//delay_us(100);
	HAL_Delay(1);
	DispFill(1,1,255,96,0);	//����Դ�	
	transfer_command(0xAF);    //Display on	
	//DAC_Start(2);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOB, OLED_BL_Pin, GPIO_PIN_SET);
}
void LCDOn(void) {
}
#endif