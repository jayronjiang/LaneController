#ifndef __TIME_H
#define	__TIME_H

#define SYS_CHANGED   		BIT0		/* 外部设备检测出现变化,需要向上位机汇报*/
#define SYS_ERR_CHK			BIT1		/*3s进行一次系统检测*/
#define CAR_WATCHING		BIT2		/*后线圈开始监视*/
#define SYSTEM_200MS          		BIT3		/*暂时未使用*/

extern uint16_t system_flag;
extern volatile uint32_t system_time_ms;

/* 用在20ms中断中,因此1s就是50个计数周期*/
#define MS_200 				(10)		// 100ms就是5个周期
#define ONE_SECOND 			(50)

#define AUTO_DETCET_TIME	(3  *ONE_SECOND)
#define ALARM_TIME			(10  *ONE_SECOND)


// xx时间片（机器周期数）
#define TIME_SPLIT_1US 	((SystemCoreClock)/1000000)		// 1微妙周期数，1.8432@22.118,400MHz
#define TIME_SPLIT_1MS 	((SystemCoreClock)/1000)	   		// 1毫秒周期数，1843.2@22.118,400MHz

extern uint8_t wdt_counter;

void Delay_Xus(uint16_t us);
void Delay_Xms(uint16_t ms);
void Delay_Ms(uint32_t myMs);

void Start_Timer_x(TIM_TypeDef* TIMx);
void Stop_Timer_x(TIM_TypeDef* TIMx);
void Timer_Start(void);

void Time_Configuration(void);
uint64_t readnowtime(void);
void DelayAndFeedDog(uint32_t myMs);

#endif /* __TMIE_H */

