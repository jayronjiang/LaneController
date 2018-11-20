#include "include.h"

/*ϵͳ�ĺ��뼶�ۼ�ʱ��,������Ϊ�޷�����,������ͬʱ��Ĳ���������*/
volatile uint32_t system_time_ms=0;

/******************************************************************************
 * ������:	Delay_Ms 
 * ����:  	������ʱ����������ms��ʱ
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
void Delay_Ms(uint32_t myMs)
{
	uint32_t i;
	while(myMs--)
	{
		// ���Ϊ9�����, ���Ϊ72M/9 = 8K.
		i=8000/TFACT;
		while(i--);
	}
}

uint16_t go_onflag;
/******************************************************************************
 * ������:	Delay_Xus 
 * ����:  	ʹ�ö�ʱ����ʽ��������us��ʱ,��Delay_Xmsһ��
 * 			ʵ�����岻��,���ܾ�����΢��һ��
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
void Delay_Xus(uint16_t us)
{   
	TIM_SetCounter(TIM4,0);
	TIM_SetAutoreload(TIM4,us);
	TIM_Cmd(TIM4, ENABLE);
	while(go_onflag==0);
	TIM_Cmd(TIM4, DISABLE);
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
 * ����:������
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

//**********************************************************************************
//                                Timerʵ��
//**********************************************************************************

/******************************************************************************
 * ������:	Start_Timer_x 
 * ����:  	ʹ�ܶ�ʱ��X
 *
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
 * ����:������
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Stop_Timer_x(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIM2, DISABLE);
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
 * ����:������
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Timer_Start(void)
{
	Start_Timer_x(TIM2);
	Start_Timer_x(TIM4);
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
 * ����:������
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
	
	/*TIM4����ж�*/
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
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
 * ����:������
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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 50000-1;	/* 50ms interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	/* The time base is 72M/720 = 1M */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM4,DISABLE);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	/* �������жϱ�־ */
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, DISABLE);
}

/******************************************************************************
 * ������:	Time_Configuration 
 * ����:  	��ʼ����ʱ������
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
 * ����:������
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	
	go_onflag=1;
}

#if 0
/*��ȡ����ʱ��(��ȷ��10us)*/
uint16_t  Rrepeati;
uint16_t  Rrepeatj;
uint64_t readnowtime(void)    
{  
	// 10s, 10msȫ�����us
	return (uint64_t)Rrepeati*10000000+(uint32_t)Rrepeatj*10000+TIM_GetCounter(TIM2)*10;
}
#endif

/******************************************************************************
 * ������:	TIM2_IRQHandler 
 * ����:  20ms ��ʱ�ж�
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
void TIM2_IRQHandler (void)
{   
	static uint16_t t_100ms = 0;

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	
	if( ++t_100ms>= MS_100)		/*100ms,ι��*/
	{
     		t_100ms=0;
		system_flag |= SYSTEM_100MS;
		// 1s�ӻ�û����,����ϵͳ�Ѿ�����,��ι��ֱ�Ӹ�λ
	    	if (++wdt_counter<=6)
	    	{
			Ext_Wdt_Feed();	/*�ж�����ι��,Ҫע�����峤��ռ���ж�ʱ��*/
	    	}
	}

	// ���Ҽ���ʱ�䵽.
	if(WatchingDelayCount > 0)
	{
		WatchingDelayCount--;
		if(WatchingDelayCount==0) 
		{
			// ���ټ��Ӻ���Ȧ,�Ӵ������߼�
			system_flag &= ~CAR_WATCHING;
			//watching_car_passing = FALSE;
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


