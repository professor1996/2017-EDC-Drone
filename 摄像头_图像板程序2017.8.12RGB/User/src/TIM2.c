#include "TIM2.h"



void TIM2_Int_Init(u16 psc , u16 arr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE); //??? TIM3 ??
	
	TIM_TimeBaseInitStructure.TIM_Prescaler= psc; //?????
	TIM_TimeBaseInitStructure.TIM_Period = arr; //??????
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //??????
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2 , &TIM_TimeBaseInitStructure);// ??????? TIM3
	TIM_ITConfig(TIM2 , TIM_IT_Update,ENABLE); //?????? 3 ????
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //??? 3 ??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 4; //????? 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 3; //????? 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// ???? NVIC
	
//	TIM_Cmd(TIM2,ENABLE); //?????? 3
	TIM_Cmd(TIM2,DISABLE); //?????? 3
}


int count_flag_1 = 0;
int count_flag_2 = 1;
int count     ;
void TIM2_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //????
	{
	 	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //???????
		
		
 
//		}
		

	}
	
}


