#ifndef __ATE_SHELL_H
#define	__ATE_SHELL_H

#define SHELL_COM		UART2_COM
#define SHELL_USART		USART2

void ATE_main(void);
uint8_t xm_load_tx_record(void);

#endif	/*__ATE_SHELL_H*/

