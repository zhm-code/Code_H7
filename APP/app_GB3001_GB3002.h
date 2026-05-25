#ifndef __APP_GB3001_GB3002_H__
#define __APP_GB3001_GB3002_H__

#include "main.h"
extern void vAPP_RunOpenLoop_GB3001_GB3002(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);
extern void vAPP_SetTestFinish_GB3001_GB3002(void);
extern void vAPP_SetLoad_GB3001_GB3002(uint32_t RunTime_ms, uint16_t usSetLoad);
extern void vAPP_SetCylinder_GB3001_GB3002(uint32_t RunTime_ms, uint16_t usSetCyl);
#endif

