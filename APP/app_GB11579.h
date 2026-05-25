#ifndef __APP_GB11579_H__
#define __APP_GB11579_H__

#include "main.h"
extern void vAPP_RunOpenLoop_GB11579(uint32_t RunTime_ms,
									  uint8_t ucSetMotorRotation,
									  uint16_t Set_Volt);//
extern void vAPP_SetTestFinish_GB11579(void);
extern void vAPP_SetLoad_GB11579(uint32_t RunTime_ms, uint16_t usSetLoad);//
extern void vAPP_SetMotorStop_GB11579(uint32_t RunTime_ms,uint16_t Set_Volt);//
extern void vAPP_DrivePowerOn_GB11579(uint32_t RunTime_ms, uint16_t Set_Volt);//
extern void vAPP_motor_set_mode_GB11579(uint32_t time, uint16_t set_mode);
extern void vAPP_SetCylinder_GB11579(uint32_t RunTime_ms, uint16_t usSetCyl);//
#endif

