#include "bsp_pid.h"


pid_t pid_pwm_set_GB1402H = {
	.fKp = 0.0001,
	.fKi = 0.001,
	.fKd = 0.001,
};

pid_t pid_pwm_set_GB10847 = {
	.fKp = 0.1,
	.fKi = 0.5,
	.fKd = 0.01,
};

pid_t pid_pwm_set_GB10865 = {
	.fInitial_Value = 30,
	.fKp = 10,
	.fKi = 1,
	.fKd = 0.5,
};

pid_t pid_pwm_set_ER17353 = {
	.fKp = 0.01,
	.fKi = 0.01,
	.fKd = 0.01,
};

pid_t pid_pwm_set_ZC17353 = {
	.fKp = 0.01,
	.fKi = 0.01,
	.fKd = 0.01,
};
/**
* @brief PID调节
*	
* @param setpoint
*			  设定的值
*			
* @param actual_value
*			  实际的值
* @return  
*/
void PID_Controller(float setpoint, float actual_value, pid_t *pid_pwm_set) 
{
    float output;        // PID输出

    // 计算误差
    pid_pwm_set->fError = setpoint - actual_value;

	//比例项
	pid_pwm_set->fProportion = pid_pwm_set->fKp * pid_pwm_set->fError;
	
    // 积分项
    pid_pwm_set->fIntegral += pid_pwm_set->fKi * pid_pwm_set->fError;

    // 微分项
    pid_pwm_set->fDerivative = pid_pwm_set->fKd * (pid_pwm_set->fError - pid_pwm_set->fPre_error);

    // PID输出
    pid_pwm_set->fOutput =  pid_pwm_set->fProportion + pid_pwm_set->fIntegral + pid_pwm_set->fDerivative;

    // 更新上一次误差
    pid_pwm_set->fPre_error = pid_pwm_set->fError;

    return;
}

