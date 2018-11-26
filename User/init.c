 /**
  ******************************************************************************
  * @file    init.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   ϵͳ��ʼ���ļ�,��ʼ����������ʹ���.
  *
  ******************************************************************************
  */

#include "include.h"

 #if 0
/*�����ϵ��־*/
static uint8_t PowerOnFlag = 0;

/******************************************************************************
 * ������:	device_detect 
 * ����: ��������Ƿ�Ӻõ��Ӻ���
 *       ԭ���Ƕ��������ο���,��1s�������״̬,���Ƿ�ɹ�.
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.19
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void device_detect(DEVICE_STATUS_LIST dev)
{
	DEVICE_CTRL_LIST dev_ctrl = BAK1_USED;

	dev_ctrl = (DEVICE_CTRL_LIST)dev;	// ȡֵ��һһ��Ӧ�ĵ�,ǿ�ƽ���ת��
	
	DelayAndFeedDog(1000);
	DeviceX_Activate(dev_ctrl);				// Output a signal.
	LED_Set(LED_COM, ON);
	DelayAndFeedDog(1000);				// Delay 1.0s

	// �����̵�����װ��,���Լ������
	if(GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_ON )
	{
		DelayAndFeedDog(1000);			// Delay 1.0s
		DeviceX_Deactivate(dev_ctrl);		// Output a signal.
		if(GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_OFF )
		{
			// �������OK
			LED_Set(LED_COM, OFF);
		}
		else
		{
			/* ���谲װ�ˣ��������쳣�������ĵ�Ҫ����*/
			DeviceX_Activate(dev_ctrl);	// NO good, Output a signal.
			LED_Set(LED_COM, OFF);
		}	
	}
}

/******************************************************************************
 * ������:	system_auto_detect 
 * ����: ϵͳ����ʱ�Զ���������Ƿ�Ӻ�
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.10.19
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void system_auto_detect(void)
{
	DEVICE_STATUS_LIST dev_status = BAK2;
	DEVICE_CTRL_LIST dev_ctrl = BAK2_USED;
	DEVICE_STATUS_LIST i = TTL;
	
	DeviceX_Activate(dev_ctrl);		// Output a signal.
	LED_Set(LED_COM, ON);
	DelayAndFeedDog(1200);		// Delay 1.2s

	/* �����̵�����װ��,���Լ������*/
	if(GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,device_status_queue[dev_status].gpio_pin) == KEY_ON)
	{
		DeviceX_Deactivate(dev_ctrl);	// Output a Low-signal.
		DelayAndFeedDog(1200);		// Delay 1.2s
		if(GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,device_status_queue[dev_status].gpio_pin) == KEY_OFF )
		{
			// �������OK
			LED_Set(LED_COM, OFF);
		}
		else
		{
			/* ���谲װ�ˣ��������쳣�������ĵ�Ҫ����*/
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
	/* ��������û�а�װ��, ������鲿��*/
	else	
	{
		DeviceX_Deactivate(dev_ctrl);
		LED_Set(LED_COM, OFF);
	}
}


/******************************************************************************
 * ������:	Key_First_Read 
 * ����: ��ʼ��ʱ���һ�ζ���ֵ.
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.11.22
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static void Key_First_Read(void)
{
	/*���ƺ�����Ҫ����ֵ,���Ǵ�ʱ�жϻ�δ��*/
	/*�����Ҫ��ѯһ���ⲿ����*/
	ReadKey();					// ǧ��ע��˺���������
	Delay_Ms(25);					// ����
	ReadKey();
	system_flag &= ~SYS_CHANGED;	// ��һ�ζ�ȡ����λ
}

/******************************************************************************
 * ������:	Param_Init 
 * ����: ��ʼ��Ҫʹ�õ�ϵͳ����
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
void Param_Init(void)
{
	DEVICE_CTRL_LIST i_ctrl = BAR_UP;

	Device_Ctrl_Queue_Init();
	Device_Stat_Queue_Init();

	/*���������İ�װ���*/
	for (i_ctrl = BAR_UP;  i_ctrl < DEV_CTRL_NUM; i_ctrl++)
	{
		DeviceX_Deactivate(i_ctrl);	 // ����ȫ����ʼ��Ϊ��Ч
	}

	device_status_used.status_word[USED] = 0;
	device_status_used.status_word[BACKUP] = 0xBB;	// bit2��bit6��ʹ��,��ʼ��Ϊ0

	device_control_used.control_word[USED] = 0;
	device_control_used.control_word[BACKUP] = 0;
	device_control_used.control_bits.ALG_down_bit = 1;		// ����Ҫ��

	Key_First_Read();
	control_device();	//���ݳ�ʼ����״̬������
}
#endif
/******************************************************************************
 * ������:	Init_System 
 * ����: ��ʼ��ϵͳ����
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
//volatile uint32_t itest = 0;  // ����ʱ���Ƿ�׼ȷ
void Init_System(void)
{	
	INT_DISABLE();		// ���������ж�
	/*There are 2 different PreemptionPriorities in the TIM init. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	Time_Configuration();	//ϵͳʱ�����ʱ��ض�ʱ����ʼ��
	
	LED_GPIO_Config();
	//EXTI_PE4_Config();

	Comm1_Init();	// USART1 ����ģʽΪ 115200 8-N-1���жϽ���
	Comm2_Init();	// USART2 ����ģʽΪ 115200 8-N-1���жϽ���
	
	/*�δ�ʱ����ʼ����1ms�ж�����*/
	/* �����ʱ��������������ѭ�����У����ڳ�ʼ�������*/
	/* Ҳ����Ӳ����ʼ����Ϻ�ʱ����ʼ����*/
	SysTick_Init();	// �����򲻴�����ϵͳ,ֻ��һ����ͨ��ms��ʱ��

/* �������Զ�ʱ�Ƿ�׼ȷ�����*/
#if 0
	INT_ENABLE();
	SysTick_start();
	itest  = system_time_ms;
	Delay_Ms(100);	// delay 100ms
	itest  = system_time_ms;
	Delay_Xms(5);	// delay 100ms
	itest  = system_time_ms;
	Delay_Xms(5000);	// delay 100ms
	itest  = system_time_ms;
#endif

	/*�ϵ���˸3��,ÿ��50ms*/
	LED_Flashing(LED_COM, 60, 3);
	//Param_Init();
	
	/*�ϻ������ӳ���,ϵͳ������������ϻ�,��ִ�к����*/
	//TestForLC301();		// ��ʱ��ʹ��
						
	INT_ENABLE();
	Timer_Start();
	SysTick_start();
}
