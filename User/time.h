#ifndef __TIME_H
#define	__TIME_H

//extern uint16_t system_flag;
extern volatile uint32_t system_time_ms;

/* ����20ms�ж���,���1s����50����������*/
#define MS_100 				(5)		// 100ms����5������
#define ONE_SECOND 			(50)

#define AUTO_DETCET_TIME	(3  *ONE_SECOND)
#define ALARM_TIME			(10  *ONE_SECOND)


void Delay_Xus(uint16_t us);
void Delay_Xms(uint16_t ms);
void Delay_Ms(uint32_t myMs);
	
//void Delay_Ms(u32 myMs);		//whileʵ�ֵ���ʱ��������time.c����Ϊһ�ֿ�ѡ�����ʱ��ʽ�����ṩ�ⲿ�ӿ�
//void Delay_Us(u32 myUs);

//void Delay_us(u32 us);	//��timerʵ��

void Start_Timer_x(TIM_TypeDef* TIMx);
void Stop_Timer_x(TIM_TypeDef* TIMx);
void Timer_Start(void);

void Time_Configuration(void);
uint64_t readnowtime(void);
void DelayAndFeedDog(uint32_t myMs);

#endif /* __TMIE_H */

