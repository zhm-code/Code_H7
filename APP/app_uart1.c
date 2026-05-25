#include "app_uart1.h"
#include "string.h"


void U1_Receiving_Process(void)
{
	//判断是否接收到数据
	switch(U1.RxFinishFlag)
	{
		//接收到数据
		case SET:
		{
			//处理模版
			U1_DMA_Send(U1.RxBuf,U1.RxCount);
			
			
			
			
			//复位操作
			U1.RxFinishFlag = RESET;
			U1.RxCount = 0;
			memset(U1.RxBuf, 0 , sizeof(U1.RxBuf));
			
			//又再次打开DMA收发
			HAL_UART_Receive_DMA(&huart1,(uint8_t *)U1.RxBuf,UART_MAX_LEN);	
			break;
		}
		
		case RESET:
		{
			
			break;
		}
		
	}
}
