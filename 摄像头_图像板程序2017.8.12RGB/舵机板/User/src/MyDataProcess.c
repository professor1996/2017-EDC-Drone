#include "MyDataProcess.h"


//Name:	EncodeS32Data()
//Describe:S32型数据截成4个字节，放在buff中
//Input:S32* f, unsigned char* buff
//Output:None
//Return:None
void EncodeS32Data(s32* f, unsigned char* buff)
{
	u16 f_c[2]={0,0};
	*(s32*)f_c = *f;
	buff[0] = (f_c[0]&0x00ff);
	buff[1] = ((f_c[0]>>8)&0x00ff);
	buff[2] = (f_c[1]&0x00ff);
	buff[3] = ((f_c[1]>>8)&0x00ff);
}

//Name:	DecodeS32Data()
//Describe:将从buff开始的4个字节位拼接转换成S32
//Input:s32* f, unsigned char* buff
//Output:None
//Return:None
void DecodeS32Data(s32* f, unsigned char* buff)
{
	u16 f_c[2] = {0,0};
	f_c[0]=((buff[1]&0x00ff)<<8)|buff[0];
	f_c[1]=((buff[3]&0x00ff)<<8)|buff[2];
	*f=*(s32 *)f_c;
}

//Name:	EncodeFloatData()
//Describe:float型数据无精度丢失地截成4个字节，放在buff中
//Input:float* f, unsigned char* buff
//Output:None
//Return:None
void EncodeFloatData(float* f, unsigned char* buff)
{
	u16 f_c[2]={0,0};
	*(float*)f_c = *f;
	buff[0] = (f_c[0]&0x00ff);
	buff[1] = ((f_c[0]>>8)&0x00ff);
	buff[2] = (f_c[1]&0x00ff);
	buff[3] = ((f_c[1]>>8)&0x00ff);
}

//Name:	DecodeFloatData()
//Describe:将从buff开始的4个字节位拼接转换成浮点数
//Input:float* f, unsigned char* buff
//Output:None
//Return:None
void DecodeFloatData(float* f, unsigned char* buff)
{
	u16 f_c[2] = {0,0};
	f_c[0]=((buff[1]&0x00ff)<<8)|buff[0];
	f_c[1]=((buff[3]&0x00ff)<<8)|buff[2];
	*f=*(float *)f_c;
}

//Name:	EncodeS16Data()
//Describe:将16位有符号数据转换成2个字节放在buff中
//Input:s16* f, unsigned char* buff
//Output:None
//Return:None
void EncodeS16Data(s16* f, unsigned char* buff)
{
	buff[0] = (uint8_t)(*f);
	buff[1] = (uint8_t)((*f)>>8);
}

//Name:	DecodeS16Data()
//Describe:将buff中的2个字节拼接为s16
//Input:s16* f, unsigned char* buff
//Output:None
//Return:None
void DecodeS16Data(s16* f, unsigned char* buff)
{
	*f = (s16)(((u16)(buff[1])<<8)|buff[0]);
}


//Name:	EecodeU16Data()
//Describe:将16位无符号数据转换成2个字节放在buff中
//Input:u16* f, unsigned char* buff
//Output:None
//Return:None
void EncodeU16Data(u16* f, unsigned char* buff)
{
	buff[0] = (uint8_t)(*f);
	buff[1] = (uint8_t)((*f)>>8);
}

//Name:	DecodeU16Data()
//Describe:将buff中的2个字节拼接为u16
//Input:u16* f, unsigned char* buff
//Output:None
//Return:None
void DecodeU16Data(u16* f, unsigned char* buff)
{
	*f = ((u16)(buff[1])<<8)|buff[0];
}

//Name:	EecodeU12Data()
//Describe:将12位无符号数据转换成2个字节放在buff中,同时加上0x30
//Input:u16* f, unsigned char* buff
//Output:None
//Return:None
void EncodeU12Data(u16* f, unsigned char* buff)
{
	buff[0] = (uint8_t)((*f)&0x003F)+0x30;
	buff[1] = (uint8_t)(((*f)&0x0FC0)>>6)+0x30;
}

//Name:	DecodeU16Data()
//Describe:将buff中的2个字节减掉0x30拼接为u12
//Input:u16* f, unsigned char* buff
//Output:None
//Return:None
void DecodeU12Data(u16* f, unsigned char* buff)
{
	*f = ((u16)(buff[1]-0x30)<<6)|(buff[0]-0x30);
}

//Name:	EecodeU16to5Dec()
//Describe:将16位无符号数据转换成5位十进制ascll码制，如u16=12345，转换后为a[5]={'1'、'2'、'3'、'4'、'5'、}
//Input:u16* f, unsigned char* buff
//Output:None
//Return:None
void EncodeU16to5Dec(u16* f, unsigned char* buff)
{
	s8 i;
	u16 temp = *f;
	for(i=4;i>=0;i--)
	{
		buff[i]=temp%10+0x30;
		temp = temp/10;
	}
}

