 /**
  ******************************************************************************
  * @file    message_handle.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   串口信息应用层处理,包括打包和解包函数.
  *
  ******************************************************************************
  */
  
#include "include.h"

const uint8_t VerInfo[]="LC-301-V4.1-181017";

/*定义通信缓冲区*/
PROTOCOL_BUF ProtocolBuf[UART_NUM];
static uint8_t SpkBuf[COMM_LENTH];	// 用来存储上面发来的信息，主要是用于语音报价


/******************************************************************************
 * 函数名:	message_unpack 
 * 描述: 对收到的信息按照规定的协议进行解包,提取。
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
static void message_unpack(PROTOCOL_BUF *buf)
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

/******************************************************************************
 * 函数名:	message_check 
 * 描述: 对收到的信息进行校验,是否符合规定的协议
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
		if((rx_buf[BSOF] == MSG_SOF) && (rx_buf[BEOF] == MSG_EOF))
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

 /******************************************************************************
 * 函数名:	message_process 
 * 描述: 信息处理函数, 对收到的信息不同情况进行不同处理.
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
static uint8_t message_process(PROTOCOL_BUF *buf)
{
	uint8_t err = 0;

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

	return err;
}


/******************************************************************************
 * 函数名:	Comm1_Init 
 * 描述: 串口1的初始化
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
void Comm1_Init(void)
{
	// 物理层初始化
	USART1_Init();
	/*初始化协议层缓冲区数据*/
	ProtocolBuf[UART1_COM].pTxBuf = UARTBuf[UART1_COM].TxBuf;
	ProtocolBuf[UART1_COM].pRxBuf = UARTBuf[UART1_COM].RxBuf;
	ProtocolBuf[UART1_COM].RxLen = 0;
	ProtocolBuf[UART1_COM].TxLen = 0;
}

/******************************************************************************
 * 函数名:	Comm2_Init 
 * 描述: 串口2的初始化
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
 void Comm2_Init(void)
{
	// 物理层初始化
	USART2_Init();
	/*初始化协议层缓冲区数据*/
	ProtocolBuf[UART2_COM].pTxBuf = UARTBuf[UART2_COM].TxBuf;
	ProtocolBuf[UART2_COM].pRxBuf = UARTBuf[UART2_COM].RxBuf;
	ProtocolBuf[UART2_COM].RxLen = 0;
	ProtocolBuf[UART2_COM].TxLen = 0;
}

/******************************************************************************
 * 函数名:	message_pack 
 * 描述: 信息的打包,形成协议格式数据
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
		pbuf[len++] = MSG_SOF;
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
		break;

	case FEE_G_MSG:
		pbuf[len++] = MSG_SOF;
		pbuf[len++] = 'Y';			// YY 费显灯为绿色
		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 'Y';
		//pbuf[4] = '\0';
		bLastLaneRedGreenOperateState = GREEN;
		break;

	case FEE_R_MSG:
		pbuf[len++] = MSG_SOF;
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

	/*上传一些重要变量的值,便于现场调试*/
	case MEM_DUMP_MSG:
		pbuf[len++] = 'X';
		pbuf[len++] = 'Y';
		pbuf[len++] = '_';
		pbuf[len++] = device_control_used.control_word[USED];
		pbuf[len++] = device_control_used.control_word[BACKUP];
		pbuf[len++] = device_status_used.status_word[USED];
		pbuf[len++] = device_status_used.status_word[BACKUP];
		
		pbuf[len++] = system_flag;
		pbuf[len++] = ALG_up_flag_bit;
		pbuf[len++] = ALG_down_flag_bit;
		
		pbuf[len++] = autoBarEnable;
		pbuf[len++] = dete_bit_recd;
		pbuf[len++] = LastLaneLampState;
		pbuf[len++] = bLastLaneRedGreenOperateState;
		
		pbuf[len++] = bFeeCleared;
		pbuf[len++] = WatchingDelayCount;
		pbuf[len++] = detect_time_counter;
		pbuf[len++] = alarm_time_counter;

		pbuf[len++] = 0x0D;	// 0D0A, 回车换行
		pbuf[len++] = 0x0A;
		break;

	case B_RES_MSG:
		pbuf[len++] = MSG_SOF;
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
		pbuf[len++] = 0x15;
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
		pbuf[len++] = MSG_SOF;

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
		pbuf[len++] = MSG_SOF;

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
		pbuf[len++] = 0x80;
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

#ifdef DEBUG_MODE
	case A_MSG:
		pbuf[len++] = MSG_SOF;
		pbuf[len++] = 'A';
		pbuf[len++] = LOCAL_ADD;
		pbuf[len++] = device_status_used.status_word[USED];
		
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

		pbuf[len++] = 1;
		pbuf[len++] = 2;
			
		pbuf[len++] = device_status_used.status_word[BACKUP];

		pbuf[len++] = '0';
		
		pbuf[len++] = MSG_EOF;
		
		xor_t = pbuf[1];			// 'A'
		for (i = 2; i < len-1; i++)	// MSG_SOF和EOF不参与异或
		{
			xor_t = xor_t^pbuf[i];	//异或校验
		}
		pbuf[len++] = xor_t;
		break;

	case CR_MSG:
	case CV_MSG:
	case CD_MSG:
		pbuf[len++] = MSG_SOF;
		pbuf[len++] = 'C';
		pbuf[len++] = LOCAL_ADD;
		if (msg_type == CR_MSG)
		{
			pbuf[len++] = 'R';
		} 
		else if (msg_type == CV_MSG)
		{
			pbuf[len++] = 'V';
		}
		else
		{
			pbuf[len++] = 'D';
		}
		
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

		pbuf[len++] = '0';
		pbuf[len++] = '0';
		pbuf[len++] = '0';
		pbuf[len++] = '0';
		
		pbuf[len++] = MSG_EOF;
		
		xor_t = pbuf[1];			// 'C'
		for (i = 2; i < len-1; i++)	// MSG_SOF和EOF不参与异或
		{
			xor_t = xor_t^pbuf[i];	//异或校验
		}
		pbuf[len++] = xor_t;
		break;
#endif

	default:
		break;
	}
	/*更新发送长度*/
	//pbuf[len++] = '\0';
	buf->TxLen = len;
}
 
/******************************************************************************
 * 函数名:	message_send_printf 
 * 描述: 信息的发送,把缓存区的数据发送至串口
 * 		这是阻塞型的发送,发送时不能干其它任务.
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
void message_send_printf(USART_LIST uartNo)
{
	uint16_t i = 0;
	uint8_t ch = 0;
	PROTOCOL_BUF *pProbuf;
	USART_TypeDef *PUSART = USART1;

	pProbuf  = &ProtocolBuf[uartNo];

	if (uartNo == UART1_COM)
	{
		PUSART = USART1;
	}
	else if (uartNo == UART2_COM)
	{
		PUSART = USART2;
	}
	else
	{
		PUSART = USART3;
	}

	/*和printf一样,是阻塞型的发送*/
	for (i = 0; i < pProbuf->TxLen; i++)
	{
		ch = pProbuf->pTxBuf[i];
		USART_SendData(PUSART, ch);
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(PUSART, USART_FLAG_TXE) == RESET);	
		
	}
	pProbuf->TxLen = 0;
	/*printf无法输出状态量0*/
	//printf("%s", pProbuf->pTxBuf);
}


/******************************************************************************
 * 函数名:	message_pack_printf 
 * 描述: 信息的打包和发送
 * 		这是阻塞型的发送,发送时不能干其它任务.
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
void message_pack_printf(uint8_t uartNo, uint8_t msg_type)
{
	uint16_t i = 0;
	uint8_t ch = 0;
	PROTOCOL_BUF *pProbuf;
	USART_TypeDef *PUSART = USART1;

	pProbuf  = &ProtocolBuf[uartNo];

	if (uartNo == UART1_COM)
	{
		PUSART = USART1;
	}
	else if (uartNo == UART2_COM)
	{
		PUSART = USART2;
	}
	else
	{
		PUSART = USART3;
	}
	
	message_pack(uartNo, msg_type, pProbuf);

	
	LED_Set(LED_COM, ON); 	// 开始通信指示
	/*和printf一样,是阻塞型的发送*/
	for (i = 0; i < pProbuf->TxLen; i++)
	{
		ch = pProbuf->pTxBuf[i];
		USART_SendData(PUSART, ch);
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(PUSART, USART_FLAG_TXE) == RESET);	
		
	}
	pProbuf->TxLen = 0;
	LED_Set(LED_COM, OFF); 	// 通信完毕
	/*printf无法输出状态量0*/
	//printf("%s", pProbuf->pTxBuf);
}

  /******************************************************************************
 * 函数名:	Comm_Proc 
 * 描述: -通讯处理函数,主程序中调用,主要处理RS 485口数据,
 *				   若是本装置的数据则进入解包处理
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
void Comm_Proc(void)
{
	USART_LIST USARTX = PC_UART;
	uint8_t err = ERR_OK;

	if (UARTBuf[USARTX].RecFlag)		                      //RS485口有数据
	{	
		#ifdef DEBUG_MODE
	  		printf("%s",UARTBuf[USARTX].RxBuf);    //将接受到的数据直接返回打印
	  	#endif
		
		ProtocolBuf[USARTX].pTxBuf = UARTBuf[USARTX].TxBuf;         //地址置换
		ProtocolBuf[USARTX].pRxBuf = UARTBuf[USARTX].RxBuf;
		ProtocolBuf[USARTX].RxLen = UARTBuf[USARTX].RxLen;
		ProtocolBuf[USARTX].TxLen = 0;
		UARTBuf[USARTX].RxLen = 0;		//已经被读取到ProtocolBuf0.RxLen, 尽快清0

		/*直接先进行透传*/
		//message_pack_printf(TRANS_UART, TRANS_MSG);

		err = message_process(&ProtocolBuf[USARTX]);		//通信协议处理

		UARTBuf[USARTX].TxLen = ProtocolBuf[USARTX].TxLen;  //置换回来，处理物理层数据
		if(UARTBuf[USARTX].TxLen >0)
		{
			/*回复B/C信息给上位机*/
			message_send_printf(PC_UART);
		}
		Delay_Ms(5);				// 稍微有点延时,可以不要
		UARTBuf[USARTX].RecFlag = 0;		//接收数据已处理，清除相关标志

		/*放在括号内,只有收到新的信息才操作*/
		if (err == ERR_OK)
		{
			params_modify_deal();		//后续的数据改变处理
		}
	}
}
/*********************************************END OF FILE**********************/
