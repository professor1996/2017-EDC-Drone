#include "Image.h"
#include "OV7725.h"


u8 gray_Image_Buffer[80][50];
int black_num=0;


u8 Fabs(u8 Pixel)
{
	if(Pixel < 0)
	{
		Pixel = -Pixel;
	}
	else
	{
		Pixel = Pixel;
	}
}

/****************************************************************************
*函数名称: void Transfer2Gray(uint16_t * image,uint8_t  * ccm_image)
*函数功能：YUV422提取8位Y分量，获取灰度图，亮度图
*输入参数：原始图像信息*image,*gray_image用来存储当前图像
****************************************************************************/

int blank_detection_flag = 0;

void Transfer1Gray(uint16_t *image , uint8_t  *gray_image)
{
	uint32_t i = 0;
	uint32_t j = 0;
	black_num = 0;
	for(i = 0 ; i < Row ; i++)
	{	
		for(j = 0 ; j < Column ; j++)
		{					
			*((uint8_t *)gray_image + i*Column + j) = (uint8_t)((*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4)) >> 8);	
			
			if(Gray_Image_Buffer[i*Column + j]  < 50)
			{
				Gray_Image_Buffer[i*Column + j] = 0;
				black_num++;
			}
			else
			{
				Gray_Image_Buffer[i*Column + j] = 240;

			}
		}	
 	}
	

//	if(black_num <= 5)
//	{
//		blank_detection_flag = 1;
//	}
//	
////	if(stay_altitude_flag)
////	{
//	if(blank_detection_flag)
//	{
//		if(Vertical_Row_BlackMidpoint >= 25)
//		{
//			car_start_detection_flag = 1;
//		}	
//	}
////	}

	

}

/****************************************************************************
*函数名称: void Transfer2Gray(uint16_t * image,uint8_t  * ccm_image)
 RGB转灰度图
*输入参数：原始图像信息*image,*gray_image用来存储当前图像
****************************************************************************/
uint8_t r,g,b;
void Transfer2Gray(uint16_t *image , uint8_t  * gray_image)
{
	uint32_t i = 0;
	uint32_t j = 0;
	
	for(i = 0 ; i < Row ; i++)
	{		
		for(j = 0 ; j < Column ; j++)
		{			
			r = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0xF800) >> 8;
			g = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0x07E0) >> 3;
		  b = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0x001F) << 3;
			*((uint8_t *)gray_image  + i*Column + j) = (uint8_t)((r*77 + g*150 + b*29+128) / 256);
		}
 	}			 		
}

int Thres = 100;
void GrayImage_twovalue_processing(uint16_t *image , uint8_t  * gray_image)
{
	uint32_t i = 0;
	uint32_t j = 0;
	black_num = 0;
	for(i = 0 ; i < Row ; i++)
	{		
		for(j = 0 ; j < Column ; j++)
		{			
			r = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0xF800) >> 8;
			g = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0x07E0) >> 3;
		  b = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0x001F) << 3;
			*((uint8_t *)gray_image  + i*Column + j) = (uint8_t)((r*77 + g*150 + b*29+128) / 256);
			
			if(Gray_Image_Buffer[i*Column + j]  > Thres  )
			{
				Gray_Image_Buffer[i*Column + j] = 240;
			}
			else
			{
				Gray_Image_Buffer[i*Column + j] = 0;
				black_num++;
			}
		}
 	}			 		
}


int car_start_detection_flag = 0  ,color_Thres = 50;;
void Red_area_detection(uint16_t *image , uint8_t  * gray_image)
{
	uint32_t i = 0;
	uint32_t j = 0;
	black_num = 0;
	for(i = 0 ; i < Row ; i++)
	{		
		for(j = 0 ; j < Column ; j++)
		{			
			r = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0xF800) >> 8;
			g = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0x07E0) >> 3;
		  b = (*((uint16_t *)image + i*4*IMAGE_COLUMN + j*4) & 0x001F) << 3;
			*((uint8_t *)gray_image  + i*Column + j) = (uint8_t)((r*77 + g*150 + b*29+128) / 256);
			
			if(r - b > color_Thres && r < 200)
			{
				Gray_Image_Buffer[i*Column + j] = 0;
				black_num++;
			}
			else
			{
				Gray_Image_Buffer[i*Column + j] = 240;
			}
		}
 	}
	if(black_num <= 4)
	{
		for(i = 0 ; i < Row ; i++)
		{		
			for(j = 0 ; j < Column ; j++)
			{			
				Gray_Image_Buffer[i*Column + j] = 240;		
			}
		}
	}
}

//小车飞机之间的距离检测：
int dis_detection_flag = 0;
void Car_dis_detection()
{
	if(black_num > 90 && black_num < 500)
	{
		dis_detection_flag = 1;               //飞机和小车距离0.5~1.5m
	}
	else
	{
		dis_detection_flag = 0;
	}
}

void Twovalue_Processing(u8 *Gray_Image_Buffer)
{
	uint32_t i = 0;
	uint32_t j = 0;
  int err1 = 0 , err2 = 0;
	int leftvalue = 0;
	int rightvalue = 0;
	black_num = 0;
	for(i = 0 ; i < Row ; i++)
	{
		
		for(j = 0 ; j < Column ; j++)
		{			

			if(Gray_Image_Buffer[i*Column + j]  > 20  )
			{
				Gray_Image_Buffer[i*Column + j] = 240;
			}
			else
			{
				Gray_Image_Buffer[i*Column + j] = 0;
				black_num++;
			}
			
		}
		
	}
		
}

void Image_normalization_detecting(void)
{
	if(black_num <= 5)    //像素总量太小，要么丢图像，要么线没接好
	{
		LED_ON();
	}
	else
	{
		LED_OFF();
	}
}


////方案如下：从第一行开始扫，扫到出现第一个黑线中点，从最后一行开始扫，扫到出现第一个黑线中点。
////根据这两点坐标的大小判断飞机方向，用第一个黑线中点作为循线控制信息。
//u8 First_Column_BlackMidpoint;
//u8 Last_Column_BlackMidpoint;
//u8 middle_Column_BlackMidpoint;
//int switch_mode_flag=0;
//int Line_count = 1,Line_detection_flag = 0,err = 0;
//extern int Start_line_detection_flag ;
//int Line_flag = 1,off_count_flag=0,end_mode2_flag=0,open_mode2_flag=0;
//int Horizontal_Column_BlackMidpoint = 0;
//void Direction_Calc(u8 *Gray_Image_Buffer)    
//{
//	int err1 = 0 , err2 = 0;
//	int leftvalue = 0;
//	int rightvalue = 0;
//	uint32_t i = 0;
//	uint32_t j = 0;
//	int First_tar_Row = 0;
//	int Last_tar_Row  = 0 ,Middle_tar_Row = 0;
//	int break_flag = 0;
//	int back_width=0,front_width=0,middle_width = 0,point_f=0,point_m=0,point_l=0;
//	int x_err1,x_err2,x_err3,max_err,min_err;
//	int First_tar = 0;
//  int Last_tar = 0;
//	
//	//找到最后一行坐标中点
//	for(i = 0 ; i < Row ; i++)
//	{			
//		for(j = 0 ; j < Column ; j++)
//		{
//			if(Gray_Image_Buffer[i * Column + j] == 0)
//			{
//				Last_tar_Row = i ;
//				break_flag = 1;
//				break;
//			}
//		}
//		if(break_flag)
//		{
//			break;
//		}
//	}
//	
//	break_flag = 0;
//	
//	if(Gray_Image_Buffer[Last_tar_Row * Column + (1 - 1)] == 240 && Gray_Image_Buffer[Last_tar_Row * Column + (Column - 1)] == 240)
//	{
//		for(j = 1 ; j < Column ; j++)
//		{			
//			err1 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
//			if(err1 >= 100)
//			{
//				rightvalue =  j - 1;
//			}
//			
//			err2 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
//			if(err2 <= -100)
//			{
//				leftvalue = j;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[Last_tar_Row * Column + (1 - 1)] == 0)
//	{
//		for(j = 1 ; j < Column ; j++)
//		{			
//			leftvalue = 1;
//			
//			err1 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
//			if(err1 >= 100)
//			{
//				rightvalue =  j - 1;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[Last_tar_Row * Column + (Column - 1)] == 0)
//	{
//		for(j = 1 ; j < Column ; j++)
//		{			
//			err2 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
//			if(err2 <= -100)
//			{
//				leftvalue =  j;
//			}
//			
//			rightvalue = Column ;
//		}
//	}
// 
//	Last_Column_BlackMidpoint = (leftvalue + rightvalue) / 2.0 ;

//	
//	//找到第一行坐标中点
//	for(i = Row - 1 ; i > 0 ; i--)
//	{			
//		for(j = 0 ; j < Column ; j++)
//		{
//			if(Gray_Image_Buffer[i * Column + j] == 0)
//			{
//				First_tar_Row = i ;
//				break_flag = 1;
//				break;
//			}
//		}
//		if(break_flag)
//		{
//			break;
//		}
//	}
//	
//	break_flag = 0;
//	
//	if(Gray_Image_Buffer[First_tar_Row * Column + (1 - 1)] == 240 && Gray_Image_Buffer[First_tar_Row * Column + (Column - 1)] == 240)
//	{
//		for(j = 1 ; j < Column ; j++)
//		{			
//			err1 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
//			if(err1 >= 100)
//			{
//				rightvalue =  j - 1;
//			}
//			
//			err2 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
//			if(err2 <= -100)
//			{
//				leftvalue = j;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[First_tar_Row * Column + (1 - 1)] == 0)
//	{
//		for(j = 1 ; j < Column ; j++)
//		{			
//			leftvalue = 1;
//			
//			err1 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
//			if(err1 >= 100)
//			{
//				rightvalue =  j - 1;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[First_tar_Row * Column + (Column - 1)] == 0)
//	{
//		for(j = 1 ; j < Column ; j++)
//		{			
//			err2 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
//			if(err2 <= -100)
//			{
//				leftvalue =  j;
//			}
//			
//			rightvalue = Column ;
//		}
//	}

//	First_Column_BlackMidpoint = (leftvalue + rightvalue) / 2.0 ;
// 
//	Horizontal_Column_BlackMidpoint = (First_Column_BlackMidpoint + Last_Column_BlackMidpoint) / 2.0;
//}

// 


////方案如下：从第一行开始扫，扫到出现第一个黑线中点，从最后一行开始扫，扫到出现第一个黑线中点。
//////////根据这两点坐标的大小判断飞机方向，用第一个黑线中点作为循线控制信息。
////检测前后方向的线坐标：
//int First_Row_BlackMidpoint;
//int Last_Row_BlackMidpoint;
//int Vertical_Row_BlackMidpoint ;
//int First_tar_Column = 0;
//int Last_tar_Column  = 0;
//void Vertical_calc(u8 *Gray_Image_Buffer)  
//{
//	int err1 = 0 , err2 = 0;
//	int downvalue = 0;
//	int upvalue = 0;
//	uint32_t i = 0;
//	uint32_t j = 0;
//	int break_flag = 0;
// 
//	//找到第一列的黑线中点：
//	for(i = 0 ; i < Column ; i++)
//	{
//		
//		for(j = 0 ; j < Row ; j++)
//		{			
//			if(Gray_Image_Buffer[j * Column + i] == 0)
//			{
//				First_tar_Column = i ;
//				break_flag = 1;
//				break;
//			}

//		}
//		if(break_flag)
//		{
//			break;
//		}
//	}	
//	
//	break_flag = 0;
//	 
//	if(Gray_Image_Buffer[First_tar_Column + Column * (1 - 1)] == 240 && Gray_Image_Buffer[First_tar_Column + Column * (Row - 1)] == 240)
//	{
//		for(j = 1 ; j < Row ; j++)
//		{			
//			err1 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
//			if(err1 >= 100)
//			{
//				downvalue =  j - 1;
//			}
//			
//			err2 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
//			if(err2 <= -100)
//			{
//				upvalue = j;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[First_tar_Column + Column * (1 - 1)] == 0)
//	{
//		for(j = 1 ; j < Row ; j++)
//		{			
//			upvalue = 1;
//			
//      err1 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
//			if(err1 >= 100)
//			{
//				downvalue =  j - 1;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[First_tar_Column + Column * (Row - 1)] == 0)
//	{
//		for(j = 1 ; j < Row ; j++)
//		{			
//			err2 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
//			if(err2 <= -100)
//			{
//				upvalue =  j;
//			}
//			
//			downvalue = Row ;
//		}
//	}
//	First_Row_BlackMidpoint = (upvalue + downvalue) / 2.0;
//	
//	
//	
//		//找到最后一列的黑线中点：
//	for(i = Column - 1 ; i > 0 ; i--)
//	{	
//		for(j = 0 ; j < Row ; j++)
//		{			
//			if(Gray_Image_Buffer[j * Column + i] == 0)
//			{
//				Last_tar_Column = i ;
//				break_flag = 1;
//				break;
//			}
//		}
//		if(break_flag)
//		{
//			break;
//		}
//	}	

//	break_flag = 0;
//	 
//	if(Gray_Image_Buffer[Last_tar_Column + Column * (1 - 1)] == 240 && Gray_Image_Buffer[Last_tar_Column + Column * (Row - 1)] == 240)
//	{
//		for(j = 1 ; j < Row ; j++)
//		{			
//			err1 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
//			if(err1 >= 100)
//			{
//				downvalue =  j - 1;
//			}
//			
//			err2 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
//			if(err2 <= -100)
//			{
//				upvalue = j;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[Last_tar_Column + Column * (1 - 1)] == 0)
//	{
//		for(j = 1 ; j < Row ; j++)
//		{			
//			upvalue = 1;
//			
//      err1 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
//			if(err1 >= 100)
//			{
//				downvalue =  j - 1;
//			}
//		}
//	}
//	else if(Gray_Image_Buffer[Last_tar_Column + Column * (Row - 1)] == 0)
//	{
//		for(j = 1 ; j < Row ; j++)
//		{			
//			err2 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
//			if(err2 <= -100)
//			{
//				upvalue =  j;
//			}
//			
//			downvalue = Row ;
//		}
//	}
//	Last_Row_BlackMidpoint = (upvalue + downvalue) / 2.0;
//	
//	Vertical_Row_BlackMidpoint = (First_Row_BlackMidpoint + Last_Row_BlackMidpoint) / 2.0;
//}

 




//方案如下：从第一行开始扫，扫到出现第一个黑线中点，从最后一行开始扫，扫到出现第一个黑线中点。
//根据这两点坐标的大小判断飞机方向，用第一个黑线中点作为循线控制信息。
 
void Car_detection(u8 *Gray_Image_Buffer)    
{
	uint32_t i = 0;
	uint32_t j = 0;
	int err =  0;
	u8 border_num = 0;
	
	//两列全部清零：
	for(i = 0 ; i < Row ; i++)
	{	
		Gray_Image_Buffer[i*Column + 0]  = 240;
		Gray_Image_Buffer[i*Column + 79] = 240;
	}
	
	for(i = 0 ; i < Row ; i++)
	{	
		for(j = 1 ; j < Column ; j++)
		{					
      err = Gray_Image_Buffer[i * Column + j] - Gray_Image_Buffer[i * Column + (j - 1)] ;
			if(err != 0)
			{
				border_num++;
			}
		}	
		if(border_num == 2)
		{
			for(j = 0 ; j < Column ; j++)
			{					
				Gray_Image_Buffer[i * Column + j] = 240;
			}	
		}
		border_num = 0;
 	}			 

}

///////////////////////////////////////////////////////////////////////////////////

////彩色部分：

u8 First_Column_BlackMidpoint;
u8 Last_Column_BlackMidpoint;
u8 middle_Column_BlackMidpoint;
int switch_mode_flag=0;
int Line_count = 1,Line_detection_flag = 0,err = 0;
extern int Start_line_detection_flag ;
int Line_flag = 1,off_count_flag=0,end_mode2_flag=0,open_mode2_flag=0;
int Horizontal_Column_BlackMidpoint = 0;
void Direction_Calc(u8 *Gray_Image_Buffer)    
{
	int err1 = 0 , err2 = 0;
	int leftvalue = 0;
	int rightvalue = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	int First_tar_Row = 0;
	int Last_tar_Row  = 0 ,Middle_tar_Row = 0;
	int break_flag = 0;
	int back_width=0,front_width=0,middle_width = 0,point_f=0,point_m=0,point_l=0;
	int x_err1,x_err2,x_err3,max_err,min_err;
	int First_tar = 0;
  int Last_tar = 0;
	int max_image = 0;
	
	//??????????
	for(i = 0 ; i < Row ; i++)
	{			
		for(j = 0 ; j < Column ; j++)
		{
			if(Gray_Image_Buffer[i * Column + j] == 0)
			{
				max_image ++;
			}
		}
		if(max_image > 4)
		{
			  Last_tar_Row = i ;
		  	max_image = 0;
				break;
		}
		max_image = 0;
	}
	
	break_flag = 0;
	
	if(Gray_Image_Buffer[Last_tar_Row * Column + (1 - 1)] == 240 && Gray_Image_Buffer[Last_tar_Row * Column + (Column - 1)] == 240)
	{
		for(j = 1 ; j < Column ; j++)
		{			
			err1 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
			if(err1 >= 100)
			{
				rightvalue =  j - 1;
			}
			
			err2 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
			if(err2 <= -100)
			{
				leftvalue = j;
			}
		}
	}
	else if(Gray_Image_Buffer[Last_tar_Row * Column + (1 - 1)] == 0)
	{
		for(j = 1 ; j < Column ; j++)
		{			
			leftvalue = 1;
			
			err1 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
			if(err1 >= 100)
			{
				rightvalue =  j - 1;
			}
		}
	}
	else if(Gray_Image_Buffer[Last_tar_Row * Column + (Column - 1)] == 0)
	{
		for(j = 1 ; j < Column ; j++)
		{			
			err2 = Gray_Image_Buffer[Last_tar_Row * Column + j] - Gray_Image_Buffer[Last_tar_Row * Column + (j - 1)] ;
			if(err2 <= -100)
			{
				leftvalue =  j;
			}
			
			rightvalue = Column ;
		}
	}
 
	Last_Column_BlackMidpoint = (leftvalue + rightvalue) / 2.0 ;

	
	//?????????
	for(i = Row - 1 ; i > 0 ; i--)
	{			
		for(j = 0 ; j < Column ; j++)
		{
			if(Gray_Image_Buffer[i * Column + j] == 0)
			{
				max_image++;
			}
		}
		if(max_image > 4)
		{
			First_tar_Row = i ;
			max_image = 0;
			break;
		}
		max_image = 0;
	}
	
	break_flag = 0;
	
	if(Gray_Image_Buffer[First_tar_Row * Column + (1 - 1)] == 240 && Gray_Image_Buffer[First_tar_Row * Column + (Column - 1)] == 240)
	{
		for(j = 1 ; j < Column ; j++)
		{			
			err1 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
			if(err1 >= 100)
			{
				rightvalue =  j - 1;
			}
			
			err2 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
			if(err2 <= -100)
			{
				leftvalue = j;
			}
		}
	}
	else if(Gray_Image_Buffer[First_tar_Row * Column + (1 - 1)] == 0)
	{
		for(j = 1 ; j < Column ; j++)
		{			
			leftvalue = 1;
			
			err1 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
			if(err1 >= 100)
			{
				rightvalue =  j - 1;
			}
		}
	}
	else if(Gray_Image_Buffer[First_tar_Row * Column + (Column - 1)] == 0)
	{
		for(j = 1 ; j < Column ; j++)
		{			
			err2 = Gray_Image_Buffer[First_tar_Row * Column + j] - Gray_Image_Buffer[First_tar_Row * Column + (j - 1)] ;
			if(err2 <= -100)
			{
				leftvalue =  j;
			}
			
			rightvalue = Column ;
		}
	}

	First_Column_BlackMidpoint = (leftvalue + rightvalue) / 2.0 ;
 
	Horizontal_Column_BlackMidpoint = (First_Column_BlackMidpoint + Last_Column_BlackMidpoint) / 2.0;
}

 


//????:???????,???????????,????????,????????????
////////????????????????,?????????????????
//??????????:
int First_Row_BlackMidpoint;
int Last_Row_BlackMidpoint;
int Vertical_Row_BlackMidpoint ;
int First_tar_Column = 0;
int Last_tar_Column  = 0;
void Vertical_calc(u8 *Gray_Image_Buffer)  
{
	int err1 = 0 , err2 = 0;
	int downvalue = 0;
	int upvalue = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	int break_flag = 0;
	int max_image = 0;
 
	//??????????:
	for(i = 0 ; i < Column ; i++)
	{
		
		for(j = 0 ; j < Row ; j++)
		{			
			if(Gray_Image_Buffer[j * Column + i] == 0)
			{
				max_image++;
			}

		}
		if(max_image > 4)
		{
			First_tar_Column = i ;
			max_image = 0;
			break;
		}
		max_image = 0;
	}	
	
	break_flag = 0;
	 
	if(Gray_Image_Buffer[First_tar_Column + Column * (1 - 1)] == 240 && Gray_Image_Buffer[First_tar_Column + Column * (Row - 1)] == 240)
	{
		for(j = 1 ; j < Row ; j++)
		{			
			err1 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
			if(err1 >= 100)
			{
				downvalue =  j - 1;
			}
			
			err2 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
			if(err2 <= -100)
			{
				upvalue = j;
			}
		}
	}
	else if(Gray_Image_Buffer[First_tar_Column + Column * (1 - 1)] == 0)
	{
		for(j = 1 ; j < Row ; j++)
		{			
			upvalue = 1;
			
      err1 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
			if(err1 >= 100)
			{
				downvalue =  j - 1;
			}
		}
	}
	else if(Gray_Image_Buffer[First_tar_Column + Column * (Row - 1)] == 0)
	{
		for(j = 1 ; j < Row ; j++)
		{			
			err2 = Gray_Image_Buffer[First_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[First_tar_Column + Column * (j - 1)] ;
			if(err2 <= -100)
			{
				upvalue =  j;
			}
			
			downvalue = Row ;
		}
	}
	First_Row_BlackMidpoint = (upvalue + downvalue) / 2.0;
	
	
	
		//???????????:
	for(i = Column - 1 ; i > 0 ; i--)
	{	
		for(j = 0 ; j < Row ; j++)
		{			
			if(Gray_Image_Buffer[j * Column + i] == 0)
			{
				max_image ++;
			}
		}
	  if(max_image > 4)
		{
			Last_tar_Column = i ;
			max_image = 0;
			break;
		}
		max_image = 0;
	}	

	break_flag = 0;
	 
	if(Gray_Image_Buffer[Last_tar_Column + Column * (1 - 1)] == 240 && Gray_Image_Buffer[Last_tar_Column + Column * (Row - 1)] == 240)
	{
		for(j = 1 ; j < Row ; j++)
		{			
			err1 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
			if(err1 >= 100)
			{
				downvalue =  j - 1;
			}
			
			err2 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
			if(err2 <= -100)
			{
				upvalue = j;
			}
		}
	}
	else if(Gray_Image_Buffer[Last_tar_Column + Column * (1 - 1)] == 0)
	{
		for(j = 1 ; j < Row ; j++)
		{			
			upvalue = 1;
			
      err1 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
			if(err1 >= 100)
			{
				downvalue =  j - 1;
			}
		}
	}
	else if(Gray_Image_Buffer[Last_tar_Column + Column * (Row - 1)] == 0)
	{
		for(j = 1 ; j < Row ; j++)
		{			
			err2 = Gray_Image_Buffer[Last_tar_Column + Column * (j - 0)] - Gray_Image_Buffer[Last_tar_Column + Column * (j - 1)] ;
			if(err2 <= -100)
			{
				upvalue =  j;
			}
			
			downvalue = Row ;
		}
	}
	Last_Row_BlackMidpoint = (upvalue + downvalue) / 2.0;
	
	Vertical_Row_BlackMidpoint = (First_Row_BlackMidpoint + Last_Row_BlackMidpoint) / 2.0;
}

 





