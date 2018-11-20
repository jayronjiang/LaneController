#include "include.h"

volatile uint16_t wachi= 0;

uint16_t system_flag = 0;

void Task_Schedule(void)
{
	static bool TTL_ALG_Wrong = FALSE;	// ��ס�ϴ�����״̬���Ĵ���״̬,�Ա㷵�ص�ʱ���ϱ�
	
	#ifdef TEST
		//�����д�����ɽ��̫����ʹ�ã�����֪������ʲôĿ��
		if(Status_Get(TTL)==0)
		{
			message_pack_printf(PC_UART, TEST_MSG);
		}
	#endif

	/* ��⵽�ⲿ��������λ,���߿����߼��ͼ���߼������*/
	if (system_flag&SYS_CHANGED)
	{
		system_flag &= ~SYS_CHANGED;
		/* �ȿ����Ƿ��ж����쳣*/
		detect_ALG_TTL_working();
		/*֪ͨ��λ���б�λ*/
		message_pack_printf(PC_UART, B_RES_MSG);
		
		if(Status_Get(BACK_COIL) == 0)	 	/* ������μ�⵽�г�*/
		{
			/*���浽�ϴ�״̬*/
			dete_bit_recd=1;
		}
	}

/*******************���ҹ����߼�************************************************/
	/* ���ܹ����з��ֺ���Ȧ�г�, ̧��,���ٴν����Զ����ܹ���*/
	if (system_flag&CAR_WATCHING)
	//if(watching_car_passing)
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
				//watching_car_passing = TRUE; 
			}
		}
	}
/*******************���ҹ����߼�************************************************/

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

