#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__
#include "main.h"





extern void Morot_DIR_PWM_Ctrl(float Percentage );
extern void Morot_DIR_PWM_Ctrl_Direct(float Percentage );
extern void Load_Ctrl(float Percentage );
extern void SetMotorSpeed_AccDec(int32_t Spe);
#endif


