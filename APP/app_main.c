#include "app_main.h"
#include "bsp_ADC.h"
#include "bsp_ENC.h"
#include "bsp_uart.h"
#include "bsp_pwm.h"



#include "adc.h"
#include "dac.h"

__IO uint64_t ullSYS_time_Count = 0U;	//时间戳



/*进RTOS前初始化*/
void app_user_init(void )
{

	
	//ADC初始化
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) ;
	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ulADC1_value, 100) != HAL_OK)
	{
//		printf("ADC_DMA ERR\r\n");
	}
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_DIFFERENTIAL_ENDED) ;
	if(HAL_ADC_Start_DMA(&hadc2, (uint32_t *)ulADC2_value, 100) != HAL_OK)
	{
//		printf("ADC_DMA ERR\r\n");
	}
	
	//编码器初始化
//	ENC_Init();
	
	HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	
	//PWM初始化
	PWM_ALL_SET(PWM_0);
	PWM_Init();

	
	//串口初始化
	Ux_ALL_Init();
	
	//开启DAC
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	
//	USB_POWER_CTRL(CTRL_OPEN); 	//给USB上电
	
	
	
	DO_24V_1_CTRL(CTRL_CLOSE);
	DO_24V_2_CTRL(CTRL_CLOSE);
	DO_24V_3_CTRL(CTRL_CLOSE);
	DO_24V_4_CTRL(CTRL_CLOSE);
	DO_24V_5_CTRL(CTRL_CLOSE);
	DO_24V_6_CTRL(CTRL_CLOSE);
	DO_24V_7_CTRL(CTRL_CLOSE);
	DO_24V_8_CTRL(CTRL_CLOSE);

}




