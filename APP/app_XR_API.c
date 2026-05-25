#include "app_XR_API.h"
#include "bsp_uart.h"


#include <stdint.h>
#include <string.h>

motor_status2_t motor_status2;


uint32_t float_to_u32(float f) {
    uint32_t u;
    memcpy(&u, &f, sizeof(u));
    return u;
}


//电机使能
void motor_enabled(void )
{
	uint8_t buf[] = {0xa5, 0x03, 0x08, 0x01};  //A5 03 08 01
	U3_DMA_Send(buf, buf[1] + 1);
	
}

//电机使能
void motor_disabled(void )
{
	uint8_t buf[] = {0xa5, 0x03, 0x08, 0x00};  //A5 03 08 00
	U3_DMA_Send(buf, buf[1] + 1);
	
}

//电机设置模式
void motor_set_mode(uint8_t mode )
{
	switch (mode)
	{
		case 0x02:	//位置
		case 0x03:	//速度
		case 0x06:	//扭矩
		{
			uint8_t buf[] = {0xa5, 0x03, 0x02, 0x00};  //A5 03 08 ģʽ
			buf[3] = mode;
			
			U3_DMA_Send(buf, buf[1] + 1);
		}
		break;
	}
}

//���õ��ID
void motor_set_id(uint8_t id )
{
	uint8_t buf[] = {0xa5, 0x03, 0x03, 0x00};  //A5 03 03 id
	buf[3] = id;
	U3_DMA_Send(buf, buf[1] + 1);
	
}

//���У׼
void motor_calibration(void )
{
	uint8_t buf[] = {0xA5, 0x02, 0x00};  //A5 02 09
	buf[2] = 0x09;
	U3_DMA_Send(buf, buf[1] + 1);
	
}

//���õ��λ��
void motor_set_pos(float pos)
{
	uint8_t buf[] = {0xa5, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00};  //A5 06 04 00 00 00 00
	
	uint32_t data = float_to_u32(pos);
	buf[3] = (uint8_t)(data >> 0);
	buf[4] = (uint8_t)(data >> 8);
	buf[5] = (uint8_t)(data >> 16);
	buf[6] = (uint8_t)(data >> 24);
	
	U3_DMA_Send(buf, buf[1] + 1);
	
}

//���õ���ٶ�
void motor_set_speed(float speed )
{
	uint8_t buf[] = {0xa5, 0x06, 0x05, 0x00, 0x00, 0x00, 0x00};  //A5 06 05 00 00 00 00
	
	uint32_t data = float_to_u32(speed);
	buf[3] = (uint8_t)(data >> 0);
	buf[4] = (uint8_t)(data >> 8);
	buf[5] = (uint8_t)(data >> 16);
	buf[6] = (uint8_t)(data >> 24);
	
	U3_DMA_Send(buf, buf[1] + 1);
}

//���õ��Ť��
void motor_set_torque(float torque )
{
	uint8_t buf[] = {0xa5, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00};  //A5 06 06 00 00 00 00
	
	uint32_t data = float_to_u32(torque);
	buf[3] = (uint8_t)(data >> 0);
	buf[4] = (uint8_t)(data >> 8);
	buf[5] = (uint8_t)(data >> 16);
	buf[6] = (uint8_t)(data >> 24);
	
	U3_DMA_Send(buf, buf[1] + 1);
}

//获取电机状态
void motor_get_state(void )
{
	uint8_t buf[] = {0xa5, 0x02, 0x07};  //A5 02 07
	U3_DMA_Send(buf, buf[1] + 1);
	
}




//自变量关节电机数据处理函数
void XR_data_processing(void )
{
	if (U3.RxBuf[0] == 0xa5 && (U3.RxCount - 1) == U3.RxBuf[1])
	{
		switch (U3.RxBuf[2])
		{
			//读取
			case 0x07:
			{
				motor_status2.motor_temperature = (float)( (U3.RxBuf[3] >> 0) | (U3.RxBuf[4] >> 8) );
				motor_status2.mos_temperature	= (float)( (U3.RxBuf[5] >> 0) | (U3.RxBuf[6] >> 8) );
				motor_status2.voltage_bus		= (float)( (U3.RxBuf[7] >> 0) | (U3.RxBuf[8] >> 8) );
			}
			break;

		}
	}
	
	
	
	
	
}