 /**
  ******************************************************************************
  * @file    xmodem.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   文件传输协议, 通过串口传输文件数据
  *
  ******************************************************************************
  */

#include "include.h"

#ifdef SPEAKER_ENABLE

#define XMODEM_SOH  0x01
#define XMODEM_STX  0x02
#define XMODEM_EOT  0x04
#define XMODEM_ACK  0x06
#define XMODEM_NAK  0x15
#define XMODEM_CAN  0x18
#define XMODEM_CTRLZ 0x1A
//////////////////////////////////////////////////////////////////
/* 回调函数*/

extern uint8_t *xmodem_buff; 

extern uint8_t xm_flush_rx_record(uint8_t len);

// 用指针方式
//unsigned char (*xm_flush_rx_record)(unsigned char len);
//unsigned char (*xm_load_tx_record) ( );

///////

#define XMODEM_XMIT_SIZE 128

#define CALL_xm_flush_rx_record(len) xm_flush_rx_record(len)
#define CALL_xm_load_tx_record() xm_load_tx_record()

/////////////////////////////////////////////////////////////////////////////////

const uint16_t crc16_tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

uint8_t  _wait_byte_1TS()
{
	uint8_t timecnt=0xff;
	uint8_t timeout=0xff;

	//USART_ReceiveData(SHELL_USART);	// 先读一次,清掉标志
	while(timecnt--)
	{
		while(timeout--) 
		{
			if (USART_GetFlagStatus(SHELL_USART, USART_FLAG_RXNE) != RESET) 
			{
				return 1;
			}
		}
	}
	return 0;
}
uint8_t  _wait_byte()
{
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;
	if( _wait_byte_1TS() ) return 1;

	return 0;
}
 
uint8_t  _inbyte()
{
    uint8_t cTm;  
    cTm = USART_ReceiveData(SHELL_USART);      
    return cTm;   
}

void _outbyte(uint8_t c)
{
	/* 发送一个字节数据到USARTx */
	USART_SendData(SHELL_USART, c);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(SHELL_USART, USART_FLAG_TXE) == RESET);
}

//////////////////////////////////

#define MAX_RETRY 10

#define E0_OK	 0	/* normal end */
#define E1_RECV  1	/* recv error */
#define E2_CAN	 2	/* canceled by remote */
#define E3_SYNC  3	 /* sync error */
#define E4_RETRY 4	/* too many retry error */
#define E9_FRW	 9

/*
如何启动传输？
1.传输由接收方启动，方法是向发送方发送"C"或者NAK。
  发送NAK信号-表示接收方打算用累加和校验；发送字符"C"-则表示接收方想打算使用CRC校验.
2.发送方认为可以发送第一个数据包，传输已经启动。
  发送方接着应该将数据以每次128/1k字节的数据加上包头，包号，包号补码，末尾加上校验和，打包成帧格式传送。

| Start Of Hearder | Packet Number | ~(Packet Number) | Packet Data | 16-Bit CRC |
*/


uint8_t xmodemReceive()
{
	uint8_t  retry;
	uint8_t  c;
	uint8_t  transz = 0;
	uint16_t i;
	uint8_t  ipacketno = 1, blockno;
	uint16_t  crc;

	// 请求发送，以启动传输	
	// 接收帧头
	retry = 0;
	start_frame:
		for( ; ; ) 
		{
			retry++;
			if(transz==0 && retry>MAX_RETRY*2) break;
			if(transz!=0 && retry>MAX_RETRY) break;
			
			if(transz==0) _outbyte('C');	// 第一次时启动传输、呼叫, 用CRC校验

			if(  _wait_byte()==0 ) continue;

			c = _inbyte();
			switch (c) 
			{
			case XMODEM_SOH:
					transz = 128;
					goto start_recv;
			case XMODEM_STX:
					transz = 250;
					goto start_recv;
			case XMODEM_EOT:
					_outbyte(XMODEM_ACK);
					return E0_OK; /* normal end */
			case XMODEM_CAN:
					_wait_byte();
					c = _inbyte();
					if( c == XMODEM_CAN)
					{
						_outbyte(XMODEM_ACK);
						return E2_CAN; /* canceled by remote */
					}
					break;
			default:
					break;
			}
		}
		_outbyte(XMODEM_CAN);
		_outbyte(XMODEM_CAN);
		_outbyte(XMODEM_CAN);
		return E3_SYNC; /* sync error */

	start_recv:
		if(  _wait_byte()==0 )  return E1_RECV; /* recv error */
		blockno = _inbyte();	
		if(  _wait_byte()==0 )  return E1_RECV; /* recv error */
		c = _inbyte();

		if( (blockno != (255-c) ) || 
			(blockno !=ipacketno && blockno !=(ipacketno-1) ) ) 
		{
			for(i=0; i<transz+2; i++)	// flush data
			{
				 _wait_byte();
				 c = _inbyte();
			}
			goto reject;
		}

		crc = 0;
		for(i=0; i<transz; i++)
		{
			if(  _wait_byte()==0 )  return E1_RECV; /* recv error */
			c = _inbyte();

			crc = crc16_tab[ ((crc>>8) ^ c) & 0xFF ] ^ (crc << 8);
			xmodem_buff[ i ] = c;
		}

		if(  _wait_byte()==0 )  return E1_RECV; /* recv error */
		c = _inbyte();
		if( c != ((crc >> 8)&0xFF) ) goto reject;

		if(  _wait_byte()==0 )  return E1_RECV; /* recv error */
		c = _inbyte();
	    if( c != (crc&0xFF) ) goto reject;   

		if( blockno ==(ipacketno-1) ) ;// 重传的包怎么处理？
		c = CALL_xm_flush_rx_record(transz);
		if( c!=0 ) 
		{
			_outbyte(XMODEM_CAN);
			_outbyte(XMODEM_CAN);
			_outbyte(XMODEM_CAN);
			return E9_FRW;
		 }
		if(blockno == ipacketno) ipacketno++;

		_outbyte(XMODEM_ACK);

		retry = 0;	// ???
		goto start_frame;

	reject:
		retry ++;
		if(retry>MAX_RETRY)
		{
			_outbyte(XMODEM_CAN);
			_outbyte(XMODEM_CAN);
			_outbyte(XMODEM_CAN);
			return E4_RETRY; /* too many retry error */
		}
	
		for(i=0; i<0xff; i++); // delay

		_outbyte(XMODEM_NAK);
		goto start_frame;
}

uint8_t xmodemTransmit()
{
	uint8_t retry;
	uint8_t c;
	uint8_t packetno = 1;
	uint16_t crc=0;

	// 等待接收方启动
	for( retry = 0; retry < MAX_RETRY*2; ++retry) 
	{
		if ( _wait_byte()==0 ) continue;

		c = _inbyte();
		switch (c) 
		{
			case 'C':
					goto start_trans; // 仅仅支持CRC校验
			case XMODEM_NAK:
//					goto start_trans;  // 暂不支持和校验
					continue;
			case XMODEM_CAN:
					_wait_byte();
					c = _inbyte();
					if( c == XMODEM_CAN) 
					{
						_outbyte(XMODEM_ACK);
						return E2_CAN; /* canceled by remote */
					}
					break;
			default:
					break;
		}
	}
	_outbyte(XMODEM_CAN);
	_outbyte(XMODEM_CAN);
	_outbyte(XMODEM_CAN);
	return E3_SYNC; /* no sync */
   
    // 开始按帧发送
	start_trans:
 
		c = CALL_xm_load_tx_record( );  
		if(c)
		{
			while(c<XMODEM_XMIT_SIZE) xmodem_buff[c++] = XMODEM_CTRLZ;	// packet size = 128
			for (retry = 0; retry < MAX_RETRY/2; ++retry)
			{
				// send packet
				_outbyte( XMODEM_SOH );
				_outbyte( packetno );
				_outbyte( 255-packetno );
				crc = 0;
				for(c=0; c<XMODEM_XMIT_SIZE; c++)
				{
					_outbyte( xmodem_buff[c] );
					crc = crc16_tab[((crc>>8) ^ xmodem_buff[c]) & 0xFF] ^ (crc << 8);
				}
				_outbyte(crc>>8);   
				_outbyte(crc); 
			
				// wait for answer
				for(c=0,crc=0; c<3 && crc==0; c++) 
					crc = _wait_byte() ;
				
				if(crc==0) continue;

				c = _inbyte();
				switch (c) 
				{
					case XMODEM_ACK:
						++packetno;
						goto start_trans;
					case XMODEM_CAN:
						_wait_byte();
						c = _inbyte(); 
						if ( c == XMODEM_CAN)
						{
							_outbyte(XMODEM_ACK);
							return E2_CAN; /* canceled by remote */
						}
						break;
					case XMODEM_NAK:
					default:
						break;
				}
			}	//- end of 'retry < MAX_RETRY'
			_outbyte(XMODEM_CAN);
			_outbyte(XMODEM_CAN);
			_outbyte(XMODEM_CAN);
			return E4_RETRY; /* too many xmit error */
		}
		else // -if c==0
		{
			for (retry = 0; retry < 10; ++retry) 
			{
				_outbyte(XMODEM_EOT);

				_wait_byte();
				c = _inbyte();
				if ( c == XMODEM_ACK) break;
			}
//			TRACE("XMODEM_EOT:%s\n", c==XMODEM_ACK? "ACK" : "??");
			return E0_OK;
		}
}
#endif

/////////////////////////////////////////////////////////


