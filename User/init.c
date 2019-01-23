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

/******************************************************************************
 * 函数名:	IWDG_Init 
 * 描述: 独立看门狗初始化
 *       原理是对外设依次控制,隔1s后读返回状态,看是否成功.
 *
 * 输入参数: prer:分频因子, 0~7(只有低3位有效)
 *		分频因子 = 4*2^prer, 但最大值为256.
 *                 rlr: 重载寄存器值,低11位有效           
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2019.01.15
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void IWDG_Init(u8 prer,u16 rlr)
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);// 打开写保护
	IWDG_SetPrescaler(prer);	// 设置预分频值,这里设为64(100b)
	IWDG_SetReload(rlr);  // 重装载值
	IWDG_ReloadCounter(); //重装载,喂狗
	//IWDG_Enable(); // 使能看门狗
}
/******************************************************************************
 * 函数名:	IWDG_Feed 
 * 描述: 重新加载重装载值,即喂狗
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2019.01.15
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void IWDG_Feed(void)
{   
 	IWDG_ReloadCounter();//reload										   
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

	for (i_ctrl = BAR_UP;  i_ctrl < DEV_CTRL_NUM; i_ctrl++)
	{
		DeviceX_Activate(i_ctrl);	 // 外设全部初始化为无效
	}

	Delay_Ms(250);
	message_send_printf(PC1_UART, TRUE, VER_PRINT_MSG);//poweron info, used for debugging	

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
	
	LED_GPIO_Config();
	
	/*滴答定时器初始化，1ms中断周期*/
	/* 这个定时器用于驱动整个循环运行，放在初始化的最后*/
	/* 也就是硬件初始化完毕后定时器开始运行*/
	SysTick_Init();	// 本程序不带操作系统,只是一个普通的ms定时器

#ifdef SPEAKER_ENABLE
	/* 先配置成115200传输声音文件*/
	Comm1_Init(115200);	// USART1 配置模式为 115200 8-N-1，中断接收
#if (UART_NUM == 2)
	Comm2_Init(115200);	// USART2 配置模式为 115200 8-N-1，中断接收
#endif

	W25QXX_Init();		//W25QXX初始化
	LED_Set(LED_COM, ON);		//编程指示灯亮
	ATE_main();
	LED_Set(LED_COM, OFF);
#endif

/*工作时要设置成9600*/
	Comm1_Init(9600);	// USART1 配置模式为 115200 8-N-1，中断接收
#if (UART_NUM == 2)
	Comm2_Init(9600);	// USART2 配置模式为 115200 8-N-1，中断接收
#endif

	/*上电闪烁3次,每次60ms*/
	LED_Flashing(LED_RUN, 60, 3);
	/*对外部设备进行初始化*/
	Param_Init();

	IWDG_Init(IWDG_Prescaler_64,625);    //预分频值为64，重装载值为625, 看门狗为1s.
	
	/*老化测试子程序,系统将在这里进入老化,不执行后面的*/
	//TestForLC301();		// 暂时不使用

	INT_ENABLE();
	Timer_Start();
	SysTick_start();
}
