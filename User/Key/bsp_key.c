 /**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   外部输出的扫描和防抖,25ms扫描一次.
  *
  ******************************************************************************
  */
#include "include.h"

/******************************************************************************
 * 函数名:	GPIO_ReadInputAll 
 * 描述:  	 一次把所有的键值读出来
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

		key_val |= tmp << dev_status;	// 第几个键值向左移几位.
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
	static uint16_t debounce_buffer[DEBOUNCE_TIME];	//防抖一次就可以
	static uint8_t buffer_index = 0;
	static uint16_t key_effective_port_last=0; /*上次经过防抖的真实按键端口状态*/
	uint16_t key_effective_port=0;	/*表示经过消抖后确认得到的按键状态*/
	uint16_t constant_high = 0xFF;	/*当相应位为1表明读取的几次相应位都为1*/
	uint16_t constant_low = 0xFF;	/*当相应位为1表明读取的几次相应位都为0*/

	uint8_t i = 0;

	/*读取本次键值,下标自增1*/
	debounce_buffer[buffer_index] = GPIO_ReadInputAll(HIGH_POLAR);
	if (++buffer_index >= DEBOUNCE_TIME)
	{
		buffer_index = 0;
	}

	/*同时读取高有效和低有效的2个值*/
        for (i = 0; i < DEBOUNCE_TIME; i++) {

            constant_high &= debounce_buffer[i];
            constant_low &= ~debounce_buffer[i];
        }

	/*先得到全为1的位*/
	/*再得到全为0的位*/
	key_effective_port |= (constant_high&0xFF);
	key_effective_port &= ((~constant_low)&0xFF);

	device_status_used.status_word[USED] = (uint8_t)key_effective_port;
	
	if (key_effective_port !=key_effective_port_last)
	{
		system_flag |= SYS_CHANGED;	// 按键真正地变位
		key_effective_port_last = key_effective_port;
	}
}

/******************************************************************************
 * 函数名:	NVIC_Configuration 
 * 描述:  	外部中断的配置
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

/******************************************************************************
 * 函数名:	EXTI_PE4_Config 
 * 描述:  	配置PE4为外部中断
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

