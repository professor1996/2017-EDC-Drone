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



//ǰ���е����꣺
int Horizontal_Column_BlackMidpoint = 0;

//�����е����꣺
int Vertical_Row_BlackMidpoint = 0;
 
//����ţ�
extern int Task_flag ;

//С���ɻ��������־��
int dis_detection_flag = 0;

//���յ����ݣ�
u8 Communication_Temp = 0;

extern u8 Immedia_Stop_Flag;
 

 
 
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1 , USART_IT_RXNE) == SET)	//���ռĴ����ǿ�
	{		
		USART_ClearITPendingBit(USART1 , USART_IT_RXNE);			//����жϱ�־
		Communication_Temp = USART_ReceiveData(USART1);		

		//��������ģʽ��
		if(Task_flag != 2)
		{
			if(Communication_Temp >= 100 && Communication_Temp < 200 )
			{				 
				Horizontal_Column_BlackMidpoint = Communication_Temp - 100;  //��ͼ������ƫ����Ϣ��
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
//	if(USART1->SR & USART_SR_ORE)//ORE�ж�
//	{
//		com_data = USART1->DR;
//	}

//  //�����ж�
//	if( USART_GetITStatus(USART1,USART_IT_RXNE) )
//	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//����жϱ�־

//  	Communication_Temp = USART_ReceiveData(USART1);		
//		
//		if(Communication_Temp >= 100 )
//		{				 
//		  rotation_angle = Communication_Temp - 100;  //��ͼ������ƫ����Ϣ��
//		}
//		else if(Communication_Temp < 100)                     //��ͼ������������Ϣ��
//		{
//	  	offset_position = Communication_Temp  ;
//		}
//		else if(Communication_Temp == 255)                     //��ͼ�����ս������
//		{
////	  	down_flag_confirm = 1;
////	  	Turn_direc_flag   = 1;
//		}
//	}
//	//���ͣ�������λ���ж�
//	if( USART_GetITStatus(USART1,USART_IT_TXE ) )
//	{
//				
// 
//  
//	}



//}
