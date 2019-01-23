#ifndef __USART_DRIVER_H
#define	__USART_DRIVER_H

//串口类型枚举,可扩展
typedef enum 
{	
	UART1_COM = 0,
	//UART2_COM,
	//UART3_COM,

	UART_NUM
}USART_LIST;

// 不需要接收/发送那么长数据, 接收要不小于COMM_LENTH
#define UART_RXBUF_SIZE		64		//接收缓冲的大小
#define UART_TXBUF_SIZE		64		//发送缓冲的大小

#define PC_USART_NUM		1		// 可以连接几个工控机?

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
extern const USART_LIST pc_com[PC_USART_NUM];

int UART_fputc(int ch);
int  UART_fgetc(void);

#endif /*__USART_DRIVER_H */

