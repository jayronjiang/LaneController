#ifndef __TIME_H
#define	__TIME_H

//extern uint16_t system_flag;
extern volatile uint32_t system_time_ms;

/* 用在20ms中断中,因此1s就是50个计数周期*/
#define MS_100 				(5)		// 100ms就是5个周期
#define ONE_SECOND 			(50)

#define AUTO_DETCET_TIME	(3  *ONE_SECOND)
#define ALARM_TIME			(10  *ONE_SECOND)


void Delay_Xus(uint16_t us);
void Delay_Xms(uint16_t ms);
void Delay_Ms(uint32_t myMs);
	
//void Delay_Ms(u32 myMs);		//while实现的延时函数，在time.c内作为一种可选择的延时方式，不提供外部接口
//void Delay_Us(u32 myUs);

//void Delay_us(u32 us);	//用timer实现

void Start_Timer_x(TIM_TypeDef* TIMx);
void Stop_Timer_x(TIM_TypeDef* TIMx);
void Timer_Start(void);

void Time_Configuration(void);
uint64_t readnowtime(void);
void DelayAndFeedDog(uint32_t myMs);

#endif /* __TMIE_H */

