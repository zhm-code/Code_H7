/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//BSP
#include "bsp_file_operate.h"
#include "bsp_lcd.h"
#include "bsp_ENC.h"
#include "bsp_uart.h"
#include "bsp_delay.h"
#include "bsp_ADC.h"
#include "bsp_key.h"
#include "bsp_ADC.h"
#include "bsp_pwm.h"
#include "bsp_motor.h"
#include "bsp_cpu_flash.h"
#include "bsp_can.h"
#include "bsp_SpeedCapture.h"
#include "bsp_DAC.h"
#include "app_SendData.h"

//MX
#include "dac.h"
#include "tim.h"


//APP
#include "app_main.h"
#include "app_uart8.h"
#include "app_uart7.h"
#include "app_uart3.h"
#include "app_uart5.h"
#include "app_user_main.h"
#include "app_sysparam.h"
#include "app_GB1402S.h"

#include "app_XR_API.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */





/* USER CODE END Variables */
/* Definitions for Send_Data */
osThreadId_t Send_DataHandle;
const osThreadAttr_t Send_Data_attributes = {
  .name = "Send_Data",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for DEBUG */
osThreadId_t DEBUGHandle;
const osThreadAttr_t DEBUG_attributes = {
  .name = "DEBUG",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Serial */
osThreadId_t SerialHandle;
const osThreadAttr_t Serial_attributes = {
  .name = "Serial",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Motor */
osThreadId_t MotorHandle;
const osThreadAttr_t Motor_attributes = {
  .name = "Motor",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Read_Loop */
osThreadId_t Read_LoopHandle;
const osThreadAttr_t Read_Loop_attributes = {
  .name = "Read_Loop",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void vSend_Data_Task(void *argument);
void DEBUG_TASK(void *argument);
void Serial_TASK(void *argument);
void Motor_TASK(void *argument);
void Read_Loop_TASK(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
	//vApplicationTickHook 每1ms调用一次
	
	
	//系统计时
	ullSYS_time_Count ++;
	
	
}
/* USER CODE END 3 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Send_Data */
  Send_DataHandle = osThreadNew(vSend_Data_Task, NULL, &Send_Data_attributes);

  /* creation of DEBUG */
  DEBUGHandle = osThreadNew(DEBUG_TASK, NULL, &DEBUG_attributes);

  /* creation of Serial */
  SerialHandle = osThreadNew(Serial_TASK, NULL, &Serial_attributes);

  /* creation of Motor */
  MotorHandle = osThreadNew(Motor_TASK, NULL, &Motor_attributes);

  /* creation of Read_Loop */
  Read_LoopHandle = osThreadNew(Read_Loop_TASK, NULL, &Read_Loop_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_vSend_Data_Task */
/**
  * @brief  Function implementing the Send_Data thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_vSend_Data_Task */
void vSend_Data_Task(void *argument)
{
  /* USER CODE BEGIN vSend_Data_Task */
  /* Infinite loop */

//  motor_disabled();
//  osDelay(100);
//  motor_enabled();
//  osDelay(100);
//  motor_set_mode(0x06);
//  osDelay(100);
//  motor_set_torque(4.0f);





  for(;;)
  {
    vSystem_Send_Loop(&stSys_send_Params);

    osDelay(1);
  }
  /* USER CODE END vSend_Data_Task */
}

/* USER CODE BEGIN Header_DEBUG_TASK */
/**
* @brief Function implementing the DEBUG thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DEBUG_TASK */
void DEBUG_TASK(void *argument)
{
  /* USER CODE BEGIN DEBUG_TASK */
  /* Infinite loop */
	//此数组只为打印任务信息，具体大小可以合理分配
  static char InfoBuffer[512] = {0};
	
  for(;;)
  {

//	vTaskList((char *) &InfoBuffer);
//	printf("\r\nName\t\tState\tPriority Residue\tNumber\r\n");
//	printf("\r\n%s\r\n", InfoBuffer);
//	  
//	  printf("\r\n%llu\r\n", ullBsp_TimeBase_ms());
	osDelay(1000);

  }
  /* USER CODE END DEBUG_TASK */
}

/* USER CODE BEGIN Header_Serial_TASK */
/**
* @brief Function implementing the Serial thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Serial_TASK */
void Serial_TASK(void *argument)
{
  /* USER CODE BEGIN Serial_TASK */
  /* Infinite loop */

//Modbus初始化

  vAPP_Modbus_Init();
  for(;;)
  {
	U8_Receiving_Process();	//Modbus控制 USART8
	U7_Receiving_Process(); //PPS
	U5_Receiving_Process();
	U3_Receiving_Process();
	  
//	FDCAN_CTRL_DriverBoard_TX();
	  

    osDelay(5);
  }
  /* USER CODE END Serial_TASK */
}

/* USER CODE BEGIN Header_Motor_TASK */
/**
* @brief Function implementing the Motor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Motor_TASK */
void Motor_TASK(void *argument)
{
  /* USER CODE BEGIN Motor_TASK */
  /* Infinite loop */

	
  for(;;)
  {
	 vApp_UserLoop();       //总任务调度
	  Spe_Calculation();      //速度计算

    osDelay(5);
  }
  /* USER CODE END Motor_TASK */
}

/* USER CODE BEGIN Header_Read_Loop_TASK */
/**
* @brief Function implementing the Read_Loop thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Read_Loop_TASK */
void Read_Loop_TASK(void *argument)
{
  /* USER CODE BEGIN Read_Loop_TASK */
  /* Infinite loop */

  for(;;)
  {
//	  vADC1_Process(); //ADC处理
//	  vADC2_Process(); //ADC处理
//	uUser_ID = uCode_Dipper_ID();			//拨码器ID读取
	  
//	  U5_RS485_Modbus_Communication();
//	  motor_get_state();
    osDelay(100);
  }
  /* USER CODE END Read_Loop_TASK */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

