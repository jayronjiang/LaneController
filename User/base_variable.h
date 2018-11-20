/********************************************************************************
*  版权所有: 	利通研究院			
*
*  文件名:       base_variable.h 
*
*  作者:             Jerry
*
*  创建日期: 2018.10.17
*
*  描述: 定义结构体和联合体数据的宏定义，会被所有的文件引用
*
*  其它: 
*
*  历史:  
********************************************************************************/

#ifndef _BASE_VARIABLE_H
#define _BASE_VARIABLE_H

#define DEBUG_MODE

typedef unsigned char bool;		// 定义bool类型，只使用TRUE和FALSE 2个值

#define BIT0 0X01
#define BIT1 0X02
#define BIT2 0X04
#define BIT3 0X08
#define BIT4 0X10
#define BIT5 0X20
#define BIT6 0X40
#define BIT7 0X80
#define BIT8 0X100
#define BIT9 0X200
#define BIT10 0X400
#define BIT11 0X800
#define BIT12 0X1000
#define BIT13 0X2000
#define BIT14 0X4000
#define BIT15 0X8000
#define BIT16 0X10000
#define BIT17 0X20000
#define BIT18 0X40000
#define BIT19 0X80000
#define BIT20 0X100000
#define BIT21 0X200000
#define BIT22 0X400000
#define BIT23 0X800000
#define BIT24 0X1000000
#define BIT25 0X2000000
#define BIT26 0X4000000
#define BIT27 0X8000000
#define BIT28 0X10000000
#define BIT29 0X20000000
#define BIT30 0X40000000
#define BIT31 0X80000000

#define	BIT(i)	(1 << (i))
#define   LBIT(i)	(1UL<<(i))	/*long型的移位操作*/


#define TRUE	       		1
#define FALSE          	0

//#define TFACT	9	// 时钟折算因子, 发现开发板时钟一直是8M ,原因不明
#define TFACT	1

#endif

