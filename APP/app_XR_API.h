#ifndef __APP_XR_API_H__
#define __APP_XR_API_H__

#include "main.h"


typedef struct
{
	int16_t 	motor_temperature; 		// 电机温度
	int16_t 	mos_temperature; 		// mos温度
	uint16_t 	voltage_bus; 			// 母线电压 mV 
	uint16_t 	rsv; 
	
}motor_status2_t;

extern motor_status2_t motor_status2;


//使能
void motor_enabled(void );
void motor_disabled(void );

//设置模式
void motor_set_mode(uint8_t mode );
void motor_set_id(uint8_t id );
void motor_calibration(void );
void motor_set_pos(float pos);
//设置扭矩
void motor_set_torque(float torque );
void motor_set_speed(float speed );
void motor_get_state(void );
void XR_data_processing(void );




#endif
