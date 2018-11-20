/**
  * @file    bsp_usart1.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usartӦ��bsp
  * ******************************************************************************
  */
  
#include "include.h"

const uint8_t VerInfo[]="LC-301-V4.1-181017";

/*����ͨ�Ż�����*/
PROTOCOL_BUF	ProtocolBuf0;
static uint8_t SpkBuf[COMM_LENTH];	// �����洢���淢������Ϣ����Ҫ��������������

 /**
  * @brief  ������Ϣ�Ĵ���ͽ��
  * @param  ��
  * @retval ��
  */
void Comm1_Init(void)
{
	// ������ʼ��
	USART2_Init();
	/*��ʼ��Э��㻺��������*/
	ProtocolBuf0.pTxBuf = UART0Buf.TxBuf;
	ProtocolBuf0.pRxBuf = UART0Buf.RxBuf;
	ProtocolBuf0.RxLen = 0;
	ProtocolBuf0.TxLen = 0;
}

// ��Ϣ���
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
		pbuf[len++] = '3';		// ����pbuf[1] 
		pbuf[len++] = '3';		// ����pbuf[2] 
		
		pbuf[len++] = '5';
		pbuf[len++] = '5';
		pbuf[len++] = '0';
		pbuf[len++] = '0';		//pbuf[3~6] , �Ѷ�
		
		pbuf[len++] = '0';
		pbuf[len++] = '0';
		pbuf[len++] = '3';
		pbuf[len++] = '3';		// pbuf[7~10] , ���
		
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';
		pbuf[len++] = '4';		//pbuf[11~16] , entrance

		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 0;		// BCC, �����������0��β
		//pbuf[len++] = '\0';
		break;

	case FEE_G_MSG:
		pbuf[0] = MSG_SOF	;
		pbuf[1] = 'Y';			// YY ���Ե�Ϊ��ɫ
		pbuf[2] = MSG_EOF;
		pbuf[3] = 'Y';
		//pbuf[4] = '\0';
		bLastLaneRedGreenOperateState = GREEN;
		break;

	case FEE_R_MSG:
		pbuf[len++] = MSG_SOF	;
		pbuf[len++] = 'Z';		// ZZ ���Ե�Ϊ��ɫ
		pbuf[len++] = MSG_EOF;
		pbuf[len++] = 'Z';
		//pbuf[len++] = '\0';
		bLastLaneRedGreenOperateState = RED;
		break;

	case VER_PRINT_MSG:
		pbuf[len++] = 'X';		// ����汾, XY-XXXX
		pbuf[len++] = 'Y';
		pbuf[len++] = '-';

		memcpy(&pbuf[len],VerInfo,strlen(VerInfo));
		len = len + strlen(VerInfo);
		
		pbuf[len++] = 0x0D;	// 0D0A, �س�����
		pbuf[len++] = 0x0A;
		break;

	// ��ʱû������������
	case MEM_DUMP_MSG:
		break;

	case B_RES_MSG:
		pbuf[len++] = MSG_SOF	;
		pbuf[len++] = 'B';						// ��ϢB
		pbuf[len++] = LOCAL_ADD;				// ��ַ0x31
		pbuf[len++] = device_status_used.status_word[USED];
		pbuf[len++] = device_status_used.status_word[BACKUP];
		pbuf[len++] = MSG_EOF;
		
		xor_t = pbuf[1];			// 'B'
		for (i = 2; i< len-1; i++)		// MSG_SOF��EOF���������
		{
			xor_t = xor_t^pbuf[i];	//���У��
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
		for (i = COST_1; i<= WEIGHT; i++)		// MSG_SOF��EOF���������
		{
			xor_t = xor_t^SpkBuf[i];			//���У��
		}
		pbuf[len++] = MSG_SOF;
		pbuf[len++] = SpkBuf[VECLE];
		pbuf[len++] = SpkBuf[WEIGHT];			//����
		
		pbuf[len++] = SpkBuf[COST_0];			// ���
		pbuf[len++] = SpkBuf[COST_1];
		pbuf[len++] = SpkBuf[COST_2];
		pbuf[len++] = SpkBuf[COST_3];
		
		pbuf[len++] = SpkBuf[LEFT_0];			//���
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

	/* ͸��,ֱ�ӿ���*/
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
	/*���·��ͳ���*/
	buf->TxLen = len;
}
 

// ���ж�δ��������£�ֻ����printf ���з���
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

	//��ʼ���
	printf("%s", pProbuf->pTxBuf);
}


//���ͬʱ���
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
 *      ���յ�����Ϣ����У��
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
			j=rx_buf[COM_T];	// J�����������ֵ
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

	/* CR ����λ����*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='R'))
	{
		// Reset the system
		INT_DISABLE();		// ��ֹ��λ������
		NVIC_SystemReset();	// ����ط���Ҫ����һ��
		return;
	}

	/* CV ��ӡ�汾��Ϣ*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='V'))
	{
		message_pack(PC_UART, VER_PRINT_MSG,buf);
		//message_send_printf(PC_UART, VER_PRINT_MSG);
		return;
	}

	/* CD ��ȡ�ڴ���Ϣ*/
	if ((rx_buf[COM_T]=='C')&&(rx_buf[CTRL_T]=='D'))
	{
		message_pack(PC_UART, MEM_DUMP_MSG,buf);
		//message_send_printf(PC_UART, MEM_DUMP_MSG);
		return;
	}


	/* A����Ϣ: ��ʼ��+ͨ����+�豸��ַ+�����ֽ�1+���(4�ֽ�)*/
	/* +���(4�ֽ�)+���վ(6�ֽ�) +����+����+�����ֽ�2+ͨѶ���*/
	/* +������+У���� (��24�� �ֽ�)*/
	
	/* ��ȡ�����Ϣ*/
	for (i = 0; i<COMM_LENTH; i++)
	{
		SpkBuf[i]= rx_buf[i];
	}

	/*�ظ�֮ǰ�ȸ�������״̬*/
	detect_ALG_TTL_working();
	/* ��Ϣ����,�ȶ���λ��PC���лظ�*/
	message_pack(PC_UART, B_RES_MSG,buf);

	/* ��ȡ���õĿ�����Ϣ*/
	/* �����ֽ�1:	*/
	// 0λ: ������--1Ϊ̧�ˣ�0Ϊ����
	// 1λ: ������--1Ϊ���ˣ�0Ϊ����
	// 2λ: �����--1Ϊ�̵ƣ�0Ϊ���
	// 3λ: ������--1Ϊ��������0Ϊ������ȡ��
	// 4λ: �ⱨ��--1Ϊ�ⱨ����0Ϊ�ⱨ��ȡ��
	// 5λ: ͨ�е�--1Ϊ�̵ƣ�0Ϊ���
	// 6λ: δ����--����
	// 7λ: δ����--����
	device_control_used.control_word[USED]=SpkBuf[CTRL_T];  // �����ֽ�
	
	/* �����ֽ�2:	*/
	// 0λ: ���˿���λ  --1:���������Զ�����ģʽ��0:���������ֶ�����ģʽ
	// 1λ: �Ѷ���ʾλ  --1:�Ѷ���ʾ��            0:�Ѷ��ʾ
	// 2λ: ��������λ  --1:���ۣ�                0:������
	// 3λ: �Ѷ���ʾ����--1:���֣�            0:ˢ�£���Ϊ1ʱ������bit1
	// 4λ: ����ѡ��λ  --1:�㶫�汾���ޱ��ֹ��ܣ�0:ɽ���汾�б��ֹ��ܣ���������Ҫ����֧��
	// 5λ: ����������  --1:���ڳ��ڣ�            0:�������
	device_control_used.control_word[BACKUP]=SpkBuf[CTRL_B];
}


/*-----------------------------------------------------------------------------*
 *  NAME
 *      message_process
 *
 *  DESCRIPTION
 *      �����յ�����Ϣ,У�鲢���͵���һ��
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
		;  // ���Ƿ���������,ʲô������
	}
	else 
	{
		// ����Ǵ������Ϣ����
		message_pack(PC_UART, ERR_RES_MSG,buf);
	}
}

/***********************************************************************************
 * ������:	CommProc 
 * ����: 
 *           			 -ͨѶ������,�������е���,��Ҫ����RS 485������,
 *				   ���Ǳ�װ�õ����������������
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
void Comm_Proc(void)
{
	
	if (UART0Buf.RecFlag)		                      //RS485��������
	{	
		#ifdef DEBUG_MODE
	  		printf("%s",UART0Buf.RxBuf);    //�����ܵ�������ֱ�ӷ��ش�ӡ
	  	#endif
		
		ProtocolBuf0.pTxBuf = UART0Buf.TxBuf;         //��ַ�û�
		ProtocolBuf0.pRxBuf = UART0Buf.RxBuf;
		ProtocolBuf0.RxLen = UART0Buf.RxLen;
		ProtocolBuf0.TxLen = 0;
		UART0Buf.RxLen = 0;		//�Ѿ�����ȡ��ProtocolBuf0.RxLen, ������0

		/*ֱ���Ƚ���͸��*/
		//message_pack_printf(TRANS_UART, TRANS_MSG);

		message_process(&ProtocolBuf0);		//ͨ��Э�鴦��

		UART0Buf.TxLen = ProtocolBuf0.TxLen;  //�û��������������������
		if(UART0Buf.TxLen >0)
		{
			/*�ظ�B/C��Ϣ����λ��*/
			message_send_printf(PC_UART);
		}
		Delay_Ms(5);				// ��΢�е���ʱ,���Բ�Ҫ
		UART0Buf.RecFlag = 0;		//���������Ѵ��������ر�־
	}
	params_modify_deal();		//���������ݸı䴦��
}



/*********************************************END OF FILE**********************/
