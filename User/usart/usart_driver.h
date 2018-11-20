#ifndef __USART_DRIVER_H
#define	__USART_DRIVER_H

// 不需要接收/发送那么长数据, 接收要不小于COMM_LENTH
#define UART_RXBUF_SIZE		64		//接收缓冲的大小
#define UART_TXBUF_SIZE		64		//发送缓冲的大小

typedef struct 
{
	uint8_t TxBuf[UART_TXBUF_SIZE];
	uint8_t RxBuf[UART_RXBUF_SIZE];
	uint16_t TxLen;
	uint16_t TxPoint;
	uint16_t RxLen;
	uint8_t RecFlag;
	uint16_t Timer;		/*T15/T35计时*/
}UART_BUF;

extern  UART_BUF UART0Buf;

void printf_test(void);

#endif /*__USART_DRIVER_H */

