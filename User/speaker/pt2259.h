#ifndef __24CXX_H
#define __24CXX_H   

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//Mini STM32开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02

#define PT2259_ID		0x88

#define MUTE_OFF		0x74		// 关闭静音
#define MUTE_ON		0x77		// 打开静音
#define CHIP_CLEAR	0xf0		// 芯片清除


#define NS4160_OUT_GRP			GPIOA
#define NS4160_POUT			GPIO_Pin_8	//栏杆起

					  
//u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//指定地址读取一个字节
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
//u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
//void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据

u8 AT24CXX_Check(void);  //检查器件
void AT24CXX_Init(void); //初始化IIC

void PT2259_Config(u8 Data);
void PT2259_Set( u8 ucData10DB,u8 ucData1DB );
void NS4160_AB_type(void);
void NS4160_Disable(void);
void NS4160_D_type(void);
void NS4160_GPIO_OUT_Config(void);
void PT2259_Init(void);
#endif

