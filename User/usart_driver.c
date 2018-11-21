/**
  * @file    uart_driver.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   usartӦ��
  * ******************************************************************************
  */
  
#include "include.h"

/*UART���ݻ�����*/
UART_BUF	 UARTBuf[UART_NUM];

/* ����printf��ȱ��, �������û��ʵ��ʹ��*/
USART_LIST uart_sel = PC_UART; // Ϊ���ض���3������,����һ��ѡ�����.


/******************************************************************************
 * ������:	UARTProcessTickEvents 
 * ����: 
 *            -�жϴ���ͨ��һ֡�����Ƿ������
 *		  ��������1ms��ʱ�ж�ÿ�������һ��,������
 *		  UART[x]Buf.Timerʱ��������timer�Լ�Ϊ0,��������֡�������;
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2018.11.21
 ******************************************************************************/
void UARTProcessTickEvents(void)
{
	USART_LIST i = UART1_COM;

	for (i = UART1_COM; i < UART_NUM; i++)
	{
		if( UARTBuf[i].Timer > 0 )
		{
			UARTBuf[i].Timer--;
			if( UARTBuf[i].Timer == 0 )
			{
				UARTBuf[i].RecFlag = 1;
			}
		}
	}
}


/// �ض���c�⺯��printf��USARTx
int fputc(int ch, FILE *f)
{
	USART_TypeDef *pusart=USART1;

	if (uart_sel == UART1_COM)
	{
		pusart = USART1;
	}
	else if (uart_sel == UART2_COM)
	{
		pusart = USART2;
	}
	else
	{
		pusart = USART3;
	}


	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(pusart, (uint8_t) ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pusart, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}


/// �ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
	USART_TypeDef *pusart=USART1;

	if (uart_sel == UART1_COM)
	{
		pusart = USART1;
	}
	else if (uart_sel == UART2_COM)
	{
		pusart = USART2;
	}
	else
	{
		pusart = USART3;
	}
	
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(pusart, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(pusart);
}

void printf_test(void)
{
	printf("\r\n ����һ�������жϽ��ջ���ʵ�� \r\n");	
	printf("\r\n ���ڳ����ն˻��ߴ��ڵ������������ַ� \r\n");	
}

/*********************************************END OF FILE**********************/
