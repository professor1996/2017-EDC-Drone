#ifndef _SCCB_H
#define _SCCB_H

#include "stm32f4xx.h"

#define SCCB_RCC 				RCC_AHB1Periph_GPIOB
#define SCCB_GPIO				GPIOB
#define SCCB_SIC_BIT    GPIO_Pin_10
#define SCCB_SID_BIT    GPIO_Pin_11

#define SCL_H   GPIOB->BSRRL = SCCB_SIC_BIT;
#define SCL_L   GPIOB->BSRRH =  SCCB_SIC_BIT;

#define SDA_H    GPIOB->BSRRL = SCCB_SID_BIT;
#define SDA_L    GPIOB->BSRRH =  SCCB_SID_BIT;
/**/
#define SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define SDA_read      GPIOB->IDR  & GPIO_Pin_11 

#define SCCB_SID_STATE	 GPIOB->IDR&SCCB_SID_BIT



void SCCB_GPIO_Config(void);
unsigned char WR_SENSOR_Reg(unsigned char regID, unsigned char regDat);
unsigned char RD_SENSOR_Reg(unsigned char regID, unsigned char *regDat);


#endif 









