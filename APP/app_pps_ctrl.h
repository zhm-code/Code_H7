#ifndef __APP_PPS_CTRL_H
#define __APP_PPS_CTRL_H
#include "main.h"


void SetProgramPowerSupplyOn(uint8_t  isSetOn);
void Control_ProgramPowerSupply_TX(void);
void SetProgramPowerSupply_volt(uint16_t  Value);
void SetProgramPowerSupply_curr(uint16_t  Value);
void ReadProgramPowerSupplyVoltCurr(void);

extern uint16_t TickForPPS_Send;
#endif