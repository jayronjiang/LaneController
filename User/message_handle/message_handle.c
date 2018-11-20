/**
  * @file    bsp_usart1.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  * ******************************************************************************
  */
  
#include "include.h"

const uint8_t VerInfo[]="LC-301-V4.1-181017";

/*定义通信缓冲区*/
PROTOCOL_BUF	ProtocolBuf0;
static uint8_t SpkBuf[COMM_LENTH];	// 用来存储上面发来的信息，主要是用于语音报价

 /**
  * @brief  串口信息的打包和解包
  * @param  无
  * @retval 无
  */
void Comm1_Init(void)
{
	// 物理层初始化
	USART2_Init();
	/*初始化协议层缓冲区数据*/
	ProtocolBuf0.pTxBuf = UART0Buf.TxBuf;
	ProtocolBuf0.pRxBuf = UART0Buf.RxBuf;
	ProtocolBuf0.RxLen = 0;
	ProtocolBuf0.TxLen = 0;
}

// 信息打包
void message_pack(uint8_t uart_no, uint8_t msg_type,PROTOCOL_BUF *buf)
{
	uint8_t *pbuf = buf->pTxBuf;
	//uint8_t *prx_buf = buf->pRxBuf;
	uint8_t len = 0;
	uint8_t xor_t = 0;
	uint8_t i = 0;

	switch (msg_type)
	{
	case INTT_MSG:
		pbuf[len++] = MSG_SOF	;
		pbuf[len++] = '3';		// 车型pbuf[1] 
		pbuf[len++] = '3';		// 车重pbuf[2] 
		
		pbuf[len++] = '5';
		pbuf[len++] = '5';
		pbuf[len++] = '0';
		pbuf[len++] = '0';		//pbuf[3~6] , 费额
		
		pbuf[len++] = '0';
		pbuf[len++] = '0';
		pbuf[len++] = '3';
		pbuf[len++] = '3';		// pbuf[7~10] , 余额
		
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';		//pbuf[11~16] , entrance

		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 0;		// BCC, 发给外设的以0结尾
		//pbuf[len++] = '\0';
		break;

	case FEE_G_MSG:
		pbuf[0] = MSG_SOF	;
		pbuf[1] = 'Y';			// YY 费显灯为绿色
		pbuf[2] = MSG_EOF;
		pbuf[3] = 'Y';
		//pbuf[4] = '\0';
		bLastLaneRedGreenOperateState = GREEN;
		break;

	case FEE_R_MSG:
		pbuf[len++] = MSG_SOF	;
		pbuf[len++] = 'Z';		// ZZ 费显灯为红色
		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 'Z';
		//pbuf[len++] = '\0';
		bLastLaneRedGreenOperateState = RED;
		break;

	case VER_PRINT_MSG:
		pbuf[len++] = 'X';		// 软件版本, XY-XXXX
		pbuf[len++] = 'Y';
		pbuf[len++] = '-';

		memcpy(&pbuf[len],VerInfo,strlen(VerInfo));
		len = len + strlen(VerInfo);
		
		pbuf[len++] = 0x0D;	// 0D0A, 回车换行
		pbuf[len++] = 0x0A;
		break;

	// 暂时没有添加这个功能
	case MEM_DUMP_MSG:
		break;

	case B_RES_MSG:
		pbuf[len++] = MSG_SOF	;
		pbuf[len++] = 'B';						// 信息B
		pbuf[len++] = LOCAL_ADD;				// 地址0x31
		pbuf[len++] = device_status_used.status_word[USED];
		pbuf[len++] = device_status_used.status_word[BACKUP];
		pbuf[len++] = MSG_EOF;
		
		xor_t = pbuf[1];			// 'B'
		for (i = 2; i< len-1; i++)		// MSG_SOF和EOF不参与异或
		{
			xor_t = xor_t^pbuf[i];	//异或校验
		}
		pbuf[len++] = xor_t;
		break;

	case ERR_RES_MSG:
		pbuf[len++] = 0x15	;
		pbuf[len++] = 'E';
		pbuf[len++] = 'E';
		pbuf[len++] = 'E';
		pbuf[len++] = 'E';
		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 0;
		break;

	case SPK_MSG:
	case COST_MSG:
		xor_t = SpkBuf[COST_0];
		for (i = COST_1; i<= WEIGHT; i++)		// MSG_SOF和EOF不参与异或
		{
			xor_t = xor_t^SpkBuf[i];			//异或校验
		}
		pbuf[len++] = MSG_SOF;
		pbuf[len++] = SpkBuf[VECLE];
		pbuf[len++] = SpkBuf[WEIGHT];			//车重
		
		pbuf[len++] = SpkBuf[COST_0];			// 金额
		pbuf[len++] = SpkBuf[COST_1];
		pbuf[len++] = SpkBuf[COST_2];
		pbuf[len++] = SpkBuf[COST_3];
		
		pbuf[len++] = SpkBuf[LEFT_0];			//余额
		pbuf[len++] = SpkBuf[LEFT_1];
		pbuf[len++] = SpkBuf[LEFT_2];
		pbuf[len++] = SpkBuf[LEFT_3];
		
		pbuf[len++] = SpkBuf[ENTRY_0];
		pbuf[len++] = SpkBuf[ENTRY_1];
		pbuf[len++] = SpkBuf[ENTRY_2];
		pbuf[len++] = SpkBuf[ENTRY_3];
		pbuf[len++] = SpkBuf[ENTRY_4];
		pbuf[len++] = SpkBuf[ENTRY_5];
		pbuf[len++] = MSG_EOF;
		pbuf[len++] = xor_t;
		if (msg_type == COST_MSG)
		{
			bFeeCleared = FALSE;
		}
		break;

	/* 透传,直接拷贝*/
	case TRANS_MSG:
		for (len = 0; len < buf->RxLen; i++)
		{
			pbuf[len++] = buf->pRxBuf[len++];
		}
		break;

	case ALL8_MSG:
		pbuf[len++] = MSG_SOF	;

		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';

		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';

		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';

		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';
		pbuf[len++] = '8';
		

		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 0;
		break;

	case VOXPLAY_MSG:
		pbuf[len++] = MSG_SOF	;

		pbuf[len++] = '5';
		pbuf[len++] = '1';
		pbuf[len++] = '1';
		pbuf[len++] = '2';

		pbuf[len++] = '3';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '3';

		pbuf[len++] = '2';
		pbuf[len++] = '1';
		pbuf[len++] = 0xBB;
		pbuf[len++] = 0xBC;

		pbuf[len++] = 0xCC;
		pbuf[len++] = 0xCD;
		pbuf[len++] = 0xDD;
		pbuf[len++] = 0xDE;

		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 1;
		break;
		
#ifdef TEST
	case TEST_MSG:
		pbuf[len++] = 0x80	;
		pbuf[len++] = 0x3f;
		pbuf[len++] = 0x30;
		pbuf[len++] = 0x5b;
		pbuf[len++] = 0x4f;
		pbuf[len++] = 0x66;
		pbuf[len++] = 0x6d;
		pbuf[len++] = 0x7d;
		pbuf[len++] = 0x07;
		pbuf[len++] = 0x7f;
		pbuf[len++] = 0x01;
		break;
#endif

	default:
		break;
	}
	/*更新发送长度*/
	buf->TxLen = len;
}
 

// 在中断未开的情况下，只能用printf 进行发送
void message_send_printf(uint8_t uartNo)
{
	PROTOCOL_BUF *pProbuf;

	if (uartNo == UART1_COM)
	{
		pProbuf  = &ProtocolBuf0;
	}
	else
	{
		;
	}
	
	//message_pack(uartNo, msg_type, pProbuf);

	//开始输出
	printf("%s", pProbuf->pTxBuf);
}


//打包同时输出
void message_pack_printf(uint8_t uartNo, uint8_t msg_type)
{
	PROTOCOL_BUF *pProbuf;

	if (uartNo == UART1_COM)
	{
		pProbuf  = &ProtocolBuf0;
	}
	else
	{
		;
	}
	
	message_pack(uartNo, msg_type, pProbuf);
	printf("%s", pProbuf->pTxBuf);
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      message_check
 *
 *  DESCRIPTION
 *      对收到的信息进行校验
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static uint8_t message_check(PROTOCOL_BUF *buf)
{
	uint8_t *rx_buf;
	uint16_t rx_len;
	uint8_t i,j;
	uint8_t err=0;

	rx_buf = buf->pRxBuf;
	rx_len = buf->RxLen;

	if (rx_len != COMM_LENTH)
	{
		err = LENGTH_ERROR;
	}
	else
	{
		if((rx_buf[0] == MSG_SOF) && (rx_buf[22] == MSG_EOF))
		{
			j=rx_buf[COM_T];	// J用来计算异或值
			for(i=2; i <= (rx_len-3); i++)
			{
				j = rx_buf[i]^j;
			}
			if(j == rx_buf[rx_len-1])
			{
				err = ERR_OK;
				if (rx_buf[ADDR] != LOCAL_ADD)
				{
					err = SITEID_ERROR;
				}
			}
			else
			{
				err = CRC_ERROR;
			}
		}
		else
		{
			err = SOF_ERROR;
		}
	}
	return err;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      blockDataUnPack
 *
 *  DESCRIPTION
 *      Unpack the data block to get the Bellman's bluetooth protocol. 
 *      p_data: the pointer to the data array.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
void message_unpack(PROTOCOL_BUF *buf)
{
	uint8_t *rx_buf;
	uint8_t  i = 0;

	rx_buf = buf->pRxBuf;

	/* CR 即复位命令*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='R'))
	{
		// Reset the system
		INT_DISABLE();		// 防止复位命令被打断
		NVIC_SystemReset();	// 这个地方需要测试一下
		return;
	}

	/* CV 打印版本信息*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='V'))
	{
		message_pack(PC_UART, VER_PRINT_MSG,buf);
		//message_send_printf(PC_UART, VER_PRINT_MSG);
		return;
	}

	/* CD 读取内存信息*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='D'))
	{
		message_pack(PC_UART, MEM_DUMP_MSG,buf);
		//message_send_printf(PC_UART, MEM_DUMP_MSG);
		return;
	}


	/* A类信息: 起始码+通信类+设备地址+控制字节1+金额(4字节)*/
	/* +余额(4字节)+入口站(6字节) +车型+车种+控制字节2+通讯序号*/
	/* +结束码+校验码 (共24个 字节)*/
	
	/* 提取相关信息*/
	for (i = 0; i<COMM_LENTH; i++)
	{
		SpkBuf[i]= rx_buf[i];
	}

	/*回复之前先更新外设状态*/
	detect_ALG_TTL_working();
	/* 信息正常,先对上位机PC进行回复*/
	message_pack(PC_UART, B_RES_MSG,buf);

	/* 读取有用的控制信息*/
	/* 控制字节1:	*/
	// 0位: 起栏杆--1为抬杆，0为保持
	// 1位: 落栏杆--1为降杆，0为保持
	// 2位: 顶棚灯--1为绿灯，0为红灯
	// 3位: 声报警--1为声报警，0为声报警取消
	// 4位: 光报警--1为光报警，0为光报警取消
	// 5位: 通行灯--1为绿灯，0为红灯
	// 6位: 未定义--备用
	// 7位: 未定义--备用
	device_control_used.control_word[USED]=SpkBuf[CTRL_T];  // 控制字节
	
	/* 控制字节2:	*/
	// 0位: 栏杆控制位  --1:车辆过后自动降杆模式，0:车辆过后手动降杆模式
	// 1位: 费额显示位  --1:费额显示，            0:费额不显示
	// 2位: 语音报价位  --1:报价，                0:不报价
	// 3位: 费额显示保持--1:保持，            0:刷新，当为1时，忽略bit1
	// 4位: 费显选择位  --1:广东版本，无保持功能，0:山西版本有保持功能，本控制需要费显支持
	// 5位: 车控器属性  --1:用于出口，            0:用于入口
	device_control_used.control_word[BACKUP]=SpkBuf[CTRL_B];
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      message_process
 *
 *  DESCRIPTION
 *      处理收到的信息,校验并传送到下一层
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void message_process(PROTOCOL_BUF *buf)
{
	uint8_t err=0;

	err = message_check(buf);

	if ( err == ERR_OK)
	{
		message_unpack(buf);
	}
	else if (err == SITEID_ERROR)
	{
		;  // 不是发给本机的,什么都不做
	}
	else 
	{
		// 如果是错误的信息处理
		message_pack(PC_UART, ERR_RES_MSG,buf);
	}
}

/***********************************************************************************
 * 函数名:	CommProc 
 * 描述: 
 *           			 -通讯处理函数,主程序中调用,主要处理RS 485口数据,
 *				   若是本装置的数据则进入解包处理
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:20181109
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
void Comm_Proc(void)
{
	
	if (UART0Buf.RecFlag)		                      //RS485口有数据
	{	
		#ifdef DEBUG_MODE
	  		printf("%s",UART0Buf.RxBuf);    //将接受到的数据直接返回打印
	  	#endif
		
		ProtocolBuf0.pTxBuf = UART0Buf.TxBuf;         //地址置换
		ProtocolBuf0.pRxBuf = UART0Buf.RxBuf;
		ProtocolBuf0.RxLen = UART0Buf.RxLen;
		ProtocolBuf0.TxLen = 0;
		UART0Buf.RxLen = 0;		//已经被读取到ProtocolBuf0.RxLen, 尽快清0

		/*直接先进行透传*/
		//message_pack_printf(TRANS_UART, TRANS_MSG);

		message_process(&ProtocolBuf0);		//通信协议处理

		UART0Buf.TxLen = ProtocolBuf0.TxLen;  //置换回来，处理物理层数据
		if(UART0Buf.TxLen >0)
		{
			/*回复B/C信息给上位机*/
			message_send_printf(PC_UART);
		}
		Delay_Ms(5);				// 稍微有点延时,可以不要
		UART0Buf.RecFlag = 0;		//接收数据已处理，清除相关标志
	}
	params_modify_deal();		//后续的数据改变处理
}



/*********************************************END OF FILE**********************/
