 /**
  ******************************************************************************
  * @file    main.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   Main ����, ϵͳ���.
  *
  ******************************************************************************
  */

#include "include.h"

/*����ϵͳ��־��,ʹ�����е�λ�����д�������¼�.*/
uint16_t system_flag = 0;

//Ҫд�뵽W25Q64���ַ�������
//const uint8_t TEXT_Buffer[]={"WarShipSTM32 SPI TEST\n"};
//const uint32_t FLASH_SIZE=32*1024*1024;		//FLASH ��СΪ16M�ֽ�
//#define SIZE (sizeof(TEXT_Buffer))
/******************************************************************************
 * ������:	Task_Schedule 
 * ����: ������������, ��������¼�������
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.11.21
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static void Task_Schedule(void)
{
	/* ��ס�ϴ�����״̬���Ĵ���״̬,�Ա㷵�ص�ʱ���ϱ�*/
	static bool TTL_ALG_Wrong = FALSE;
	USART_LIST i = PC1_UART;

	//uint8_t datatemp[64];
	
	#ifdef TEST
	/*�����д�����ɽ��̫����ʹ�ã�����֪������ʲôĿ��*/
		if(Status_Get(TTL)==0)
		{
			message_pack_printf(PC1_UART, TEST_MSG);
		}
	#endif

	/* ��⵽�ⲿ��������λ,���߿����߼��ͼ���߼������*/
	if (system_flag&SYS_CHANGED)
	{
		system_flag &= ~SYS_CHANGED;
		if (Status_Get(ALG) == 0)	// ����ǽ���״̬,��ȡ������
		{
			system_flag &= ~CAR_WATCHING;
		}
		/* �ȿ����Ƿ��ж����쳣*/
		detect_ALG_TTL_working();
		/*֪ͨ������λ���б�λ*/
		for (i = pc_com[0]; i <= pc_com[PC_USART_NUM-1]; i++)
		{
			message_pack_printf(i, B_RES_MSG);
		}
		
		if(Status_Get(BACK_COIL) == 0)	 	/* ������μ�⵽�г�*/
		{
			/*���浽�ϴ�״̬*/
			dete_bit_recd = TRUE;
		}

#if 0
		/* ����SPI, LG��ӦKEY1, ALARM ��ӦKEY0*/
		if(system_flag &SYSTEM_1S)		// key1 ����
		{
		`	debug_puts("Start Write W25Q128...");
			W25QXX_Write((uint8_t*)TEXT_Buffer,FLASH_SIZE-100,SIZE);	//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
			debug_puts("W25Q128 Write Finished!");
			system_flag &= ~SYSTEM_1S;
			debug_puts("Start Read W25Q128...");
			W25QXX_Read(datatemp,FLASH_SIZE-100,32);//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
			debug_puts("The Data Readed Is:");
			debug_puts(datatemp);
		}
#endif
	}

/*******************���ҹ����߼�************************************************/
	/* ���ܹ����з��ֺ���Ȧ�г�, ̧��,���ٴν����Զ����ܹ���*/
	if (system_flag&CAR_WATCHING)
	{
		/*����Ȧ�г�*/
		if(Status_Get(BACK_COIL) == 0)	
		{
			/*���ڽ���...*/
			if(ALG_down_flag_bit&Status_Get(ALG))
			{
				device_control_used.control_bits.ALG_up_bit = 1; /*̧��*/
				device_control_used.control_bits.ALG_down_bit = 0;
				control_device();
				autoBarEnable = TRUE; 
			}
		}
	}

	/*�����Զ�����*/
	if (autoBarEnable)
	{
		/* ����ϴ��г�, ����޳�,��ʾ���Ѿ��뿪*/
		if(Status_Get(BACK_COIL))				//Back Coil Flag =1, �����޳�
		{
			if(dete_bit_recd == TRUE)			//�ϴ��г�
			{
				dete_bit_recd = FALSE;
				autoBarEnable = FALSE;		//��λΪ�ֶ�������ģʽ	
				device_control_used.control_bits.ALG_up_bit = 0; 
				device_control_used.control_bits.ALG_down_bit = 1;		/*��ʼ����*/
				device_control_used.control_bits.Lane_lamp_bit = 0;		/*�����*/
				control_device();
				system_flag |= CAR_WATCHING;
			}
		}
	}
/*******************���ҹ����߼�����********************************************/

	/*�ֳ����˰�����,����10s,һֱ���ͻ�һֱ��*/
	if(Status_Get(ALARM) == 0)		//valid is 0
	{
		alarm_time_counter = ALARM_TIME;
		device_control_used.control_bits.VOX_alarm_bit = 1;
		device_control_used.control_bits.Light_alarm_bit = 1;
		control_device_expt_lg();
	}

	/* �豸ÿ��3s�Զ������״̬*/
	if (system_flag&SYS_ERR_CHK)				//�豸����״̬�Զ���ⲿ��	
	{
		system_flag &= ~SYS_ERR_CHK;
    		if(detect_ALG_TTL_working())
		{
			TTL_ALG_Wrong = TRUE;			//����Ǵ���״̬��ÿ���ñ�־��λʱ����λ���㱨
			system_flag |= SYS_CHANGED;
		}
		else
		{
			/*�Ӵ���״̬���ص�����״̬ʱ����ʱ����λ���㱨*/
			if(TTL_ALG_Wrong) 
			{
				system_flag |= SYS_CHANGED;
			}
			TTL_ALG_Wrong = FALSE;
		}
	}

}


/******************************************************************************
 * ������:	main 
 * ����: ����ʼ���.
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.21
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
int main(void)
{	
	Init_System();
	while(1)
	{
		/*��������ʼ��,��ʱ�ж�ι��,��ͬʱ��ֹ���������жϳ����ܷ�*/
		wdt_counter = 0; 
		Comm_Proc();
		Task_Schedule();			// ϵͳ�����б�
	}
}
/*********************************************END OF FILE**********************/

