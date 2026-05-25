#ifndef __APP_CG1402S_H__
#define __APP_CG1402S_H__

#include "main.h"

//3中不同位置判断
typedef enum 
{
	ROLLING_BRUSH_BOTTOM = 1,				//滚刷底部位置
	DEFAULT = 2,							//原点
	DEEP_CLEANING_BOTTOM = 3				//深度清洁底部位置
	
}LOCATION;

typedef enum
{
	GB1402S_LEFT = 0,
	GB1402S_RIGFT,
}GB1402S_DIR;

typedef enum
{
	STATIC = 0,
	MOVE,
}MOVE_STATE;

typedef struct {
	
	LOCATION location_flag;					//位置标记
	uint8_t location_init_flag;				//位置初始完成标志（找到初始位置了）
	MOVE_STATE move_flag;						//移动标志
	GB1402S_DIR FR_flag;						//方向标志
	uint8_t DCL_to_RB_flag;
	
	float RB_DEFAULT_distance;				//滚刷开始距离
	float RB_END_distance;					//滚刷底部距离
	float RB_range_difference;				//滚刷底部距离差值
	
	float DCL_DEFAULT_distance;				//深度清洁开始位置距离
	float DCL_END_distance;					//深度清洁底部位置距离
	float DCL_range_difference;				//深度清洁底部距离差值
	
	uint64_t RB_default_time;
	uint64_t RB_bottom_time;
	
	uint64_t DCL_default_time;
	uint64_t DCL_bottom_time;
	
	uint16_t RB_lifting_time;					//抬升时间
	uint16_t RB_decline_time;					//下降时间
	
	uint16_t DCL_arrival_time;
	
	uint16_t pulse_count_1;
	uint16_t pulse_count_2;
	uint8_t  pulse_count_flag;
	
	float AD_ROLLING_BRUSH_BOTTOM;
	float AD_DEFAULT;
	float AD_DEEP_CLEANING_BOTTOM;
	
	uint8_t zero_flag;
	
	
	float cur_mean;
	float cur_MAX;
	float cur_STD;
}GB1402S_Data_t;
extern GB1402S_Data_t GB1402S_Data;
extern void Detect_Location(void );

extern void vAPP_RunOpenLoop_GB1402S(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);

extern void vAPP_SetTestFinish_GB1402S(void);
extern void vAPP_RB_Distance_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_RB_AD_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_RB_Process_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_DCL_Distance_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_DCL_AD_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_DCL_Process_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_Decoupling_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
extern void vAPP_Set_Zeor_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//设置气缸
extern void vAPP_SetCylinder_GB1402S(uint32_t RunTime_ms, uint16_t usSetCyl);

#endif

