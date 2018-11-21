/**
  * @file    bsp_usart1.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  * ******************************************************************************
  */
  
#include "include.h"

 /**
  * @brief  USART2 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART2 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART2 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	/* 使能串口2接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* 使能串口2空闲中断 */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
	
	NVIC_USART2_Configuration();
	
	printf("USART2初始化完成\n");
}

/// 配置USART2接收中断
void NVIC_USART2_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void Param_USART2_Init(void)
{
	/*初始化物理层缓冲区数据*/
	UARTBuf[UART2_COM].RxLen = 0;
	UARTBuf[UART2_COM].TxLen = 0;
	UARTBuf[UART2_COM].TxPoint= 0;
	UARTBuf[UART2_COM].RecFlag= 0;
	UARTBuf[UART2_COM].Timer =0;
}


void USART2_Init(void)
{
	USART2_Config();
	Param_USART2_Init();
}

void USART2_IRQHandler(void)
{
	uint8_t ch;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{	
		ch = USART_ReceiveData(USART2);

		UARTBuf[UART2_COM].RxBuf[UARTBuf[UART2_COM].RxLen] = ch ;
		if(UARTBuf[UART2_COM].RxLen < UART_RXBUF_SIZE)
		{
			UARTBuf[UART2_COM].RxLen++;
			/*需要根据波特率添加延时，判断串口通信一帧数据是否结束*/
			//UART0Buf.Timer = 50;		// 如果50ms还没有数据,本次数据帧结束
		}
	} 
	else if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)	// 直接使用空闲帧中断
	{
		UARTBuf[UART2_COM].RecFlag = TRUE;
		/* 需要读这2个寄存器清除中断标志*/
		/*USART_GetITStatus 函数已经读了SR寄存器*/
		//ch = USART2->SR;
		/*读DR寄存器清除标志*/
		ch = USART_ReceiveData(USART2);;
	}
	
}
/*********************************************END OF FILE**********************/

