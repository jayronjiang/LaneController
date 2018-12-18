#ifndef __MESSAGE_HANDLE_H
#define	__MESSAGE_HANDLE_H

#define PC_UART			UART1_COM	//定义与PC通信的为哪个串口?
#define FEE_UART			UART2_COM	//定义与费显和声音的串口
#define TRANS_UART		UART2_COM	//定义透传的串口

// 位定义
#define MSG_SOF			0x02		// 信息的开始
#define MSG_EOF			0x03		// 信息的结束

#define COM_CTRL_LENTH 		24		// 信息总长24
#define COM_FX1_LENTH 		52		// 费显,信息总长24
#define COM_FX2_LENTH 		6		// 费显红绿灯,长度6
#define COM_FX3_LENTH 		4		// 清除费显,或全亮4

// 信息类型定义
#define INTT_MSG				0
#define FEE_G_MSG			1
#define FEE_R_MSG			2
#define VER_PRINT_MSG		3		//打印版本号
#define MEM_DUMP_MSG		4		//拷贝内存,什么作用
#define B_RES_MSG			5		//以信息B回复
#define ERR_RES_MSG			6		//信息格式错误
#define SPK_MSG				7		// 语音报价
#define COST_ON_MSG				8		// 费额显示
#define COST_OFF_MSG				9		// 费额清除
#define TRANS_MSG			0x0A		// 透传命令

#define ALL8_MSG				0x10		// 显示全8，拷机测试
#define VOXPLAY_MSG			0x11		// 拷机测试播放测试声音

#define A_MSG				98		// 模拟A信息测试
#define CR_MSG				97		// 模拟C信息测试
#define CV_MSG				96		// 模拟C信息测试
#define CD_MSG				95		// 模拟C信息测试

#define TEST_MSG				99		// 测试功能

#define LOCAL_ADD			('1')		//0x31

#define ERR_OK				0		/*校验OK*/
#define SITEID_ERROR		1		/*本地地址不对*/
#define LENGTH_ERROR		2		/*包长度不对*/
#define CRC_ERROR			3		/*CRC校验错误*/
#define SOF_ERROR			4		/*前导符或结束符错误*/
#define DUMMY_ERROR			5		/*数据帧为不需处理的*/
#define TRANS_REQ			6		/*不校验,直接转发的数据*/


// 通信协议的字节定义
#define BSOF					0	//起始码
#define COM_T				1	//通信类A/B/C类
#define ADDR					2	//设备地址
#define CTRL_T				3	//控制字节
#define COST_0				4	//金额
#define COST_1				5	//金额
#define COST_2				6	//金额
#define COST_3				7	//金额
#define LEFT_0				8	//余额
#define LEFT_1				9	//余额
#define LEFT_2				10	//余额
#define LEFT_3				11	//余额
#define ENTRY_0				12	//入口站
#define ENTRY_1				13	//入口站
#define ENTRY_2				14	//入口站
#define ENTRY_3				15	//入口站
#define ENTRY_4				16	//入口站
#define ENTRY_5				17	//入口站
#define VECLE				18	//车型
#define WEIGHT				19	//车种
#define CTRL_B				20	//控制字节2
#define SQU					21	//通讯序号
#define BEOF					22	//结束
#define BCC					23	//校验码

// 费显命令码定义
#define FX_SOF				0xF5		// 起始码
#define FX_DISPLAY			0x01		// 操作码,显示费显
#define FX_DISCLR				0x02		// 操作码,清屏
#define FX_DISALL				0x03		// 操作码,全屏点亮
#define FX_LED				0x10		// 操作码,红绿灯控制
#define FX_EOF				0xFE		// 结束码


typedef  struct 
{
	uint8_t *pTxBuf;
	uint8_t *pRxBuf;
	uint16_t TxLen;
	uint16_t RxLen;
}PROTOCOL_BUF;

extern PROTOCOL_BUF	ProtocolBuf[UART_NUM];

void Comm1_Init(uint32_t baudrate);
void Comm2_Init(uint32_t baudrate);
void message_pack(USART_LIST uart_no, uint8_t msg_type,PROTOCOL_BUF *buf);
void message_send_printf(uint8_t uartNo);
void message_pack_printf(uint8_t uartNo, uint8_t msg_type);
void Comm_Proc(void);

#endif /* __MESSAGE_HANDLE_H */
