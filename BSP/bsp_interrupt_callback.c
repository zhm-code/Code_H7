#include "bsp_interrupt_callback.h"
#include "bsp_SpeedCapture.h"
#include "bsp_delay.h"
#include "time.h"
#include "bsp_ADC.h"
#include "adc.h"
#include "fdcan.h"
#include "app_GB1402S.h"
#include "app_uart5.h"
#include "app_sysparam.h"

#define IS_PE1 ( HAL_GPIO_ReadPin(PE1_GPIO_Port,PE1_Pin) )
#define IS_PE2 ( HAL_GPIO_ReadPin(PE2_GPIO_Port,PE2_Pin) )

extern DMA_HandleTypeDef hdma_adc1;

uint16_t pulse_count_1;
uint16_t pulse_count_2;

//外部中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
		case GPIO_PIN_5:
    	{
			if(HAL_GPIO_ReadPin(PESwitch_A_T15_2_GPIO_Port, PESwitch_A_T15_2_Pin) == GPIO_PIN_RESET)
			{
				uSysROParam.cMotorRotationIn = -1;
			}
			else
			{
				uSysROParam.cMotorRotationIn = 1;
			}
    	}
    	break;
		case GPIO_PIN_2:
    	{
			if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET)
			{
				uSysROParam.cFCT_MotorRotation = -1;
			}
			else
			{
				uSysROParam.cFCT_MotorRotation = 1;
			}
    	}
    	break;
		
		case GPIO_PIN_4:
    	{
			if(HAL_GPIO_ReadPin(DI_24V_2_GPIO_Port, DI_24V_2_Pin) == GPIO_PIN_RESET)
			{
				uSysROParam.cMotorRotationIn = 1;
			}
			else
			{
				uSysROParam.cMotorRotationIn = -1;
			}
    	}
    	break;
		
    	default:
    	{
    		
    	}
    	break;
    }
}


//PWM反转回调
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

}



//ADC完成回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if(hadc == &hadc1)
    {
        HAL_ADC_Stop_DMA(&hadc1); 		//停掉DMA传输，等待数据处理
        ADC1_DMA_TX_Finish_Flag = SET; 	//完成标志置一，转到任务函数处理
    }

    if(hadc == &hadc2)
    {
        HAL_ADC_Stop_DMA(&hadc2); 		//停掉DMA传输，等待数据处理
        ADC2_DMA_TX_Finish_Flag = SET; 	//完成标志置一，转到任务函数处理
    }

}


void HAL_FDCAN_RxBufferNewMessageCallback(FDCAN_HandleTypeDef *hfdcan)
{
    if (hfdcan == &hfdcan1)
    {

    }

    if (hfdcan == &hfdcan2)
    {


    }
}

//------------------------------------重写捕获完成中断回调函数------------------------------------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if(&htim2 == htim)
	{
		if(FG_SpeedCapture.Data_Busy_Flag == RESET)//判断数据是否忙碌
		{
			
			switch(FG_SpeedCapture.Cap_Step) //捕获步骤判断
			{
				case 0: //等待捕获第一个上升沿
				{
					//定时器计数清零
					__HAL_TIM_SET_COUNTER(&htim2,0);
					FG_SpeedCapture.Cap_Step ++;//捕获成功
					break;
					
				}
					
					
				case 1:
				{
					//记录周期的定时器计数 每个计数等于1us
					FG_SpeedCapture.Cycle_Tim_Count = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
					//复位捕获步骤等待下次处理
					FG_SpeedCapture.Cap_Step = RESET;
					//数据已经捕获完成等待处理    
					FG_SpeedCapture.Data_Busy_Flag = SET;
					
					break;
				}
					
					
			}
		}
		
	}
	
	if(&htim15 == htim)
	{
		if(PE_SpeedCapture.Data_Busy_Flag == RESET)//判断数据是否忙碌
		{
			
			switch(PE_SpeedCapture.Cap_Step) //捕获步骤判断
			{
				case 0: //等待捕获第一个上升沿
				{
					//定时器计数清零
					__HAL_TIM_SET_COUNTER(&htim15,0);
					PE_SpeedCapture.Cap_Step ++;//捕获成功
					break;
					
				}
					
					
				case 1:
				{
					//记录周期的定时器计数 每个计数等于1us
					PE_SpeedCapture.Cycle_Tim_Count = HAL_TIM_ReadCapturedValue(&htim15, TIM_CHANNEL_2);
					//复位捕获步骤等待下次处理
					PE_SpeedCapture.Cap_Step = RESET;
					//数据已经捕获完成等待处理    
					PE_SpeedCapture.Data_Busy_Flag = SET;
					
					break;
				}
					
					
			}
		}
		
	}
	
}
