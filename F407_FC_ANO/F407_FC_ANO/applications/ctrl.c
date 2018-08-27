/******************** (C) COPYRIGHT 2014 ANO Tech ********************************
  * 作者   ：匿名科创
 * 文件名  ：ctrl.c
 * 描述    ：飞控控制
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
**********************************************************************************/

#include "ctrl.h"
#include "height_ctrl.h"

ctrl_t ctrl_1;
ctrl_t ctrl_2;
int over_flag = 0;
extern int stay_altitude_flag;



void Ctrl_Para_Init()		//设置默认参数
{
//====================================
	ctrl_1.PID[PIDROLL].kdamp  = 1;
	ctrl_1.PID[PIDPITCH].kdamp = 1;
	ctrl_1.PID[PIDYAW].kdamp 	 = 1;
	
	ctrl_1.FB = 0.20;   //外  0<fb<1
}

xyz_f_t except_A = {0,0,0};

xyz_f_t ctrl_angle_offset = {0,0,0};

xyz_f_t compensation;


/*                     以下为本人自己加的函数：                               */

//任务调度：
int Task_flag = 0;
void Task_scheduler(int task) 
{
	if(task != 2)
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	}
	switch(task)
	{
		case 1: Task_1();  //A区定点悬停
		break;
		
		case 2: Task_2();  //发出声光指示
		break;
		
		case 3: Task_3();  //8点飞至小车上方悬停并降落
		break;
		
		case 4: Task_4();  //8点飞至小车上方悬停并追随小车至2点
		break;
		
		case 5: Task_5();  //8点飞至小车上方悬停并追随小车至任意4点
		break;
		
		case 6: Task_6();  // 
		break;

	}
}


//与瑞萨板通信：
int start_setmode_flag = 0;
int send_time = 0;
void Ruisa_commun()
{
/*向垃圾瑞萨发模式号*/
	if(start_setmode_flag)
	{
		start_setmode_flag = 0;
		switch (Task_flag)
		{
			case 1:send_time = 40;break;
			case 2:send_time = 80;break;
			case 3:send_time = 120;break;
			case 4:send_time = 160;break;
			case 5:send_time = 200;break;
		}
	}
	if(send_time !=0)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_14);
		send_time --;
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	}
}

//任务1： A区原地定点悬停：

//给图像板发送任务号标志：
int task_send_flag = 1;

//PITCH角度控制量：
float Move_y_angle = 0;

//发送标志计数：
int SendData_count = 496;

//停止标志计数：
int Stop_rotor_count = 0;
extern u8 Immedia_Stop_Flag;
void Task_1()
{
	
	//发送任务号：
	SendData_count++;
	if(SendData_count >= 500)
	{
		SendData_count = 1;
		USART_SendData(USART1 , Task_flag);
	}
	
	
	if(start_ctrl3_flag)
	{
		Stop_rotor_count++;
		if(Stop_rotor_count >= 4000)     //悬停2ms * 10000 = 20s
		{
			Stop_rotor_count = 1;
			Immedia_Stop_Flag = 1;         //原地降落
		}
	}
}




//任务2：靠近小车时会有声光报警：
extern u8 Immedia_Stop_Flag;
extern int dis_detection_flag;
extern int Buzzer_start_flag;
extern int open_sound_flag;
extern int close_sound_flag;
int close_acc = 0;
void Task_2()
{
		//发送任务号：
	SendData_count++;
	if(SendData_count >= 500)
	{
		SendData_count = 1;
		USART_SendData(USART1 , Task_flag);
	}
	
	/////////////////////////////////////

		
		
		
	if(Buzzer_start_flag)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_15); 
		LED1_ON;LED2_ON;LED3_ON;
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_15); 
	}
}


//任务3：

//横线检测标志：
 
extern u8 Immedia_Stop_Flag;

 
//A区定点计时：
int A_area_stable_count = 1;

//往前飞标志：
int move_forward_flag = 1;
int move_stable_count = 1;

extern float Turn_x_angle;

//降落5s计时：
int Task_3_downcount = 1;
//择机降落标志：
u8 Choice_down_flag = 0;
 
int send_one_time_flag = 0;
int open_sound_flag;
int close_sound_flag;
void Task_3()
{
	//发送任务号：
	SendData_count++;
	if(SendData_count >= 500)
	{
		SendData_count = 1;
		USART_SendData(USART1 , Task_flag );
	}

	if(start_ctrl3_flag)
	{
		Stop_rotor_count++;
		if(Stop_rotor_count >= 4000)     //悬停2ms * 10000 = 20s
		{
			Stop_rotor_count = 1;
			Choice_down_flag = 1;    //择地降落
		}
	}
	
//	//小车距离检测：
	if(start_ctrl3_flag)
	{
		if(dis_detection_flag)
		{
			if(open_sound_flag) 
			{
				USART_SendData(USART2 , 100);
			}
			open_sound_flag = 0;
		}
		else
		{
			close_acc++;
			if(close_acc > 50) 
			{
				close_acc = 0;
				close_sound_flag = 1;
			}
			if(close_sound_flag)
			{
				USART_SendData(USART2 , 101);
				open_sound_flag = 1;
			}
			close_sound_flag = 0;
		}			
  }
	if(start_ctrl3_flag == 0)
	{
		USART_SendData(USART2 , 101);
	}


	
}


//任务4：
int fly_motorvalue_setting = 1;
extern float Task_4_kp ;
extern int noloading_motorvalue;
extern int fly_motorvalue_offset;
void Task_4()
{
	//发送任务号：
	SendData_count++;
	if(SendData_count >= 500)
	{
		SendData_count = 1;
		USART_SendData(USART1 , Task_flag);
	}

 
//	//小车距离检测：
	if(start_ctrl3_flag)
	{
		if(dis_detection_flag)
		{
			if(open_sound_flag) 
			{
				USART_SendData(USART2 , 100);
			}
			open_sound_flag = 0;
		}
		else
		{
			close_acc++;
			if(close_acc > 50) 
			{
				close_acc = 0;
				close_sound_flag = 1;
			}
			if(close_sound_flag)
			{
				USART_SendData(USART2 , 101);
				open_sound_flag = 1;
			}
			close_sound_flag = 0;
		}			
  }
	if(start_ctrl3_flag == 0)
	{
		USART_SendData(USART2 , 101);
	}

}
	
void Task_5()
{
		//发送任务号：
	SendData_count++;
	if(SendData_count >= 500)
	{
		SendData_count = 1;
		USART_SendData(USART1 , Task_flag);
	}
	 
//	//小车距离检测：
	if(start_ctrl3_flag)
	{
		if(dis_detection_flag)
		{
			if(open_sound_flag) 
			{
				USART_SendData(USART2 , 100);
			}
			open_sound_flag = 0;
		}
		else
		{
			close_acc++;
			if(close_acc > 50) 
			{
				close_acc = 0;
				close_sound_flag = 1;
			}
			if(close_sound_flag)
			{
				USART_SendData(USART2 , 101);
				open_sound_flag = 1;
			}
			close_sound_flag = 0;
		}			
  }
	if(start_ctrl3_flag == 0)
	{
		USART_SendData(USART2 , 101);
	}

}

void Task_6()
{
}
	
	
///////////////////////////////////////////////////////////////////////////////////////
//降落函数 
extern u8 Immedia_Stop_Flag;
int immedia_flag=0;
float thr_value;
float last_thr_value = 0;

//期望高度：
extern float exp_height;
//x偏移角：
extern float Turn_x_angle;
//Z轴角速度：
extern float Turn_Zgyro;

//下降不循线标志：
extern u8 stable_stop_flag;
void Down_flow_func(void)
{
	if(Immedia_Stop_Flag==1)
	{
		if(stable_stop_flag)
		{
			start_ctrl3_flag = 0;
			Turn_x_angle     = 0;
			Turn_Zgyro       = 0;
		}
		exp_height = exp_height - 6;    //逐渐降低期望高度
		if( ultra_dis_lpf <= 260)//thr_value <= 280 ||
		{
			fly_ready = 0 ;
		}
	}
	
	if(Choice_down_flag)  //任务3，4，5降落标志！
	{
		if(exp_height < 400)
		{
			Move_y_angle = 3.0;
			Turn_x_angle = -3.0;
			start_ctrl3_flag = 0;
		}

		exp_height = exp_height - 15;    //逐渐降低期望高度
		if( ultra_dis_lpf <= 300)//thr_value <= 280 ||
		{
			fly_ready = 0 ;
		}
	}
}


//计数：
int Turn_count_flag = 1;
 
//转角；
float Turn_x_angle = 0;

//间断：
int Move_change_flag = 0;


//转向角：
float rotation_angle;
//转角缓冲标志：
int Turn_Buffer_flag = 1;
 
int Turn_Buffer_Count = 1;

//左移右移标志：
int move_right_flag = 0;
int move_left_flag = 0;
int move_flag_count = 1;

//上一次的位置点：
int   last_point_com = 0;
int   last_point = 0;
float move_speed = 0;
float qh_move_speed = 0;
int   speed_calc_count = 15;
int   speed_calc_flag_1  = 1;
int   speed_calc_flag_2  = 1;

//判断区域标志：
int   area_1_flag = 1; //左或右
int   area_2_flag = 0; //左或右

//Z轴转角速度：
float Turn_Zgyro = 0;

//位置坐标：
float position_com = 0;

//进行定点标志：
extern int No_pointfix_flag  ;

//各任务的D参数：
 
float Task_1_kp = 0;
float Task_2_kp = 0;
float Task_3_kp = 0;
float Task_4_kp = 0.1;

float Task_1_kd = 1;
float Task_2_kd = 1.2;
float Task_3_kd = 1.2;
float Task_4_kd = 1.2;
 
float Pout = 0;
float Dout = 0;

//任务3的角度限幅：
float Task_3_angle_limit = 2;

//循线PID控制：
void CTRL_3()
{
	//左右偏线控制：
	///////////////////以下是PID控制循线

	if(Horizontal_Column_BlackMidpoint < 40 && Horizontal_Column_BlackMidpoint > 0)  //如果飞机在线左边，则flag置1
	{
		area_1_flag = 1;
	}
	else if(Horizontal_Column_BlackMidpoint > 40 && Horizontal_Column_BlackMidpoint < 80) // 如果飞机在线右边，则flag置0
	{
		area_1_flag = 0;
	}
		
	if(area_1_flag == 1 && Horizontal_Column_BlackMidpoint == 0)
	{
		Horizontal_Column_BlackMidpoint = 0;
	}
	else if(area_1_flag == 0 && Horizontal_Column_BlackMidpoint == 0)
	{
		Horizontal_Column_BlackMidpoint = 80;
	}
		
	//计算左右横移速度：
	move_speed = Horizontal_Column_BlackMidpoint - last_point_com;
	speed_calc_flag_1++;
	if(speed_calc_flag_1 >= speed_calc_count)
	{
	  last_point_com = Horizontal_Column_BlackMidpoint ;
		speed_calc_flag_1 = 1;
	}
 
	///////////////////////////////////////////////////////////////
	//先判断前后区域：
	if(Vertical_Row_BlackMidpoint < 25 && Vertical_Row_BlackMidpoint > 0)  //如果飞机在线前方，则flag置1
	{
		area_2_flag = 1;
	}
	else if(Vertical_Row_BlackMidpoint > 25 && Vertical_Row_BlackMidpoint < 50) // 如果飞机在线后方，则flag置0
	{
		area_2_flag = 0;
	}
	
	if(area_2_flag == 1 && Vertical_Row_BlackMidpoint == 0)  //No_pointfix_flag == 1 为没检测到空白或长线  && No_pointfix_flag == 1
	{
		Vertical_Row_BlackMidpoint = 0;
	}
	else if(area_2_flag == 0 && Vertical_Row_BlackMidpoint == 0)  //&& No_pointfix_flag == 1
	{
		Vertical_Row_BlackMidpoint = 50;
	}
	
	//计算前后平移速度：
	qh_move_speed = Vertical_Row_BlackMidpoint - last_point;
	speed_calc_flag_2++;
	if(speed_calc_flag_2 >= speed_calc_count)
	{
	  last_point = Vertical_Row_BlackMidpoint ;
		speed_calc_flag_2 = 1;
	}
	
	
  //根据任务号进行参数分配：
	if(Task_flag == 1)    //任务号为1，开启定点悬停模式     
	{
	//左右移动串级PD 输出PID：
		Pout = (40 - Horizontal_Column_BlackMidpoint) * 0.17;   // 0.14
		Dout = - move_speed * 0.9;
		if(Pout >= 2.4 )   // 5 
		{
			Pout = 2.4;
		}
		if(Pout <= -2.4)
		{
			Pout = -2.4;
		}
		if(Dout >= 2 )   // 5 
		{	
			Dout = 2;
		}
		if(Dout <= -2)
		{
			Dout = -2;
		}
		Turn_x_angle = Pout + Dout;
		
		//前后移动串级PD 输出PID：
		Pout = (25 - Vertical_Row_BlackMidpoint) * 0.19;      //0.16
		Dout = - qh_move_speed * 0.9;
		if(Pout >= 2.4 )   // 5 
		{
			Pout = 2.4;
		}
		if(Pout <= -2.4)
		{
			Pout = -2.4;
		}
		if(Dout >= 2 )   // 5 
		{	
			Dout = 2;
		}
		if(Dout <= -2)
		{
			Dout = -2;
		}
		Move_y_angle = Pout + Dout;	

  
	}
	
	////////////////////////////////////////////////////////////////
	
  if(Task_flag == 2)    //任务号为2，开启直线行驶模式
	{

	}
	
	//////////////////////////////////////////////////////////////////
	
	if(Task_flag == 3)    //任务号为3
	{
	//左右移动串级PD 输出PID：
		Pout = (40 - Horizontal_Column_BlackMidpoint) * 0.2;
		Dout = - move_speed * 0.9;
		if(Pout >= 1.9 )   // 2.4 !!!
		{
			Pout = 1.9;
		}
		if(Pout <= -1.9)
		{
			Pout = -1.9;
		}
		if(Dout >= 6 )   // 5 
		{	
			Dout = 6;
		}
		if(Dout <= -6)
		{
			Dout = -6;
		}
		Turn_x_angle = Pout + Dout;

		
		//前后移动串级PD 输出PID：
		Pout = (25 - Vertical_Row_BlackMidpoint) * 0.22;
		Dout = - qh_move_speed * 0.9;
		if(Pout >= 1.9 )   // 2.4 !!!
		{
			Pout = 1.9;
		}
		if(Pout <= -1.9)
		{
			Pout = -1.9;
		}
		if(Dout >= 6 )   // 5 
		{	
			Dout = 6;
		}
		if(Dout <= -6)
		{
			Dout = -6;
		}
		Move_y_angle = Pout + Dout;	
	}
 
 ///////////////////////////////////////////////////////////////
	
	if(Task_flag == 4)    //
	{
	//左右移动串级PD 输出PID：
		Pout = (40 - Horizontal_Column_BlackMidpoint) * 0.14;
		Dout = - move_speed * 0.9;
		if(Pout >= 2.4 )   // 5 
		{
			Pout = 2.4;
		}
		if(Pout <= -2.4)
		{
			Pout = -2.4;
		}
		if(Dout >= 6 )   // 5 
		{	
			Dout = 6;
		}
		if(Dout <= -6)
		{
			Dout = -6;
		}
		Turn_x_angle = Pout + Dout;

		
		//前后移动串级PD 输出PID：
		Pout = (25 - Vertical_Row_BlackMidpoint) * 0.16;
		Dout = - qh_move_speed * 0.9;
		if(Pout >= 2.4 )   // 5 
		{
			Pout = 2.4;
		}
		if(Pout <= -2.4)
		{
			Pout = -2.4;
		}
		if(Dout >= 6 )   // 5 
		{	
			Dout = 6;
		}
		if(Dout <= -6)
		{
			Dout = -6;
		}
		Move_y_angle = Pout + Dout;	
	}
	
	//////////////////////////////////////////////////////////////////////
		
	if(Task_flag == 5)    //
	{
	//左右移动串级PD 输出PID：
		Pout = (40 - Horizontal_Column_BlackMidpoint) * 0.15;
		Dout = - move_speed * 0.9;
		if(Pout >= 2.4 )   // 5 
		{
			Pout = 2.4;
		}
		if(Pout <= -2.4)
		{
			Pout = -2.4;
		}
	  if(Dout >= 6 )   // 5 
		{	
			Dout = 6;
		}
		if(Dout <= -6)
		{
			Dout = -6;
		}
		Turn_x_angle = Pout + Dout;

		
		//前后移动串级PD 输出PID：
		Pout = (25 - Vertical_Row_BlackMidpoint) * 0.17;
		Dout = - qh_move_speed * 0.9;
	  if(Pout >= 2.4 )   // 2.4 !!!
		{
			Pout = 2.4;
		}
		if(Pout <= -2.4)
		{
			Pout = -2.4;
		}
    if(Dout >= 6 )   // 5 
		{	
			Dout = 6;
		}
		if(Dout <= -6)
		{
			Dout = -6;
		}
		Move_y_angle = Pout + Dout;	
	}


}





/*                     以上为本人自己加的函数：                               */








float Roll_offset  = 0 ;
float Pitch_offset = 0;

void CTRL_2(float T)
{
// 	static xyz_f_t acc_no_g;
// 	static xyz_f_t acc_no_g_lpf;
//=========================== 期望角度 ========================================
//	except_A.x  = MAX_CTRL_ANGLE  *( my_deathzoom( ( CH_filter[ROL]) ,30 )/500.0f );   //30
//	except_A.y  = MAX_CTRL_ANGLE  *( my_deathzoom( (-CH_filter[PIT]) ,30 )/500.0f );  //30
	
	/////////////////////////////////////////////////

	except_A.x = Roll_offset  + Turn_x_angle;
	except_A.y = Pitch_offset + Move_y_angle;
	////////////////////////////////////////
	
//  except_A.z = rotation_angle ;
	
//	if( Thr_Low == 0 )
//	{
//		except_A.z += (s16)( MAX_CTRL_YAW_SPEED *( my_deathzoom_2( (CH_filter[YAW]) ,40 )/500.0f ) ) *T ;  //50
//	}
//	else
//	{
//		except_A.z += 1 *3.14 *T *( Yaw - except_A.z );
//	}
//	except_A.z = To_180_degrees(except_A.z);
//==============================================================================
// 	acc_no_g.x =  mpu6050.Acc.x - reference_v.x *4096;
// 	acc_no_g.y =  mpu6050.Acc.y - reference_v.y *4096;
// 	acc_no_g.z =  mpu6050.Acc.z - reference_v.z *4096;
// 	
// 	acc_no_g_lpf.x += 0.5f *T *3.14f * ( acc_no_g.x - acc_no_g_lpf.x );
// 	acc_no_g_lpf.y += 0.5f *T *3.14f * ( acc_no_g.y - acc_no_g_lpf.y );
// 	acc_no_g_lpf.z += 0.5f *T *3.14f * ( acc_no_g.z - acc_no_g_lpf.z );
// 	
// 	compensation.x = LIMIT( 0.003f *acc_no_g_lpf.x, -10,10 );
// 	compensation.y = LIMIT( 0.003f *acc_no_g_lpf.y, -10,10 );
// 	compensation.z = LIMIT( 0.003f *acc_no_g_lpf.z, -10,10 );
//==============================================================================	

  /* 得到角度误差 */
	ctrl_2.err.x =  To_180_degrees( ctrl_angle_offset.x + except_A.x - Roll  );
	ctrl_2.err.y =  To_180_degrees( ctrl_angle_offset.y + except_A.y - Pitch );
	ctrl_2.err.z =  To_180_degrees( ctrl_angle_offset.z + except_A.z - Yaw	 );
	/* 计算角度误差权重 */
	ctrl_2.err_weight.x = ABS(ctrl_2.err.x)/ANGLE_TO_MAX_AS;
	ctrl_2.err_weight.y = ABS(ctrl_2.err.y)/ANGLE_TO_MAX_AS;
	ctrl_2.err_weight.z = ABS(ctrl_2.err.z)/ANGLE_TO_MAX_AS;
	/* 角度误差微分（跟随误差曲线变化）*/
	ctrl_2.err_d.x = 10 *ctrl_2.PID[PIDROLL].kd  *(ctrl_2.err.x - ctrl_2.err_old.x) *( 0.005f/T ) *( 0.65f + 0.35f *ctrl_2.err_weight.x );
	ctrl_2.err_d.y = 10 *ctrl_2.PID[PIDPITCH].kd *(ctrl_2.err.y - ctrl_2.err_old.y) *( 0.005f/T ) *( 0.65f + 0.35f *ctrl_2.err_weight.y );
	ctrl_2.err_d.z = 10 *ctrl_2.PID[PIDYAW].kd 	 *(ctrl_2.err.z - ctrl_2.err_old.z) *( 0.005f/T ) *( 0.65f + 0.35f *ctrl_2.err_weight.z );
	/* 角度误差积分 */
	ctrl_2.err_i.x += ctrl_2.PID[PIDROLL].ki  *ctrl_2.err.x *T;
	ctrl_2.err_i.y += ctrl_2.PID[PIDPITCH].ki *ctrl_2.err.y *T;
	ctrl_2.err_i.z += ctrl_2.PID[PIDYAW].ki 	*ctrl_2.err.z *T;
	/* 角度误差积分分离 */
	ctrl_2.eliminate_I.x = Thr_Weight *CTRL_2_INT_LIMIT;
	ctrl_2.eliminate_I.y = Thr_Weight *CTRL_2_INT_LIMIT;
	ctrl_2.eliminate_I.z = Thr_Weight *CTRL_2_INT_LIMIT;
	/* 角度误差积分限幅 */
	ctrl_2.err_i.x = LIMIT( ctrl_2.err_i.x, -ctrl_2.eliminate_I.x,ctrl_2.eliminate_I.x );
	ctrl_2.err_i.y = LIMIT( ctrl_2.err_i.y, -ctrl_2.eliminate_I.y,ctrl_2.eliminate_I.y );
	ctrl_2.err_i.z = LIMIT( ctrl_2.err_i.z, -ctrl_2.eliminate_I.z,ctrl_2.eliminate_I.z );
	/* 对用于计算比例项输出的角度误差限幅 */
	ctrl_2.err.x = LIMIT( ctrl_2.err.x, -90, 90 );
	ctrl_2.err.y = LIMIT( ctrl_2.err.y, -90, 90 );
	ctrl_2.err.z = LIMIT( ctrl_2.err.z, -90, 90 );
	/* 角度PID输出 */
	ctrl_2.out.x = ctrl_2.PID[PIDROLL].kp  *( ctrl_2.err.x + ctrl_2.err_d.x + ctrl_2.err_i.x );	//rol
	ctrl_2.out.y = ctrl_2.PID[PIDPITCH].kp *( ctrl_2.err.y + ctrl_2.err_d.y + ctrl_2.err_i.y );  //pit
	ctrl_2.out.z = ctrl_2.PID[PIDYAW].kp   *( ctrl_2.err.z + ctrl_2.err_d.z + ctrl_2.err_i.z );
	/* 记录历史数据 */	
	ctrl_2.err_old.x = ctrl_2.err.x;
	ctrl_2.err_old.y = ctrl_2.err.y;
	ctrl_2.err_old.z = ctrl_2.err.z;

}

xyz_f_t except_AS;

float g_old[ITEMS];

void CTRL_1(float T)  //x roll,y pitch,z yaw
{
	xyz_f_t EXP_LPF_TMP;
	/* 给期望（目标）角速度 */
	EXP_LPF_TMP.x = MAX_CTRL_ASPEED *(ctrl_2.out.x/ANGLE_TO_MAX_AS);//*( (CH_filter[0])/500.0f );//
	EXP_LPF_TMP.y = MAX_CTRL_ASPEED *(ctrl_2.out.y/ANGLE_TO_MAX_AS);//*( (CH_filter[1])/500.0f );//
	EXP_LPF_TMP.z = MAX_CTRL_ASPEED *(ctrl_2.out.z/ANGLE_TO_MAX_AS);
	
	except_AS.x = EXP_LPF_TMP.x;//20 *3.14 *T *( EXP_LPF_TMP.x - except_AS.x );//
	except_AS.y = EXP_LPF_TMP.y;//20 *3.14 *T *( EXP_LPF_TMP.y - except_AS.y );//
	except_AS.z = EXP_LPF_TMP.z;//20 *3.14 *T *( EXP_LPF_TMP.z - except_AS.z );//
	
	//////////////////////////
	except_AS.z = Turn_Zgyro ;
//	if(start_ctrl3_flag == 1 && Task_flag == 3)
//	{
//		except_AS.z = 0;
//	}
  /////////////////////////////
	/* 期望角速度限幅 */
	except_AS.x = LIMIT(except_AS.x, -MAX_CTRL_ASPEED,MAX_CTRL_ASPEED );
	except_AS.y = LIMIT(except_AS.y, -MAX_CTRL_ASPEED,MAX_CTRL_ASPEED );
	except_AS.z = LIMIT(except_AS.z, -MAX_CTRL_ASPEED,MAX_CTRL_ASPEED );

	
	/* 角速度直接微分（角加速度），负反馈可形成角速度的阻尼（阻碍角速度的变化）*/
	ctrl_1.damp.x = ( mpu6050.Gyro_deg.x - g_old[A_X]) *( 0.002f/T );//ctrl_1.PID[PIDROLL].kdamp
	ctrl_1.damp.y = (-mpu6050.Gyro_deg.y - g_old[A_Y]) *( 0.002f/T );//ctrl_1.PID[PIDPITCH].kdamp *
	ctrl_1.damp.z = (-mpu6050.Gyro_deg.z - g_old[A_Z]) *( 0.002f/T );//ctrl_1.PID[PIDYAW].kdamp	 *
	/* 角速度误差 */
	ctrl_1.err.x =  ( except_AS.x - mpu6050.Gyro_deg.x ) *(300.0f/MAX_CTRL_ASPEED);
	ctrl_1.err.y =  ( except_AS.y + mpu6050.Gyro_deg.y ) *(300.0f/MAX_CTRL_ASPEED);  //-y
	ctrl_1.err.z =  ( except_AS.z + mpu6050.Gyro_deg.z ) *(300.0f/MAX_CTRL_ASPEED);	 //-z
	/* 角速度误差权重 */
	ctrl_1.err_weight.x = ABS(ctrl_1.err.x)/MAX_CTRL_ASPEED;
	ctrl_1.err_weight.y = ABS(ctrl_1.err.y)/MAX_CTRL_ASPEED;
	ctrl_1.err_weight.z = ABS(ctrl_1.err.z)/MAX_CTRL_YAW_SPEED;
	/* 角速度微分 */
	ctrl_1.err_d.x = ( ctrl_1.PID[PIDROLL].kd  *( -10 *ctrl_1.damp.x) *( 0.002f/T ) );
	ctrl_1.err_d.y = ( ctrl_1.PID[PIDPITCH].kd *( -10 *ctrl_1.damp.y) *( 0.002f/T ) );
	ctrl_1.err_d.z = ( ctrl_1.PID[PIDYAW].kd   *( -10 *ctrl_1.damp.z) *( 0.002f/T ) );

//	ctrl_1.err_d.x += 40 *3.14 *0.002 *( 10 *ctrl_1.PID[PIDROLL].kd  *(ctrl_1.err.x - ctrl_1.err_old.x) *( 0.002f/T ) - ctrl_1.err_d.x);
//	ctrl_1.err_d.y += 40 *3.14 *0.002 *( 10 *ctrl_1.PID[PIDPITCH].kd *(ctrl_1.err.y - ctrl_1.err_old.y) *( 0.002f/T ) - ctrl_1.err_d.y);
//	ctrl_1.err_d.z += 40 *3.14 *0.002 *( 10 *ctrl_1.PID[PIDYAW].kd   *(ctrl_1.err.z - ctrl_1.err_old.z) *( 0.002f/T ) - ctrl_1.err_d.z);

	/* 角速度误差积分 */
	ctrl_1.err_i.x += ctrl_1.PID[PIDROLL].ki  *(ctrl_1.err.x - ctrl_1.damp.x) *T;
	ctrl_1.err_i.y += ctrl_1.PID[PIDPITCH].ki *(ctrl_1.err.y - ctrl_1.damp.y) *T;
	ctrl_1.err_i.z += ctrl_1.PID[PIDYAW].ki 	*(ctrl_1.err.z - ctrl_1.damp.z) *T;
	/* 角速度误差积分分离 */
	ctrl_1.eliminate_I.x = Thr_Weight *CTRL_1_INT_LIMIT ;
	ctrl_1.eliminate_I.y = Thr_Weight *CTRL_1_INT_LIMIT ;
	ctrl_1.eliminate_I.z = Thr_Weight *CTRL_1_INT_LIMIT ;
	/* 角速度误差积分限幅 */
	ctrl_1.err_i.x = LIMIT( ctrl_1.err_i.x, -ctrl_1.eliminate_I.x,ctrl_1.eliminate_I.x );
	ctrl_1.err_i.y = LIMIT( ctrl_1.err_i.y, -ctrl_1.eliminate_I.y,ctrl_1.eliminate_I.y );
	ctrl_1.err_i.z = LIMIT( ctrl_1.err_i.z, -ctrl_1.eliminate_I.z,ctrl_1.eliminate_I.z );
	/* 角速度PID输出 */
	ctrl_1.out.x = 3 *( ctrl_1.FB *LIMIT((0.45f + 0.55f*ctrl_2.err_weight.x),0,1)*except_AS.x + ( 1 - ctrl_1.FB ) *ctrl_1.PID[PIDROLL].kp  *( ctrl_1.err.x + ctrl_1.err_d.x + ctrl_1.err_i.x ) );
										//*(MAX_CTRL_ASPEED/300.0f);
	ctrl_1.out.y = 3 *( ctrl_1.FB *LIMIT((0.45f + 0.55f*ctrl_2.err_weight.y),0,1)*except_AS.y + ( 1 - ctrl_1.FB ) *ctrl_1.PID[PIDPITCH].kp *( ctrl_1.err.y + ctrl_1.err_d.y + ctrl_1.err_i.y ) );
										//*(MAX_CTRL_ASPEED/300.0f);
	ctrl_1.out.z = 3 *( ctrl_1.FB *LIMIT((0.45f + 0.55f*ctrl_2.err_weight.z),0,1)*except_AS.z + ( 1 - ctrl_1.FB ) *ctrl_1.PID[PIDYAW].kp   *( ctrl_1.err.z + ctrl_1.err_d.z + ctrl_1.err_i.z ) );
										//*(MAX_CTRL_ASPEED/300.0f);
	Thr_Ctrl(T);// 油门控制
	
	All_Out(ctrl_1.out.x , ctrl_1.out.y , ctrl_1.out.z);

	ctrl_1.err_old.x = ctrl_1.err.x;
	ctrl_1.err_old.y = ctrl_1.err.y;
	ctrl_1.err_old.z = ctrl_1.err.z;

	g_old[A_X] =  mpu6050.Gyro_deg.x ;
	g_old[A_Y] = -mpu6050.Gyro_deg.y ;
	g_old[A_Z] = -mpu6050.Gyro_deg.z ;
}


int fly_low_value = 380;
int fly_motorvalue_offset = 0;
int origin_fly_low_value = 0;
int fly_speedup_flag = 1;

u8 Thr_Low;
float Thr_Weight;

extern float speed_up;
extern float original_exp_height;
int acc_motor_flag=0;


void Thr_Ctrl(float T)
{
	static float thr;
	static float Thr_tmp;
	thr = 500 + CH_filter[THR]; //油门值 0 ~ 1000
	
	Thr_tmp += 10 *3.14f *T *(thr/400.0f - Thr_tmp); //低通滤波
	Thr_Weight = LIMIT(Thr_tmp,0,1);    							//后边多处分离数据会用到这个值
	
	if( thr < 100 )
	{
		Thr_Low = 1;
	}
	else
	{
		Thr_Low = 0;
	}
	
	#if(CTRL_HEIGHT)
	Height_Ctrl(T,thr);
	////////////////////////////////////////////////////////
	
	if(fly_speedup_flag)
	{
		origin_fly_low_value = fly_low_value;
		fly_speedup_flag = 0;
	}
	
	if(stay_altitude_flag)
	{ 
		//任务1的起飞高度模式：
		if(Choice_down_flag == 0 && Immedia_Stop_Flag == 0 && Task_flag == 1)
		{
			exp_height = exp_height + 15;
			if(exp_height >= original_exp_height + 1250)  
			{
				exp_height = original_exp_height + 1250;
			}
		}
			
		//任务3 、4 、5的起飞高度模式：
		if((Choice_down_flag  == 0 && Immedia_Stop_Flag == 0 && Task_flag == 3) || (Choice_down_flag  == 0 && Immedia_Stop_Flag == 0 && Task_flag == 4) || (Choice_down_flag  == 0 && Immedia_Stop_Flag == 0 && Task_flag == 5))
		{
			exp_height = exp_height + 20;
			if(exp_height >= original_exp_height + 1250)  
			{
				exp_height = original_exp_height + 1250;
			}
		}
		
		thr_value = Thr_Weight *height_ctrl_out - 600 + fly_low_value;   //实际使用值  Thr_Weight *height_ctrl_out - 600 + 
	}
	else 
	{
		acc_motor_flag++;
		if(acc_motor_flag == 20)
		{
			fly_low_value ++;
			acc_motor_flag = 0;
		}
		if(fly_low_value >= origin_fly_low_value + fly_motorvalue_offset)
		{
			fly_low_value = origin_fly_low_value + fly_motorvalue_offset;
		}
		thr_value = fly_low_value;
	}	
		
	////////////////////////////////////////////////////////
	#else
	thr_value = thr;   //实际使用值
	#endif
	
	thr_value = LIMIT(thr_value,0,10 *MAX_THR *MAX_PWM/100);
}


float motor[MAXMOTORS];
float posture_value[MAXMOTORS];
float curve[MAXMOTORS];

//开启飞行标志：
int fly_ready = 0;

void All_Out(float out_roll,float out_pitch,float out_yaw)
{
	s16 motor_out[MAXMOTORS];
	u8 i;
	float posture_value[MAXMOTORS];
  float curve[MAXMOTORS];
	

	out_yaw = LIMIT( out_yaw , -5*MAX_THR ,5*MAX_THR ); //50%
	
	posture_value[0] = - out_roll + out_pitch + out_yaw ;
	posture_value[1] = + out_roll + out_pitch - out_yaw ;
	posture_value[2] = + out_roll - out_pitch + out_yaw ;
	posture_value[3] = - out_roll - out_pitch - out_yaw ;
	
	for(i=0;i<4;i++)
	{
		posture_value[i] = LIMIT(posture_value[i], -1000,1000 );
	}
	
	curve[0] = (0.55f + 0.45f *ABS(posture_value[0])/1000.0f) *posture_value[0] ;
	curve[1] = (0.55f + 0.45f *ABS(posture_value[1])/1000.0f) *posture_value[1] ;
	curve[2] = (0.55f + 0.45f *ABS(posture_value[2])/1000.0f) *posture_value[2] ;
	curve[3] = (0.55f + 0.45f *ABS(posture_value[3])/1000.0f) *posture_value[3] ;
	
	
	//////////////////////////////
  Down_flow_func();   //降落函数
	/////////////////////////////
	
	
  motor[0] = thr_value + Thr_Weight *curve[0] ;
	motor[1] = thr_value + Thr_Weight *curve[1] ;
	motor[2] = thr_value + Thr_Weight *curve[2] ;
	motor[3] = thr_value + Thr_Weight *curve[3] ;
	

	/* 是否解锁 */
	if(fly_ready)
	{
		if( !Thr_Low )  //油门拉起
		{
			for(i=0;i<4;i++)
			{
				motor[i] = LIMIT(motor[i], 0,(10*MAX_PWM) );
			}
		}
		else						//油门低
		{
			for(i=0;i<4;i++)
			{
				motor[i] = LIMIT(motor[i], 0,(10*MAX_PWM) );
			}
		}
	}
	else
	{
		motor[0] = motor[1] = motor[2] = motor[3] = 0;
	}
	/* xxx */
	motor_out[0] = (s16)(motor[0]);  
	motor_out[1] = (s16)(motor[1]);	 
	motor_out[2] = (s16)(motor[2]);
	motor_out[3] = (s16)(motor[3]);

	
	SetPwm(motor_out,0,1000); //1000
	
}

//飞行缓冲数值
 
int fly_buffer_flag = 0;
int fly_buffer_count = 1;
s16 fly_buffer[4] = {100 , 100 , 100 , 100};
void fly_buffer_func()
{
	if(fly_buffer_flag)
	{
	  SetPwm(fly_buffer,0,1000); //1000
		fly_buffer_count++;
		
		if(fly_buffer_count >= 500)
		{
			start_mpu_flag = 1;
      fly_ready = 1;
			fly_buffer_flag = 0;
			fly_buffer_count = 1;
		}
	}
}

/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/


