#ifdef SSD_1115_DW
#include "oled.h"
#include "stdlib.h"
#include "string.h"
#include "oledfont.h"  	 
//#include "delay.h"
//#include "spi.h"
//??SSD1106???h??????
//dat:?????????/????
//cmd:????/?????? 0,???????;1,???????;

#define OLED_WR_Byte OLED_SOFT_WR_Byte
static char SPI2_ReadWriteByte(uint8_t txdata)
{
	uint8_t rxdata=00;
	//HAL_SPI_TransmitReceive(&hspi2,&txdata,&rxdata,1,3);
	return rxdata;
}
//更新显存到OLED	
void OLED_Refresh(void)
{
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
		OLED_SCL_Clr();
		if(dat&0x80)
		   OLED_SDA_Set();
		else 
		   OLED_SDA_Clr();
		OLED_SCL_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x01,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x16,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}
void clear_screen(void) {
	OLED_Clear();
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//???t?????			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n????
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//???2??????
//x,y :???????	 
//len :????????
//size:??????
//mode:g?	0,???g?;1,????g?
//num:???(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
	}
} 
//???h??????W?
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
//???????
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}
/***********????????????????BMP??128??64??'??????(x,y),x?k??0??127??y???k??0??7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 


//??'??				    
void OLED_Init(void)
{ 	

	OLED_RES_Clr();
	HAL_Delay(200);
	OLED_RES_Set();

	OLED_DC_Set();
	OLED_CS_Set();
	//gpio_bit_write(GPIOB, GPIO_PIN_7, RESET);
	OLED_DC_Clr();
	//gpio_bit_write(GPIOC, GPIO_PIN_8, RESET);
	OLED_CS_Clr();
	

	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA0,OLED_CMD);//--Set SEG/Column Mapping     0xa0??????? 0xa1????
	OLED_WR_Byte(0xC1,OLED_CMD);//Set COM/Row Scan Direction   0xc0??????? 0xc8????
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 

	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  
void OLED_SetBackLight(uint8_t Data)
{
	if(Data == 0x0) {
		OLED_WR_Byte(0xAE,OLED_CMD);
	}
	else {
		OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
		OLED_WR_Byte(Data,OLED_CMD); // Set SEG Output Current Brightness
		OLED_WR_Byte(0xAF,OLED_CMD);
		//bLCDOff = false;
	}
}

void OLED_Data(uint8_t Data)  // Write LCD data
{
	OLED_WR_Byte(Data,OLED_DATA);
}
void OLED_ClearT(void) {
	OLED_ShowString(0,0, (u8*)"                ");
}
void OLED_Clear0(void)
{
	OLED_XYStr(0, 0, "                ");  // Clear LCD's 1nd row
}
void OLED_Clear1(void)
{
	OLED_XYStr(0, 1, "                ");  // Clear LCD's 2nd row
}
void OLED_Clear2(void)
{
	OLED_XYStr(0, 2, "                ");  // Clear LCD's 2nd row
}
void OLED_Clear3(void)
{
	OLED_XYStr(0, 3, "                ");  // Clear LCD's 2nd row
}
void OLED_XYChar(uint8_t x, uint8_t y, const char c)  // Display char at x:0-15, y:0-1
{
	
	uint8_t rx = x * 8;
	uint8_t page = (y*16) / 8; 
	OLED_ShowChar(rx,page,c);
}


void OLED_XYStr(uint8_t x, uint8_t y, const char *str)  // Display string at x:0-15, y:0-1
{
	uint8_t rx = x * 8;
	uint8_t page = (y*16) / 8; 
	char truncatedStr[17];
	strncpy(truncatedStr, str, 16);
	truncatedStr[16] = '\0'; // end for str

	OLED_ShowString(rx,page,(u8*)truncatedStr);
}
void OLED_FullStr(const char *str)  // Display string to full(2x16 chars) LCD, fill with blank if string is less than 32 chars
{
	const char *p;
	char c;
	uint8_t x, y;
	uint8_t bInStr;

	p = str;
	bInStr = 1;
	for (y = 1; y <= 3; y++)
	{
		for (x = 0; x <= 15; x++)
		{

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
#endif