 /**
  ******************************************************************************
  * @file    ext_wdt.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   外部看门狗处理逻辑.
  *
  ******************************************************************************
  */
  
#include "include.h"

uint8_t wdt_counter = 0;


/***********************************************************************************
 * 函数名:	Ext_Wdt_Feed 
 * 描述: 
 *           	-喂狗函数, 输出一个窄高脉冲.
 *		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:20181109
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
 #if 0
void Ext_Wdt_Feed(void)
{
	// 输出一个高脉冲
	GPIO_SetBits(EXT_WDT_GRP,EXT_WDT_PIN);
	// 需要几个NOP要调一下
	 __ASM ("nop");
	 __ASM ("nop");
	//__NOP; //会被优化
	 __ASM ("nop")
    	GPIO_ResetBits(EXT_WDT_GRP,EXT_WDT_PIN);
}
#endif
void Ext_Wdt_Feed(void)
{
	 __ASM ("nop");	// 没有外部看门狗
}

/***********************************************************************************
 * 函数名:	Ext_Wdt_GPIO_Config 
 * 描述: 
 *           	-外部看门狗的GPIO配置.
 *		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:20181109
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
void Ext_Wdt_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd( EXT_WDT_RCC_GRP, ENABLE); // 使能PC端口时钟  
	
  GPIO_InitStructure.GPIO_Pin = EXT_WDT_PIN;	//选择对应的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(EXT_WDT_GRP, &GPIO_InitStructure);  //初始化PC端口
	
  GPIO_ResetBits(EXT_WDT_GRP,EXT_WDT_PIN);	 // 初始化为低电平
  Ext_Wdt_Feed();
}

