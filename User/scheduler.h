#ifndef __SCHEDULER_H
#define	__SCHEDULER_H


#define SYS_CHANGED   		BIT0		/* 外部设备检测出现变化,需要向上位机汇报*/
#define SYS_ERR_CHK			BIT1		/*3s进行一次系统检测*/
#define CAR_WATCHING		BIT2		/*后线圈开始监视*/
#define SYSTEM_100MS          	BIT3		/*暂时未使用*/

//#define CHECK_SYSTEM       	BIT5
//#define SYSTEM_1S          		BIT6
//#define PASSWORD_OK 		BIT7
//#define SYSTEM_200MS		BIT8

extern uint16_t system_flag;

void Task_Schedule(void);

#endif /* __SCHEDULER_H */

