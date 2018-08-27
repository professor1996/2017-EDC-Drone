#ifndef _CTRL_H
#define	_CTRL_H

#include "stm32f4xx.h"
#include "include.h"
#include "pwm_out.h"
#include "rc.h"
#include "imu.h"
#include "mpu6050.h"
#include "communication.h"
#include "led.h"

enum {
    PIDROLL,
    PIDPITCH,
    PIDYAW,
		PID4,
		PID5,
		PID6,

		PIDITEMS
};



typedef struct
{
	xyz_f_t err;
	xyz_f_t err_old;
	xyz_f_t err_i;
	xyz_f_t eliminate_I;
	xyz_f_t err_d;
	xyz_f_t damp;
	xyz_f_t out;
	pid_t 	PID[PIDITEMS];
	xyz_f_t err_weight;
	float FB;

}ctrl_t;

extern ctrl_t ctrl_1;
extern ctrl_t ctrl_2;


void CTRL_3();
void CTRL_2(float);
void CTRL_1(float);
void Ctrl_Para_Init(void);
void Thr_Ctrl(float);
void All_Out(float x,float y,float z);

void Task_scheduler(int task);
void Task_0();
void Task_1();
void Task_2();
void Task_3();
void Task_4();
void Task_5();
void Task_6();
 

void Ruisa_commun();
void Turn_func_moveright();
void Turn_func_moveleft();
void Down_flow_func(void);
void fly_buffer_func();


extern u8 Thr_Low;
extern float Thr_Weight;
extern float motor[MAXMOTORS];
extern int move_right_flag  ;
extern int move_left_flag  ;
extern int fly_low_value;
extern float Roll_offset  ;
extern float Pitch_offset  ;

#endif

