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
	}												//OV7725��ʼ��
	LED_OFF();
	delay_ms(200);
	OV_Start();
	delay_ms(200);
	TIM3_Int_Init(200 , 1000);//200

	while(1)
	{
//			Transfer1Gray(Image, Gray_Image_Buffer);		//  YUV422��ʽת��Ϊ�Ҷ�ͼ��������ͼ
//			Transfer2Gray(Image, Gray_Image_Buffer);		  //  RGB��ʽת��Ϊ�Ҷ�ͼ��������ͼ		
		
	  	/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*��flash��Ͷ���ֵThres��ע�⣬�� flash��û�д���ֵʱ����read_flash_flag��ʼ��Ϊ0����ֹ��ֵ��Ϊ0��*/
		/*write_flash_flagֻ���ڴ��ڽ��յ��ɿذ巢������ֵ����ʱ����1������ֵд��flash*/
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
			
			//ͼ�����֣�	  
      Image_normalization_detecting();              //  ͼ���Ƿ��������   ������˵����ͼ�������Ҫ����ͼ���
						
			//ͼ���㷨�������
			Task_scheduler(Task_flag);
					
		  //ͼ����ʾ��
			Area_Judge();
			Picture_Display();                                //��ʾͼ��
      Value_Display();                                  //��Ҫ������ʾ
			UpdataTFT_flag--;                                 //�����־λ
		}

  }	

}


//һ��ͼƬ������ɣ����ж�
void DMA2_Stream1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
  {
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
		DCMI_CaptureCmd(DISABLE);			
		UpdataTFT_flag++;
		DCMI_CaptureCmd(ENABLE); 				//�ж�ֻ�����־λ�Ĺ����������ŵ��ж�����
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


//�����־��
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


//ͼ�����㷨����
void Task_scheduler(int task_flag)
{
	switch(task_flag)
	{
		case 0: Task_0_imageprocessing();  //����ʾ���۲�ͼ���Ƿ�������
		break;
		
		case 1: Task_1_imageprocessing();  //������ͣ
		break;
		
		case 2: Task_2_imageprocessing();  //??????
		break;
	
		case 3: Task_3_imageprocessing();  //��Ȧ����
		break;
			
		case 4: Task_4_imageprocessing();  //ֱ�߷��в�����
		break;
					
		case 5: Task_5_imageprocessing();  //׷С��
		break;
							
		case 6: Task_6_imageprocessing();
		break;
	}
}

//ͼ��۲죺
void Task_0_imageprocessing()
{
	GrayImage_twovalue_processing(Image, Gray_Image_Buffer);		 // ��ȡ��ɫͼ��ת��Ϊ�Ҷ�ͼ��ֵ��
  Direction_Calc(Gray_Image_Buffer);
	Vertical_calc(Gray_Image_Buffer);             //������������ɨȡ�������ڵ�����
}

//������ͣ��
void Task_1_imageprocessing()
{
	GrayImage_twovalue_processing(Image, Gray_Image_Buffer);		 // ��ȡ��ɫͼ��ת��Ϊ�Ҷ�ͼ��ֵ��
  Direction_Calc(Gray_Image_Buffer);
	Vertical_calc(Gray_Image_Buffer);             //������������ɨȡ�������ڵ�����
}

//ֱ�߷��в����䣺
void Task_2_imageprocessing()
{
 	Red_area_detection(Image , Gray_Image_Buffer);     //��ɫͼ���㷨������ɫС��
	Car_dis_detection();                               //С�������⣺
}

//��Ȧ���У�
void Task_3_imageprocessing()
{
	Red_area_detection(Image , Gray_Image_Buffer);     //��ɫͼ���㷨������ɫС��
	Car_dis_detection();                               //С�������⣺
  Direction_Calc(Gray_Image_Buffer);            //��ǰ��������ɨȡ�������ڵ�����
	Vertical_calc(Gray_Image_Buffer);             //������������ɨȡ�������ڵ�����
}


//ֱ�߷��в����ؽ���
void Task_4_imageprocessing()
{
	Red_area_detection(Image , Gray_Image_Buffer);     //��ɫͼ���㷨������ɫС��
	Car_dis_detection();                               //С�������⣺
  Direction_Calc(Gray_Image_Buffer);            //��ǰ��������ɨȡ�������ڵ�����
	Vertical_calc(Gray_Image_Buffer);             //������������ɨȡ�������ڵ�����
}

void Task_5_imageprocessing()
{
	Red_area_detection(Image , Gray_Image_Buffer);     //��ɫͼ���㷨������ɫС��
	Car_dis_detection();                               //С�������⣺
  Direction_Calc(Gray_Image_Buffer);            //��ǰ��������ɨȡ�������ڵ�����
	Vertical_calc(Gray_Image_Buffer);             //������������ɨȡ�������ڵ�����
}

void Task_6_imageprocessing()
{
	
}