#include "stm32f10x.h"
#include "Pwm.h"
#include "Led.h"
#include "Can1.h"


int main(void)
{
	TIM3_Init( );
	Led_Config();
	CAN_Configuration();

	while(1)
	{
		start_steering_engine();
	}
}
