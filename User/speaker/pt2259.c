 /**
  ******************************************************************************
  * @file    myiic.c
  * @author  Jerry
  * @date    17-Dec-2018
  *
  * @brief   pt2259驱动文件
  *
  ******************************************************************************
  */
#include "include.h" 

//初始化IIC接口
void PT2259_Init(void)
{
	IIC_Init();
	NS4160_GPIO_OUT_Config();
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void PT2259_WriteOneByte(u8 DataToWrite)
{				   	  	    																    										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();//产生一个停止条件 
	//Delay_Ms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
#if 0
//在PT2259指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 PT2259_ReadOneByte(u16 ReadAddr)
{	
	u8 temp=0;		  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(PT2259_ID);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(PT2259_ID+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(PT2259_ID+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}

void PT2259_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		PT2259_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}


//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 PT2259_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=PT2259_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}


//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 PT2259_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)
	{
		return 0;
	}
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)
		{
			return 0;
		}
	}
	return 1;											  
}


//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void PT2259_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=PT2259_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 
#endif
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void PT2259_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		PT2259_WriteOneByte(*pBuffer);
		pBuffer++;
	}
}

// 设置音量,即设置衰减量,见手册P9,需要连续写入2个字节:
// 比如衰减-42dB,需要先衰减-40dB，然后衰减-2dB.
void PT2259_Set( u8 ucData10DB,u8 ucData1DB )
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID );
	PT2259_WriteOneByte( ucData10DB );
	PT2259_WriteOneByte( ucData1DB  );
}

void PT2259_Config(u8 Data)
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID);  // is PT2259 address
	PT2259_WriteOneByte( Data ); // clear register
}

void PT2259_Clear()
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID);  // is PT2259 address
	PT2259_WriteOneByte( 0xf0 ); // clear register
}

void PT2259_OpenVolume(u8 Data)
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID);  // is PT2259 address
	PT2259_WriteOneByte( Data ); // clear register
}


void NS4160_Output_High(void)	
{
	GPIO_SetBits(NS4160_OUT_GRP, NS4160_POUT);
}

void NS4160_Output_Low(void)	
{
	GPIO_ResetBits(NS4160_OUT_GRP, NS4160_POUT);
}

void NS4160_AB_type(void)	
{
	NS4160_Output_Low();
	Delay_Us(30);
	NS4160_Output_High();
	Delay_Ms(150);
	//NS4160_Output_Low();
	//NS4160_Output_High();
}

void NS4160_D_type(void)	
{
	NS4160_Output_Low();
	Delay_Us(100);
	NS4160_Output_High();
	DELAY_4uS();
	NS4160_Output_Low();
	DELAY_4uS();
	NS4160_Output_High();
	Delay_Ms(100);
}

void NS4160_Disable(void)	
{
	GPIO_ResetBits(NS4160_OUT_GRP, NS4160_POUT);
}

/***********************************************************************************
 * 函数名:	 
 * 描述: 
 *           	-配置指定输出外设.
 *		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:20181109
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
void NS4160_GPIO_OUT_Config(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启输出按键端口PX的时钟*/
	RCC_Clock_Set(NS4160_OUT_GRP, ENABLE);
	//RCC_APB2PeriphClockCmd( DEVICE_RCC_GRP, ENABLE); // 使能PC端口时钟  
	GPIO_InitStructure.GPIO_Pin = NS4160_POUT;	//选择对应的引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NS4160_OUT_GRP, &GPIO_InitStructure);  //初始化端口
	NS4160_Output_Low();
}

