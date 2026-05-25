#ifndef __APP_SYSPARAM_EX_H__
#define __APP_SYSPARAM_EX_H__
#include "main.h"

//驱动器上电
typedef struct {
	//设置域
	float RunTime;
	uint16_t SetVol;
	
	//上传域
	
	
}DrivePowerOn_DATA_t;

//编码器辨识
typedef struct {
	//设置域
	float RunTime;
	
	//上传域
}EncoderIdentification_DATA_t;

typedef struct {
	//设置域
	float RunTime;
	uint16_t SetVol;
	
	//上传域
	float spd_dev;
	float spd_motor;
	float upload_curr;
	float upload_vol;
	float upload_power;
	
}RunClosedLoop_DATA_t;


typedef struct {

	//GB1402S
	float RB_Test_FR;
	float DCL_Test_FR;
	float Whole_Engine_Test_FR;
	
	float RB_Test_Volt;
	float DCL_Test_Volt;
	float Whole_Engine_Volt;
	
	float RB_Set_Test_Volt;
	float DCL_Set_Test_Volt;
	float Whole_Engine_Set_Volt;
	
	float RB_Test_Time_s;
	float DCL_Test_Time_s;
	float Whole_Engine_Test_Time_s;
	
	float RB_Set_Test_Time_s;
	float DCL_Set_Test_Time_s;
	float Whole_Engine_Set_Test_Time_s;
	
	float RB_Curr_mean;
	float RB_Curr_MAX;
	float DCL_Curr_mean;
	float DCL_Curr_MAX;
	
	float DCL_DEFAULT_distance;		//深度清洁默认距离
	float DCL_END_distance;			//深度清洁结束距离
	float DCL_range_difference;		//深度清洁距离差
	

	float RB_DEFAULT_distance;		//滚刷默认距离
	float RB_END_distance;		    //滚刷结束距离
	float RB_range_difference;	    //滚刷距离差
	
	float AD_ROLLING_BRUSH_BOTTOM_Volt;	//滚刷底部AD电压
	float AD_DEFAULT_Volt;				//滚刷原点AD电压
	float AD_DEEP_CLEANING_BOTTOM_Volt;
	
	uint64_t ROLLING_BRUSH_BOTTOM_Time;
	uint64_t DEFAULT_Time;
	uint64_t DEEP_CLEANING_BOTTOM_Time;
	
	uint16_t RB_Lifting_Time;					//抬升时间
	uint16_t RB_Decline_Time;					//下降时间
	uint16_t DCL_Arrival_Time;			//移动时间

	float Load_Cur;				//空载电流
	float NLoad_Cur;			//带载电流
	float Difference_Value_Cur;	//差值电流
	
	uint8_t pulse_count_1;		//1号光栅脉冲
	uint8_t pulse_count_2;		//2号光栅脉冲
	
	uint8_t zero_flag;			//停止标志
	
	float RB_Homing_Distance;	//归位抬升距离差
	float DCL_Homing_Distance;	//归位深度清洁距离差

}GB1402S_DATA_t;

#endif
