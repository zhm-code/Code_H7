#ifndef __DEBUGER_H
#define __DEBUGER_H
#include "main.h"
#include "stdio.h"
#include "string.h"

#define LCDShowFlowStrSubMax 20
#define ErrStrLines 20
#define ErrStrBuffSub 20

extern char cErrStrBuff[ErrStrLines][ErrStrBuffSub];
void vAPP_AddErr(const char *p);
void vApp_ShowStringFlow(char *OutPutStr,
                         uint8_t ucOutLenLimt,
                         char *InputStr,
                         uint8_t ucInputLen,
                         uint16_t usFlowSpeedms);

#endif