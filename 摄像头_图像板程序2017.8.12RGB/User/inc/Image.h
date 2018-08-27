#ifndef _IMAGE_H
#define _IMAGE_H

#include "stm32f4xx.h"
#include "TIM3.h"
#include "math.h"
#include "Usart.h"
#include "led.h"


#define IMAGE_COLUMN 320
#define IMAGE_ROW    200

#define Column 80
#define Row 50

#define FirstRow_StandardStation 80

extern u8 Row_WhiteMidpoint ;
extern u8 Column_WhiteMidpoint  ;
extern int Horizontal_Column_BlackMidpoint;
extern int Last_Row_BlackMidpoint;
extern int Ten_flag ;

u8 Fabs(u8 Pixel);
void Transfer1Gray(uint16_t *image,uint8_t  *gray_image);
void GrayImage_twovalue_processing(uint16_t *image , uint8_t  * gray_image);
void Transfer2Gray(uint16_t *image,uint8_t  *gray_image);
void Red_area_detection(uint16_t *image , uint8_t  * gray_image);
void Twovalue_Processing(u8 *Gray_Image_Buffer);
void Image_normalization_detecting(void);

int get_max(int x1,int y1,int z1);
int get_min(int x1,int y1,int z1);

void Direction_Calc(u8 *Gray_Image_Buffer);
void Blank_Detection(u8 *Gray_Image_Buffer);
void LongLine_Detection(u8 *Gray_Image_Buffer);
void CircleMove_Line_Detection(u8 *Gray_Image_Buffer);
void line_detection_2(u8 *Gray_Image_Buffer);
void Line_detection3(u8 *Gray_Image_Buffer);
void Line_detection4(u8 *Gray_Image_Buffer);
void cross_detection(u8 *Gray_Image_Buffer);
void Car_detection(u8 *Gray_Image_Buffer)   ;
void Vertical_calc(u8 *Gray_Image_Buffer) ;
void Car_dis_detection();
extern int FirstRow_WhiteMid_Station ;
extern int SecondRow_WhiteMid_Station ;
extern int ThirdRow_WhiteMid_Station ;
extern int BehindRow_WhiteMid_Station  ;
extern int lost_image_flag ;
extern int WhiteLine  ;
extern int Triangle_Right_Flag ;
extern u8 First_Column_BlackMidpoint;
extern u8 Last_Column_BlackMidpoint;
extern int Circle_Detection_flag ;
extern int First_tar_Column  ;
extern int Last_tar_Column  ;
extern int err ;
extern int Line_detection_flag,Line_count,black_num;
extern int First_Row_BlackMidpoint;
extern int Last_Row_BlackMidpoint;
extern int Vertical_Row_BlackMidpoint ;
extern int LongLine_detection_flag ;
extern int start_cross_dec_flag,cross_flag;
extern int forwardBlank_Detection_flag;
extern int err_line;
extern int stay_altitude_flag;
extern int cross_num;
extern int cal_tim_flag;
extern uint8_t r,g,b;
extern int move_forward_flag;
extern int dis_detection_flag ;
#define abs(x) ((x)>0? (x):(-(x)))




#endif


























