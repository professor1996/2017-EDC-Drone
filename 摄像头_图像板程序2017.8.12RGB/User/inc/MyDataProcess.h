#ifndef _MYDATAPROCESS_H
#define _MYDATAPROCESS_H

#include "math.h"
#include "stdio.h"
#include "stm32f4xx.h"


void EncodeFloatData(float* f, unsigned char* buff);
void DecodeFloatData(float* f, unsigned char* buff);

void EncodeS16Data(s16* f, unsigned char* buff);
void DecodeS16Data(s16* f, unsigned char* buff);

void EncodeU16Data(u16* f, unsigned char* buff);
void DecodeU16Data(u16* f, unsigned char* buff);

void EncodeS32Data(s32* f, unsigned char* buff);
void DecodeS32Data(s32* f, unsigned char* buff);

#endif






