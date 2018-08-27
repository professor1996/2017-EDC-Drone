#include "main.h"

u8 UpdataTFT_flag = 0;
int turn_flag = 0 , read_flash_flag = 0 ,write_flash_flag = 0;
unsigned char dwqdqw = 100;

u8 TEXT_Buffer[2]={0};
#define SIZE 2

#define FLASH_SAVE_ADDR  0X0800C004 
u8 datatemp[SIZE];
extern int Thres , color_Thres;


int main()
{

	SystemInit();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Led_Config();
	ili9320_Initializtion();
	usart_init(115200);
	TIM2_Int_Init(100 , 3000);
//	can_config();
	while(OV_Init()==1)			
	{
		LED_TOGLE();
		delay_ms(50);
	}												//OV7725初始化
	LED_OFF();
	delay_ms(200);
	OV_Start();
	delay_ms(200);
	TIM3_Int_Init(200 , 1000);//200

	while(1)
	{
//			Transfer1Gray(Image, Gray_Image_Buffer);		//  YUV422格式转化为灰度图，或亮度图
//			Transfer2Gray(Image, Gray_Image_Buffer);		  //  RGB格式转化为灰度图，或亮度图		
		
	  	/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*向flash存和读阈值Thres，注意，当 flash中没有存阈值时，将read_flash_flag初始化为0，防止阈值变为0；*/
		/*write_flash_flag只有在串口接收到飞控板发来的阈值数据时才置1，将阈值写入flash*/
		if(read_flash_flag) 
		{
			read_flash_flag = 0;
			STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)datatemp,SIZE);
			Thres = datatemp[0];
			color_Thres = datatemp[1];
		}
		if(write_flash_flag)
		{
			TEXT_Buffer[0] = Thres;
			TEXT_Buffer[1] = color_Thres;
			write_flash_flag = 0;
			STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////	
	
		if(UpdataTFT_flag != 0)
		{
			
			//图像处理部分：	  
      Image_normalization_detecting();              //  图像是否正常检测   蓝灯亮说明丢图象或者需要重启图像板
						
			//图像算法任务管理：
			Task_scheduler(Task_flag);
					
		  //图像显示：
			Area_Judge();
			Picture_Display();                                //显示图像
      Value_Display();                                  //重要变量显示
			UpdataTFT_flag--;                                 //清除标志位
		}

  }	

}


//一张图片传输完成，场中断
void DMA2_Stream1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
  {
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
		DCMI_CaptureCmd(DISABLE);			
		UpdataTFT_flag++;
		DCMI_CaptureCmd(ENABLE); 				//中断只负责标志位的管理，处理函数放到中断外面
	}
}

void Picture_Display(void)
{
	
	int i,j,k;
	ili9320_SetCursor(0,0);
	LCD->LCD_REG = R34; 
	for(i = 0 ; i < Row ; i++)
	{
		
		for(j = 0 ;j < Column ; j++)
		{
			LCD->LCD_RAM = *((uint8_t *)Gray_Image_Buffer + i*Column + j);
		}
		
		for(k = Column ; k < IMAGE_COLUMN ; k++)
		{
			LCD->LCD_RAM = Black;
		}

	}
 
}

void Value_Display(void)
{
	ili9320_SetCursor(0,0);
	LCD->LCD_REG = R34; 
	GUI_Text(0,60,(u8*)"Task_flag:", 9 , Black , White);
	float_number_show(120 , 60 , Task_flag);
//	
	GUI_Text(0,80,(u8*)"Column_point:", 12 , Black , White);
	float_number_show(120 , 80 , Horizontal_Column_BlackMidpoint);
//	
	GUI_Text(0,100,(u8*)"Row_Midp:", 8 , Black , White);
	float_number_show(120 , 100 , Vertical_Row_BlackMidpoint);
////	
	GUI_Text(0,120,(u8*)"black_num:", 9 , Black , White);
	float_number_show(120 , 120 , black_num );
	
	GUI_Text(0,140,(u8*)"color_Thres:", 11 , Black , White);
	float_number_show(120 , 140 , color_Thres );
	
	GUI_Text(0,160,(u8*)"Thres:", 5 , Black , White);
	float_number_show(120 , 160 ,  Thres );

	GUI_Text(0,180,(u8*)"dis_detec_flag:", 14 , Black , White);
	float_number_show(120 , 180 , dis_detection_flag );
	
//	GUI_Text(0,200,(u8*)"Line_flag:", 9 , Black , White);
//	float_number_show(120 , 200 , LongLine_detection_flag );
//	
//	GUI_Text(0,220,(u8*)"cross_flag:", 10 , Black , White);
//	float_number_show(120 , 220 , cross_flag );

//	GUI_Text(170,60,(u8*)"Row_Midp:", 8 , Black , White);
//	float_number_show(270 , 60 , Vertical_Row_BlackMidpoint );
//	
//	GUI_Text(170,160,(u8*)"black_num:", 9 , Black , White);
//	float_number_show(270 , 160 , black_num );
	
//	GUI_Text(170,100,(u8*)"stay_altitude_flag:", 9 , Black , White);
//	float_number_show(270 , 100 , stay_altitude_flag );
	
}


//区域标志：
int area_flag = 0;
void Area_Judge(void)
{
	if(First_Column_BlackMidpoint < 40 && First_Column_BlackMidpoint > 0)  //????????,?flag?1
	{
		area_flag = 1;
	}
	else if(First_Column_BlackMidpoint > 40 && First_Column_BlackMidpoint < 80) // ????????,?flag?0
	{
		area_flag = 0;
	}
	
	if(area_flag == 1 && First_Column_BlackMidpoint == 0)
	{
		First_Column_BlackMidpoint = 0;
	}
	else if(area_flag == 0 && First_Column_BlackMidpoint == 0)
	{
		First_Column_BlackMidpoint = 80;
	}
		
}


//图像处理算法管理：
void Task_scheduler(int task_flag)
{
	switch(task_flag)
	{
		case 0: Task_0_imageprocessing();  //用显示屏观察图像是否正常：
		break;
		
		case 1: Task_1_imageprocessing();  //顶点悬停
		break;
		
		case 2: Task_2_imageprocessing();  //??????
		break;
	
		case 3: Task_3_imageprocessing();  //绕圈飞行
		break;
			
		case 4: Task_4_imageprocessing();  //直线飞行并返回
		break;
					
		case 5: Task_5_imageprocessing();  //追小车
		break;
							
		case 6: Task_6_imageprocessing();
		break;
	}
}

//图像观察：
void Task_0_imageprocessing()
{
	GrayImage_twovalue_processing(Image, Gray_Image_Buffer);		 // 获取彩色图像并转化为灰度图二值化
  Direction_Calc(Gray_Image_Buffer);
	Vertical_calc(Gray_Image_Buffer);             //从左右两方向扫取左右两黑点坐标
}

//定点悬停：
void Task_1_imageprocessing()
{
	GrayImage_twovalue_processing(Image, Gray_Image_Buffer);		 // 获取彩色图像并转化为灰度图二值化
  Direction_Calc(Gray_Image_Buffer);
	Vertical_calc(Gray_Image_Buffer);             //从左右两方向扫取左右两黑点坐标
}

//直线飞行并降落：
void Task_2_imageprocessing()
{
 	Red_area_detection(Image , Gray_Image_Buffer);     //彩色图像算法：检测红色小车
	Car_dis_detection();                               //小车距离检测：
}

//绕圈飞行：
void Task_3_imageprocessing()
{
	Red_area_detection(Image , Gray_Image_Buffer);     //彩色图像算法：检测红色小车
	Car_dis_detection();                               //小车距离检测：
  Direction_Calc(Gray_Image_Buffer);            //从前后两方向扫取左右两黑点坐标
	Vertical_calc(Gray_Image_Buffer);             //从左右两方向扫取左右两黑点坐标
}


//直线飞行并返回降落
void Task_4_imageprocessing()
{
	Red_area_detection(Image , Gray_Image_Buffer);     //彩色图像算法：检测红色小车
	Car_dis_detection();                               //小车距离检测：
  Direction_Calc(Gray_Image_Buffer);            //从前后两方向扫取左右两黑点坐标
	Vertical_calc(Gray_Image_Buffer);             //从左右两方向扫取左右两黑点坐标
}

void Task_5_imageprocessing()
{
	Red_area_detection(Image , Gray_Image_Buffer);     //彩色图像算法：检测红色小车
	Car_dis_detection();                               //小车距离检测：
  Direction_Calc(Gray_Image_Buffer);            //从前后两方向扫取左右两黑点坐标
	Vertical_calc(Gray_Image_Buffer);             //从左右两方向扫取左右两黑点坐标
}

void Task_6_imageprocessing()
{
	
}