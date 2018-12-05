#ifndef __USART1_H
#define	__USART1_H

void USART1_Config(uint32_t baudrate);
void NVIC_USART1_Configuration(void);
void USART1_Init(uint32_t baudrate);

#endif /* __USART1_H */
