#include "app_uart5.h"
#include "string.h"
#include "stm32h743xx.h"
#include "app_sysparam.h"

static void RS485_Modbus_Process(void);
U5_MSG_t U5_MSG;

static uint8_t U5_Send_Addr1_MSG[6] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x02};
static uint8_t U5_Send_Addr2_MSG[6] = {0x02, 0x04, 0x00, 0x00, 0x00, 0x02};
static uint8_t U5_Send_Addr3_MSG[6] = {0x03, 0x04, 0x00, 0x00, 0x00, 0x02};

void U5_Receiving_Process(void)
{
	//判断是否接收到数据
	switch(U5.RxFinishFlag)
	{
		//接收到数据
		case SET:
		{

			
			RS485_Modbus_Process();
			
			
			//复位操作
			U5.RxFinishFlag = RESET;
			U5.RxCount = 0;
			memset(U5.RxBuf, 0 , sizeof(U5.RxBuf));
			
			//又再次打开DMA收发
			HAL_UART_Receive_DMA(&huart5,(uint8_t *)U5.RxBuf,UART_MAX_LEN);	
			__HAL_DMA_DISABLE_IT(&hdma_uart5_rx, DMA_IT_HT);
			break;
		}
		
		case RESET:
		{
			break;
		}
		
	}
}


static void RS485_Modbus_Process(void)
{
	uint16_t usCRC_Value;
	
		// 计算CRC校验码 （后面两位不需要计算，本身就是效验码预留）
		usCRC_Value = MB_CRC16((uint8_t*)&U5.RxBuf, U5.RxCount - 2);
		
		if ( usCRC_Value == (U5.RxBuf[U5.RxCount-2] | U5.RxBuf[U5.RxCount-1] << 8) )
		{
			U5_MSG.Torque_Sensor_Torque = ( (U5.RxBuf[0] << 24) | (U5.RxBuf[1] << 16) | (U5.RxBuf[2] << 8) ) / 256000.f;
			U5_MSG.Torque_Sensor_RPM    = ( (U5.RxBuf[3] << 24) | (U5.RxBuf[4] << 16) | (U5.RxBuf[5] << 8) ) / 2560.f;
			
//			debug_pas++;
		}
		else
		{
//			debug_ng++;
			
		}
}


void U5_RS485_Modbus_Send(uint8_t * msg, uint8_t len)
{
	uint8_t buf[32] = {0};
	int i = 0;
	for ( i = 0; i < len; i++)
	{
		buf[i] = msg[i];
	}
	uint16_t usCRC_Value = MB_CRC16((uint8_t*)msg, len);
	
	buf[i++] = usCRC_Value & 0x00ff;
	buf[i++] = (usCRC_Value & 0xff00) >> 8;
	
	
	U5_DMA_Send(buf,i);
	
//	HAL_UART_Transmit_IT(&huart5, buf, i);
	
}

//循环发送
void U5_RS485_Modbus_Communication(void)
{
	static uint8_t addr = 0x01;
	switch (addr)
    {
    	case 0x01:
    	{
			addr = 0x02;
			U5_RS485_Modbus_Send(U5_Send_Addr1_MSG,6);
    		break;
    	}
    	case 0x02:
    	{
			addr = 0x03;
			U5_RS485_Modbus_Send(U5_Send_Addr2_MSG,6);
    		break;
    	}
		case 0x03:
    	{
			addr = 0x01;
			U5_RS485_Modbus_Send(U5_Send_Addr3_MSG,6);
    		break;
    	}
    	default:
    	{
    		break;
    	}
    }
}

/*串口中断接收数据完毕回调函数*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   if(huart->Instance == UART5)
	{

	}
}
/*串口中断发送数据完毕回调函数*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == UART5)
	{
		
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

}
