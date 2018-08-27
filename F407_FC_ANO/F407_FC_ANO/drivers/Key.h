#include "stm32f4xx.h"

#define KEY_STATE	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8) 


void KEY_Init()  ;
void Key_func();

