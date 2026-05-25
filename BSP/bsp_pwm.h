#ifndef __BSP_PWM_H__
#define __BSP_PWM_H__
#include "main.h"
#define PWM_100   100
#define PWM_0     0

extern void PWM_SetOutput(TIM_HandleTypeDef htimx, unsigned int TIM_Channel, float Percentage);
extern void PWM_SET_Period(TIM_HandleTypeDef htimx, uint32_t ms);


extern void PWM_Init(void );
extern void PWM_ALL_SET(float Percentage);
#endif

