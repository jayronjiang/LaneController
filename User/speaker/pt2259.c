 /**
  ******************************************************************************
  * @file    myiic.c
  * @author  Jerry
  * @date    17-Dec-2018
  *
  * @brief   pt2259�����ļ�
  *
  ******************************************************************************
  */
#include "include.h" 

//��ʼ��IIC�ӿ�
void PT2259_Init(void)
{
	IIC_Init();
	NS4160_GPIO_OUT_Config();
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void PT2259_WriteOneByte(u8 DataToWrite)
{				   	  	    																    										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();//����һ��ֹͣ���� 
	//Delay_Ms(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
#if 0
//��PT2259ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 PT2259_ReadOneByte(u16 ReadAddr)
{	
	u8 temp=0;		  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(PT2259_ID);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(PT2259_ID+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(PT2259_ID+1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}

void PT2259_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		PT2259_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}


//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 PT2259_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=PT2259_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}


//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 PT2259_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)
	{
		return 0;
	}
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)
		{
			return 0;
		}
	}
	return 1;											  
}


//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void PT2259_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=PT2259_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 
#endif
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void PT2259_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		PT2259_WriteOneByte(*pBuffer);
		pBuffer++;
	}
}

// ��������,������˥����,���ֲ�P9,��Ҫ����д��2���ֽ�:
// ����˥��-42dB,��Ҫ��˥��-40dB��Ȼ��˥��-2dB.
void PT2259_Set( u8 ucData10DB,u8 ucData1DB )
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID );
	PT2259_WriteOneByte( ucData10DB );
	PT2259_WriteOneByte( ucData1DB  );
}

void PT2259_Config(u8 Data)
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID);  // is PT2259 address
	PT2259_WriteOneByte( Data ); // clear register
}

void PT2259_Clear()
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID);  // is PT2259 address
	PT2259_WriteOneByte( 0xf0 ); // clear register
}

void PT2259_OpenVolume(u8 Data)
{
	IIC_Start();
	PT2259_WriteOneByte( PT2259_ID);  // is PT2259 address
	PT2259_WriteOneByte( Data ); // clear register
}


void NS4160_Output_High(void)	
{
	GPIO_SetBits(NS4160_OUT_GRP, NS4160_POUT);
}

void NS4160_Output_Low(void)	
{
	GPIO_ResetBits(NS4160_OUT_GRP, NS4160_POUT);
}

void NS4160_AB_type(void)	
{
	NS4160_Output_Low();
	Delay_Us(30);
	NS4160_Output_High();
	Delay_Ms(150);
	//NS4160_Output_Low();
	//NS4160_Output_High();
}

void NS4160_D_type(void)	
{
	NS4160_Output_Low();
	Delay_Us(100);
	NS4160_Output_High();
	DELAY_4uS();
	NS4160_Output_Low();
	DELAY_4uS();
	NS4160_Output_High();
	Delay_Ms(100);
}

void NS4160_Disable(void)	
{
	GPIO_ResetBits(NS4160_OUT_GRP, NS4160_POUT);
}

/***********************************************************************************
 * ������:	 
 * ����: 
 *           	-����ָ���������.
 *		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
void NS4160_GPIO_OUT_Config(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������������˿�PX��ʱ��*/
	RCC_Clock_Set(NS4160_OUT_GRP, ENABLE);
	//RCC_APB2PeriphClockCmd( DEVICE_RCC_GRP, ENABLE); // ʹ��PC�˿�ʱ��  
	GPIO_InitStructure.GPIO_Pin = NS4160_POUT;	//ѡ���Ӧ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NS4160_OUT_GRP, &GPIO_InitStructure);  //��ʼ���˿�
	NS4160_Output_Low();
}

