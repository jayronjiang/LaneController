#ifndef __SCHEDULER_H
#define	__SCHEDULER_H


#define SYS_CHANGED   		BIT0		/* �ⲿ�豸�����ֱ仯,��Ҫ����λ���㱨*/
#define SYS_ERR_CHK			BIT1		/*3s����һ��ϵͳ���*/
#define CAR_WATCHING		BIT2		/*����Ȧ��ʼ����*/
#define SYSTEM_100MS          	BIT3		/*��ʱδʹ��*/

//#define CHECK_SYSTEM       	BIT5
//#define SYSTEM_1S          		BIT6
//#define PASSWORD_OK 		BIT7
//#define SYSTEM_200MS		BIT8

extern uint16_t system_flag;

void Task_Schedule(void);

#endif /* __SCHEDULER_H */

