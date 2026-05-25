/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim15;

extern __IO uint64_t ullSYS_time_Count;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DI_24V_5_Pin GPIO_PIN_2
#define DI_24V_5_GPIO_Port GPIOE
#define DI_24V_6_Pin GPIO_PIN_3
#define DI_24V_6_GPIO_Port GPIOE
#define DI_24V_7_Pin GPIO_PIN_4
#define DI_24V_7_GPIO_Port GPIOE
#define DI_24V_8_Pin GPIO_PIN_5
#define DI_24V_8_GPIO_Port GPIOE
#define DI_24V_8_EXTI_IRQn EXTI9_5_IRQn
#define PESwitch_A_T15_2_Pin GPIO_PIN_6
#define PESwitch_A_T15_2_GPIO_Port GPIOE
#define AIR_Cylinder_CTRL_Pin GPIO_PIN_8
#define AIR_Cylinder_CTRL_GPIO_Port GPIOI
#define ICS_POWER_CTRL_Pin GPIO_PIN_13
#define ICS_POWER_CTRL_GPIO_Port GPIOC
#define DO_24V_1_Pin GPIO_PIN_0
#define DO_24V_1_GPIO_Port GPIOF
#define DO_24V_2_Pin GPIO_PIN_1
#define DO_24V_2_GPIO_Port GPIOF
#define DO_24V_3_Pin GPIO_PIN_2
#define DO_24V_3_GPIO_Port GPIOF
#define DO_24V_4_Pin GPIO_PIN_3
#define DO_24V_4_GPIO_Port GPIOF
#define DO_24V_5_Pin GPIO_PIN_4
#define DO_24V_5_GPIO_Port GPIOF
#define DO_24V_6_Pin GPIO_PIN_5
#define DO_24V_6_GPIO_Port GPIOF
#define DO_24V_7_Pin GPIO_PIN_6
#define DO_24V_7_GPIO_Port GPIOF
#define DO_24V_8_Pin GPIO_PIN_7
#define DO_24V_8_GPIO_Port GPIOF
#define LED_RED_Pin GPIO_PIN_8
#define LED_RED_GPIO_Port GPIOF
#define LED_GREEN_Pin GPIO_PIN_9
#define LED_GREEN_GPIO_Port GPIOF
#define Start_Button_Pin GPIO_PIN_10
#define Start_Button_GPIO_Port GPIOF
#define DO_24V_3PIN_1_Pin GPIO_PIN_0
#define DO_24V_3PIN_1_GPIO_Port GPIOC
#define DI_24V_3PIN_1_Pin GPIO_PIN_1
#define DI_24V_3PIN_1_GPIO_Port GPIOC
#define DO_24V_3PIN_2_Pin GPIO_PIN_2
#define DO_24V_3PIN_2_GPIO_Port GPIOC
#define DI_24V_3PIN_2_Pin GPIO_PIN_3
#define DI_24V_3PIN_2_GPIO_Port GPIOC
#define IN_EX2_Pin GPIO_PIN_2
#define IN_EX2_GPIO_Port GPIOA
#define IN_EX2_EXTI_IRQn EXTI2_IRQn
#define IIC_CSL_Pin GPIO_PIN_4
#define IIC_CSL_GPIO_Port GPIOH
#define IIC_SDA_Pin GPIO_PIN_5
#define IIC_SDA_GPIO_Port GPIOH
#define ADC_VOL_0_10_Pin GPIO_PIN_5
#define ADC_VOL_0_10_GPIO_Port GPIOA
#define PWM_MOT_CTRL_T3C1_Pin GPIO_PIN_6
#define PWM_MOT_CTRL_T3C1_GPIO_Port GPIOA
#define PWM_MOT_CTRL_T3C2_Pin GPIO_PIN_7
#define PWM_MOT_CTRL_T3C2_GPIO_Port GPIOA
#define ADC_HO1_MOT_VOL_A1C4_INP_Pin GPIO_PIN_4
#define ADC_HO1_MOT_VOL_A1C4_INP_GPIO_Port GPIOC
#define ADC_HO1_MOT_VOL_A1C4_INN_Pin GPIO_PIN_5
#define ADC_HO1_MOT_VOL_A1C4_INN_GPIO_Port GPIOC
#define ADC_MOT_CUR_A1C9_Pin GPIO_PIN_0
#define ADC_MOT_CUR_A1C9_GPIO_Port GPIOB
#define PWM_ICS_CTRL_Pin GPIO_PIN_1
#define PWM_ICS_CTRL_GPIO_Port GPIOB
#define DialSW_B0_Pin GPIO_PIN_14
#define DialSW_B0_GPIO_Port GPIOF
#define DialSW_B1_Pin GPIO_PIN_15
#define DialSW_B1_GPIO_Port GPIOF
#define DialSW_B2_Pin GPIO_PIN_0
#define DialSW_B2_GPIO_Port GPIOG
#define DialSW_B3_Pin GPIO_PIN_1
#define DialSW_B3_GPIO_Port GPIOG
#define Rotary_Knob_MID_Pin GPIO_PIN_9
#define Rotary_Knob_MID_GPIO_Port GPIOH
#define Rotary_Knob_ENC_A_T5C1_Pin GPIO_PIN_10
#define Rotary_Knob_ENC_A_T5C1_GPIO_Port GPIOH
#define Rotary_Knob_ENC_B_T5C2_Pin GPIO_PIN_11
#define Rotary_Knob_ENC_B_T5C2_GPIO_Port GPIOH
#define LCD_BL_Pin GPIO_PIN_2
#define LCD_BL_GPIO_Port GPIOG
#define T_PEN_Pin GPIO_PIN_3
#define T_PEN_GPIO_Port GPIOG
#define T_MISO_Pin GPIO_PIN_4
#define T_MISO_GPIO_Port GPIOG
#define T_MOSI_Pin GPIO_PIN_5
#define T_MOSI_GPIO_Port GPIOG
#define T_CLK_Pin GPIO_PIN_6
#define T_CLK_GPIO_Port GPIOG
#define T_CS_Pin GPIO_PIN_7
#define T_CS_GPIO_Port GPIOG
#define USB_POWER_CTRL_Pin GPIO_PIN_8
#define USB_POWER_CTRL_GPIO_Port GPIOA
#define BUZ_POWER_Pin GPIO_PIN_15
#define BUZ_POWER_GPIO_Port GPIOH
#define DO_5V_3PIN_1_Pin GPIO_PIN_0
#define DO_5V_3PIN_1_GPIO_Port GPIOI
#define DI_5V_3PIN_1_Pin GPIO_PIN_1
#define DI_5V_3PIN_1_GPIO_Port GPIOI
#define DO_5V_3PIN_2_Pin GPIO_PIN_2
#define DO_5V_3PIN_2_GPIO_Port GPIOI
#define DI_5V_3PIN_2_Pin GPIO_PIN_3
#define DI_5V_3PIN_2_GPIO_Port GPIOI
#define KEY_MID_Pin GPIO_PIN_10
#define KEY_MID_GPIO_Port GPIOG
#define KEY_RHT_Pin GPIO_PIN_11
#define KEY_RHT_GPIO_Port GPIOG
#define KEY_LET_Pin GPIO_PIN_12
#define KEY_LET_GPIO_Port GPIOG
#define KEY_DWN_Pin GPIO_PIN_13
#define KEY_DWN_GPIO_Port GPIOG
#define KEY_UP_Pin GPIO_PIN_14
#define KEY_UP_GPIO_Port GPIOG
#define DI_24V_1_Pin GPIO_PIN_4
#define DI_24V_1_GPIO_Port GPIOI
#define DI_24V_1_EXTI_IRQn EXTI4_IRQn
#define DI_24V_2_Pin GPIO_PIN_5
#define DI_24V_2_GPIO_Port GPIOI
#define DI_24V_3_Pin GPIO_PIN_6
#define DI_24V_3_GPIO_Port GPIOI
#define DI_24V_4_Pin GPIO_PIN_7
#define DI_24V_4_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */
//用户宏定义开始

#define 	CTRL_CLOSE	GPIO_PIN_RESET
#define 	CTRL_OPEN	GPIO_PIN_SET
#define  	USER_CLOSE	false
#define    	USER_OPEN	true
/*IO开关*/

//USB
#define USB_POWER_CTRL(x) 				( HAL_GPIO_WritePin(USB_POWER_CTRL_GPIO_Port, USB_POWER_CTRL_Pin, x) )
		
//气缸		
#define AIR_Cylinder_CTRL(x) 			( HAL_GPIO_WritePin( AIR_Cylinder_CTRL_GPIO_Port, AIR_Cylinder_CTRL_Pin, x ) )
		
//恒流源刹车		
#define ICS_POWER_CTRL(x) 				( HAL_GPIO_WritePin( ICS_POWER_CTRL_GPIO_Port, ICS_POWER_CTRL_Pin, x ) )
		
//蜂鸣器		
#define BUZ_POWER_CTRL(x) 				( HAL_GPIO_WritePin( BUZ_POWER_GPIO_Port, BUZ_POWER_Pin, x) )
		
//LCD背光		
//#define LCD_BL_CTRL(x) 					( HAL_GPIO_WritePin( LCD_BL_GPIO_Port, LCD_BL_Pin, x) )
		
//LED		
#define LED_RED_CTRL(x) 				( HAL_GPIO_WritePin( LED_RED_GPIO_Port, LED_RED_Pin, x ) )
#define LED_GREEN_CTRL(x) 				( HAL_GPIO_WritePin( LED_GREEN_GPIO_Port, LED_GREEN_Pin, x ) )
		
//DO		
#define DO_24V_1_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_1_GPIO_Port, DO_24V_1_Pin, x ) )
#define DO_24V_2_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_2_GPIO_Port, DO_24V_2_Pin, x ) )
#define DO_24V_3_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_3_GPIO_Port, DO_24V_3_Pin, x ) )
#define DO_24V_4_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_4_GPIO_Port, DO_24V_4_Pin, x ) )
#define DO_24V_5_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_5_GPIO_Port, DO_24V_5_Pin, x ) )
#define DO_24V_6_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_6_GPIO_Port, DO_24V_6_Pin, x ) )
#define DO_24V_7_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_7_GPIO_Port, DO_24V_7_Pin, x ) )
#define DO_24V_8_CTRL(x) 				( HAL_GPIO_WritePin( DO_24V_8_GPIO_Port, DO_24V_8_Pin, x ) )
#define DO_24V_3PIN_1_CTRL(x) 			( HAL_GPIO_WritePin( DO_24V_3PIN_1_GPIO_Port, DO_24V_3PIN_1_Pin, x ) )
#define DO_24V_3PIN_2_CTRL(x) 			( HAL_GPIO_WritePin( DO_24V_3PIN_2_GPIO_Port, DO_24V_3PIN_2_Pin, x ) )
#define DO_5V_3PIN_1_CTRL(x) 			( HAL_GPIO_WritePin( DO_5V_3PIN_1_GPIO_Port, DO_5V_3PIN_1_Pin, x ) )
#define DO_5V_3PIN_2_CTRL(x) 			( HAL_GPIO_WritePin( DO_5V_3PIN_2_GPIO_Port, DO_5V_3PIN_2_Pin, x ) )



/*IO读取*/

//DI	初始下拉	检测高电平
#define IS_DI_24V_1_Signal() 			( HAL_GPIO_ReadPin( DI_24V_1_GPIO_Port, DI_24V_1_Pin ) ) 					? true : false
#define IS_DI_24V_2_Signal() 			( HAL_GPIO_ReadPin( DI_24V_2_GPIO_Port, DI_24V_2_Pin ) ) 					? true : false
#define IS_DI_24V_3_Signal() 			( HAL_GPIO_ReadPin( DI_24V_3_GPIO_Port, DI_24V_3_Pin ) ) 					? true : false
#define IS_DI_24V_4_Signal() 			( HAL_GPIO_ReadPin( DI_24V_4_GPIO_Port, DI_24V_4_Pin ) ) 					? true : false
#define IS_DI_24V_5_Signal() 			( HAL_GPIO_ReadPin( DI_24V_5_GPIO_Port, DI_24V_5_Pin ) ) 					? true : false
#define IS_DI_24V_6_Signal() 			( HAL_GPIO_ReadPin( DI_24V_6_GPIO_Port, DI_24V_6_Pin ) ) 					? true : false
#define IS_DI_24V_7_Signal() 			( HAL_GPIO_ReadPin( DI_24V_7_GPIO_Port, DI_24V_7_Pin ) ) 					? true : false
#define IS_DI_24V_8_Signal() 			( HAL_GPIO_ReadPin( DI_24V_8_GPIO_Port, DI_24V_8_Pin ) ) 					? true : false

//		初始浮空	检测上升沿	
#define IS_DI_24V_3PIN_1_Signal() 		( HAL_GPIO_ReadPin( DO_24V_3PIN_1_GPIO_Port, DO_24V_3PIN_1_Pin ) )			? true : false
#define IS_DI_24V_3PIN_2_Signal() 		( HAL_GPIO_ReadPin( DO_24V_3PIN_2_GPIO_Port, DO_24V_3PIN_2_Pin ) )  		? true : false
#define IS_DI_5V_3PIN_1_Signal() 		( HAL_GPIO_ReadPin( DO_5V_3PIN_1_GPIO_Port, DO_5V_3PIN_1_Pin ) )    		? true : false
#define IS_DI_5V_3PIN_2_Signal() 		( HAL_GPIO_ReadPin( DO_5V_3PIN_2_GPIO_Port, DO_5V_3PIN_2_Pin ) )    		? true : false
		
//拨码器ID	初始上拉	检测低电平	
#define IS_DialSW_B0_SET() 				( HAL_GPIO_ReadPin( DialSW_B0_GPIO_Port, DialSW_B0_Pin ) ) 					? false : true
#define IS_DialSW_B1_SET() 				( HAL_GPIO_ReadPin( DialSW_B1_GPIO_Port, DialSW_B1_Pin ) ) 					? false : true
#define IS_DialSW_B2_SET() 				( HAL_GPIO_ReadPin( DialSW_B2_GPIO_Port, DialSW_B2_Pin ) ) 					? false : true
#define IS_DialSW_B3_SET() 				( HAL_GPIO_ReadPin( DialSW_B3_GPIO_Port, DialSW_B3_Pin ) ) 					? false : true

//SD检测	初始下拉	检测高电平
//#define IS_SD_INP_SET() 				( HAL_GPIO_ReadPin( SD_INP_GPIO_Port, SD_INP_Pin ) )						? true : false

//摇杆按键	初始上拉	检测低电平
#define IS_KEY_UP_Press() 				( HAL_GPIO_ReadPin( KEY_UP_GPIO_Port, KEY_UP_Pin ) ) 						? false : true
#define IS_KEY_DWN_Press() 				( HAL_GPIO_ReadPin( KEY_DWN_GPIO_Port, KEY_DWN_Pin ) ) 						? false : true
#define IS_KEY_LET_Press() 				( HAL_GPIO_ReadPin( KEY_LET_GPIO_Port, KEY_LET_Pin ) ) 						? false : true
#define IS_KEY_RHT_Press() 				( HAL_GPIO_ReadPin( KEY_RHT_GPIO_Port, KEY_RHT_Pin ) ) 						? false : true
#define IS_KEY_MID_Press() 				( HAL_GPIO_ReadPin( KEY_MID_GPIO_Port, KEY_MID_Pin ) ) 						? false : true

//旋转编码器中键 	初始上拉	检测低电平
#define IS_Rotary_Knob_MID_Press() 		( HAL_GPIO_ReadPin( Rotary_Knob_MID_GPIO_Port, Rotary_Knob_MID_Pin ) )		? false : true
	
//启动按键		初始上拉	检测低电平
#define IS_Start_Button_Press() 		( HAL_GPIO_ReadPin( Start_Button_GPIO_Port, Start_Button_Pin ) )			? false : true
	


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
