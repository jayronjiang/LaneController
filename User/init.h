#ifndef __INIT_H
#define	__INIT_H

#include "stm32f10x.h"
#include "init.h"
#include "scheduler.h"

#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_SysTick.h"
#include "scheduler.h"


#define INT_ENABLE()			__set_PRIMASK(0)
#define INT_DISABLE()			__set_PRIMASK(1)

void Init_System(void);

#endif /* __INIT_H */


