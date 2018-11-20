#ifndef __MESSAGE_HANDLE_H
#define	__MESSAGE_HANDLE_H

#define UART1_COM		0
#define UART2_COM		1
#define UART3_COM		2

#define PC_UART			UART1_COM	//������PCͨ�ŵ�Ϊ�ĸ�����?
#define FEE_UART			UART1_COM	//��������Ժ������Ĵ���
#define TRANS_UART		UART1_COM	//����͸���Ĵ���

// λ����
#define MSG_SOF			0x02		// ��Ϣ�Ŀ�ʼ
#define MSG_EOF			0x03		// ��Ϣ�Ľ���

#define COMM_LENTH 		24		// ��Ϣ�ܳ�24

// ��Ϣ���Ͷ���
#define INTT_MSG				0
#define FEE_G_MSG			1
#define FEE_R_MSG			2
#define VER_PRINT_MSG		3		//��ӡ�汾��
#define MEM_DUMP_MSG		4		//�����ڴ�,ʲô����
#define B_RES_MSG			5		//����ϢB�ظ�
#define ERR_RES_MSG			6		//��Ϣ��ʽ����
#define SPK_MSG				7		// ��������
#define COST_MSG			8		// �Ѷ���ʾ
#define TRANS_MSG			9		// ͸������

#define ALL8_MSG				0x10		// ��ʾȫ8����������
#define VOXPLAY_MSG			0x11		// �������Բ��Ų�������

#define A_MSG				98		// ģ��A��Ϣ����
#define CR_MSG				97		// ģ��C��Ϣ����
#define CV_MSG				96		// ģ��C��Ϣ����
#define CD_MSG				95		// ģ��C��Ϣ����

#define TEST_MSG				99		// ���Թ���

#define LOCAL_ADD			('1')		//0x31

#define ERR_OK				0		/*У��OK*/
#define SITEID_ERROR		1		/*���ص�ַ����*/
#define LENGTH_ERROR		2		/*�����Ȳ���*/
#define CRC_ERROR			3		/*CRCУ�����*/
#define SOF_ERROR			4		/*ǰ���������������*/


// ͨ��Э����ֽڶ���
#define BSOF					0	//��ʼ��
#define COM_T				1	//ͨ����A/B/C��
#define ADDR					2	//�豸��ַ
#define CTRL_T				3	//�����ֽ�
#define COST_0				4	//���
#define COST_1				5	//���
#define COST_2				6	//���
#define COST_3				7	//���
#define LEFT_0				8	//���
#define LEFT_1				9	//���
#define LEFT_2				10	//���
#define LEFT_3				11	//���
#define ENTRY_0				12	//���վ
#define ENTRY_1				13	//���վ
#define ENTRY_2				14	//���վ
#define ENTRY_3				15	//���վ
#define ENTRY_4				16	//���վ
#define ENTRY_5				17	//���վ
#define VECLE				18	//����
#define WEIGHT				19	//����
#define CTRL_B				20	//�����ֽ�2
#define SQU					21	//ͨѶ���
#define BEOF					22	//����
#define BCC					23	//У����


typedef  struct 
{
	uint8_t *pTxBuf;
	uint8_t *pRxBuf;
	uint16_t TxLen;
	uint16_t RxLen;
}PROTOCOL_BUF;

extern PROTOCOL_BUF	ProtocolBuf0;

void Comm1_Init(void);
void message_pack(uint8_t uart_no, uint8_t msg_type,PROTOCOL_BUF *buf);
void message_send_printf(uint8_t uartNo);
void message_pack_printf(uint8_t uartNo, uint8_t msg_type);
void Comm_Proc(void);

#endif /* __MESSAGE_HANDLE_H */
