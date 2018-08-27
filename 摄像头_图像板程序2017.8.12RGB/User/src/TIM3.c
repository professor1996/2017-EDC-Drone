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

//���ݷ����л���־��
u8 change_flag = 1;

//������־��
int flag = 1;

//�����־��  ��ͼ������
int Down_Flag = 0;
int Down_Flag_Count = 1;

int last_point;

//���ص��ƶ��ٶȣ���������Ϊ������ƫ���ٶ�
float speed = 0;

//���Ͱ�ͷ��־��
int flag_baotou = 1;
int flag_send   = 0;

//����ű�־��
extern int Task_flag ;

//������ǰ��λ�����꣺
extern int Vertical_Row_BlackMidpoint;
//����������λ�����꣺
extern int Horizontal_Column_BlackMidpoint;
//�հ׼���־��
extern int car_start_detection_flag;
//����ֱ�߼���־��
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
		case 0: Sending_task_0();  // ������ͣ
		break;
		
		case 1: Sending_task_1();  // ������ͣ
		break;
		
		case 2: Sending_task_2();  // ֱ�߷��в�����
		break;
				
		case 3: Sending_task_3();  //  
		break;
		
		case 4: Sending_task_4();  // ֱ�߷��в�����
		break;
		
		case 5: Sending_task_5();  // ׷С��
		break;
	}
}

void Sending_task_0()
{
  //ԭ�ض�����ͣģʽ��

	if(change_flag == 1)//��������ƫ�ƽ��ٶȣ�
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //��������YAW��Ƕȣ�
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 1;
	}
}



void Sending_task_1()
{
  //ԭ�ض�����ͣģʽ��

	if(change_flag == 1)//��������ƫ�ƽ��ٶȣ�
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //��������YAW��Ƕȣ�
	{
		USART_SendData(USART2 , Vertical_Row_BlackMidpoint);
		change_flag = 2;
	}
	else if(change_flag == 2) //��������YAW��Ƕȣ�
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
	 //�ɻ�׷С��ģʽ��
	if(change_flag == 1)//��������ƫ�ƽ��ٶȣ�
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //��������YAW��Ƕȣ�
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
	 //�ɻ�׷С��ģʽ��
	if(change_flag == 1)//��������ƫ�ƽ��ٶȣ�
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //��������YAW��Ƕȣ�
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
	 //�ɻ�׷С��ģʽ��
	if(change_flag == 1)//��������ƫ�ƽ��ٶȣ�
	{
		USART_SendData(USART2 , Horizontal_Column_BlackMidpoint + 100);   //(ROLL_gyro_tar + 1) * 100
		change_flag = 0 ;
	}
	else if(change_flag == 0) //��������YAW��Ƕȣ�
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







