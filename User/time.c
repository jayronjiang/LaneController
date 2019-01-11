 /**
  ******************************************************************************
  * @file    time.c
  * @author  Jerry
  * @date    21-Nov-2018
  *
  * @brief   处理时钟中断部分,这里用了2个定时器TIM2(20ms), TIM4(动态).
  *          ms定时器在stm32f10x_it.c中被处理.
  *
  ******************************************************************************
  */

#include "include.h"

/*系统的毫秒级累加时间,由于其为无符号型,*/
/* 暂时未被使用*/
volatile uint32_t system_time_ms=0;

/******************************************************************************
 * 函数名:	Delay_Ms 
 * 描述:  	不带定时器的阻塞型ms延时
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
void Delay_Ms(uint32_t myMs)
{
	uint32_t i;
	while(myMs--)
	{
		// 汇编为9个语句, 因此为72M/9 = 8K.
		i=8000;
		while(i--);
	}
}

#if 1
// 不是很准确
// 主要是while 占用语句多，i=8/TFACT微不足道
void Delay_Us(uint32_t myUs)
{
	uint32_t i;
	while(myUs--)
	{
		i=6;
		while(i--);
	}
}
#endif

uint16_t go_onflag;
/******************************************************************************
 * 函数名:	Delay_Xus 
 * 描述:  	使用定时器方式的阻塞型us延时,和Delay_Xms一样
 * 			实际意义不大,可能精度稍微高一点
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
void Delay_Xus(uint16_t us)
{   
	TIM_SetCounter(TIM5,0);
	TIM_SetAutoreload(TIM5,us);
	TIM_Cmd(TIM5, ENABLE);
	while(go_onflag==0);
	TIM_Cmd(TIM5, DISABLE);
	go_onflag=0;
}

/******************************************************************************
 * 函数名:	Delay_Xms 
 * 描述:  	带定时器的阻塞型ms延时
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
void Delay_Xms (uint16_t ms)
{
	uint16_t icount = ms/TFACT;
    	for ( ; icount > 0; icount--)
    	{
	 	Delay_Xus(1000);
	 }
}


/******************************************************************************
 * 函数名:	DelayAndFeedDog 
 * 描述:  	带喂狗功能的延时,延时中每20ms喂狗一次
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
void DelayAndFeedDog(uint32_t myMs)
{
	// Do not let the myMs be greater than the watchdog period.
	//  Feed the Watchdog every 20ms.
	uint32_t wcount;
	for ( wcount=0; wcount<myMs/20; wcount++)
	{
		Ext_Wdt_Feed();
		// Feed the Watchdog every 20ms.
		Delay_Ms(20);
	}
	// when jump out the loop, feed the last watchdog.
	Ext_Wdt_Feed();
}

//*****************************************************************************
//                                Timer实现
//*****************************************************************************

/******************************************************************************
 * 函数名:	Start_Timer_x 
 * 描述:  	使能定时器X
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
void Start_Timer_x(TIM_TypeDef* TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, ENABLE);
}

/******************************************************************************
 * 函数名:	Stop_Timer_x 
 * 描述:  	停止定时器X
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
void Stop_Timer_x(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
}

/******************************************************************************
 * 函数名:	Timer_Start 
 * 描述:  	使能所有定时器
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
void Timer_Start(void)
{
	Start_Timer_x(TIM2);
	Start_Timer_x(TIM5);
	//Start_Timer_x(TIM4);
}

/******************************************************************************
 * 函数名:	Time_NVIC_Configuration 
 * 描述:  	定时器优先级的配置
 *
 * 定时器2: 抢占优先级为0, 子优先级为0
 * 定时器4: 抢占优先级为1, 子优先级为1
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
void Time_NVIC_Configuration(void)
{   
	NVIC_InitTypeDef NVIC_InitStructure; 

	/*TIM2溢出中断*/
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 	
	// 设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// 设置子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  
	
	/*TIM5溢出中断*/
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;	
	// 设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// 设置子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*TIM4溢出中断*/
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	// 设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// 设置子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	  
}

/******************************************************************************
 * 函数名:	Time_Mode_Config 
 * 描述:  	初始化定时器2为20ms,定时器4为50ms
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
void Time_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* 配置计时模式的模式的基本定时器TIM2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 2000-1;		/* 20ms time interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;		/* The time base is 72M/720 = 100K */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	/* 清除溢出中断标志 */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM2, ENABLE);
	
	/* 配置延时定时器TIM4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 50000-1;	/* 50ms interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	/* The time base is 72M/720 = 1M */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM5,DISABLE);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);	/* 清除溢出中断标志 */
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM5, DISABLE);
}

/******************************************************************************
 * 函数名:	Time_Configuration 
 * 描述:  	初始化定时器配置
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
void Time_Configuration(void)
{
	Time_NVIC_Configuration();
	Time_Mode_Config();
}

/******************************************************************************
 * 函数名:	TIM4_IRQHandler 
 * 描述:  动态定时器,其ARR 是在Delay_Xus中被更新
 *         			暂时用作us定时器
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
void TIM5_IRQHandler (void)
{
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);	
	go_onflag=1;
}

/******************************************************************************
 * 函数名:	TIM2_IRQHandler 
 * 描述:  20ms 定时中断
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
void TIM2_IRQHandler (void)
{   
	static uint16_t t_100ms = 0;

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	/*1s,喂狗, 100ms标志位暂时未使用*/
	if( ++t_100ms>= MS_100)
	{
     		t_100ms=0;
		system_flag |= SYSTEM_100MS;
		// 1s钟还没清零,表明系统已经卡死,不喂狗直接复位
	    	if (++wdt_counter<=6)
	    	{
			Ext_Wdt_Feed();	/*中断里面喂狗,要注意脉冲长度占用中断时间*/
	    	}
	}

	/* 防砸监视时间到.*/
	if(WatchingDelayCount > 0)
	{
		WatchingDelayCount--;
		if(WatchingDelayCount==0) 
		{
			// 不再监视后线圈,接触防砸逻辑
			system_flag &= ~CAR_WATCHING;
		}
	}

	/* 系统检测状态和控制位是否一致,每3s检测一次*/
	if(detect_time_counter > 0)
	{
		detect_time_counter--;
	}
	else
	{
		system_flag |= SYS_ERR_CHK;
		detect_time_counter = AUTO_DETCET_TIME;
	}

	/* 报警长度,10S*/
	if(alarm_time_counter > 0)
	{
		alarm_time_counter--;
		if(alarm_time_counter == 0)
		{
			DeviceX_Deactivate(VOX_ALM);
			DeviceX_Deactivate(LIGHT_ALM);
		}
	}
} 



/////////////////////////////////////////////////////////////////////////////

//TIM4 CH3 PWM输出设置 PB8
//PWM输出初始化
//arr：自动重装值,arr必须为0xFF, 
// 	      因为PCM的采样值都是以0xFF为最大溢出值作为参考的
//psc：时钟预分频数, 255*psc = 72M/8K, APB1是36M?
void TIM4_PWM_Init(u16 arr,u16 psc)
{		 					 
	
 	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIMx外设
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB外设时钟使能
	
 
  //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM4_CH3
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM4_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用功能输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIO

	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置自动重装载周期值
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置预分频值 不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //CH3 PWM2模式	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = DUTY_50; //设置50%的脉宽,即开始不发声
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //OC1 低电平有效 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据指定的参数初始化外设TIMx

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3 预装载使能
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	//TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE 主输出使能,高级定时器必须开启这个 
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	/* 清除溢出中断标志 */
	//TIM_ITConfig(TIM4,TIM_IT_CC3,ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM4, DISABLE);  //使能TIMx  										  
} 

