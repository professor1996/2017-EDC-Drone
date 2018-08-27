#include "PID.h"

float p1 = 0.6 ;
float p3 = 0.4 ;
float p2 = 0.2 ;

float p4 = 0.2 ;

float Pulse1 = 450 ;
float Pulse2 = 350 ;

//void PID_Calculation(int Real_FirstStation , int Real_BehindStation , int Real_SecondStation , int Real_ThirdStation , int Set_Station)        //小车偏右白线中点位置偏小，偏左则偏大 
//{
//	
//	int err1 = 0 , err3 = 0 , err2 = 0 , err4 = 0;
//	float P1 = 0 , P3 = 0 , P2 = 0 , P4 = 0;
//	
//	err1 = Real_FirstStation - Set_Station ;
//	P1 = p1 * err1 ;

//	err3 = Real_SecondStation - Set_Station ;
//	P3 = p3 * err3 ;
//	
//	err2 = Real_BehindStation - Set_Station ;
//	P2 = p2 * err2 ;
//	
////	err4 = Real_ThirdStation - Set_Station ;
////	P4 = p4 * err4 ;
//	
//	
//	Pulse = 450 + P1 + P3 + P2 + P4;

//	if(Triangle_Right_Flag)
//	{
//		Pulse = 515;
//	}
//	else
//	{
//		if(Pulse > 515)
//		{
//			Pulse = 515;
//		}
//		if(Pulse < 385)
//		{
//			Pulse = 385;
//		}
//  }

//	
//}                                                                //脉冲值小于450，向左打转，大于450，向右打转








