#include "TIM3.h"



void TIM3_Int_Init(u16 psc , u16 arr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //??? TIM3 ??
	
	TIM_TimeBaseInitStructure.TIM_Prescaler= psc; //?????
	TIM_TimeBaseInitStructure.TIM_Period = arr; //??????
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //??????
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// ??????? TIM3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //?????? 3 ????
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //??? 3 ??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //????? 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3; //????? 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// ???? NVIC
	
	TIM_Cmd(TIM3,ENABLE); //?????? 3
}

//数据发送切换标志：
u8 change_flag = 1;

//计数标志：
int flag = 1;

//降落标志：  由图像板给出
int Down_Flag = 0;
int Down_Flag_Count = 1;

int last_point;

//像素点移动速度，并近似作为飞行器偏移速度
float speed = 0;

//发送包头标志：
int flag_baotou = 1;
int flag_send   = 0;

//任务号标志：
extern int Task_flag ;

//飞行器前后位置坐标：
extern int Vertical_Row_BlackMidpoint;
//飞行器左右位置坐标：
extern int Horizontal_Column_BlackMidpoint;
//空白检测标志：
extern int car_start_detection_flag;
//长条直线检测标志：
extern int LongLine_detection_flag;

void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
	 	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 

		Sending_scheduler(Task_flag);
		
	}
	
	
	
}


void Sending_scheduler(int task_flag)
{
	switch(task_flag)
	{
		case 0: Sending_task_0();  // 定点悬停
		break;
		
		case 1: Sending_task_1();  // 定点悬停
		break;
		
		case 2: Sending_task_2();  // 直线飞行并降落
		break;
				
		case 3: Sending_task_3();  //  
		break;
		
		case 4: Sending_task_4();  // 直线飞行并返回
		break;
		
		case 5: Sending_task_5();  // 追小车
		break;
	}
}

void Sending_task_0()
{
  //原地定点悬停模式：

	if(change_flag == 1)//发送期望偏移角速度：
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //发送期望YAW轴角度：
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 1;
	}
}



void Sending_task_1()
{
  //原地定点悬停模式：

	if(change_flag == 1)//发送期望偏移角速度：
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //发送期望YAW轴角度：
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 2;
	}
	else if(change_flag == 2) //发送期望YAW轴角度：
	{
//		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 1;
	}
}


extern int dis_detection_flag;
void Sending_task_2()
{
 		USART_SendData(USART2 , dis_detection_flag);
}



void Sending_task_3()
{
	 //飞机追小车模式：
	if(change_flag == 1)//发送期望偏移角速度：
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //发送期望YAW轴角度：
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 2;
	}
	else if(change_flag == 2)
	{
		if(dis_detection_flag)
		{
		  USART_SendData(USART2 ,  200);
		}
		else
		{
		  USART_SendData(USART2 ,  201);			
		}
		change_flag = 1;
	}

}

void Sending_task_4()
{		
	 //飞机追小车模式：
	if(change_flag == 1)//发送期望偏移角速度：
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //发送期望YAW轴角度：
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 2;
	}
	else if(change_flag == 2)
	{
		if(dis_detection_flag)
		{
		  USART_SendData(USART2 ,  200);
		}
		else
		{
		  USART_SendData(USART2 ,  201);			
		}
		change_flag = 1;
	}
}

void Sending_task_5()
{
	 //飞机追小车模式：
	if(change_flag == 1)//发送期望偏移角速度：
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //发送期望YAW轴角度：
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 2;
	}
	else if(change_flag == 2)
	{
		if(dis_detection_flag)
		{
		  USART_SendData(USART2 ,  200);
		}
		else
		{
		  USART_SendData(USART2 ,  201);			
		}
		change_flag = 1;
	}
}







