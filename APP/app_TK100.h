#ifndef __APP_TK100_H__
#define __APP_TK100_H__

#include "main.h"

extern uint8_t Tog_Flag;

extern void vAPP_RunOpenLoop_TK100(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);
extern void vAPP_SetTestFinish_TK100(void);
extern void vAPP_SetLoad_TK100(uint32_t RunTime_ms, uint16_t usSetLoad);
extern void vAPP_SetMotorStop_TK100(uint32_t RunTime_ms,uint16_t Set_Volt);

#endif

