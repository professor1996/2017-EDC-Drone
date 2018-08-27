#include "Usart.h"



void usart_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//GPIOA ? USART1 ?????
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //?? GPIOA ??
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//?? USART1 ??
	//USART_DeInit(USART1); //???? 1 ?
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //PA9 ??? USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //PA10???USART1
	//USART1_TX PA.9 PA.10 ?
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA9 ? GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //?? 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
	GPIO_Init(GPIOA,&GPIO_InitStructure); //??? PA9,PA10
	//USART ????? ?
	USART_InitStructure.USART_BaudRate = bound;//????? 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//??? 8 ?????
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//?????
	USART_InitStructure.USART_Parity = USART_Parity_No;//??????
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //????
	USART_Init(USART2, &USART_InitStructure); //?????

	USART_ITConfig(USART2 , USART_IT_RXNE , ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	USART_ClearFlag (USART2 , USART_IT_RXNE);
	USART_ClearITPendingBit(USART2 , USART_IT_RXNE);
	
	USART_Cmd(USART2, ENABLE); //???? 
}

//从飞控板接收数据：
u8 Temp = 0;

//飞行器目前高度：
int Rotor_attitude = 0;

//飞行器PITCH轴目前角度
int PITCH          = 0;

//任务号标志：
int Task_flag      = 0;

//开启定高模式标志：
int stay_altitude_flag = 0;

//开始检测横线标志：
int Start_line_detection_flag = 0;

//往前飞标志：
int move_forward_flag = 0;

//阈值
extern int Thres ,write_flash_flag , color_Thres; 

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2 , USART_IT_RXNE) == SET)	//接收寄存器非空
	{		
//		USART_ClearFlag (USART2,USART_IT_RXNE);			//清除中断标志
		USART_ClearITPendingBit(USART2 , USART_IT_RXNE);		
		Temp = USART_ReceiveData(USART2);		
		

	  if(Temp <= 10)
		{
			Task_flag = Temp;
		}
		else if(Temp > 10 && Temp <= 110)
		{
			color_Thres = Temp -10;
			write_flash_flag = 1;
		}
		else if(Temp > 110)
		{
			Thres = Temp - 110;
			write_flash_flag = 1;
		}
	}
}





		
		