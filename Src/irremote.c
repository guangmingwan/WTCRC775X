#include "irremote.h"
extern void delay_us(uint16_t us);

uint32_t IRCode = 0;
uint8_t IR_Flag = 0;
uint8_t IR_HighLevelPeriod(void){
	uint8_t t=0;
	while(HAL_GPIO_ReadPin(REMOTE_IR_GPIO_Port,REMOTE_IR_Pin)==1){
		t++;
		delay_us(20);
		if(t>=250) return t;
	}
	return t;
}
// NEC ir protocol
void IR_HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	uint8_t Tim=0,Ok=0,Data,Num=0;
	if(GPIO_Pin == REMOTE_IR_Pin) {
	while(1){
	   	if(HAL_GPIO_ReadPin(REMOTE_IR_GPIO_Port,REMOTE_IR_Pin)==1){
			 Tim = IR_HighLevelPeriod();

			 if(Tim>=250) 
			 	break;//not using
			 if(Tim>=100 && Tim<250)
			 	Ok=1;	//start signal
			 else if(Tim>=60 && Tim<90)
			 	Data=1;	//data 1
			 else if(Tim>=10 && Tim<50)
			 	Data=0; // data 0
			 else {
				 Tim = 0;//no ! bad tim value for ok signal
			 }

			 if(Ok==1){
			 	IRCode <<= 1;
				IRCode += Data;
				if(Num>=32){
					IR_Flag=1;
				    break;
				}
			 }
			 Num++;
		}
	}
}
}

