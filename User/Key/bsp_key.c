#include "include.h"

/******************************************************************************
 * ������:	GPIO_ReadInputAll 
 * ����:  	 һ�ΰ����еļ�ֵ������,���ص��Ǿ���ת���ĸ���Ч��ֵ
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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

		tmp = tmp << dev_status;	// �ڼ�����ֵ�����Ƽ�λ.
		if ( polar == LOW_POLAR )
		{
			tmp = ~tmp;			// �ѵ���Чȫ��תΪ����Ч
		}
		key_val |= tmp;
	}

	return key_val;
}
 
/******************************************************************************
 * ������:	ReadKey 
 * ����:  	��ȡ�ⲿ����״̬
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void ReadKey(void)
{
	static uint16_t key_port_last=0;	/*�ϴ�ɨ��õ��İ����˿�״̬*/
	static uint16_t key_effective_port_last=0; /*�ϴξ�����������ʵ�����˿�״̬*/
	uint16_t key_active_port=0;	/*����ɨ����̶˿ڵõ���״̬*/
	uint16_t key_effective_port=0;	/*��ʾ����������ȷ�ϵõ��İ���״̬*/
	
        /*��������*/
	key_active_port = GPIO_ReadInputAll(LOW_POLAR);	/*�����˿ڵ͵�ƽ��Ч,*/
	
	key_effective_port = (key_active_port&key_port_last);		/*��ǰ״̬���ϴ�״̬,����*/
	device_status_used.status_word[USED] = key_effective_port&0xFF;
	key_port_last = key_active_port;		/*Ϊ����������Ҫ�õ��ϴε�״̬*/
	
	if (key_active_port !=key_effective_port_last)
	{
		system_flag |= SYS_CHANGED;	// ���������ر�λ
		key_effective_port_last = key_active_port;
	}
}


static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* �����ж�Դ */
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* EXTI line mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}

/******************************************************************************
 * ������:	EXTI4_IRQHandler 
 * ����:  	�ⲿ�����ж�, PE4
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 // bug:���ﰴ���е�����,�����غ��½��ض��������ж�
void EXTI4_IRQHandler(void)
{
	Delay_Ms(20);
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{
		// LED1 ȡ��		
		LED1_TOGGLE;
		printf("USART1\n");
		EXTI_ClearITPendingBit(EXTI_Line4);     //����жϱ�־λ
	}  
}


