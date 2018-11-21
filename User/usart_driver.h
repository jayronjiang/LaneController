#ifndef __USART_DRIVER_H
#define	__USART_DRIVER_H

//串口类型枚举,可扩展
typedef enum 
{	
	UART1_COM = 0,
	UART2_COM,
	//UART3_COM,

	UART_NUM
}USART_LIST;

// 不需要接收/发送那么长数据, 接收要不小于COMM_LENTH
#define UART_RXBUF_SIZE		32		//接收缓冲的大小
#define UART_TXBUF_SIZE		32		//发送缓冲的大小

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

extern  UART_BUF UARTBuf[UART_NUM];;
extern uint8_t uart_sel;

void printf_test(void);

#endif /*__USART_DRIVER_H */

