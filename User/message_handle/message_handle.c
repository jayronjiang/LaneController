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
static PROTOCOL_BUF *pScBuf = &ProtocolBuf[0];

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
		return;
	}

	/* CD 读取内存信息*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='D'))
	{
		return;
	}

	/* 信息正常,对主机进行回复*/
	message_pack(PC_UART, S_RES_MSG,buf);
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
static void message_process(PROTOCOL_BUF *buf)
{
	message_unpack(buf);
	/* 错误信息或者其他的什么都不做*/
	/* 等待发第二次*/
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

	case VER_PRINT_MSG:
		pbuf[len++] = 'X';		// 软件版本, XY-XXXX
		pbuf[len++] = 'Y';
		pbuf[len++] = '-';

		memcpy(&pbuf[len],VerInfo,strlen(VerInfo));
		len = len + strlen(VerInfo);
		
		pbuf[len++] = 0x0D;	// 0D0A, 回车换行
		pbuf[len++] = 0x0A;
		break;

	/* 透传,直接拷贝*/
	case TRANS_MSG:
		for (len = 0; len < pScBuf->RxLen; i++)
		{
			pbuf[len] = pScBuf->pRxBuf[len];
			len++;
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

	case S_RES_MSG:
		pbuf[len++] = MSG_SOF;
		pbuf[len++] = 'S';						// 信息B
		pbuf[len++] = LOCAL_ADD;				// 地址0x31
		pbuf[len++] = 0x00;
		pbuf[len++] = 0x00;
		pbuf[len++] = MSG_EOF;
		
		xor_t = pbuf[1];			// 'B'
		for (i = 2; i< len-1; i++)		// MSG_SOF和EOF不参与异或
		{
			xor_t = xor_t^pbuf[i];	//异或校验
		}
		pbuf[len++] = xor_t;
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
		pScBuf = &ProtocolBuf[USARTX];

		/*直接先进行透传*/
		message_pack_printf(TRANS_UART, TRANS_MSG);

		message_process(&ProtocolBuf[USARTX]);		//通信协议处理

		UARTBuf[USARTX].TxLen = ProtocolBuf[USARTX].TxLen;  //置换回来，处理物理层数据
		if(UARTBuf[USARTX].TxLen >0)
		{
			/*回复B/C信息给上位机*/
			message_send_printf(PC_UART);
		}
		Delay_Ms(5);				// 稍微有点延时,可以不要
		UARTBuf[USARTX].RecFlag = 0;		//接收数据已处理，清除相关标志

		/*放在括号内,只有收到新的信息才操作*/
		//params_modify_deal();		//后续的数据改变处理
	}
}
/*********************************************END OF FILE**********************/
