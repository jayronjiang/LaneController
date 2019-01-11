 /**
  ******************************************************************************
  * @file    time.c
  * @author  Jerry
  * @date    21-Nov-2018
  *
  * @brief   ����ʱ���жϲ���,��������2����ʱ��TIM2(20ms), TIM4(��̬).
  *          ms��ʱ����stm32f10x_it.c�б�����.
  *
  ******************************************************************************
  */

#include "include.h"

/*ϵͳ�ĺ��뼶�ۼ�ʱ��,������Ϊ�޷�����,*/
/* ��ʱδ��ʹ��*/
volatile uint32_t system_time_ms=0;

/******************************************************************************
 * ������:	Delay_Ms 
 * ����:  	������ʱ����������ms��ʱ
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Delay_Ms(uint32_t myMs)
{
	uint32_t i;
	while(myMs--)
	{
		// ���Ϊ9�����, ���Ϊ72M/9 = 8K.
		i=8000;
		while(i--);
	}
}

#if 1
// ���Ǻ�׼ȷ
// ��Ҫ��while ռ�����࣬i=8/TFACT΢�����
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
 * ������:	Delay_Xus 
 * ����:  	ʹ�ö�ʱ����ʽ��������us��ʱ,��Delay_Xmsһ��
 * 			ʵ�����岻��,���ܾ�����΢��һ��
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
 * ������:	Delay_Xms 
 * ����:  	����ʱ����������ms��ʱ
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
 * ������:	DelayAndFeedDog 
 * ����:  	��ι�����ܵ���ʱ,��ʱ��ÿ20msι��һ��
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
//                                Timerʵ��
//*****************************************************************************

/******************************************************************************
 * ������:	Start_Timer_x 
 * ����:  	ʹ�ܶ�ʱ��X
 *
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Start_Timer_x(TIM_TypeDef* TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, ENABLE);
}

/******************************************************************************
 * ������:	Stop_Timer_x 
 * ����:  	ֹͣ��ʱ��X
 *
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Stop_Timer_x(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
}

/******************************************************************************
 * ������:	Timer_Start 
 * ����:  	ʹ�����ж�ʱ��
 *
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Timer_Start(void)
{
	Start_Timer_x(TIM2);
	Start_Timer_x(TIM5);
	//Start_Timer_x(TIM4);
}

/******************************************************************************
 * ������:	Time_NVIC_Configuration 
 * ����:  	��ʱ�����ȼ�������
 *
 * ��ʱ��2: ��ռ���ȼ�Ϊ0, �����ȼ�Ϊ0
 * ��ʱ��4: ��ռ���ȼ�Ϊ1, �����ȼ�Ϊ1
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Time_NVIC_Configuration(void)
{   
	NVIC_InitTypeDef NVIC_InitStructure; 

	/*TIM2����ж�*/
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  
	
	/*TIM5����ж�*/
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*TIM4����ж�*/
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	  
}

/******************************************************************************
 * ������:	Time_Mode_Config 
 * ����:  	��ʼ����ʱ��2Ϊ20ms,��ʱ��4Ϊ50ms
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Time_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* ���ü�ʱģʽ��ģʽ�Ļ�����ʱ��TIM2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 2000-1;		/* 20ms time interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;		/* The time base is 72M/720 = 100K */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	/* �������жϱ�־ */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM2, ENABLE);
	
	/* ������ʱ��ʱ��TIM4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 50000-1;	/* 50ms interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	/* The time base is 72M/720 = 1M */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM5,DISABLE);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);	/* �������жϱ�־ */
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM5, DISABLE);
}

/******************************************************************************
 * ������:	Time_Configuration 
 * ����:  	��ʼ����ʱ������
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Time_Configuration(void)
{
	Time_NVIC_Configuration();
	Time_Mode_Config();
}

/******************************************************************************
 * ������:	TIM4_IRQHandler 
 * ����:  ��̬��ʱ��,��ARR ����Delay_Xus�б�����
 *         			��ʱ����us��ʱ��
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void TIM5_IRQHandler (void)
{
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);	
	go_onflag=1;
}

/******************************************************************************
 * ������:	TIM2_IRQHandler 
 * ����:  20ms ��ʱ�ж�
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void TIM2_IRQHandler (void)
{   
	static uint16_t t_100ms = 0;

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	/*1s,ι��, 100ms��־λ��ʱδʹ��*/
	if( ++t_100ms>= MS_100)
	{
     		t_100ms=0;
		system_flag |= SYSTEM_100MS;
		// 1s�ӻ�û����,����ϵͳ�Ѿ�����,��ι��ֱ�Ӹ�λ
	    	if (++wdt_counter<=6)
	    	{
			Ext_Wdt_Feed();	/*�ж�����ι��,Ҫע�����峤��ռ���ж�ʱ��*/
	    	}
	}

	/* ���Ҽ���ʱ�䵽.*/
	if(WatchingDelayCount > 0)
	{
		WatchingDelayCount--;
		if(WatchingDelayCount==0) 
		{
			// ���ټ��Ӻ���Ȧ,�Ӵ������߼�
			system_flag &= ~CAR_WATCHING;
		}
	}

	/* ϵͳ���״̬�Ϳ���λ�Ƿ�һ��,ÿ3s���һ��*/
	if(detect_time_counter > 0)
	{
		detect_time_counter--;
	}
	else
	{
		system_flag |= SYS_ERR_CHK;
		detect_time_counter = AUTO_DETCET_TIME;
	}

	/* ��������,10S*/
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

//TIM4 CH3 PWM������� PB8
//PWM�����ʼ��
//arr���Զ���װֵ,arr����Ϊ0xFF, 
// 	      ��ΪPCM�Ĳ���ֵ������0xFFΪ������ֵ��Ϊ�ο���
//psc��ʱ��Ԥ��Ƶ��, 255*psc = 72M/8K, APB1��36M?
void TIM4_PWM_Init(u16 arr,u16 psc)
{		 					 
	
 	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʹ��TIMx����
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOB����ʱ��ʹ��
	
 
  //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM4_CH3
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM4_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //���ù������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��GPIO

	TIM_TimeBaseStructure.TIM_Period = arr-1; //�����Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����Ԥ��Ƶֵ ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //CH3 PWM2ģʽ	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = DUTY_50; //����50%������,����ʼ������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //OC1 �͵�ƽ��Ч 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����ָ���Ĳ�����ʼ������TIMx

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3 Ԥװ��ʹ��
	
	TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	//TIM_CtrlPWMOutputs(TIM4,ENABLE);	//MOE �����ʹ��,�߼���ʱ�����뿪����� 
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	/* �������жϱ�־ */
	//TIM_ITConfig(TIM4,TIM_IT_CC3,ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx  										  
} 

