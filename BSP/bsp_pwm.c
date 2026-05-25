#include "bsp_pwm.h"

void PWM_Init(void )
{
//	HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_2);
//	HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
//	HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
//	
//	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
	
	//备用PWM
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	
	//电机控制PWM
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	
}

void PWM_ALL_SET(float Percentage)
{
//	HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_2);
//	HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_3);
//	HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_4);
////	
//	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_2);

	//备用PWM
	PWM_SetOutput(htim4, TIM_CHANNEL_2, Percentage);
	PWM_SetOutput(htim4, TIM_CHANNEL_3, Percentage);
	PWM_SetOutput(htim4, TIM_CHANNEL_4, Percentage);
	
	//电机控制PWM
	PWM_SetOutput(htim3, TIM_CHANNEL_1, Percentage);
	PWM_SetOutput(htim3, TIM_CHANNEL_2, Percentage);
	
}

/*
功能：设置PWM百分比
参数：
	htimx 定时器
	TIM_Channel 通道
	Percentage 百分比
*/
void PWM_SetOutput(TIM_HandleTypeDef htimx, unsigned int TIM_Channel, float Percentage)
{
	uint32_t set_CCR1_Value = 0;
	uint32_t ARR_value = 0;
//    if(Once_Flag == RESET)
//    {    
//		ARR_value = __HAL_TIM_GET_AUTORELOAD(&htimx); //在运行时获取TIM自动加载寄存器值
//		Once_Flag = SET;
//    }    
    
    //计算逻辑
    set_CCR1_Value = 65535 * Percentage / 100.0f; 
    __HAL_TIM_SET_COMPARE(&htimx, TIM_Channel, set_CCR1_Value);
}
/*
功能：设置PWM 周期
参数：
	htimx 定时器
	ms 设置周期为多少秒 

注意： 如果ms为零则会使用默认设置 且不能超65.535
*/
uint8_t Once_Flag;
void PWM_SET_Period(TIM_HandleTypeDef htimx, uint32_t ms)
{
	uint32_t set_ARR_value = 0;
	if(ms > 0 && ms <=  65)
	{
		Once_Flag = RESET; //使下一次能重新获取自动加载寄存器值
		
		//计算公式
		set_ARR_value = ms * 1000;
		
		//设置最新的ARR值
		__HAL_TIM_SET_AUTORELOAD(&htimx, set_ARR_value);
	}
}

