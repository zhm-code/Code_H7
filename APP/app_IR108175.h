#ifndef __APP_IR108175_H__
#define __APP_IR108175_H__

#include "main.h"


static uint8_t Command_Query[6] 	= {0x02, 0x01, 0x04, 0x40, 0x84, 0x03};
static uint8_t Set_CW2000RPM[9] 	= {0x02, 0x04, 0x08, 0x07, 0xD0, 0x00, 0x15, 0x74, 0x03};
static uint8_t Set_CCW2000RPM[9] 	= {0x02, 0x04, 0x08, 0xF8, 0x30, 0x00, 0xCA, 0xA5, 0x03};
static uint8_t Set_0RPM[9] 			= {0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x85, 0xC3, 0x03};

extern void vAPP_RunOpenLoop_IR108175(uint32_t RunTime_ms,
									  uint8_t ucSetMotorRotation,
									  uint16_t Set_Volt);

extern void vAPP_SetTestFinish_IR108175(void);

extern void vAPP_SetLoad_IR108175(uint32_t RunTime_ms, uint16_t usSetLoad);

extern void vAPP_SetMotorStop_IR108175(uint32_t RunTime_ms,uint16_t Set_Volt);

extern void vAPP_DrivePowerOn_IR108175(uint32_t RunTime_ms, uint16_t Set_Volt);
#endif

