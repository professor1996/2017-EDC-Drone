/******************** (C) COPYRIGHT 2014 ANO Tech ********************************
  * 作者   ：匿名科创
 * 文件名  ：led.c
 * 描述    ：LED驱动
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
**********************************************************************************/

#include "led.h"
#include "include.h"

void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_StructInit(&GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(ANO_RCC_LED,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin   = ANO_Pin_LED1| ANO_Pin_LED2| ANO_Pin_LED3| ANO_Pin_LED4;
	GPIO_Init(ANO_GPIO_LED, &GPIO_InitStructure);
	
	GPIO_SetBits(ANO_GPIO_LED, ANO_Pin_LED1);		
	GPIO_SetBits(ANO_GPIO_LED, ANO_Pin_LED2);		
	GPIO_SetBits(ANO_GPIO_LED, ANO_Pin_LED3);		
	GPIO_SetBits(ANO_GPIO_LED, ANO_Pin_LED4);		
}

void KEY_Init()     
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//GPIOE??
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//??????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOC, &GPIO_InitStructure);//???GPIOE2,3,4
	

}

//void Buzzer_Init()     
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//GPIOE??
// 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOC , &GPIO_InitStructure);
//	
//	GPIO_ResetBits(GPIOC , GPIO_Pin_9);
//}

extern int fly_motorvalue_setting  ;//设置起飞油门标志：
extern int Task_flag; //最终任务号
extern int fly_ready; // 起飞信号
int task_num = 0;    // task_num为0，飞行器不进行任何操作
int last_task_num = 0;
int Key_confirm_flag = 0;
int Key_confirm_count = 1;

int task_confirm_flag = 0;
int task_confirm_count = 1;
int ruisa_sending_flag = 1;
extern int fly_buffer_flag;
extern int start_setmode_flag;
int final_task_confirm_flag = 0;
int final_key_confirm_flag  = 0;
int final_task_confirm_count = 1;

void Key_func()
{
	last_task_num = task_num;
	if(KEY_STATE == 0)	    //有按键按下
	{
		if(final_task_confirm_flag == 0)
		{
			Key_confirm_flag = 1;
			Key_confirm_count = 1;
			task_confirm_flag = 0;
			task_confirm_count = 1;
			fly_motorvalue_setting = 1;
			fly_ready = 0;
		}
		
		if(final_task_confirm_flag)
		{
			final_key_confirm_flag = 1 ;
		}
	}
	
	//按键确认标志：
	if(Key_confirm_flag)
	{
		Key_confirm_count++;
		if(Key_confirm_count >= 50)
		{
			fly_ready = 0;
			fly_motorvalue_setting = 1;
			Key_confirm_count = 1;
			Key_confirm_flag = 0;
			task_num++;
			
			if(task_num == 8)
			{
				task_num = 0;
			}
		}
	}
	
	//任务标号变化，进行判断：
	if(task_num != last_task_num)
	{
		Task_flag = task_num ;
		task_confirm_flag = 1;
	}
	//经过2ms * 3000 = 6s后确认最终任务号
	if(task_confirm_flag)
	{
		task_confirm_count++;
		if(task_confirm_count >= 100 && ruisa_sending_flag == 1)
		{
			start_setmode_flag = 1;
			ruisa_sending_flag = 0;
		}
		if(task_confirm_count >= 2500)
		{
			task_confirm_count = 1;
			task_confirm_flag = 0;
			final_task_confirm_flag = 1;
			LED4_ON;
		}
	}
	
	//最终确认起飞标志：
	if(final_task_confirm_flag == 1 && final_key_confirm_flag == 1)
	{
		final_task_confirm_count++;
		if(final_task_confirm_count >= 3000)
		{
			final_task_confirm_flag = 0;
			final_key_confirm_flag  = 0;
			final_task_confirm_count = 1;
			start_mpu_flag = 1;
			fly_ready = 1;
		}
	}
	
	//如果任务数等于0，则强制不进行飞行运动
	if(Task_flag == 0 || Task_flag == 2)
	{
		fly_ready = 0;
		start_mpu_flag = 0;
	}
	
}


//上电后给一定时间缓冲：
int fly_ready_buffer_flag = 1;
int fly_ready_buffer_count = 1;
extern int dis_detection_flag;
void LED_Display( u8 duty[4] ) //0~20
{
//	LED1_ON;   // 蓝
//	LED2_ON;   // 绿
//	LED3_ON;   // 红
	
	//按图像R G B 来记颜色

	switch(Task_flag)
	{
		case 0: LED1_OFF;LED2_OFF;LED3_OFF;  //黑色
		break;
		
		case 1: LED3_ON;LED2_OFF;LED1_OFF;   //红色
		break;
		
		case 2: if(dis_detection_flag == 0)LED2_ON;LED3_OFF;LED1_OFF;   //绿色
		break;
		
		case 3: LED1_ON;LED2_OFF;LED3_OFF;   //蓝色
		break;
		
		case 4: LED3_ON;LED2_ON;LED1_OFF;    //黄色
		break;
		
		case 5: LED3_ON;LED1_ON;LED2_OFF;    //紫色
		break;
		
		case 6: LED2_ON;LED1_ON;LED3_OFF;    //青色
		break;
 
	}
 
//	LED4_ON;  // 固定蓝
	
//	static u8 LED_cnt[4];
//	u8 i;
//	
//	for(i=0;i<4;i++)
//	{
//		if( LED_cnt[i] < 19 )
//		{
//			LED_cnt[i]++;
//		}
//		else
//		{
//			LED_cnt[i] = 0;
//		}
//			
//		if( LED_cnt[i] < duty[i] )
//		{
//			switch(i)
//			{
//				case 0:	
//					LED1_ON;
//				break;
//				case 1:	
//					LED2_ON;
//				break;
//				case 2:	
//					LED3_ON;
//				break;
//				case 3:	
//					LED4_ON;
//				break;
//			}
//		}
//		else
//		{
//						switch(i)
//			{
//				case 0:	
//					LED1_OFF;
//				break;
//				case 1:	
//					LED2_OFF;
//				break;
//				case 2:	
//					LED3_OFF;
//				break;
//				case 3:	
//					LED4_OFF;
//				break;
//			}
//		}
//	}
}

#include "rc.h"
#include "ak8975.h"

extern u8 height_ctrl_mode;

u8 LED_Brightness[4] = {0,20,0,0}; //TO 20
u8 LED_status[2];  //  0:old;  1:now
void LED_Duty()
{
	static s16 led_temp;
	static u8 f;
	
	if(Mag_CALIBRATED)
	{
		LED_status[1] = 1;
	}
	else if(height_ctrl_mode==1)
	{
		LED_status[1] = 2;
	}
	else if(height_ctrl_mode==2)
	{
		LED_status[1] = 3;
	}
	else if(height_ctrl_mode==0)
	{
		LED_status[1] = 0;
	}
	
	switch(LED_status[1])
	{
		case 0:
			if(!fly_ready)
			{
				if( f )
				{
					led_temp += 4;
					if(led_temp>100) f = 0;
				}
				else
				{
					led_temp -= 4;
					if(led_temp < 0 ) f = 1;
				}
				
				LED_Brightness[1] = led_temp/10;
				LED_Brightness[2] = led_temp/10;
				LED_Brightness[3] = led_temp/10;
			}
			else
			{
				LED_Brightness[0] = 0;
				LED_Brightness[1] = 0;
				LED_Brightness[2] = 0;
				LED_Brightness[3] = 20;
			}
		break;
		case 1:
				if( f )
				{
					led_temp += 12;
					if(led_temp>100) f = 0;
				}
				else
				{
					led_temp -= 12;
					if(led_temp < 0 ) f = 1;
				}
				LED_Brightness[0] = 0;			
				LED_Brightness[1] = led_temp/10;
				LED_Brightness[2] = 0;
				LED_Brightness[3] = led_temp/10;
		break;
		case 2:
			if(!fly_ready)
			{
				if( f )
				{
					led_temp += 4;
					if(led_temp>100) f = 0;
				}
				else
				{
					led_temp -= 4;
					if(led_temp < 0 ) f = 1;
				}
				
				LED_Brightness[1] = 0;
				LED_Brightness[2] = led_temp/10;
				LED_Brightness[3] = led_temp/10;
			}
			else
			{
				LED_Brightness[0] = 0;
				LED_Brightness[1] = 0;
				LED_Brightness[2] = 12;
				LED_Brightness[3] = 12;
			}
		break;
		case 3:
			if(!fly_ready)
			{
				if( f )
				{
					led_temp += 4;
					if(led_temp>100) f = 0;
				}
				else
				{
					led_temp -= 4;
					if(led_temp < 0 ) f = 1;
				}
				
				LED_Brightness[1] = led_temp/10;
				LED_Brightness[2] = led_temp/10;
				LED_Brightness[3] = 0;
			}
			else
			{
				LED_Brightness[0] = 0;
				LED_Brightness[1] = 12;
				LED_Brightness[2] = 12;
				LED_Brightness[3] = 0;
			}
		break;
				
		default:break;

	}

}

void LED_MPU_Err(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	while(1)
	{
		LED1_ON;
		Delay_ms(150);
		LED1_OFF;
		Delay_ms(200);
		
	}
}

void LED_Mag_Err(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	while(1)
	{
		LED1_ON;
		Delay_ms(150);
		LED1_OFF;
		Delay_ms(150);
		LED1_ON;
		Delay_ms(150);
		LED1_OFF;
		Delay_ms(600);
	}
}

void LED_MS5611_Err(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	while(1)
	{
		LED1_ON;
		Delay_ms(150);
		LED1_OFF;
		Delay_ms(150);
		
		LED1_ON;
		Delay_ms(150);
		LED1_OFF;
		Delay_ms(150);
		
		LED1_ON;
		Delay_ms(150);
		LED1_OFF;
		Delay_ms(600);
	}

}

/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/

