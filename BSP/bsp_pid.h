#ifndef __BSP_PID_H__
#define __BSP_PID_H__
#include "main.h"

typedef struct{
	float fInitial_Value;	//初始值
	float fActual_Value;	//实际值
	
	float fError;        	// 计算误差
	float fPre_error; 		// 上一次的误差，用于计算微分项
	
	float fKp; 				//比例增益
	float fKi; 				//积分增益
	float fKd; 				//微分增益
	
	float fProportion;		//比例
	float fIntegral;		//积分项
	float fDerivative;		// 微分项
	
	float fOutput; 			//实际输出
	
	
}pid_t;

extern pid_t pid_pwm_set_GB1402H;
extern pid_t pid_pwm_set_GB10847;
extern pid_t pid_pwm_set_GB10865;
extern pid_t pid_pwm_set_ER17353;
extern pid_t pid_pwm_set_ZC17353;
extern pid_t pid_pwm_set_GB5745X;

extern void PID_Controller(float setpoint, float actual_value, pid_t *pid_pwm_set) ;



#endif
