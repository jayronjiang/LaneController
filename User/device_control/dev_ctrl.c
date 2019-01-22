/**
  ******************************************************************************
  * @file    dev_ctrl.c
  * @author  Jerry
  * @date    22-Nov-2018
  *
  * @brief   外设的读取和控制处理
  *
  ******************************************************************************
  */
  
#include "include.h"

DEVICE_CONTROL_BITS device_control_used;  // 设备控制字,根据此字具体的值控制设备

// device_status_char1:设备状态字节
// 0位: 雨棚灯状态      --0为红灯，1为绿灯
// 1位: 封道栏杆状态--0为打开，1为关闭
// 2位: 脚踏报警状态--0为有报警，1为无报警
// 3位: 自动栏杆状态--0为降杆，1为抬杆
// 4位: 前线圈                 --0为有车，1为无车
// 5位: 后线圈                 --0为有车，1为无车
// 6位: 车道通行灯      --0为红灯，1为绿灯
// 7位: 备用                       --0为启动，1为无
DEVICE_STATUS_BITS device_status_used;	// 外部状态字,检测外部设备的状态

// bug 1: 注意这2个都没有被初始化!
//DEVICE_CONTROL_BITS device_control_backup;  // 设备控制字备份

// device_status_bakused: 其中设备状态字节2:
// 0位: 备用
// 1位: 备用
// 2位: 栏杆       --('0'表示正常, '1'表示出错)
// 3位: 备用
// 4位: 备用
// 5位: 备用
// 6位: 顶棚灯 --('0'表示正常, '1'表示出错)
// 7位: 备用
//DEVICE_STATUS_BITS device_status_backup;	   // 设备状态字的备份

DEVICE_CTRL	device_ctrl_queue[DEV_CTRL_NUM];	// 设备控制配置组,配置控制口的具体物理地址
DEVICE_STATUS device_status_queue[DEV_STATUS_NUM]; //设备状态配置组

uint8_t bLastLaneRedGreenOperateState = RED; // 保存上次的通行灯状态
bool ALG_down_flag_bit = FALSE;			// 指示降杠是否开始TRUE:降杠开始
bool ALG_up_flag_bit = FALSE;			// 指示抬杠是否开始TRUE:抬杠开始
bool LastLaneLampState = FALSE;			// 保存上次亮灯的状态
//bool watching_car_passing = FALSE;		// 后线圈监视
bool autoBarEnable = FALSE;				// 自动落杠使能
bool dete_bit_recd = FALSE;				// 这个是保存BACK_COIL的上次状态
bool bFeeCleared = FALSE;				// 费县是否被清除?

uint16_t WatchingDelayCount = 0;					// 防砸监视时间
uint16_t detect_time_counter = AUTO_DETCET_TIME;	//系统参数检查间隔时间
uint16_t alarm_time_counter = 0;					// 警报持续时间

/***********************************************************************************
 * 函数名:	Device_Ctrl_Queue_Init 
 * 描述: 
 *           	-控制外设的GPIO初始化,初始化和IO口有关的第一个字节.
 *		这个函数移植时需要改变
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
void Device_Ctrl_Queue_Init(void)
{
	DEVICE_CTRL_LIST dev_type = BAR_UP;

	for( dev_type=BAR_UP; dev_type<DEV_CTRL_NUM; dev_type++)
	{
		switch(dev_type)
		{
		case BAR_UP:
			device_ctrl_queue[dev_type].gpio_grp = BAR_UP_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAR_UP_POUT;
			break;

		case BAR_DOWN:
			device_ctrl_queue[dev_type].gpio_grp = BAR_DOWN_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAR_DOWN_POUT;
			break;

		case TTL_GREEN:
			device_ctrl_queue[dev_type].gpio_grp = TTL_GREEN_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = TTL_GREEN_POUT;
			break;

		case VOX_ALM:
			device_ctrl_queue[dev_type].gpio_grp = VOX_ALM_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = VOX_ALM_POUT;
			break;

		case LIGHT_ALM:
			device_ctrl_queue[dev_type].gpio_grp = LIGHT_ALM_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = LIGHT_ALM_POUT;
			break;

		case LAN_LAMP:
			device_ctrl_queue[dev_type].gpio_grp = LAN_LAMP_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = LAN_LAMP_POUT;
			break;

		case BAK1_USED:
			device_ctrl_queue[dev_type].gpio_grp = BAK1_USED_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAK1_USED_POUT;
			break;

		case BAK2_USED:
			device_ctrl_queue[dev_type].gpio_grp = BAK2_USED_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAK2_USED_POUT;
			break;

		default:
			break;
		}
		DEVICE_GPIO_OUT_Config(dev_type);
	}
}

/***********************************************************************************
 * 函数名:	Device_Stat_Queue_Init 
 * 描述: 
 *           	-外设传感器的GPIO初始化,初始化和IO口有关的第一个字节.
 *		这个函数移植时需要改变
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
void Device_Stat_Queue_Init(void)
{
	DEVICE_STATUS_LIST dev_type = TTL;

	for( dev_type=TTL; dev_type<DEV_STATUS_NUM; dev_type++)
	{
		switch(dev_type)
		{
		case TTL:
			device_status_queue[dev_type].gpio_grp = TTL_IN_GRP;
			device_status_queue[dev_type].gpio_pin = TTL_PIN;
			break;

		case LG:
			device_status_queue[dev_type].gpio_grp = LG_IN_GRP;
			device_status_queue[dev_type].gpio_pin = LG_PIN;
			break;

		case ALARM:
			device_status_queue[dev_type].gpio_grp = ALARM_IN_GRP;
			device_status_queue[dev_type].gpio_pin = ALARM_PIN;
			break;

		case ALG:
			device_status_queue[dev_type].gpio_grp = ALG_IN_GRP;
			device_status_queue[dev_type].gpio_pin = ALG_PIN;
			break;

		case FRONT_COIL:
			device_status_queue[dev_type].gpio_grp = FRONT_IN_GRP;
			device_status_queue[dev_type].gpio_pin = FRONT_PIN;
			break;

		case BACK_COIL:
			device_status_queue[dev_type].gpio_grp = BACK_COIN_IN_GRP;
			device_status_queue[dev_type].gpio_pin = BACK_COIN_PIN;
			break;

		case BAK1:
			device_status_queue[dev_type].gpio_grp = BAK1_IN_GRP;
			device_status_queue[dev_type].gpio_pin = BAK1_PIN;
			break;

		case BAK2:
			device_status_queue[dev_type].gpio_grp = BAK2_IN_GRP;
			device_status_queue[dev_type].gpio_pin = BAK2_PIN;
			break;

		default:
			break;
		}
		DEVICE_GPIO_IN_Config(dev_type);
	}
}

/***********************************************************************************
 * 函数名:	DeviceX_Activate 
 * 描述: 
 *           	-激活指定外设,低电平有效,使用复位语句
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
void DeviceX_Activate(DEVICE_CTRL_LIST dev)
{
	assert_param(dev<DEV_CTRL_NUM);
	GPIO_ResetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}

/***********************************************************************************
 * 函数名:	DeviceX_Deactivate 
 * 描述: 
 *           	-无效指定外设,低电平有效,使用置位语句
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
void DeviceX_Deactivate(DEVICE_CTRL_LIST dev)
{
	assert_param(dev<DEV_CTRL_NUM);
	GPIO_SetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}

/***********************************************************************************
 * 函数名:	Drop_LG_Start 
 * 描述: 
 *           	-降杠动作开始
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
void Drop_LG_Start(void)
{
	// BAK1_USED是降杠和抬杠选择逻辑开关
	// high: 降杠	low: 抬杠
	// high/low是逻辑高电平/低电平，不是物理电平
	DeviceX_Activate(BAK1_USED);
	DeviceX_Activate(BAR_DOWN);
	ALG_down_flag_bit = TRUE;		//指示开始降杠
	ALG_up_flag_bit = FALSE;		//清除抬杠指示
}

/***********************************************************************************
 * 函数名:	Rise_LG_Start 
 * 描述: 
 *           	-抬杠动作开始
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
void Rise_LG_Start(void)
{
	// BAK1_USED是降杠和抬杠选择逻辑开关
	// high: 降杠	low: 抬杠
	// high/low是逻辑高电平/低电平，不是物理电平
	DeviceX_Deactivate(BAK1_USED);
	DeviceX_Activate(BAR_UP);
	ALG_down_flag_bit = FALSE;		//清除降杠指示
	ALG_up_flag_bit = TRUE;		//抬杠开始
}

/***********************************************************************************
 * 函数名:	LG_Act_End 
 * 描述: 
 *           	-抬杠或降杠脉冲结束
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
void LG_Act_End(void)
{
	DeviceX_Deactivate(BAR_UP);
	DeviceX_Deactivate(BAR_DOWN);
}

/***********************************************************************************
 * 函数名:	RCC_Clock_Set 
 * 描述: 
 *           	-配置外设的时钟
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
void RCC_Clock_Set(GPIO_TypeDef* GPIOx, FunctionalState iState)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
	if (GPIOx == GPIOA)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, iState); 
	}
	else if (GPIOx == GPIOB)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, iState); 
	}
	else if (GPIOx == GPIOC)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, iState); 
	}
	else if (GPIOx == GPIOD)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, iState); 
	}
	else if (GPIOx == GPIOE)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, iState); 
	}
	else if (GPIOx == GPIOF)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, iState); 
	}
	else
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, iState); 
	}
}

/***********************************************************************************
 * 函数名:	DEVICE_GPIO_OUT_Config 
 * 描述: 
 *           	-配置指定输出外设.
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
void DEVICE_GPIO_OUT_Config(DEVICE_CTRL_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启输出按键端口PX的时钟*/
	RCC_Clock_Set(device_ctrl_queue[dev].gpio_grp, ENABLE);
	//RCC_APB2PeriphClockCmd( DEVICE_RCC_GRP, ENABLE); // 使能PC端口时钟  
	GPIO_InitStructure.GPIO_Pin = device_ctrl_queue[dev].gpio_pin;	//选择对应的引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(device_ctrl_queue[dev].gpio_grp, &GPIO_InitStructure);  //初始化端口
	DeviceX_Deactivate(dev);	 // 初始化为无效
}

/***********************************************************************************
 * 函数名:	DEVICE_GPIO_IN_Config 
 * 描述: 
 *           	-配置指定输入外设.
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
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /*开启输入按键端口（PX）的时钟*/
	RCC_Clock_Set(device_status_queue[dev].gpio_grp, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = device_status_queue[dev].gpio_pin;; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_Init(device_status_queue[dev].gpio_grp, &GPIO_InitStructure);
}

/***********************************************************************************
 * 函数名:	Command_Get 
 * 描述: 
 *           	-得到指定控制命令的值
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
uint8_t Command_Get(DEVICE_CTRL_LIST dev)
{
	uint8_t re_val = 0;
	uint8_t posdev = (uint8_t)dev;
	uint8_t temp = 0;

	assert_param(dev<DEV_CTRL_NUM);	// 检查参数不能大于DEV_CTRL_NUM

	temp = BIT(posdev); // 得到第posdev位的掩码.
	temp = (device_control_used.control_word[USED]&temp);  // 取device_control_used.control_word低8位中第posdev位
	re_val = temp?TRUE:FALSE;		// 如果相关位不为0就返回TRUE.

#if 0
	switch(dev)
	{
	case BAR_UP:
		re_val = device_control_used.control_bits.ALG_up_bit;
		break;

	case BAR_DOWN:
		re_val = device_control_used.control_bits.ALG_down_bit;
		break;

	case TTL_GREEN:
		re_val = device_control_used.control_bits.TTL_green_bit;
		break;

	case VOX_ALM:
		re_val = device_control_used.control_bits.VOX_alarm_bit;
		break;

	case LIGHT_ALM:
		re_val = device_control_used.control_bits.Light_alarm_bit;
		break;

	case LAN_LAMP:
		re_val = device_control_used.control_bits.Lane_lamp_bit;
		break;

	case BAK1_USED:
		re_val = device_control_used.control_bits.control_bak1_bit;
		break;

	case BAK2_USED:
		re_val = device_control_used.control_bits.control_bak2_bit;
		break;

	default:
		break;
	}
#endif

	return re_val;
}

/***********************************************************************************
 * 函数名:	Command_Act 
 * 描述: 
 *           	-根据控制字,直接控制IO口
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
void Command_Act(uint8_t act_word)
{
	uint8_t i = 0;

	for (i = 0; i < 8; i++)
	{
		if (act_word & BIT(i))
		{
			GPIO_SetBits(device_ctrl_queue[i].gpio_grp, device_ctrl_queue[i].gpio_pin);
		}
		else
		{
			GPIO_ResetBits(device_ctrl_queue[i].gpio_grp, device_ctrl_queue[i].gpio_pin);
		}
	}
}

/***********************************************************************************
 * 函数名:	detect_ALG_TTL_working 
 * 描述: 
 *           	-在回复PC机信息的时候,检测下动作执行是否成功.
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
bool detect_ALG_TTL_working(void)
{
	// 抬杠已经开始
	if(ALG_up_flag_bit)
	{
		if (Status_Get(ALG))
		{
			device_status_used.status_bits.ALG_work_status = OPT_OK;	//抬杆成功
		}
		else
		{
			device_status_used.status_bits.ALG_work_status = OPT_FAIL;	//抬杠出错
		}
	}

	// 降杠动作已经开始
	if(ALG_down_flag_bit)
	{
		if (Status_Get(ALG))
		{
			device_status_used.status_bits.ALG_work_status = OPT_FAIL;	//降杠出错
		}
		else
		{
			device_status_used.status_bits.ALG_work_status = OPT_OK;	//降杠成功
		}
	}

	/*雨棚灯无论是控制还是状态都是0:红灯, 1: 绿灯*/
	if (Status_Get(TTL) == Command_Get(TTL_GREEN))
	{
		device_status_used.status_bits.TTL_work_status = OPT_OK;
	}
	else
	{
		device_status_used.status_bits.TTL_work_status = OPT_FAIL;
	}

	if (device_status_used.status_bits.ALG_work_status ||device_status_used.status_bits.ALG_work_status)
	{
		return OPT_FAIL;
	}
	else
	{
		return OPT_OK;
	}
}


// 
/***********************************************************************************
 * 函数名:	Status_Get 
 * 描述: 
 *           	-获取指定外设传感器的值.
 * 	在刚开始的自动测试时P2读到的状态和后面的意义是不一样的
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
uint8_t Status_Get(DEVICE_STATUS_LIST dev)
{
	uint8_t re_val = 0;
	
	switch(dev)
	{
	case TTL:
		re_val = device_status_used.status_bits.TTL_detect_bit;
		break;

	case LG:
		re_val = device_status_used.status_bits.LG_detect_bit;
		break;

	case ALARM:
		re_val = device_status_used.status_bits.Alarm_detect_bit;
		break;

	case ALG:
		re_val = device_status_used.status_bits.ALG_detect_bit;
		break;

	case FRONT_COIL:
		re_val = device_status_used.status_bits.FRONT_coil_bit;
		break;

	case BACK_COIL:
		re_val = device_status_used.status_bits.BACK_coil_bit;
		break;

	case BAK1:
		re_val = device_status_used.status_bits.bakup_detect1_bit;
		break;

	case BAK2:
		re_val = device_status_used.status_bits.bakup_detect2_bit;
		break;

	default:
		break;
	}

	return (re_val?TRUE:FALSE);
}

/******************************************************************************
 * 函数名:	control_device_expt_lg 
 * 描述: 设备控制函数，控制除栏杆外的其他设备
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.15
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void control_device_expt_lg(void)
{
	// 雨棚灯控制
	if(Command_Get(TTL_GREEN))
	{
		DeviceX_Activate(TTL_GREEN);
	}
	else
	{
		DeviceX_Deactivate(TTL_GREEN);
	}

	// 声音报警
	if(Command_Get(VOX_ALM))
	{
		DeviceX_Activate(VOX_ALM);
	}
	else
	{
		DeviceX_Deactivate(VOX_ALM);
	}

	// 灯光报警
	if(Command_Get(LIGHT_ALM))
	{
		DeviceX_Activate(LIGHT_ALM);
	}
	else
	{
		DeviceX_Deactivate(LIGHT_ALM);
	}

	// 车道通行灯
	if(Command_Get(LAN_LAMP))
	{
		DeviceX_Activate(LAN_LAMP);
	}
	else
	{
		DeviceX_Deactivate(LAN_LAMP);
	}

	// 备用2
	if(Command_Get(BAK2_USED))
	{
		DeviceX_Activate(BAK2_USED);
	}
	else
	{
		DeviceX_Deactivate(BAK2_USED);
	}
}


/******************************************************************************
 * 函数名:	control_device 
 * 描述: 设备控制函数，根据控制状态字, 驱动相应的输出口
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.08
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void control_device(void)
{
	/* 刚开始都无效*/
	DeviceX_Deactivate(BAR_UP);
	DeviceX_Deactivate(BAR_DOWN);

	/* ALG_down_bit 初始化的时候为1*/
	if(Command_Get(BAR_DOWN))
	{
		// 后线圈没车,高电平
		if (Status_Get(BACK_COIL))
		{
			message_pack_printf(FEE_UART, FEE_R_MSG); 	// 降杠, 费显要为红色
			Drop_LG_Start(); 							//降杠
			WatchingDelayCount = 4*ONE_SECOND;		//防砸监视4S
		}
		else
		{
			// 后线圈有车, 启动自动落杆逻辑
			autoBarEnable = TRUE;
			device_control_used.control_bits.Lane_lamp_bit = 1; //强行改变通行灯命令,因为没降杆
		}
	}

	// ALG_up_bit 初始化的时候为0
	if(Command_Get(BAR_UP))
	{
		message_pack_printf(FEE_UART, FEE_G_MSG); // 降杠, 费显要为红色
		Rise_LG_Start(); 				//降杠
	}

	control_device_expt_lg();

#if 0
	// 雨棚灯控制
	if(Command_Get(TTL_GREEN))
	{
		DeviceX_Activate(TTL_GREEN);
	}
	else
	{
		DeviceX_Deactivate(TTL_GREEN);
	}

	// 声音报警
	if(Command_Get(VOX_ALM))
	{
		DeviceX_Activate(VOX_ALM);
	}
	else
	{
		DeviceX_Deactivate(VOX_ALM);
	}

	// 灯光报警
	if(Command_Get(LIGHT_ALM))
	{
		DeviceX_Activate(LIGHT_ALM);
	}
	else
	{
		DeviceX_Deactivate(LIGHT_ALM);
	}

	// 车道通行灯
	if(Command_Get(LAN_LAMP))
	{
		DeviceX_Activate(LAN_LAMP);
	}
	else
	{
		DeviceX_Deactivate(LAN_LAMP);
	}
#endif

	// 备份车道通行灯位.
	LastLaneLampState = device_control_used.control_bits.Lane_lamp_bit;

	/* 延时250ms, 因为外设需要一个脉冲控制*/
	DelayAndFeedDog(250);
	/*  脉冲输出完成*/
	LG_Act_End();

	/* 参数检查间隔时间*/
	detect_time_counter = AUTO_DETCET_TIME;
}


/***********************************************************************************
 * 函数名:	BarOpRectifyLaneLamp 
 * 描述: 
 *           			自动调整逻辑，以防接收到的上位机命令是矛盾的
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:20181113
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
static void BarOpRectifyLaneLamp(void)
{
	if(Command_Get(BAR_UP)&Command_Get(BAR_DOWN)) 	//错误操作
	{
		device_control_used.control_bits.ALG_down_bit = 0;		// 如果同时接收到抬杠和降杠，则抬杠
	}
	//FeedDog();
	if(Command_Get(BAR_UP))
	{
		//绿灯, 如果抬杠，则必须亮绿灯
		device_control_used.control_bits.Lane_lamp_bit = 1;
	}

	if(Command_Get(BAR_DOWN))
	{
		//红灯, 如果降杠，则必须亮红灯
		device_control_used.control_bits.Lane_lamp_bit = 0;
	}

	// 如果抬杠降杠都没有命令,则保持上次的亮灯状态
	if((Command_Get(BAR_UP)|Command_Get(BAR_DOWN)) == FALSE)
	{
		device_control_used.control_bits.Lane_lamp_bit = LastLaneLampState;
	}
}


/***********************************************************************************
 * 函数名:	ClearFEEdisplay 
 * 描述: 
 *           			 -清除费率显示
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:20181109
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
void ClearFEEdisplay(void)
{
	// 已经清除过了,不用再次清除
	if (bFeeCleared)
	{
	 	return;
	}

	//FeedDog();
	bFeeCleared = TRUE;
	if(bLastLaneRedGreenOperateState == GREEN)
	{
		if(device_control_used.control_bits.bFeeDispSelection ==0)
		{
			//对于陕西版本的费显，只要开绿灯不清除费额
			;
		}
		else
		{
			//对于广东版本的费显，只要开绿灯或红灯都清除费额
			message_pack_printf(FEE_UART, COST_OFF_MSG);
		}
	}
	else
	{
		//不管是哪个版本的费显，只要开红灯都清除费额
		message_pack_printf(FEE_UART, COST_OFF_MSG);
	}
}


/***********************************************************************************
 * 函数名:	params_modify_deal 
 * 描述: 
 *           	-通讯解包后的参数处理
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
void params_modify_deal(void)
{
	/*收到抬杠命令*/
	if(Command_Get(BAR_UP))
	{
		autoBarEnable = device_control_used.control_bits.ALG_control_mode;		/*读取自动降杆模式*/
		/*清除该设置位, 汕芬潮州站军警车道遥控栏杆机需求发起*/
		device_control_used.control_bits.ALG_control_mode = 0;
		system_flag &= ~CAR_WATCHING;
		//watching_car_passing=0;
		/*抬杆后开始计后线圈有无车过 P20100603*/
		dete_bit_recd = FALSE;
	}

	/*根据现场被控设备逻辑情况，调整控制位*/
	BarOpRectifyLaneLamp();
	control_device();			/*收到命令后再次对外设进行控制*/

	//扬声器操作
	if(device_control_used.control_bits.SPEAK_control_bit == 1)
	{
		/* 必须是栏杆没有被控制*/
		if((Command_Get(BAR_UP) == 0)&&(Command_Get(BAR_DOWN) == 0))
		{
			//message_pack_printf(FEE_UART, SPK_MSG);	// 扬声器输出,暂时不输出
		}

		/* 如果是抬杠命令, 和费显绿灯一样的语音*/
		/* 但是要注意串口的选择*/
		if(Command_Get(BAR_UP))
		{
			message_pack_printf(FEE_UART, FEE_G_MSG);
		}
	}
	//该处延时约等于if内的时间，保证连续对费显的操作的间隔时间
	else
	{
		Delay_Ms(50);		// 这个时间需要调
	}

	//费额显示操作,如果不保持费显,则需要更新
	if(device_control_used.control_bits.FEE_displaykeep_bit ==0)
	{
		// 费额需要显示
		if(device_control_used.control_bits.FEE_display_bit == 1)
		{
			message_pack_printf(FEE_UART, COST_ON_MSG);
		}
		else
		{
			//清除费额，打红灯(有些打绿灯也可以)就清除费额显示
			ClearFEEdisplay();	
		}
	}
}
