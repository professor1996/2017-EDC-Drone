#include "Can1.h"
 

void can_config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}
//void Can1_Configuration(void)
//{
//		CAN_InitTypeDef can1;
//		GPIO_InitTypeDef       gpio;
//		CAN_FilterInitTypeDef  can_filter;
//		NVIC_InitTypeDef       nvic;
//	
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	
//		GPIO_PinAFConfig(GPIOA, GPIO_PinSource11,GPIO_AF_CAN1);
//		GPIO_PinAFConfig(GPIOA, GPIO_PinSource12,GPIO_AF_CAN1);
//	
//		gpio.GPIO_Mode=GPIO_Mode_AF;
//		gpio.GPIO_Pin=GPIO_Pin_11 | GPIO_Pin_12;
//		GPIO_Init(GPIOA, &gpio);
//	
////		nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
////		nvic.NVIC_IRQChannelPreemptionPriority =3;
////		nvic.NVIC_IRQChannelSubPriority = 4;
////		nvic.NVIC_IRQChannelCmd = ENABLE;
////		NVIC_Init(&nvic);
////	
//				
//		CAN_DeInit(CAN1);
//		CAN_StructInit(&can1);
//		
//		can1.CAN_TTCM = DISABLE;
//		can1.CAN_ABOM = DISABLE;
//		can1.CAN_AWUM = DISABLE;
//		can1.CAN_NART = DISABLE;
//		can1.CAN_RFLM = DISABLE;
//		can1.CAN_TXFP = ENABLE;
//		can1.CAN_Mode = CAN_Mode_Normal;
//		can1.CAN_SJW  = CAN_SJW_1tq;
//		can1.CAN_BS1 = CAN_BS1_9tq;
//		can1.CAN_BS2 = CAN_BS2_4tq;
//		can1.CAN_Prescaler = 3;        
//		CAN_Init(CAN1, &can1);
//		
//		can_filter.CAN_FilterNumber=0;
//		can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
//		can_filter.CAN_FilterScale=CAN_FilterScale_32bit;	
//		can_filter.CAN_FilterIdHigh=((0x00000101<<21)&0xffff0000)>>16;
//	  can_filter.CAN_FilterIdLow=((0x00000101<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
//	  can_filter.CAN_FilterMaskIdHigh=0xFFFF;
//	  can_filter.CAN_FilterMaskIdLow=0xFFFF;	
//		can_filter.CAN_FilterFIFOAssignment=0;		
//		can_filter.CAN_FilterActivation=ENABLE;
//		CAN_FilterInit(&can_filter);
//			
//		CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);	
//}



//void SendPosition(s16 data1 , s16 data2)
//{
//	CanTxMsg tx_message;
//	tx_message.StdId = 0x145;							//报文ID号
//	tx_message.DLC = 0x08;								//数据字节数
//	tx_message.RTR = CAN_RTR_Data;				//数据帧
//	tx_message.IDE = CAN_Id_Standard;			//标准ID
//	
//	EncodeS16Data(&data1,&tx_message.Data[0]);
//	EncodeS16Data(&data2,&tx_message.Data[2]);
//	CAN_Transmit(CAN1,&tx_message);
//}






/////*
////*******************************************
////* Name : CAN1_RX0_IRQHandler
////* Describe : 接受来自其他传感器的信号并记录
////* Input : None
////* Output : None
////* Return : None
////*******************************************
////*/
////int m = 0;
////void CAN1_RX0_IRQHandler(void)
////{
////	//int i = 0;
////	CanRxMsg rx_message;
////	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
////	{
////		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);	
////		switch(rx_message.StdId)
////		{
////			case 0x0101:
////				{				
////					DecodeS16Data(&(HuorDistance),&rx_message.Data[0]);
////					DecodeS16Data(&(DirectionFlag),&rx_message.Data[2]);
////					DecodeFloatData(&(UltrosonicDistance),&rx_message.Data[4]);
////				}
////			default:break;
////		}
////		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
////		CAN_ClearFlag(CAN1, CAN_IT_FMP0);		
////	}
////}
