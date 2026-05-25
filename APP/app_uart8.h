#ifndef __APP_UART8_H__
#define __APP_UART8_H__

#include "bsp_uart.h"
#include "app_uart5.h"

/* -------------------------start modbus use-----------------------------------*/
/* RTU通信需要确定超时时间 */
#if MX_RS485_UARTx_BAUDRATE <= 19200
  /* 1.5个字符的超时时间 */
  #define OVERTIME_15CHAR             ((float)(200000000/(GENERAL_TIM_PRESCALER+1))*11*1.5f/MX_RS485_UARTx_BAUDRATE)
  /* 3个字符的超时时间 */
  #define OVERTIME_35CHAR             ((float)(200000000/(GENERAL_TIM_PRESCALER+1))*11*3.5f/MX_RS485_UARTx_BAUDRATE)
#else 
  /* 波特率超过19200bit/s的情况下建议的超时时间 */
  #define OVERTIME_15CHAR                750.0f    // 750us 
  #define OVERTIME_35CHAR               1750.0f  // 1.75ms  
  
#endif
  
/* 函数声明 ------------------------------------------------------------------*/
extern void UART_Tx(uint8_t *Tx_Buf,uint16_t TxCount);
extern void vAPP_Modbus_Init(void);
  
extern void FillBuf(uint8_t* buf, uint8_t Code);
extern void vApp_Modbus_Loop(void);
extern void vPPS_DataProcess(void);
  
//Json数据处理
static void JsonProcess(void);




/* ----------------end modbus use--------------------------------*/

extern void U8_Receiving_Process(void);

#endif
