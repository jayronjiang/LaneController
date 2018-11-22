/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   SysTick 系统滴答时钟1ms中断函数库,中断时间可自由配置.
  *          常用的有 1us 10us 1ms 中断.
  ******************************************************************************
  */
  
#include "include.h"

/******************************************************************************
 * 函数名:	SysTick_start 
 * 描述:  	启动系统滴答定时器 SysTick
 *
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void SysTick_start(void)
{
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}

/******************************************************************************
 * 函数名:	SysTick_stop 
 * 描述:  	停止系统滴答定时器 SysTick
 *
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void SysTick_stop(void)
{
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	//if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0库版本
	if (SysTick_Config(SystemCoreClock / 1000 /TFACT))	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while (1);
	}
		
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;	// 关闭滴答定时器  
}


////************* 此后为延时程序，对应中断间隔为10us *********************************************************

//static __IO u32 TimingDelay;

///**
//  * @brief   us延时程序,10us为一个单位
//  * @param  
//  *		@arg nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
//  * @retval  无
//  */
//void Delay_us(__IO u32 nTime)
//{ 
//	TimingDelay = nTime;	

//	// 使能滴答定时器  
//	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

//	while(TimingDelay != 0);
//}

///**
//  * @brief  获取节拍程序
//  * @param  无
//  * @retval 无
//  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
//  */
//void TimingDelay_Decrement(void)
//{
//	if (TimingDelay != 0x00)
//	{ 
//		TimingDelay--;
//	}
//}
/*********************************************END OF FILE**********************/
