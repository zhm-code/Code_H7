#ifndef __BSP_ENC_H__
#define __BSP_ENC_H__

#include "main.h"

typedef struct 
{
	int ENC_Pulses_Count; //采集的脉冲个数
	uint8_t DIR_Flag : 1;
	float RPM;
	uint32_t Motor_Pulses_Count; //电机不经减少比出的脉冲数
}ENC_t;
extern ENC_t xKnob_ENC; //旋转编码器用于LVGL
extern ENC_t xMotor_ENC; //电机AB编码器用于测速

extern uint8_t ER1503_Once_Flag;
extern uint8_t ER1503_Dir;


enum {
	NONDIR = 0,    //无方向
	LEFT    = 1,	//左
	RIGHT   = 2,	//右
	
};
typedef uint32_t DIR_VAL;

extern void ENC_Init(void ); 			//初始化
extern void Get_ENC_PulsesCount(void); 	//读取脉冲次数
extern DIR_VAL Get_ENC_DIR(ENC_t * ENC, TIM_HandleTypeDef * htim);
#endif 
