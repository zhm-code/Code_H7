#ifndef __APP_GB5745X_H__
#define __APP_GB5745X_H__

#include "main.h"




extern uint8_t Tog_Flag;

extern void vAPP_RunOpenLoop_GB5745X(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);
extern void vAPP_SetTestFinish_GB5745X(void);
extern void vAPP_SetLoad_GB5745X(uint32_t RunTime_ms, uint16_t usSetLoad);
extern void vAPP_DrivePowerOn_GB5745X(uint32_t RunTime_ms, uint16_t Set_Volt);
extern void vAPP_SetMotorStop_GB5745X(uint32_t RunTime_ms,uint16_t Set_Volt);
//¶ÁÈ¡ÎÂ¶È
extern void vAPP_ReadTmp_GB5745X(uint32_t RunTime_ms,uint16_t Set_Volt);
#endif

