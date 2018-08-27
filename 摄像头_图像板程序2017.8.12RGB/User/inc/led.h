#ifndef _LED_H
#define _LED_H

#include "stm32f4xx.h"

#define LED_ON() 			GPIO_ResetBits(GPIOA,GPIO_Pin_15)  //GPIOA->BSRRL = GPIO_Pin_15
#define LED_OFF() 		GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define LED_TOGLE() 	GPIOA->ODR^=GPIO_Pin_15

void Led_Config(void);

#endif




