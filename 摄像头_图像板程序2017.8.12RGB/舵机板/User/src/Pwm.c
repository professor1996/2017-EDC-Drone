#include "Pwm.h"


float Pwm = 450;
void TIM3_Init(void)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	
  TIM_TimeBaseStructure.TIM_Prescaler = 240 - 1 ;    
	TIM_TimeBaseStructure.TIM_Period = 6000 ; 
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;//TIM_CKD_DIV1;	  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;        
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0 ;        
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	
  TIM_OC1Init(TIM3 , &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3 , TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM3 , ENABLE);   
	TIM_CtrlPWMOutputs(TIM3 , ENABLE);
	TIM_Cmd(TIM3 , ENABLE);	
	
	
}


void start_steering_engine(void)
{
	TIM3->CCR1 = Pwm ;
}










