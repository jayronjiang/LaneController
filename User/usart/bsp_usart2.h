#ifndef __USART2_H
#define	__USART2_H

#if (UART_NUM == 2)
void USART2_Config(uint32_t baudrate);
void NVIC_USART2_Configuration(void);
void USART2_Init(uint32_t baudrate);
#endif

#endif /* __USART1_H */

