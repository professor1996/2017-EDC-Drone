#ifndef _OV7725_H
#define _OV7725_H

#include "ILI9325.h"
#include "delay.h"
#include "stm32f4xx_dcmi.h"
#include "Image.h"
#include "Sccb.h"
#include "OV_Config.h"
#define   OV7725RGB        //       OV7725YUV          OV7725RGB


extern u16 Image[IMAGE_COLUMN * IMAGE_ROW];
extern uint8_t * Gray_Image_Buffer;

u8 OV_Init(void);
void OV_Start(void);
void OV_Stop(void);
void OV_DCMI_DMA_Init(uint32_t address, uint16_t buffer_size);

#endif

