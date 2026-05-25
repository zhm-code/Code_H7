#include "bsp_ENC.h"
#include "tim.h"
#include "app_sysparam.h"
#include "bsp_SpeedCapture.h"

ENC_t xKnob_ENC;
ENC_t xMotor_ENC;


/** 
*@brief 初始化配置
*	
*/
void ENC_Init(void )
{
	HAL_TIM_Encoder_Start(&htim5,TIM_CHANNEL_ALL); //开启编码器
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL); 
}


/** 
*@brief 读取脉冲次数
*			
*	
*
* @return  脉冲次数
*/

void Get_ENC_PulsesCount(void)
{
	xMotor_ENC.ENC_Pulses_Count=(short)(__HAL_TIM_GET_COUNTER(&htim2)); //得到脉冲次数
	xMotor_ENC.DIR_Flag = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2,0);//清零定时器计数
}

DIR_VAL Get_ENC_DIR(ENC_t * ENC, TIM_HandleTypeDef * htim)
{
	DIR_VAL dir = NONDIR;
	
	ENC->DIR_Flag = __HAL_TIM_IS_TIM_COUNTING_DOWN(htim);	//读取方向
	ENC->ENC_Pulses_Count  = (short)(__HAL_TIM_GET_COUNTER(htim)); //得到脉冲次数
	
	if(ENC->ENC_Pulses_Count > 0) //大于零说明旋转编码器转动
	{
		if(ENC->ENC_Pulses_Count == 1)
		{
			dir = RIGHT;
			
		}
		else
		{
			dir = LEFT;
		}
		
	}
	__HAL_TIM_SET_COUNTER(htim,0);//清零定时器计数
	
	return dir;
}




