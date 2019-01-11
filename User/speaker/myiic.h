#ifndef __MYIIC_H
#define __MYIIC_H

/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define I2C_GPIO_PORT			GPIOB			/* GPIO端口 */
#define I2C_RCC_PORT 		RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define I2C_SCL_PIN		GPIO_Pin_10			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_Pin_11			/* 连接到SDA数据线的GPIO */

/*IO方向设置, PIN11已经是CRH中设置了*/
#define SDA_IN()  {I2C_GPIO_PORT->CRH &= 0XFFFFFFFF; I2C_GPIO_PORT->CRH |=((u32)0x88888888&(GPIO_CRH_MODE11 |GPIO_CRH_CNF11));} 
#define SDA_OUT() {I2C_GPIO_PORT->CRH &= 0XFFFFFFFF; I2C_GPIO_PORT->CRH |=((u32)0x33333333&(GPIO_CRH_MODE11 |GPIO_CRH_CNF11));}

//#define SDA_IN()  {GPIOB->CRH &= 0XFFFFFFFF; GPIOB->CRH |=(u32)8<<12;} 
//#define SDA_OUT() {GPIOB->CRH &= 0XFFFFFFFF; GPIOB->CRH |=(u32)3<<12;}

//IO操作函数	 
#define I2C_SCL_1()  (I2C_GPIO_PORT->BSRR = I2C_SCL_PIN)				/* SCL = 1 */
#define I2C_SCL_0()  (I2C_GPIO_PORT->BRR = I2C_SCL_PIN)				/* SCL = 0 */
	
#define I2C_SDA_1()  (I2C_GPIO_PORT->BSRR = I2C_SDA_PIN)				/* SDA = 1 */
#define I2C_SDA_0()  (I2C_GPIO_PORT->BRR = I2C_SDA_PIN)				/* SDA = 0 */
	
#define I2C_SDA_READ()  ((I2C_GPIO_PORT->IDR & I2C_SDA_PIN) != 0)		/* 读SDA口线状态 */

// 测试发现这2种操作方式时间都是一样的
#define IIC_SCL    PBout(10) //SCL = 0; SCL = 1; 
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //输入SDA 


//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号	  
#endif

