 /**
  ******************************************************************************
  * @file    init.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   系统初始化文件,初始化各种外设和串口.
  *
  ******************************************************************************
  */

#include "include.h"

/*初次上电标志*/
//static uint8_t PowerOnFlag = 0;

/******************************************************************************
 * 函数名:	device_detect 
 * 描述: 检测外设是否接好的子函数
 *       原理是对外设依次控制,隔1s后读返回状态,看是否成功.
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void device_detect(DEVICE_STATUS_LIST dev)
{
	DEVICE_CTRL_LIST dev_ctrl = BAK1_USED;

	dev_ctrl = (DEVICE_CTRL_LIST)dev;	// 取值是一一对应的的,强制进行转换
	
	DelayAndFeedDog(1000);
	DeviceX_Activate(dev_ctrl);				// Output a signal.
	LED_Set(LED_COM, ON);
	DelayAndFeedDog(1000);				// Delay 1.0s

	// 表明继电器安装了,可以继续检查
	if(GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_ON )
	{
		DelayAndFeedDog(1000);			// Delay 1.0s
		DeviceX_Deactivate(dev_ctrl);		// Output a signal.
		if(GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_OFF )
		{
			// 检查外设OK
			LED_Set(LED_COM, OFF);
		}
		else
		{
			/* 外设安装了，但是有异常，附带的灯要报警*/
			DeviceX_Activate(dev_ctrl);	// NO good, Output a signal.
			LED_Set(LED_COM, OFF);
		}	
	}
}

/******************************************************************************
 * 函数名:	system_auto_detect 
 * 描述: 系统开机时自动检测外设是否接好
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.10.19
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void system_auto_detect(void)
{
	DEVICE_STATUS_LIST dev_status = BAK2;
	DEVICE_CTRL_LIST dev_ctrl = BAK2_USED;
	DEVICE_STATUS_LIST i = TTL;
	
	DeviceX_Activate(dev_ctrl);		// Output a signal.
	LED_Set(LED_COM, ON);
	DelayAndFeedDog(1200);		// Delay 1.2s

	/* 表明继电器安装了,可以继续检查*/
	if(GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,device_status_queue[dev_status].gpio_pin) == KEY_ON)
	{
		DeviceX_Deactivate(dev_ctrl);	// Output a Low-signal.
		DelayAndFeedDog(1200);		// Delay 1.2s
		if(GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,device_status_queue[dev_status].gpio_pin) == KEY_OFF )
		{
			// 检查外设OK
			LED_Set(LED_COM, OFF);
		}
		else
		{
			/* 外设安装了，但是有异常，附带的灯要报警*/
			DeviceX_Activate(dev_ctrl);	// NO good, Output a signal.
			LED_Set(LED_COM, OFF);
		}

		/* check the device left 1by1 */
		for (i =TTL, dev_status=BAK1; i < BAK2; i++, dev_status--)
		{
			device_detect(dev_status);
		}

		message_pack_printf(FEE_UART, INTT_MSG);
		LED_Flashing(LED_COM, 1500, 4);		// flashing for 4times, 1.5s once.
		message_pack_printf(FEE_UART, FEE_G_MSG);
		DelayAndFeedDog(3000);	// delay 3s
		message_pack_printf(FEE_UART, FEE_R_MSG);
	}
	/* 否则，外设没有安装好, 跳过检查部分*/
	else	
	{
		DeviceX_Deactivate(dev_ctrl);
		LED_Set(LED_COM, OFF);
	}
}

/******************************************************************************
 * 函数名:	Key_First_Read 
 * 描述: 初始化时候第一次读键值.
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.22
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
static void Key_First_Read(void)
{
	/*控制函数需要读键值,但是此时中断还未开*/
	/*因此需要查询一次外部输入*/
	ReadKey();					// 千万注意此函数的重入
	Delay_Ms(25);					// 防抖
	ReadKey();
	system_flag &= ~SYS_CHANGED;	// 第一次读取不变位
}

/******************************************************************************
 * 函数名:	Param_Init 
 * 描述: 初始化要使用的系统参数
 *         		
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
void Param_Init(void)
{
	DEVICE_CTRL_LIST i_ctrl = BAR_UP;

	Device_Ctrl_Queue_Init();
	Device_Stat_Queue_Init();

	#if 0
	if(PowerOnFlag != (uint8_t)0xAA)
	{
		PowerOnFlag = (uint8_t)0xAA;
		system_auto_detect();
	#ifdef DEBUG_EN
		//message_pack_printf(PC_UART, A_MSG);
		//Delay_Ms(10);
		//message_pack_printf(PC_UART, CR_MSG);
		//Delay_Ms(10);
		//message_pack_printf(PC_UART, CV_MSG);
		//Delay_Ms(10);
		//message_pack_printf(PC_UART, CD_MSG);
	#endif
	}
	else	
	{
		message_pack_printf(PC_UART,VER_PRINT_MSG);	//poweron info, used for debugging
	}
	#endif

	for (i_ctrl = BAR_UP;  i_ctrl < DEV_CTRL_NUM; i_ctrl++)
	{
		DeviceX_Activate(i_ctrl);	 // 外设全部初始化为无效
	}

	Delay_Ms(250);
	message_pack_printf(PC_UART,VER_PRINT_MSG);	//poweron info, used for debugging

	for (i_ctrl = BAR_UP;  i_ctrl < DEV_CTRL_NUM; i_ctrl++)
	{
		DeviceX_Deactivate(i_ctrl);	 // 外设全部初始化为无效
	}

	device_status_used.status_word[USED] = 0;
	device_status_used.status_word[BACKUP] = 0xBB;	// bit2和bit6被使用,初始化为0

	device_control_used.control_word[USED] = 0;
	device_control_used.control_word[BACKUP] = 0;
	device_control_used.control_bits.ALG_down_bit = 1;		// 栏杆要落

	Key_First_Read();
	control_device();	//根据初始化的状态降栏杆
}


/******************************************************************************
 * 函数名:	Init_System 
 * 描述: 初始化系统配置
 *         		
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
volatile uint32_t itest = 0;  // 测试时钟是否准确
void Init_System(void)
{	
	INT_DISABLE();		// 屏蔽所有中断
	/*There are 2 different PreemptionPriorities in the TIM init. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	Time_Configuration();	//系统时间和延时相关定时器初始化
	TIM4_PWM_Init(0xFF,PWM_8KHZ);	//TIM4 PWM初始化, Fpwm=72M/9000=8Khz.
	
	LED_GPIO_Config();
	//EXTI_PE4_Config();

	Comm1_Init(9600);	// USART1 配置模式为 115200 8-N-1，中断接收
	Comm2_Init(9600);	// USART2 配置模式为 115200 8-N-1，中断接收

	W25QXX_Init();		//W25QXX初始化
	
	/*滴答定时器初始化，1ms中断周期*/
	/* 这个定时器用于驱动整个循环运行，放在初始化的最后*/
	/* 也就是硬件初始化完毕后定时器开始运行*/
	SysTick_Init();	// 本程序不带操作系统,只是一个普通的ms定时器

/* 用来测试定时是否准确的语句*/
#if 0
	INT_ENABLE();
	SysTick_start();
	itest  = system_time_ms;
	Delay_Ms(100);	// delay 100ms
	itest  = system_time_ms;
	LED_Flashing(LED_COM, 60, 3);
	itest  = system_time_ms;
	DelayAndFeedDog(260);
	itest  = system_time_ms;

	LED_Set(LED_RUN, OFF); 	// 开始通信指示
	Delay_Us(10);
	LED_Set(LED_RUN, ON); 	// 开始通信指示
	LED_Set(LED_RUN, OFF);
	LED_Set(LED_RUN, ON);
	LED_Set(LED_RUN, ON); 	// 开始通信指示
	LED_Set(LED_RUN, OFF);
	LED_Set(LED_RUN, OFF);
	LED_Set(LED_RUN, ON); 	// 开始通信指示
	LED_Set(LED_RUN, OFF);
	LED_Set(LED_RUN, ON);
	Delay_Us(10);
	LED_Set(LED_RUN, OFF); 	// 开始通信指示
	Delay_Us(10);
	LED_Set(LED_RUN, ON); 	// 开始通信指示
	Delay_Us(10);
	LED_Set(LED_RUN, OFF); 	// 开始通信指示
	Delay_Us(10);
	LED_Set(LED_RUN, ON); 	// 开始通信指示
	Delay_Us(5);
	LED_Set(LED_RUN, OFF); 	// 开始通信指示LED_Set(LED_RUN, ON); 	// 开始通信指示
	Delay_Us(15);
	LED_Set(LED_RUN, ON); 	// 开始通信指示
	Delay_Ms(5);
	LED_Set(LED_RUN, OFF); 	// 开始通信指示
#endif
	

	/*上电闪烁3次,每次50ms*/
	LED_Flashing(LED_COM, 60, 3);
	Param_Init();
	//debug_puts("SPI test: KEY1:Write  KEY0:Read");

	LED_Set(LED_COM, ON);		//编程指示灯亮
	ATE_main();
	LED_Set(LED_COM, OFF);

	Speaker_Init();		//PT	2259初始化
	Dac_Init();			// DAC初始化
	
	/*老化测试子程序,系统将在这里进入老化,不执行后面的*/
	//TestForLC301();		// 暂时不使用

	INT_ENABLE();
	Timer_Start();
	SysTick_start();

	Speaker_Test();
}
