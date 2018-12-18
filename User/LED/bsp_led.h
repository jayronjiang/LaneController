#ifndef __BSP_LED_H
#define	__BSP_LED_H

#include "stm32f10x.h"

/* the macro definition to trigger the led on or off 
 * 1 - ON
 - 0 - OFF
 */

#define digitalToggle(p,i) {p->ODR ^=i;}
#define LED1_TOGGLE 		digitalToggle(GPIOC,GPIO_Pin_10)
#define LED2_TOGGLE 		digitalToggle(GPIOC,GPIO_Pin_11)

//#define ON  ENABLE
//#define OFF DISABLE

#define ON   DISABLE
#define OFF  ENABLE

#define LED_1		0
#define LED_2		1
//#define LED_2		GPIO_Pin_8
//#define LED_3		GPIO_Pin_9

#define LED_COM		LED_1
#define LED_RUN		LED_2	// 运行灯

/*带参宏，可以像内联函数一样使用*/
#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_10);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_10)
					
#define LED2(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_11);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_11)

//#define LED2(a)	if (a)	\
//					GPIO_SetBits(GPIOB,GPIO_Pin_8);\
//					else		\
//					GPIO_ResetBits(GPIOB,GPIO_Pin_8)

//#define LED3(a)	if (a)	\
//					GPIO_SetBits(GPIOB,GPIO_Pin_9);\
//					else		\
//					GPIO_ResetBits(GPIOB,GPIO_Pin_9)

void LED_GPIO_Config(void);
void LED_Flashing(uint16_t LED_No,uint16_t mtime, uint16_t flashtime);
void LED_Set(uint16_t LED_No, FunctionalState iState);
		

#endif /* __BSP_LED_H */

