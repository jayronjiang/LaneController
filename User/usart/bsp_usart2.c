/**
  * @file    bsp_usart1.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usartӦ��bsp
  * ******************************************************************************
  */
  
#include "include.h"

/*UART���ݻ�����*/
 UART_BUF	 UART0Buf;

 /**
  * @brief  USART2 GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
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
	
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
	
	NVIC_USART2_Configuration();
	
	printf("USART2��ʼ�����\n");
}

/// ����USART2�����ж�
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
	/*��ʼ������㻺��������*/
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


/// �ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART2, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// �ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
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
//	  	printf( "%c", ch );    //�����ܵ�������ֱ�ӷ��ش�ӡ
		
//		//�Ҿ�������ط�Ӧ����USART_SendData����printf������ʱ��Ȼ��Ч
//		USART_SendData(USART1, (uint8_t) ch);
		
		ch = USART_ReceiveData(USART2);

		UART0Buf.RxBuf[UART0Buf.RxLen] = ch ;
		if(UART0Buf.RxLen < UART_RXBUF_SIZE)
		{
			UART0Buf.RxLen++;
			/*��Ҫ���ݲ����������ʱ���жϴ���ͨ��һ֡�����Ƿ����*/
			//UART0Buf.Timer = 50;		// ���50ms��û������,��������֡����
		}
		//Receive_Handle(ch);
		#ifndef DEBUG_MODE
	  		printf("%c",ch);    //�����ܵ�������ֱ�ӷ��ش�ӡ
	  	#endif
	} 
	else if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)	// ֱ��ʹ�ÿ���֡�ж�
	{
		UART0Buf.RecFlag = TRUE;
		/* ��Ҫ����2���Ĵ�������жϱ�־*/
		/*USART_GetITStatus �����Ѿ�����SR�Ĵ���*/
		//ch = USART2->SR;
		/*��DR�Ĵ��������־*/
		ch = USART_ReceiveData(USART2);;
	}
	
}

/******************************************************************************
 * ������:	UARTProcessTickEvents 
 * ����: 
 *            -�жϴ���ͨ��һ֡�����Ƿ������
 		  ��������1ms��ʱ�ж�ÿ�������һ��,������UART0Buf.Timerʱ��������
 		  timer�Լ�Ϊ0,��������֡������ϣ�
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2013.03.12
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
	printf("\r\n ����һ�������жϽ��ջ���ʵ�� \r\n");	
	printf("\r\n ���ڳ����ն˻��ߴ��ڵ������������ַ� \r\n");	
}
/*********************************************END OF FILE**********************/

