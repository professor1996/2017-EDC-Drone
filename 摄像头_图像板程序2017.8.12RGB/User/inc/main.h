#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"
#include "ILI9325.h"
#include "OV7725.h"
#include "Sccb.h"
#include "Image.h"
#include "Can1.h"
#include "PID.h"
#include "MyDataProcess.h"
#include "Usart.h"
#include "TIM3.h"
#include "ili9325_api.h"
#include "Show_Number.h"
#include "TIM2.h"
#include "stmflash.h"



void Picture_Display(void);
void Value_Display(void);
void Area_Judge(void);

void Task_scheduler(int task_flag);
void Task_0_imageprocessing();
void Task_1_imageprocessing();
void Task_2_imageprocessing();
void Task_3_imageprocessing();
void Task_4_imageprocessing();
void Task_5_imageprocessing();
void Task_6_imageprocessing();
	
extern int Blank_Detection_flag;
extern int cross_num ;
extern int err_line ;
extern int stay_altitude_flag;


