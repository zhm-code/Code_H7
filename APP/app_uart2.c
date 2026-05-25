#include "app_uart2.h"
#include "string.h"

void U2_Receiving_Process(void)
{
//	//判断是否接收到数据
//	switch(U2.RxFinishFlag)
//	{
//		//接收到数据
//		case SET:
//		{
//			//处理模版
//			U2_DMA_Send(U2.RxBuf,U2.RxCount);
//			
//			
//			
//			
//			//复位操作
//			U2.RxFinishFlag = RESET;
//			U2.RxCount = 0;
//			memset(U2.RxBuf, 0 , sizeof(U2.RxBuf));
//			
//			//又再次打开DMA收发
//			HAL_UART_Receive_DMA(&huart2,(uint8_t *)U2.RxBuf,UART_MAX_LEN);	
//			break;
//		}
//		
//		case RESET:
//		{
//			
//			
//			
//			break;
//		}
//		
//	}
}
