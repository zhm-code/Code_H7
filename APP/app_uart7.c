#include "app_uart7.h"
#include "string.h"
#include "app_sysparam.h"
#include "app_pps_ctrl.h"

__IO uint16_t Rx_MSG_U6 = MSG_IDLE;   // 接收报文状态
uint32_t ulGetFrameTick_U6 = 0;

void vPPS_DataProcess(void)
{
    uint16_t usCRC_Value;

	//收到数据
//    if(Rx_MSG_U6 == MSG_COM)
	if(U7.RxFinishFlag == SET)
    {
        usCRC_Value = MB_CRC16((uint8_t*)&U7.RxBuf, U7.RxCount - 2);// 计算CRC校验码 （后面两位不需要计算，本身就是效验码预留）

        if(usCRC_Value == (U7.RxBuf[U7.RxCount-2] | U7.RxBuf[U7.RxCount-1] << 8))                
        {
			//CRC校验通过
			#if (SET_TARGETTYPE == APP_OBJ_GB3220)
			//设置电源开关
			if(U7.RxBuf[0] == 1 && U7.RxBuf[1] == 0x06 && U7.RxBuf[3] == 0x01)

			{
				uSysROParam.PrevSetPPS_On = U7.RxBuf[4]<<8 | U7.RxBuf[5];
			}  

			//设置电源电压
			if(U7.RxBuf[0] == 1 && U7.RxBuf[1] == 0x06 && U7.RxBuf[3] == 0x30)

			{
				uSysROParam.SetPPS_Volt = U7.RxBuf[4]<<8 | U7.RxBuf[5];
			} 

			//读取电压电流
			if(U7.RxBuf[0] == 1 && U7.RxBuf[1] == 0x03 && U7.RxBuf[2] == 0x04)
			{
				TickForPPS_Send = 0;
				uSysROParam.GetPPS_Volt =  (uint16_t)(U7.RxBuf[3]<< 8 | U7.RxBuf[4] );
		        uSysROParam.GetPPS_Current = (uint16_t)(U7.RxBuf[5]<<8 | U7.RxBuf[6] );			
			}	
            #endif			
        }		

//        Rx_MSG_U6 = MSG_IDLE;
		U7.RxFinishFlag = RESET;
    }
}



void vApp_usart6_DMA_IDLE_SetFinish(void)
{
    Rx_MSG_U6 = MSG_COM;
    ulGetFrameTick_U6++;
}

uint8_t firstflag_U6 = 0;



void U7_Receiving_Process(void)
{
	//判断是否接收到数据
	switch(U7.RxFinishFlag)
	{
		//接收到数据
		case SET:
		{
			//处理模版
//			U7_DMA_Send(U7.RxBuf,U7.RxCount);
			
			vPPS_DataProcess();
			
			
			
			//复位操作
			U7.RxFinishFlag = RESET;
			U7.RxCount = 0;
			memset(U7.RxBuf, 0 , sizeof(U7.RxBuf));
			
			//又再次打开DMA收发
			HAL_UART_Receive_DMA(&huart7,(uint8_t *)U7.RxBuf,UART_MAX_LEN);	
			break;
		}
		
		case RESET:
		{
			
			
			
			break;
		}
		
	}
}