#ifndef __BSP_KEY_H
#define	__BSP_KEY_H

#define KEY_ON		0
#define KEY_OFF		1


/*----------------------------------------------------------------------------*
 *  需要注意的是代码里面的逻辑判断已经是低有效
 *  比如, 判断是否发生报警:
 *  if(Status_Get(ALARM) == 0) {}
 *
 *  所以在检测的时候要使用高有效,不要电平取反!
 *  在没有接电路,测试时候可以使用低有效
 *----------------------------------------------------------------------------*/
/**/
/**/
/*实际电路逻辑也是低有效*/
/*所以这里要选高有效*/
#define HIGH_POLAR	0	// 高有效
#define LOW_POLAR	1	// 低有效

void EXTI_PE4_Config(void);
uint16_t GPIO_ReadInputAll(uint8_t polar);
void ReadKey(void);

#endif /* __BSP_KEY_H */
