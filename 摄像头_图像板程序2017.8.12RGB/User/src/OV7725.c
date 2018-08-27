#include "OV7725.h"
#include "ILI9325.h"
#include "delay.h"
#include "OV_Config.h"
#include "Image.h"

u16 Image[IMAGE_COLUMN * IMAGE_ROW] = {0};							//����ͷԭʼ����
uint8_t * Gray_Image_Buffer = ((uint8_t *)0x10000000);		//8λ�Ҷ�ͼ
/**********************************************
OV7725�ܽ�����
DCMI:PA4,6,9,10, PB6,7, PH11,12,14, PI6,7;
PWDN:PG7
RST:PG8
XCLK:PA8
***********************************************/
void OV_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
	//��ʼ��DCMI�ܽ�
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

	//��ʼ��PWDN�ܽź�RST�ܽ�
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
	
	//ģ��I2C�ܽų�ʼ��
  SCCB_GPIO_Config();
	
	//XCLK�ܽų�ʼ��
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
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;			//�����ɼ�ģʽ
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;				//Ӳ��ͬ��( HSYNC��VSYNC)
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;					//PCLK(����ʱ�Ӳ�����)Ϊ�½���
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;							//��ֱͬ������						
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;								//ˮƽͬ������
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;				//����֡������(ֻ����������ģʽ����Ч)
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;		//ÿ������ʱ�Ӳ���10λ����(D0-D1��û���ݣ���ʵ��Ӳ�������)
	
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
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DCMI->DR);		//���ֲ��֪DR�Ĵ���ƫ�Ƶ�ַΪ0x28
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)address;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					//���赽�ڴ�
  DMA_InitStructure.DMA_BufferSize = buffer_size;														
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//�ڴ��ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//�������ݿ��Ϊ32λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//�ڴ����ݿ��16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;													//DMAѭ������
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
    ��������OV����ʾ����,sx,sy��ͼ������λ��
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

//0�ɹ�  1ʧ��
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


//����ͷ����
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

//0�ɹ�  1ʧ��
u8 OV_Init(void)
{
	u8 Sensor_IDCode = 0;	
	
	OV_GPIO_Init();
	delay_ms(10); 
	//I2C��ȡ���ID�����ɹ�����ѭ��
	if(WR_SENSOR_Reg(0x12, 0x80)==1)
		return 1;	
	delay_ms(10); 
	if(RD_SENSOR_Reg(0x1C,&Sensor_IDCode)==1)	//0x1C��ӦMIDH������ֻ���Ĵ���ֵҲ���Ե�������ã��꿴�ֲ�
		return 1;			
	if(Sensor_IDCode != 0x7F)
		return 1;		
	OV_DCMI_Init();																									//����DCMI
	OV_DCMI_DMA_Init((uint32_t)Image,IMAGE_COLUMN * IMAGE_ROW/2);		//����DMA
	if(OV_QVGAConfig()==1)																					//����OV�Ĵ���
		return 1;
	OV_Window_Set(0,0,IMAGE_COLUMN,IMAGE_ROW);											//ͼ���С����
	return 0;
}
