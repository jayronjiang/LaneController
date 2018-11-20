#ifndef __USART_DRIVER_H
#define	__USART_DRIVER_H

// ����Ҫ����/������ô������, ����Ҫ��С��COMM_LENTH
#define UART_RXBUF_SIZE		64		//���ջ���Ĵ�С
#define UART_TXBUF_SIZE		64		//���ͻ���Ĵ�С

typedef struct 
{
	uint8_t TxBuf[UART_TXBUF_SIZE];
	uint8_t RxBuf[UART_RXBUF_SIZE];
	uint16_t TxLen;
	uint16_t TxPoint;
	uint16_t RxLen;
	uint8_t RecFlag;
	uint16_t Timer;		/*T15/T35��ʱ*/
}UART_BUF;

extern  UART_BUF UART0Buf;

void printf_test(void);

#endif /*__USART_DRIVER_H */

