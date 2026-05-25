#include "app_uart3.h"
#include "string.h"
#include "app_XR_API.h"

void U3_Receiving_Process(void)
{
	//判断是否接收到数据
	switch(U3.RxFinishFlag)
	{
		//接收到数据
		case SET:
		{
			//处理模版
			XR_data_processing();
			
			
			
			//复位操作
			U3.RxFinishFlag = RESET;
			U3.RxCount = 0;
			memset(U3.RxBuf, 0 , sizeof(U3.RxBuf));
			
			//又再次打开DMA收发
			HAL_UART_Receive_DMA(&huart3,(uint8_t *)U3.RxBuf,UART_MAX_LEN);	
			break;
		}
		
		case RESET:
		{
			
			
			
			break;
		}
		
	}
}
