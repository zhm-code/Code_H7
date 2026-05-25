/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PC14-OSC32_IN (OSC32_IN)   ------> RCC_OSC32_IN
     PC15-OSC32_OUT (OSC32_OUT)   ------> RCC_OSC32_OUT
     PH0-OSC_IN (PH0)   ------> RCC_OSC_IN
     PH1-OSC_OUT (PH1)   ------> RCC_OSC_OUT
     PA13 (JTMS/SWDIO)   ------> DEBUG_JTMS-SWDIO
     PA14 (JTCK/SWCLK)   ------> DEBUG_JTCK-SWCLK
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, AIR_Cylinder_CTRL_Pin|DO_5V_3PIN_1_Pin|DO_5V_3PIN_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, ICS_POWER_CTRL_Pin|DO_24V_3PIN_1_Pin|DO_24V_3PIN_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, DO_24V_1_Pin|DO_24V_2_Pin|DO_24V_3_Pin|DO_24V_4_Pin
                          |DO_24V_5_Pin|DO_24V_6_Pin|DO_24V_7_Pin|DO_24V_8_Pin
                          |LED_RED_Pin|LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, IIC_CSL_Pin|IIC_SDA_Pin|Rotary_Knob_MID_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LCD_BL_Pin|T_MOSI_Pin|T_CLK_Pin|T_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, USB_POWER_CTRL_Pin|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZ_POWER_GPIO_Port, BUZ_POWER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DI_24V_5_Pin DI_24V_6_Pin DI_24V_7_Pin */
  GPIO_InitStruct.Pin = DI_24V_5_Pin|DI_24V_6_Pin|DI_24V_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : DI_24V_8_Pin */
  GPIO_InitStruct.Pin = DI_24V_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(DI_24V_8_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AIR_Cylinder_CTRL_Pin */
  GPIO_InitStruct.Pin = AIR_Cylinder_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AIR_Cylinder_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ICS_POWER_CTRL_Pin */
  GPIO_InitStruct.Pin = ICS_POWER_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ICS_POWER_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DO_24V_1_Pin */
  GPIO_InitStruct.Pin = DO_24V_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DO_24V_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DO_24V_2_Pin DO_24V_3_Pin DO_24V_4_Pin DO_24V_5_Pin
                           DO_24V_6_Pin DO_24V_7_Pin DO_24V_8_Pin LED_RED_Pin
                           LED_GREEN_Pin */
  GPIO_InitStruct.Pin = DO_24V_2_Pin|DO_24V_3_Pin|DO_24V_4_Pin|DO_24V_5_Pin
                          |DO_24V_6_Pin|DO_24V_7_Pin|DO_24V_8_Pin|LED_RED_Pin
                          |LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : Start_Button_Pin */
  GPIO_InitStruct.Pin = Start_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Start_Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DO_24V_3PIN_1_Pin DO_24V_3PIN_2_Pin */
  GPIO_InitStruct.Pin = DO_24V_3PIN_1_Pin|DO_24V_3PIN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : DI_24V_3PIN_1_Pin */
  GPIO_InitStruct.Pin = DI_24V_3PIN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DI_24V_3PIN_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DI_24V_3PIN_2_Pin */
  GPIO_InitStruct.Pin = DI_24V_3PIN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DI_24V_3PIN_2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_EX2_Pin */
  GPIO_InitStruct.Pin = IN_EX2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN_EX2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IIC_CSL_Pin */
  GPIO_InitStruct.Pin = IIC_CSL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(IIC_CSL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IIC_SDA_Pin */
  GPIO_InitStruct.Pin = IIC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DialSW_B0_Pin DialSW_B1_Pin */
  GPIO_InitStruct.Pin = DialSW_B0_Pin|DialSW_B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : DialSW_B2_Pin DialSW_B3_Pin T_PEN_Pin T_MISO_Pin
                           KEY_MID_Pin KEY_RHT_Pin KEY_LET_Pin KEY_DWN_Pin
                           KEY_UP_Pin */
  GPIO_InitStruct.Pin = DialSW_B2_Pin|DialSW_B3_Pin|T_PEN_Pin|T_MISO_Pin
                          |KEY_MID_Pin|KEY_RHT_Pin|KEY_LET_Pin|KEY_DWN_Pin
                          |KEY_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : Rotary_Knob_MID_Pin */
  GPIO_InitStruct.Pin = Rotary_Knob_MID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Rotary_Knob_MID_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : T_MOSI_Pin T_CLK_Pin */
  GPIO_InitStruct.Pin = T_MOSI_Pin|T_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : T_CS_Pin */
  GPIO_InitStruct.Pin = T_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(T_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_POWER_CTRL_Pin */
  GPIO_InitStruct.Pin = USB_POWER_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_POWER_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZ_POWER_Pin */
  GPIO_InitStruct.Pin = BUZ_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZ_POWER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DO_5V_3PIN_1_Pin DO_5V_3PIN_2_Pin */
  GPIO_InitStruct.Pin = DO_5V_3PIN_1_Pin|DO_5V_3PIN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : DI_5V_3PIN_1_Pin DI_5V_3PIN_2_Pin */
  GPIO_InitStruct.Pin = DI_5V_3PIN_1_Pin|DI_5V_3PIN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DI_24V_1_Pin */
  GPIO_InitStruct.Pin = DI_24V_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DI_24V_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DI_24V_2_Pin DI_24V_3_Pin DI_24V_4_Pin */
  GPIO_InitStruct.Pin = DI_24V_2_Pin|DI_24V_3_Pin|DI_24V_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*AnalogSwitch Config */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC2, SYSCFG_SWITCH_PC2_CLOSE);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
