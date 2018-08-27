#include "Sccb.h"
#include "stm32f4xx.h"
/*
-----------------------------------------------
   ����: ��ʼ��ģ��SCCB�ӿ�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void SCCB_GPIO_Config(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(SCCB_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  SCCB_SIC_BIT|SCCB_SID_BIT; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;	
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;			 
	GPIO_Init(SCCB_GPIO, &GPIO_InitStructure);
  
}
void I2C_delay(void)
{
   u8 i=100; 
   while(i) 
   { 
     i--; 
   }  
}

void I2C_Stop(void)
{
	SCL_L
	I2C_delay();
	SDA_L
	I2C_delay();
	SCL_H
	I2C_delay();
	SDA_H
	I2C_delay();
} 

int I2C_Start(void)
{
	SDA_H
	SCL_H
	I2C_delay();
	if(!SDA_read)return (0);	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L
	I2C_delay();
	if(SDA_read) return (0);	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L
	I2C_delay();
	return (1);
}

void I2C_Ack(void)
{	
	SCL_L
	I2C_delay();
	SDA_L
	I2C_delay();
	SCL_H
	I2C_delay();
	SCL_L
	I2C_delay();
}   


void I2C_NoAck(void)
{	
	SCL_L
	I2C_delay();
	SDA_H
	I2C_delay();
	SCL_H
	I2C_delay();
	SCL_L
	I2C_delay();
} 

int I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L
	I2C_delay();
	SDA_H			
	I2C_delay();
	SCL_H
	I2C_delay();
	if(SDA_read)
	{
      SCL_L
	  I2C_delay();
      return (0);
	}
	SCL_L
	I2C_delay();
	return (1);
}


void I2C_SendByte(unsigned char SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL_L
        I2C_delay();
      if(SendByte&0x80)
			  SDA_H 
      else 
        SDA_L   
        SendByte<<=1;
        I2C_delay();
		    SCL_H
        I2C_delay();
    }
    SCL_L
}  


unsigned char I2C_ReadByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L
      I2C_delay();
			SCL_H
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L
    return ReceiveByte;
} 

//0�ɹ�  1ʧ��
unsigned char WR_SENSOR_Reg(unsigned char regID, unsigned char regDat)
{
	I2C_Start();
  I2C_SendByte(0x42);  
	if(0==I2C_WaitAck())
	{
		I2C_Stop();
		return(1);
	}
	I2C_delay();
        
  I2C_SendByte(regID);  
	if(0==I2C_WaitAck())
	{
		I2C_Stop();
		return(1);
	}
	I2C_delay();
  I2C_SendByte((unsigned char)regDat);  
	if(0==I2C_WaitAck())
	{
		I2C_Stop();
		return(1);
	}
  	I2C_Stop();
	
  	return(0);
}

//0�ɹ�  1ʧ��
unsigned char RD_SENSOR_Reg(unsigned char regID, unsigned char *regDat)
{
	unsigned int val;
	I2C_Start();
        I2C_SendByte(0x42);
	if(0==I2C_WaitAck())
	{
		I2C_Stop();
		return(1);
	}
	I2C_delay();
  I2C_SendByte(regID);
  if(0==I2C_WaitAck())
	{
		I2C_Stop();
		return(1);
	}
  I2C_Stop();
	I2C_delay();

	I2C_Start();
  I2C_SendByte(0x43);
	if(0==I2C_WaitAck())
	{
		I2C_Stop();
		return(1);
	}
	I2C_delay();
  val=I2C_ReadByte();
	*regDat=val;
	I2C_delay();
  I2C_NoAck();
  I2C_delay();
	I2C_Stop();
  return(0);
}












