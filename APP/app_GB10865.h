#ifndef __APP_GB10865_H__
#define __APP_GB10865_H__

#include "main.h"

extern uint8_t Tog_Flag;

extern void vAPP_RunOpenLoop_GB10865(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);
extern void vAPP_SetTestFinish_GB10865(void);
extern void vAPP_SetLoad_GB10865(uint32_t RunTime_ms, uint16_t usSetLoad);
extern void vAPP_DrivePowerOn_GB10865(uint32_t RunTime_ms,uint16_t Set_Volt);
extern void vAPP_EncoderIdentification_GB10865(uint32_t RunTime_ms);
extern void vAPP_SetCylinder_GB10865(uint32_t RunTime_ms, uint16_t usSetCyl);
#endif

