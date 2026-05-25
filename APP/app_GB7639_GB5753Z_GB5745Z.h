#ifndef __APP_GB7639_GB5753Z_GB5745Z_H__
#define __APP_GB7639_GB5753Z_GB5745Z_H__


#include "main.h"



extern void vAPP_RunOpenLoop_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms,
									  uint8_t ucSetMotorRotation,
									  uint16_t Set_Volt);

extern void vAPP_SetTestFinish_GB7639_GB5753Z_GB5745Z(void);

extern void vAPP_SetLoad_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms, uint16_t usSetLoad);

extern void vAPP_SetMotorStop_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms,uint16_t Set_Volt);

extern void vAPP_DrivePowerOn_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms, uint16_t Set_Volt);

extern void vAPP_motor_set_mode_GB7639_GB5753Z_GB5745Z(uint32_t time, uint16_t set_mode);

extern void vAPP_motor_enabled_GB7639_GB5753Z_GB5745Z(uint32_t time);

extern void vAPP_motor_disabled_GB7639_GB5753Z_GB5745Z(uint32_t time);

extern void vAPP_ReadTmp_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms,uint16_t Set_Volt);

extern void vAPP_SetCylinder_GB6816(uint32_t RunTime_ms, uint16_t usSetCyl);
#endif





