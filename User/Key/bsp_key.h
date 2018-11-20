#ifndef __BSP_KEY_H
#define	__BSP_KEY_H

#define KEY_ON		0
#define KEY_OFF		1

#define HIGH_POLAR	0	// 高有效
#define LOW_POLAR	1	// 低有效

//extern uint8_t sys_changed;

//void EXTI_PA0_Config(void);
void EXTI_PE4_Config(void);

uint16_t GPIO_ReadInputAll(uint8_t polar);

#endif /* __BSP_KEY_H */
