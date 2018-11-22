#include "include.h"

/******************************************************************************
 * 函数名:	GPIO_ReadInputAll 
 * 描述:  	 一次把所有的键值读出来,返回的是经过转换的高有效的值
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
uint16_t GPIO_ReadInputAll(uint8_t polar)
{
	uint16_t key_val = 0;
	uint16_t tmp = 0;
	DEVICE_STATUS_LIST dev_status = TTL;

	for( dev_status=TTL; dev_status < DEV_STATUS_NUM; dev_status++)
	{
		tmp = GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,
										device_status_queue[dev_status].gpio_pin);

		key_val = tmp << dev_status;	// 第几个键值向左移几位.
	}
	if ( polar == LOW_POLAR )
	{
		key_val = ~key_val;			// 把低有效全部转为高有效
	}

	return key_val;
}
 
/******************************************************************************
 * 函数名:	ReadKey 
 * 描述:  	读取外部输入状态
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
void ReadKey(void)
{
	static uint16_t key_port_last=0;	/*上次扫描得到的按键端口状态*/
	static uint16_t key_effective_port_last=0; /*上次经过防抖的真实按键端口状态*/
	uint16_t key_active_port=0;	/*本次扫描键盘端口得到的状态*/
	uint16_t key_effective_port=0;	/*表示经过消抖后确认得到的按键状态*/
	
        /*按键处理*/
	key_active_port = GPIO_ReadInputAll(LOW_POLAR);	/*按键端口低电平有效,*/
	
	key_effective_port = (key_active_port&key_port_last);		/*当前状态与上次状态,消抖*/
	device_status_used.status_word[USED] = key_effective_port&0xFF;
	key_port_last = key_active_port;		/*为了消抖，需要得到上次的状态*/
	
	if (key_active_port !=key_effective_port_last)
	{
		system_flag |= SYS_CHANGED;	// 按键真正地变位
		key_effective_port_last = key_active_port;
	}
}


static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI_PE4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
											
	/* config the NVIC */
	NVIC_Configuration();

	/* EXTI line gpio config*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       		// PE4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* EXTI line mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}

/******************************************************************************
 * 函数名:	EXTI4_IRQHandler 
 * 描述:  	外部按键中断, PE4
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
 // bug:这里按键有点问题,上升沿和下降沿都产生了中断
void EXTI4_IRQHandler(void)
{
	Delay_Ms(20);
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) //确保是否产生了EXTI Line中断
	{
		// LED1 取反		
		LED1_TOGGLE;
		printf("USART1\n");
		EXTI_ClearITPendingBit(EXTI_Line4);     //清除中断标志位
	}  
}


