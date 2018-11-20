/**
  * @file    bsp_usart1.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  * ******************************************************************************
  */
  
#include "include.h"

/*UART数据缓冲区*/
 UART_BUF	 UART0Buf;

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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Param_USART2_Init(void)
{
	/*初始化物理层缓冲区数据*/
	UART0Buf.RxLen = 0;
	UART0Buf.TxLen = 0;
	UART0Buf.TxPoint= 0;
	UART0Buf.RecFlag= 0;
	UART0Buf.Timer =0;
}


void USART2_Init(void)
{
	USART2_Config();
	Param_USART2_Init();
}


/// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART2, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART2);
}


void USART2_IRQHandler(void)
{
	uint8_t ch;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 	
//	    //ch = USART1->DR;
//			ch = USART_ReceiveData(USART1);
//	  	printf( "%c", ch );    //将接受到的数据直接返回打印
		
//		//我觉得这个地方应该用USART_SendData，在printf不能用时依然有效
//		USART_SendData(USART1, (uint8_t) ch);
		
		ch = USART_ReceiveData(USART2);

		UART0Buf.RxBuf[UART0Buf.RxLen] = ch ;
		if(UART0Buf.RxLen < UART_RXBUF_SIZE)
		{
			UART0Buf.RxLen++;
			/*需要根据波特率添加延时，判断串口通信一帧数据是否结束*/
			//UART0Buf.Timer = 50;		// 如果50ms还没有数据,本次数据帧结束
		}
		//Receive_Handle(ch);
		#ifndef DEBUG_MODE
	  		printf("%c",ch);    //将接受到的数据直接返回打印
	  	#endif
	} 
	else if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)	// 直接使用空闲帧中断
	{
		UART0Buf.RecFlag = TRUE;
		/* 需要读这2个寄存器清除中断标志*/
		/*USART_GetITStatus 函数已经读了SR寄存器*/
		//ch = USART2->SR;
		/*读DR寄存器清除标志*/
		ch = USART_ReceiveData(USART2);;
	}
	
}

/******************************************************************************
 * 函数名:	UARTProcessTickEvents 
 * 描述: 
 *            -判断串口通信一帧数据是否结束。
 		  本函数由1ms定时中断每毫秒调用一次,当超过UART0Buf.Timer时间无数据
 		  timer自减为0,表明数据帧接收完毕；
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2013.03.12
 ******************************************************************************/
void UARTProcessTickEvents(void)
{
	if( UART0Buf.Timer > 0 )
	{
		UART0Buf.Timer--;
		if( UART0Buf.Timer == 0 )
		{
			UART0Buf.RecFlag = 1;
		}
	}
}

void printf_test(void)
{
	printf("\r\n 这是一个串口中断接收回显实验 \r\n");	
	printf("\r\n 请在超级终端或者串口调试助手输入字符 \r\n");	
}
/*********************************************END OF FILE**********************/

