#include "include.h"

/******************************************************************************
 * 函数名:	LED_GPIO_Config 
 * 描述:  	LED灯的初始化
 * 			LED1: PB5,     LED2:PE5
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void LED_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); // 使能PC端口时钟  

  // LED1	
  GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5);	//选择对应的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PC端口

  // LED2
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); // 使能PC端口时钟  
	
  GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5);	//选择对应的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);  //初始化PC端口

  /* 关闭所有LED*/
  LED_Set(LED_1, OFF);
  LED_Set(LED_2, OFF);
}

/******************************************************************************
 * 函数名:	LED_Flashing 
 * 描述:  	LED_No闪烁flashtime次,每次亮mtime,灭mtime.
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.16
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void LED_Flashing(uint16_t LED_No,uint16_t mtime, uint16_t flashtime)
{
	do
	{
		switch (LED_No)
		{
		case (LED_1): 
	     		LED1(ON);
			DelayAndFeedDog(mtime);
			LED1(OFF);
			DelayAndFeedDog(mtime);
	      		break;

		case (LED_2): 
	     		LED2(ON);
			DelayAndFeedDog(mtime);
			LED2(OFF);
			DelayAndFeedDog(mtime);
	      		break;

		//case (LED_3): 
	     	//	LED3(ON);
		//	DelayAndFeedDog(mtime);
		//	LED3(OFF);
		//	DelayAndFeedDog(mtime);
	      //		break;

		default: 
			break;
		}
	}while(--flashtime>0);
}

/******************************************************************************
 * 函数名:	LED_Set 
 * 描述:  	控制LED_No亮或者灭
 *
 * 输入参数: iState: ENABLE, 亮,   DISABLE: 灭
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.16
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void LED_Set(uint16_t LED_No, FunctionalState iState)
{
	switch (LED_No)
	{
	case (LED_1): 
	     	LED1(iState);
	      	break;

	case (LED_2): 
	     	LED2(iState);
	      	break;

	//case (LED_3): 
	//     	LED3(iState);
	//      	break;

	default: 
		break;
	}
}


