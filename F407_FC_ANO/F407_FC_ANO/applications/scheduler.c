/******************** (C) COPYRIGHT 2014 ANO Tech ********************************
  * 作者   ：匿名科创
 * 文件名  ：scheduler.c
 * 描述    ：任务调度
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
**********************************************************************************/
#include "scheduler.h"
#include "include.h"
#include "time.h"
#include "mpu6050.h"
#include "ak8975.h"
#include "led.h"
#include "rc.h"
#include "imu.h"
#include "pwm_in.h"
#include "ctrl.h"
#include "ms5611.h"
#include "parameter.h"
#include "ultrasonic.h"
#include "height_ctrl.h"

s16 loop_cnt;
int start_mpu_flag = 0;

loop_t loop;

void Loop_check()  //TIME INTTERRUPT
{
	loop.time++; //u16
	loop.cnt_2ms++;
	loop.cnt_5ms++;
	loop.cnt_10ms++;
	loop.cnt_20ms++;
	loop.cnt_50ms++;

	if( loop.check_flag == 1)
	{
		loop.err_flag ++;     //每累加一次，证明代码在预定周期内没有跑完。
	}
	else
	{	
		loop.check_flag = 1;	//该标志位在循环的最后被清零
	}
}

void Duty_1ms()
{
	Get_Cycle_T(1);
	LED_Display( LED_Brightness );								//20级led渐变显示
//	ANO_DT_Data_Exchange();												//数传通信定时调用

}

float test[5];
extern int Task_flag ;
void Duty_2ms()
{
	float inner_loop_time;
	
	inner_loop_time = Get_Cycle_T(0); 						//获取内环准确的执行周期
	
	test[0] = GetSysTime_us()/1000000.0f;
	
	MPU6050_Read(); 															//读取mpu6轴传感器

	MPU6050_Data_Prepare( inner_loop_time );			//mpu6轴传感器数据处理
	
	//////////////////////////////////////////////////////////////

  Key_func();                  //按键使能
	
	Task_scheduler(Task_flag);   //任务分配

  if(start_mpu_flag == 1)
	{
	  CTRL_1( inner_loop_time ); 			//内环角速度控制。输入：执行周期，期望角速度，测量角速度，角度前馈；输出：电机PWM占空比。<函数未封装>
	}

	
	//////////////////////////////////////////////////////////////////
	test[1] = GetSysTime_us()/1000000.0f;
}

//
int Stay_altitude_flag = 0;
int change_flag_1 = 1;
void Duty_5ms()
{
	float outer_loop_time;
	
	outer_loop_time = Get_Cycle_T(2);								//获取外环准确的执行周期
	
	test[2] = GetSysTime_us()/1000000.0f;
	
	/*IMU更新姿态。输入：半个执行周期，三轴陀螺仪数据（转换到度每秒），三轴加速度计数据（4096--1G）；输出：ROLPITYAW姿态角*/
 	IMUupdate(0.5f *outer_loop_time,mpu6050.Gyro_deg.x, mpu6050.Gyro_deg.y, mpu6050.Gyro_deg.z, mpu6050.Acc.x, mpu6050.Acc.y, mpu6050.Acc.z,&Roll,&Pitch,&Yaw);
	
	//////////////////////////////////////////////////////////////////////////////
	Yaw = 0;

	Ruisa_commun();              //与瑞萨通信：
	
	if(start_ctrl3_flag == 1)
	{
    CTRL_3(); 											// 开启循线
	}
	
  if(start_mpu_flag == 1)
	{
    CTRL_2( outer_loop_time ); 			// 外环角度控制。输入：执行周期，期望角度（摇杆量），姿态角度；输出：期望角速度。<函数未封装>
	}


	
	///////////////////////////////////////////////////////////////////////////////
	test[3] = GetSysTime_us()/1000000.0f;
}

void Duty_10ms()
{
// 		if( MS5611_Update() ) 				//更新ms5611气压计数据
//		{	
//			baro_ctrl_start = 1;  //20ms
//		}
		
	  ANO_AK8975_Read();			//获取电子罗盘数据	
}

void Duty_20ms()
{
	Parameter_Save();
}

void Duty_50ms()
{
	Mode();
	LED_Duty();								//LED任务
	Ultra_Duty();
}


void Duty_Loop()   					//最短任务周期为1ms，总的代码执行时间需要小于1ms。
{
  
//	if(time_1s > 10 && start_mpu_flag==0) 
//	{
//		start_mpu_flag = 1;
//	}
	
	if( loop.check_flag == 1 )
	{
		loop_cnt = time_1ms;
		
		Duty_1ms();							//周期1ms的任务
		
		if( loop.cnt_2ms >= 2 )
		{
			loop.cnt_2ms = 0;
			Duty_2ms();						//周期2ms的任务
		}
		if( loop.cnt_5ms >= 5 )
		{
			loop.cnt_5ms = 0;
			Duty_5ms();						//周期5ms的任务
		}
		if( loop.cnt_10ms >= 10 )
		{
			loop.cnt_10ms = 0;
			Duty_10ms();					//周期10ms的任务
		}
		if( loop.cnt_20ms >= 20 )
		{
			loop.cnt_20ms = 0;
			Duty_20ms();					//周期20ms的任务
		}
		if( loop.cnt_50ms >= 50 )
		{
			loop.cnt_50ms = 0;
			Duty_50ms();					//周期50ms的任务
		}
		
		loop.check_flag = 0;		//循环运行完毕标志
	}
}




	/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/
	

