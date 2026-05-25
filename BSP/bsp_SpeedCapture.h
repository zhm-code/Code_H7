#ifndef __BSP_SPEEDCAPTURE_H__
#define __BSP_SPEEDCAPTURE_H__
#include "main.h"

//转速捕获描述符
typedef struct {
	uint32_t Cycle_Tim_Count;		//周期的定时器计数
	uint8_t  Cap_Step;				//捕获步骤		0：等待捕获	1：捕获进行中
	uint8_t  Data_Busy_Flag : 1;		//数据处理忙碌标志  0：不忙碌   1：在忙
	
}SpeedCapture_t;
extern SpeedCapture_t PE_SpeedCapture; //光电测速
extern SpeedCapture_t FG_SpeedCapture; //FG反馈

typedef struct 
{
	uint8_t uStart_Flag; //调零开始标志
	float fSet_PWM;
	
}Zero_Set_t;
extern Zero_Set_t Zero_Set;

typedef struct{
	
	uint8_t SP_Flag : 1;
	uint8_t AB_Flag : 1;
	
	uint8_t SP_Count;
	uint8_t AB_Count;
	
	float SP_STD_arr[10];
	float AB_STD_arr[10];
	
	float SP_mean;
	float AB_mean;
	
	float SP_STD;
	float AB_STD;
	
}STD_t;

extern STD_t user_spd_STD;


extern void Spe_Calculation(void);
#endif
