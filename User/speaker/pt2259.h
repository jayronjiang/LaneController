#ifndef __24CXX_H
#define __24CXX_H   

#define PT2259_ID		0x88

#define MUTE_OFF		0x74		// 关闭静音
#define MUTE_ON		0x77		// 打开静音
#define CHIP_CLEAR	0xf0		// 芯片清除

/*注意-20是0xE0 | 0x02, -2是0xD0 |0x02*/
#define NEG_10DB		0xE0		// 双声道增益-10
#define NEG_1DB		0xD0		// 双声道增益-1


#define NS4160_OUT_GRP			GPIOA
#define NS4160_POUT			GPIO_Pin_8	//栏杆起


void PT2259_Config(u8 Data);
void PT2259_Set( u8 ucData10DB,u8 ucData1DB );
void NS4160_AB_type(void);
void NS4160_Disable(void);
void NS4160_D_type(void);
void NS4160_GPIO_OUT_Config(void);
void Speaker_Init(void);
void Speaker_Test(void);
#endif

