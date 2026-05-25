#ifndef __APP_UART5_H__
#define __APP_UART5_H__

#include "bsp_uart.h"
#include "app_uart5.h"


typedef struct  
{
	__IO float voltage1;				//ºÏ≤‚π‚’§1µÁ—π
	__IO float voltage2;				//ºÏ≤‚π‚’§2µÁ—π
	__IO float distance1;			//æ‡¿Î1£∫…Ó∂»«ÂΩ‡æ‡¿Î
	__IO float distance2;			//æ‡¿Î2£∫œ¬Ωµæ‡¿Î
	
	int16_t  Motor_Temp;
	int16_t  Motor_CurA;
	int16_t  Motor_CurB;
	int16_t  Motor_CurC;
	int16_t  Motor_RPM;
	uint16_t Motor_Vol;
	uint16_t Motor_SR;
	uint16_t Motor_ESR;
	uint16_t Motor_Type;
	
	float Torque_Sensor_Torque;
	float Torque_Sensor_RPM;

	
}U5_MSG_t;





extern U5_MSG_t U5_MSG;

extern void U5_Receiving_Process(void);
extern void U5_RS485_Modbus_Send(uint8_t * msg, uint8_t len);
extern void U5_RS485_Modbus_Communication(void);
#endif
