#ifndef __EXT_WDT_H
#define	__EXT_WDT_H

#define EXT_WDT_PIN			GPIO_Pin_8
#define EXT_WDT_GRP			GPIOB
#define EXT_WDT_RCC_GRP		RCC_APB2Periph_GPIOB

extern uint8_t wdt_counter;

void Ext_Wdt_Feed(void);
void Ext_Wdt_GPIO_Config(void);

#endif
