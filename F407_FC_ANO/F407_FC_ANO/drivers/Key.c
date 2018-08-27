#include "Key.h"


void KEY_Init()     
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//GPIOE??
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//??????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOC, &GPIO_InitStructure);//???GPIOE2,3,4
	

}

int task_num = 0;         //???
int Key_confirm_flag = 0;
void Key_func()
{
	if(KEY_STATE == 0)	 
	{
	  Key_confirm_flag = 1;
		 task_num+=KEY_STATE;
		 if(task_num==6)task_num=0;
	}
}
