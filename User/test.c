#include "include.h"

/******************************************************************************
 * ������:	InputDedect 
 * ����: ��ѯʽ��ȡһ������״̬
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.19
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static bool InputDedect(DEVICE_STATUS_LIST dev)
{
	if (GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_ON)
	{
		Delay_Ms(5);		// 5ms����
		if (GPIO_ReadInputDataBit(device_status_queue[dev].gpio_grp,device_status_queue[dev].gpio_pin) == KEY_ON)
		{
			return TRUE;
		}
	}
	return FALSE;
}


/******************************************************************************
 * ������:	UpdateFeeDisplay 
 * ����: �����и��·���
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.19
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static void UpdateFeeDisplay(unsigned char Index)
{
	if (Index%2)
	{
		message_pack_printf(PC_UART, FEE_R_MSG); // ����Ϊ��ɫ
		Delay_Ms(250);
		message_pack_printf(PC_UART, ALL8_MSG);
	}
	else
	{
		message_pack_printf(PC_UART, FEE_G_MSG); // ����Ϊ��ɫ
	}
}


/******************************************************************************
 * ������:	PlayVoice 
 * ����: �����в�������
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.19
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static void PlayVoice(unsigned char Index)
{
	if (Index%5)
	{
		message_pack_printf(PC_UART, VOXPLAY_MSG); 
	}
	else
	{
		message_pack_printf(PC_UART, FEE_G_MSG); // ����ֹͣ?
	}
}


static void SetControlPort(bool bOnOff)
{
	uint8_t tmpPortData = 0;
	uint8_t i = 0;

	tmpPortData =0xFE;
	Command_Act(tmpPortData);
	for (i = 0; i < 16; i++)
	{
		Command_Act(tmpPortData);
		tmpPortData <<= 1;
		Delay_Ms(10);
	}
	DelayAndFeedDog(400);
	
	tmpPortData = 0xFF;
	if(bOnOff)
	{
		tmpPortData = 0;	//set on
	}

	Command_Act(tmpPortData);
	Delay_Ms(2);
	tmpPortData |= 0x03;
	Command_Act(tmpPortData);
}


/******************************************************************************
 * ������:	TestForLC301 
 * ����: �ϻ������ӳ���
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.19
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void TestForLC301(void)
{
	unsigned char Index=0;

	/*����ϵ��ȡP2.6��Ч,�����ϻ�����*/
	if(!InputDedect(BAK1))
	{
		return;
	}

	while(1)
	{
		UpdateFeeDisplay(Index);
		PlayVoice(Index);
		
		SetControlPort(TRUE);
		DelayAndFeedDog(2000);	// ��ʱ2s
		SetControlPort(FALSE);
		
		Delay_Ms(50);
		Index++;
	}
}


