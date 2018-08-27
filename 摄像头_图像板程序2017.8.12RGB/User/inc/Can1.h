#ifndef _CAN1_H
#define _CAN1_H

#include "stm32f4xx.h"
#include "MyDataProcess.h"

void Can1_Configuration(void);

void SendPosition(s16 data1 , s16 data2);
 
void can_config();

#endif



