#include "include.h"

volatile uint16_t wachi= 0;

uint16_t system_flag = 0;

void Task_Schedule(void)
{
	static bool TTL_ALG_Wrong = FALSE;	// 记住上次栏杆状态检测的错误状态,以便返回的时候上报
	
	#ifdef TEST
		//这两行代码在山西太旧中使用，但不知道用于什么目的
		if(Status_Get(TTL)==0)
		{
			message_pack_printf(PC_UART, TEST_MSG);
		}
	#endif

	/* 检测到外部传感器变位,或者控制逻辑和检测逻辑不配合*/
	if (system_flag&SYS_CHANGED)
	{
		system_flag &= ~SYS_CHANGED;
		if (Status_Get(ALG) == 0)	// 如果是降杠状态,则取消监视
		{
			system_flag &= ~CAR_WATCHING;
		}
		/* 先看看是否有动作异常*/
		detect_ALG_TTL_working();
		/*通知上位机有变位*/
		message_pack_printf(PC_UART, B_RES_MSG);
		
		if(Status_Get(BACK_COIL) == 0)	 	/* 如果本次检测到有车*/
		{
			/*保存到上次状态*/
			dete_bit_recd = TRUE;
		}
	}

/*******************防砸功能逻辑************************************************/
	/* 降杠过程中发现后线圈有车, 抬杠,并再次进入自动降杠过程*/
	if (system_flag&CAR_WATCHING)
	{
		/*后线圈有车*/
		if(Status_Get(BACK_COIL) == 0)	
		{
			/*正在降落...*/
			if(ALG_down_flag_bit&Status_Get(ALG))
			{
				device_control_used.control_bits.ALG_up_bit = 1; /*抬杠*/
				device_control_used.control_bits.ALG_down_bit = 0;
				control_device();
				autoBarEnable = TRUE; 
			}
		}
	}

	/*处理自动降杠*/
	if (autoBarEnable)
	{
		/* 如果上次有车, 这次无车,表示车已经离开*/
		if(Status_Get(BACK_COIL))				//Back Coil Flag =1, 本次无车
		{
			if(dete_bit_recd == TRUE)			//上次有车
			{
				dete_bit_recd = FALSE;
				autoBarEnable = FALSE;		//复位为手动降栏杆模式	
				device_control_used.control_bits.ALG_up_bit = 0; 
				device_control_used.control_bits.ALG_down_bit = 1;		/*开始降杠*/
				device_control_used.control_bits.Lane_lamp_bit = 0;		/*亮红灯*/
				control_device();
				system_flag |= CAR_WATCHING;
			}
		}
	}
/*******************防砸功能逻辑结束********************************************/

	/*现场有人按警报,报警10s,一直按就会一直响*/
	if(Status_Get(ALARM) == 0)		//valid is 0
	{
		alarm_time_counter = ALARM_TIME;
		device_control_used.control_bits.VOX_alarm_bit = 1;
		device_control_used.control_bits.Light_alarm_bit = 1;
		control_device_expt_lg();
	}

	/* 设备每隔3s自动检测下状态*/
	if (system_flag&SYS_ERR_CHK)				//设备错误状态自动检测部分	
	{
		system_flag &= ~SYS_ERR_CHK;
    		if(detect_ALG_TTL_working())
		{
			TTL_ALG_Wrong = TRUE;			//如果是错误状态，每当该标志置位时向上位机汇报
			system_flag |= SYS_CHANGED;
		}
		else
		{
			/*从错误状态返回到正常状态时，及时向上位机汇报*/
			if(TTL_ALG_Wrong) 
			{
				system_flag |= SYS_CHANGED;
			}
			TTL_ALG_Wrong = FALSE;
		}
	}
	
}

