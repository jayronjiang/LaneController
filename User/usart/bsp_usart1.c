/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   USART1板级应用.
  *
  ******************************************************************************
  */
#include "include.h"

 /******************************************************************************
 * 函数名:	USART1_Config 
 * 描述: USART1 GPIO 配置,工作模式配置。115200 8-N-1
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void USART1_Config(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* 使能串口1接收中断 */
	// 注意,使用printf 发送不是中断方式发送,不需要打开发送中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* 使能串口1空闲中断 */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	
	//配置串口1对应的中断向量
	NVIC_USART1_Configuration();
	
	//printf("USART1初始化完成\n");
}

 /******************************************************************************
 * 函数名:	NVIC_USART1_Configuration 
 * 描述: 配置USART1中断
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void NVIC_USART1_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

 /******************************************************************************
 * 函数名:	Param_USART1_Init 
 * 描述: 缓冲初始化
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Param_USART1_Init(void)
{
	/*初始化物理层缓冲区数据*/
	UARTBuf[UART1_COM].RxLen = 0;
	UARTBuf[UART1_COM].TxLen = 0;
	UARTBuf[UART1_COM].TxPoint= 0;
	UARTBuf[UART1_COM].RecFlag= 0;
	UARTBuf[UART1_COM].Timer =0;
}

 /******************************************************************************
 * 函数名:	USART1_Init 
 * 描述: USART1初始化
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void USART1_Init(uint32_t baudrate)
{
	USART1_Config(baudrate);
	Param_USART1_Init();
}

 /******************************************************************************
 * 函数名:	USART1_IRQHandler 
 * 描述: USART1中断处理, 处理接收中断和空闲中断.
 *	USART_IT_RXNE: 没接收一个字节触发一次.
 *	USART_IT_IDLE: 接收完毕触发,测试发现间隔大概为30ms
 *注意:
 *	更严格的帧判断应该使用定时器UARTProcessTickEvents.
 *	
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void USART1_IRQHandler(void)
{
	uint8_t ch;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		LED_Set(LED_COM, ON); 	// 开始通信指示
		ch = USART_ReceiveData(USART1);

		UARTBuf[UART1_COM].RxBuf[UARTBuf[UART1_COM].RxLen] = ch ;
		if(UARTBuf[UART1_COM].RxLen < UART_RXBUF_SIZE)
		{
			UARTBuf[UART1_COM].RxLen++;
			/*需要根据波特率添加延时，判断串口通信一帧数据是否结束*/
			//UART0Buf.Timer = 50;		// 如果50ms还没有数据,本次数据帧结束
		}
	} 
	else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)	// 直接使用空闲帧中断
	{
		UARTBuf[UART1_COM].RecFlag = TRUE;
		/* 需要读这2个寄存器清除中断标志*/
		/*USART_GetITStatus 函数已经读了SR寄存器*/
		//ch = USART2->SR;
		/*读DR寄存器清除标志*/
		ch = USART_ReceiveData(USART1);
		LED_Set(LED_COM, OFF); 	// 通信完毕
	}
}
/*********************************************END OF FILE**********************/
