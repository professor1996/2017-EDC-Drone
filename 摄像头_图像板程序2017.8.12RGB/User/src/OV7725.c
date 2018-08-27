#include "OV7725.h"
#include "ILI9325.h"
#include "delay.h"
#include "OV_Config.h"
#include "Image.h"

u16 Image[IMAGE_COLUMN * IMAGE_ROW] = {0};							//摄像头原始数据
uint8_t * Gray_Image_Buffer = ((uint8_t *)0x10000000);		//8位灰度图
/**********************************************
OV7725管脚配置
DCMI:PA4,6,9,10, PB6,7, PH11,12,14, PI6,7;
PWDN:PG7
RST:PG8
XCLK:PA8
***********************************************/
void OV_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
	//初始化DCMI管脚
  RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA | 
													RCC_AHB1Periph_GPIOB | 
													RCC_AHB1Periph_GPIOH |
													RCC_AHB1Periph_GPIOI , ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_DCMI);	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_DCMI);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
  
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource12, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource14, GPIO_AF_DCMI);
  
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource6, GPIO_AF_DCMI);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource7, GPIO_AF_DCMI);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_9| GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14;
  GPIO_Init(GPIOH, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOI, &GPIO_InitStructure);

	//初始化PWDN管脚和RST管脚
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	/*PWDN(PG7),RST(PG8)*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG, GPIO_Pin_7);//power on
	GPIO_SetBits(GPIOG, GPIO_Pin_8);
	
	//模拟I2C管脚初始化
  SCCB_GPIO_Config();
	
	//XCLK管脚初始化
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);//MCO1:PA8
  /*XCLK(PA8)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);	   
  
  RCC_MCO1Config(RCC_MCO1Source_HSI, RCC_MCO1Div_1);//16MHZ

	
}
/*****************************************************
Configures the DCMI to interface with the OV camera module
Enable DCMI clock
*****************************************************/
 
void OV_DCMI_Init(void)
{
	DCMI_InitTypeDef DCMI_InitStructure;
	
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);			
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;			//连续采集模式
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;				//硬件同步( HSYNC，VSYNC)
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;					//PCLK(像素时钟捕获沿)为下降沿
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;							//垂直同步极性						
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;								//水平同步极性
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;				//所有帧都捕获(只在连续捕获模式下有效)
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;		//每个像素时钟捕获10位数据(D0-D1口没数据，其实是硬件搞错了)
	
  DCMI_Init(&DCMI_InitStructure);
}

/*****************************************************
Configures the DMA2 to transfer Data from DCMI
Enable DMA2 clock
*****************************************************/
void OV_DCMI_DMA_Init(uint32_t address, uint16_t buffer_size)
{
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef  nvic;
	

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	nvic.NVIC_IRQChannel = DMA2_Stream1_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 3;
  nvic.NVIC_IRQChannelSubPriority = 3;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);
	
	/* DMA2 Stream1 Configuration */
  DMA_DeInit(DMA2_Stream1);
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DCMI->DR);		//查手册可知DR寄存器偏移地址为0x28
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)address;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					//外设到内存
  DMA_InitStructure.DMA_BufferSize = buffer_size;														
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不增加
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址增加
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据宽度为32位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//内存数据宽度16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;													//DMA循环传送
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  /* DMA2 IRQ channel Configuration */
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);	
}



/*
    可以设置OV的显示窗口,sx,sy是图像的起点位置
*/
void OV_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
    uint8_t temp;
#ifdef OV7670RGB
    uint16_t endx;
    uint16_t endy;

    sx = 184 + sx * 2;
    sy = 10 + sy * 2;
    endx = (sx + width * 2) % 784;	
    endy = sy + height * 2;	

    //HREF
    RD_SENSOR_Reg(0X32,&temp);	
    temp&=0XC0;
    temp|=((endx&0X07)<<3)|(sx&0X07);	
    WR_SENSOR_Reg(0X032,temp);
    WR_SENSOR_Reg(0X17,sx>>3);	
    WR_SENSOR_Reg(0X18,endx>>3);	

    //VREF
    RD_SENSOR_Reg(0X03,&temp);	
    temp&=0XF0;
    temp|=((endy&0X03)<<2)|(sy&0X03);
    WR_SENSOR_Reg(0X03,temp);	
    WR_SENSOR_Reg(0X19,sy >> 2);	
    WR_SENSOR_Reg(0X1A,endy >> 2);
#endif

#ifdef OV7670YUV
    uint16_t endx;
    uint16_t endy;

    sx = 184 + sx * 2;
    sy = 10 + sy * 2;
    endx = (sx + width * 2) % 784;	
    endy = sy + height * 2;	

    //HREF
    RD_SENSOR_Reg(0X32,&temp);	
    temp&=0XC0;
    temp|=((endx&0X07)<<3)|(sx&0X07);	
    WR_SENSOR_Reg(0X032,temp);
    WR_SENSOR_Reg(0X17,sx>>3);	
    WR_SENSOR_Reg(0X18,endx>>3);	

    //VREF
    RD_SENSOR_Reg(0X03,&temp);	
    temp&=0XF0;
    temp|=((endy&0X03)<<2)|(sy&0X03);
    WR_SENSOR_Reg(0X03,temp);	
    WR_SENSOR_Reg(0X19,sy >> 2);	
    WR_SENSOR_Reg(0X1A,endy >> 2);
#endif

#ifdef OV7725RGB
    //Voutsize
    WR_SENSOR_Reg(0x2c,(height) >> 1);
    //HREF
    RD_SENSOR_Reg(0x2a,&temp);	
    temp &= ((height & 0x01) << 2);
    WR_SENSOR_Reg(0x2a,temp);   
      
    //Houtsize
    WR_SENSOR_Reg(0x29,(width) >> 2);
    //HREF
    RD_SENSOR_Reg(0x2a,&temp);	
    temp &= ((width & 0x03) << 0);
    WR_SENSOR_Reg(0x2a,temp);  
#endif

#ifdef OV7725YUV
    //Voutsize
    WR_SENSOR_Reg(0x2c,(height) >> 1);
    //HREF
    RD_SENSOR_Reg(0x2a,&temp);	
    temp &= ((height & 0x01) << 2);
    WR_SENSOR_Reg(0x2a,temp);   
      
    //Houtsize
    WR_SENSOR_Reg(0x29,(width) >> 2);
    //HREF
    RD_SENSOR_Reg(0x2a,&temp);	
    temp &= ((width & 0x03) << 0);
    WR_SENSOR_Reg(0x2a,temp);  
#endif
}

//0成功  1失败
u8 OV_QVGAConfig(void)
{
  uint32_t i;

#ifdef OV7670RGB
	for(i=0; i<sizeof(OV7670RGB_reg)/sizeof(OV7670RGB_reg[0]);i++)
  {
		if(WR_SENSOR_Reg(OV7670RGB_reg[i][0],OV7670RGB_reg[i][1])==1)
		{
			return 1;
		}
    delay_ms(5);
  }
#endif
	
#ifdef OV7670YUV
	for(i=0; i<sizeof(OV7670YUV_reg)/sizeof(OV7670YUV_reg[0]);i++)
  {
		if(WR_SENSOR_Reg(OV7670YUV_reg[i][0],OV7670YUV_reg[i][1])==1)
		{
			return 1;
		}
    delay_ms(5);
  }
#endif	
	
#ifdef OV7725RGB
	for(i=0; i<sizeof(OV7725RGB_reg)/sizeof(OV7725RGB_reg[0]);i++)
  {
		if(WR_SENSOR_Reg(OV7725RGB_reg[i][0],OV7725RGB_reg[i][1])==1)
		{
			return 1;
		}
    delay_ms(5);
  }
#endif
	
#ifdef OV7725YUV
	for(i=0; i<sizeof(OV7725YUV_reg)/sizeof(OV7725YUV_reg[0]);i++)
  {
		if(WR_SENSOR_Reg(OV7725YUV_reg[i][0],OV7725YUV_reg[i][1])==1)
		{
			return 1;
		}
    delay_ms(5);
  }
#endif
	
	return 0;
}


//摄像头启动
void OV_Start(void)
{
  	DMA_Cmd(DMA2_Stream1, ENABLE); 
  	DCMI_Cmd(ENABLE); 
  	DCMI_CaptureCmd(ENABLE); 
}


void OV_Stop(void)
{
	DMA_Cmd(DMA2_Stream1, DISABLE); 
  DCMI_Cmd(DISABLE); 
  DCMI_CaptureCmd(DISABLE); 
}

//0成功  1失败
u8 OV_Init(void)
{
	u8 Sensor_IDCode = 0;	
	
	OV_GPIO_Init();
	delay_ms(10); 
	//I2C读取检查ID，不成功则死循环
	if(WR_SENSOR_Reg(0x12, 0x80)==1)
		return 1;	
	delay_ms(10); 
	if(RD_SENSOR_Reg(0x1C,&Sensor_IDCode)==1)	//0x1C对应MIDH，其他只读寄存器值也可以当做检测用，详看手册
		return 1;			
	if(Sensor_IDCode != 0x7F)
		return 1;		
	OV_DCMI_Init();																									//配置DCMI
	OV_DCMI_DMA_Init((uint32_t)Image,IMAGE_COLUMN * IMAGE_ROW/2);		//配置DMA
	if(OV_QVGAConfig()==1)																					//配置OV寄存器
		return 1;
	OV_Window_Set(0,0,IMAGE_COLUMN,IMAGE_ROW);											//图像大小缩减
	return 0;
}
