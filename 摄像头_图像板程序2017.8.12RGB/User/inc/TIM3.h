#include "stm32f4xx.h"
#include "Image.h"


#define Tar_Column_Midpoint  40


void TIM3_Int_Init(u16 psc , u16 arr);
void Position_Control(u8 FirstColumn_Midpoint , u8 LastColumn_Midpoint , u8 Tar_BlackBelt);
extern float speed  ;
extern float PITCH_tar    ;
extern float ROLL_tar     ;//4
extern float YAW_tar       ;
extern float PITCH_gyro_tar ;
extern float ROLL_gyro_tar   ;
extern float YAW_gyro_tar   ;
extern int Down_Flag ;
extern int car_start_detection_flag;
void Sending_scheduler(int task_flag);

void Sending_task_0();
void Sending_task_1();
void Sending_task_2();
void Sending_task_3();
void Sending_task_4();
void Sending_task_5();








