#include "Can1.h"
//VP230---CAN_TX---PA12(CANTX) 
//VP230---CAN_RX---PA11(CANRX) 


u16 ID_Master_TEST = 0x145;




void CAN_Configuration(void)
{

	CAN_FilterInitTypeDef  can_filter;		
	GPIO_InitTypeDef 	   gpio;
	CAN_InitTypeDef        can;
  NVIC_InitTypeDef   	   nvic;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,  ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO , ENABLE);	

	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_IPU;       
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);	//CAN_RX 								 

	gpio.GPIO_Pin = GPIO_Pin_12;	   
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);    //CAN_TX
	
	CAN_DeInit(CAN1);

	can.CAN_TTCM = DISABLE;
	can.CAN_ABOM = DISABLE;
	can.CAN_AWUM = DISABLE;
	can.CAN_NART = DISABLE;	  
	can.CAN_RFLM = DISABLE;																
	can.CAN_TXFP = ENABLE;		
 	can.CAN_Mode = CAN_Mode_Normal;
	can.CAN_SJW = CAN_SJW_1tq;
	can.CAN_BS1 = CAN_BS1_5tq;
	can.CAN_BS2 = CAN_BS2_3tq;
	can.CAN_Prescaler = 3;     
	CAN_Init(CAN1, &can);
	
	nvic.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	can_filter.CAN_FilterNumber=0;
	can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
	can_filter.CAN_FilterScale=CAN_FilterScale_32bit;	
	can_filter.CAN_FilterIdHigh=((((u32)ID_Master_TEST)<<21)&0xffff0000)>>16;
	can_filter.CAN_FilterIdLow=((((u32)ID_Master_TEST)<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
	can_filter.CAN_FilterMaskIdHigh=0xFFFF;
	can_filter.CAN_FilterMaskIdLow=0xFFFF;	
	can_filter.CAN_FilterFIFOAssignment=0;			
	can_filter.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&can_filter);
    
  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	  
}

void USB_LP_CAN1_RX0_IRQHandler(void)                  //CAN RX
{

	CanRxMsg rx_message;
  if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		
		if((rx_message.IDE == CAN_Id_Standard)&&(rx_message.RTR == CAN_RTR_Data))          //สýพÝึก
		{
			
			if((rx_message.StdId == ID_Master_TEST) && (rx_message.DLC==0x08))	      
			{
				DecodeFloatData(&Pwm , &rx_message.Data[0]);	
			}
			
	  }
  }
}






