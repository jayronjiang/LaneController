/**
  ******************************************************************************
  * @file    test.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   拷机测试子程序.
  ******************************************************************************
  */

#include "include.h"

/******************************************************************************
 * 函数名:	InputDedect 
 * 描述: 查询式读取一个输入状态
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
static bool InputDedect(DEVICE_STATUS_LIST dev)
{
	if (GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_ON)
	{
		Delay_Ms(5);		// 5ms消抖
		if (GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_ON)
		{
			return TRUE;
		}
	}
	return FALSE;
}


/******************************************************************************
 * 函数名:	UpdateFeeDisplay 
 * 描述: 拷机中更新费显
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
static void UpdateFeeDisplay(unsigned char Index)
{
	if (Index%2)
	{
		message_pack_printf(PC_UART, FEE_R_MSG); // 费显为红色
		Delay_Ms(250);
		message_pack_printf(PC_UART, ALL8_MSG);
	}
	else
	{
		message_pack_printf(PC_UART, FEE_G_MSG); // 费显为绿色
	}
}

/******************************************************************************
 * 函数名:	PlayVoice 
 * 描述: 拷机中播放声音
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
static void PlayVoice(unsigned char Index)
{
	if (Index%5)
	{
		message_pack_printf(PC_UART, VOXPLAY_MSG); 
	}
	else
	{
		message_pack_printf(PC_UART, FEE_G_MSG); // 声音停止?
	}
}

/******************************************************************************
 * 函数名:	SetControlPort 
 * 描述: 控制外设依次输出指定的电平.
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
static void SetControlPort(bool bOnOff)
{
	uint8_t tmpPortData = 0;
	uint8_t i = 0;

	tmpPortData =0xFE;
	Command_Act(tmpPortData);
	for (i = 0; i < 16; i++)
	{
		Command_Act(tmpPortData);
		tmpPortData <<= 1;
		Delay_Ms(10);
	}
	DelayAndFeedDog(400);
	
	tmpPortData = 0xFF;
	if(bOnOff)
	{
		tmpPortData = 0;	//set on
	}

	Command_Act(tmpPortData);
	Delay_Ms(2);
	tmpPortData |= 0x03;
	Command_Act(tmpPortData);
}


/******************************************************************************
 * 函数名:	TestForLC301 
 * 描述: 老化测试子程序
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void TestForLC301(void)
{
	unsigned char Index=0;

	/*如果上电读取P2.6有效,进入老化测试*/
	if(!InputDedect(BAK1))
	{
		return;
	}

	while(1)
	{
		UpdateFeeDisplay(Index);
		PlayVoice(Index);
		
		SetControlPort(TRUE);
		DelayAndFeedDog(2000);	// 延时2s
		SetControlPort(FALSE);
		
		Delay_Ms(50);
		Index++;
	}
}

