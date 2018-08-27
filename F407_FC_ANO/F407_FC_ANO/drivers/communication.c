#include "communication.h"



void Usart1_Init(u32 br_num)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //??USART2??
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	
	
	//???????
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	//??PD5??USART2 Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	//??PD6??USART2 Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//??USART1
	//??????
	USART_InitStructure.USART_BaudRate = 115200;       //????????????
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8???
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //??????1????
	USART_InitStructure.USART_Parity = USART_Parity_No;    //??????
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //???????
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //???????
	//??USART2??
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //???????
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK??????????->???
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //?????????????
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //?????????????SCLK??
	
	USART_Init(USART1, &USART_InitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStruct);

	//??USART2????
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//??USART2
	USART_Cmd(USART1, ENABLE);
}



//前后行的坐标：
int Horizontal_Column_BlackMidpoint = 0;

//左右列的坐标：
int Vertical_Row_BlackMidpoint = 0;
 
//任务号：
extern int Task_flag ;

//小车飞机距离检测标志：
int dis_detection_flag = 0;

//接收的数据：
u8 Communication_Temp = 0;

extern u8 Immedia_Stop_Flag;
 

 
 
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1 , USART_IT_RXNE) == SET)	//接收寄存器非空
	{		
		USART_ClearITPendingBit(USART1 , USART_IT_RXNE);			//清除中断标志
		Communication_Temp = USART_ReceiveData(USART1);		

		//接收数据模式：
		if(Task_flag != 2)
		{
			if(Communication_Temp >= 100 && Communication_Temp < 200 )
			{				 
				Horizontal_Column_BlackMidpoint = Communication_Temp - 100;  //从图像板接收偏移信息：
			}
			
			if(Communication_Temp < 100)
			{
				Vertical_Row_BlackMidpoint = Communication_Temp;	 
			}
			
			if(Communication_Temp == 200)
			{
				dis_detection_flag = 1;
			}
			else if(Communication_Temp == 201)
			{
				dis_detection_flag = 0;
			}
		}
		if(Task_flag == 2)
		{
			dis_detection_flag = Communication_Temp;
		}
	}
}

//void Usart1_IRQ(void)
//{
//	u8 com_data;
//	
//	if(USART1->SR & USART_SR_ORE)//ORE中断
//	{
//		com_data = USART1->DR;
//	}

//  //接收中断
//	if( USART_GetITStatus(USART1,USART_IT_RXNE) )
//	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除中断标志

//  	Communication_Temp = USART_ReceiveData(USART1);		
//		
//		if(Communication_Temp >= 100 )
//		{				 
//		  rotation_angle = Communication_Temp - 100;  //从图像板接收偏移信息：
//		}
//		else if(Communication_Temp < 100)                     //从图像板接收自旋信息：
//		{
//	  	offset_position = Communication_Temp  ;
//		}
//		else if(Communication_Temp == 255)                     //从图像板接收降落命令：
//		{
////	  	down_flag_confirm = 1;
////	  	Turn_direc_flag   = 1;
//		}
//	}
//	//发送（进入移位）中断
//	if( USART_GetITStatus(USART1,USART_IT_TXE ) )
//	{
//				
// 
//  
//	}



//}
