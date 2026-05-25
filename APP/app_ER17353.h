#ifndef __APP_ER17353_H__
#define __APP_ER17353_H__

#include "main.h"

extern uint8_t Tog_Flag;

extern void vAPP_RunOpenLoop_ER17353(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);
extern void vAPP_SetTestFinish_ER17353(void);
extern void vAPP_SetLoad_ER17353(uint32_t RunTime_ms, uint16_t usSetLoad);
extern void vAPP_DrivePowerOn_ER17353(uint32_t RunTime_ms,uint16_t Set_Volt);
#endif

