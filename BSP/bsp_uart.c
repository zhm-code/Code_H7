#include "bsp_uart.h"
#include "usart.h"
#include "string.h"

xUART_t U1;
//xUART_t U2;
xUART_t U3;
// 必须加 aligned(4)
//__attribute__((aligned(4))) xUART_t U5;
xUART_t U5;
xUART_t U7;
xUART_t U8;


//空闲中断回调
void vUx_IDLE_Callback(UART_HandleTypeDef huartx, DMA_HandleTypeDef DMA_Ux_RX, xUART_t Ux)
{
    uint32_t tmp_flag = 0;                                           //标志位临时变量
    uint32_t surplus_Count;                                          //记录DMA中未传输的数据个数
    tmp_flag =__HAL_UART_GET_FLAG(&huartx,UART_FLAG_IDLE);               //获取IDLE标志位


    if((tmp_flag != RESET))                                          //idle标志被置位
    { 
        __HAL_UART_CLEAR_IDLEFLAG(&huartx);                              //清除标志位

        HAL_UART_DMAStop(&huartx);                                       //停止DMA
        surplus_Count  =  __HAL_DMA_GET_COUNTER(&DMA_Ux_RX);         // 获取DMA中未传输的数据个数   

		
        Ux.RxCount =  UART_MAX_LEN - surplus_Count;                  //总计数减去未传输的数据个数，得到已经接收的数据个数
        Ux.RxFinishFlag = 1;                                         // 接受完成标志位置1    
     }
}
//初始化
void vUx_Init(UART_HandleTypeDef huartx, xUART_t Ux)
{
	__HAL_UART_ENABLE_IT(&huartx, UART_IT_IDLE); //使能IDLE中断
    //DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
    HAL_UART_Receive_DMA(&huartx, (uint8_t *)Ux.RxBuf, UART_MAX_LEN);
}

//DMA发送
void vUx_DMA_Send(UART_HandleTypeDef huartx, uint8_t* ucBuf, uint8_t ucTxCount)
{
	HAL_UART_Transmit_DMA(&huartx, ucBuf, ucTxCount);
	while(__HAL_UART_GET_FLAG(&huartx, UART_FLAG_TC) == RESET)
	{
		osDelay(1);
	}
}





/****************************************U1****************************************/
//串口1空闲中断回调
void UART1_IDLE_Callback(void )
{
    uint32_t tmp_flag = 0;
    uint32_t surplus_Count; //记录DMA中未传输的数据个数
    tmp_flag =__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE); //获取IDLE标志位
    if((tmp_flag != RESET))//idle标志被置位
    { 
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);//清除标志位
        //temp = huart1.Instance->SR;  //清除状态寄存器SR,读取SR寄存器可以实现清除SR寄存器的功能
        //temp = huart1.Instance->DR; //读取数据寄存器中的数据
        //这两句和上面那句等效
		
        HAL_UART_DMAStop(&huart1); //
        surplus_Count  =  __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);// 获取DMA中未传输的数据个数   
        //temp  = hdma_usart1_rx.Instance->NDTR;//读取NDTR寄存器 获取DMA中未传输的数据个数，
        //这句和上面那句等效
		
        U1.RxCount =  UART_MAX_LEN - surplus_Count; //总计数减去未传输的数据个数，得到已经接收的数据个数
        U1.RxFinishFlag = 1;    // 接受完成标志位置1    
     }
}
//串口1初始化
void U1_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能IDLE中断
    //DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
    HAL_UART_Receive_DMA(&huart1,(uint8_t *)U1.RxBuf,UART_MAX_LEN);
}
//串口1DMA发送
void U1_DMA_Send(uint8_t* uBuf, uint8_t uTxCount)
{
	HAL_UART_Transmit_DMA(&huart1, uBuf, uTxCount);
	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET)
	{
		osDelay(1);
	}
}

/****************************************U2****************************************/
//串口2空闲中断回调
void UART2_IDLE_Callback(void )
{
//    uint32_t tmp_flag = 0;
//    uint32_t surplus_Count; //记录DMA中未传输的数据个数
//    tmp_flag =__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE); //获取IDLE标志位
//    if((tmp_flag != RESET))//idle标志被置位
//    { 
//        __HAL_UART_CLEAR_IDLEFLAG(&huart2);//清除标志位
//        //temp = huart1.Instance->SR;  //清除状态寄存器SR,读取SR寄存器可以实现清除SR寄存器的功能
//        //temp = huart1.Instance->DR; //读取数据寄存器中的数据
//        //这两句和上面那句等效
//		
//        HAL_UART_DMAStop(&huart2); //
//        surplus_Count  =  __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);// 获取DMA中未传输的数据个数   
//        //temp  = hdma_usart1_rx.Instance->NDTR;//读取NDTR寄存器 获取DMA中未传输的数据个数，
//        //这句和上面那句等效
//		
//        U2.RxCount =  UART_MAX_LEN - surplus_Count; //总计数减去未传输的数据个数，得到已经接收的数据个数
//        U2.RxFinishFlag = 1;    // 接受完成标志位置1    
//     }
}
//串口2初始化
void U2_Init(void)
{
//	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //使能IDLE中断
//    //DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
//    HAL_UART_Receive_DMA(&huart2,(uint8_t *)U2.RxBuf,UART_MAX_LEN);	
}
//串口2DMA发送
void U2_DMA_Send(uint8_t* uBuf, uint8_t uTxCount)
{
//	HAL_UART_Transmit_DMA(&huart2, uBuf, uTxCount);
//	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET)
//	{
//		osDelay(1);
//	}
}

/****************************************U3****************************************/
//串口3空闲中断回调
void UART3_IDLE_Callback(void )
{
    uint32_t tmp_flag = 0;
    uint32_t surplus_Count;                                         //记录DMA中未传输的数据个数

    tmp_flag =__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE);          //获取IDLE标志位
    if((tmp_flag != RESET))                                         //idle标志被置位
    { 
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);                         //清除标志位
        HAL_UART_DMAStop(&huart3);                                  //停止DMA传输，清除DMA的相关标志位
        surplus_Count  =  __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);   // 获取DMA中未传输的数据个数   
        U3.RxCount =  UART_MAX_LEN - surplus_Count;                 //总计数减去未传输的数据个数，得到已经接收的数据个数
        U3.RxFinishFlag = 1;                                        // 接受完成标志位置1    
     }
}
//串口3初始化
void U3_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);                    //使能IDLE中断
    HAL_UART_Receive_DMA(&huart3,(uint8_t *)U3.RxBuf,UART_MAX_LEN);	//DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
}
//串口3DMA发送
void U3_DMA_Send(uint8_t* uBuf, uint8_t uTxCount)
{
	HAL_UART_Transmit_DMA(&huart3, uBuf, uTxCount);                 //发送数据
	while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET)      //等待发送完成
    {
        osDelay(1);
    }
}

/****************************************U5****************************************/
//串口5空闲中断回调
void UART5_IDLE_Callback(void )
{
    uint32_t tmp_flag = 0;
    uint32_t surplus_Count; //记录DMA中未传输的数据个数
    tmp_flag =__HAL_UART_GET_FLAG(&huart5,UART_FLAG_IDLE); //获取IDLE标志位
    if((tmp_flag != RESET))//idle标志被置位
    { 
        __HAL_UART_CLEAR_IDLEFLAG(&huart5);//清除标志位

        HAL_UART_DMAStop(&huart5); //

        surplus_Count  =  __HAL_DMA_GET_COUNTER(&hdma_uart5_rx);// 获取DMA中未传输的数据个数   

        U5.RxCount =  UART_MAX_LEN - surplus_Count; //总计数减去未传输的数据个数，得到已经接收的数据个数
        U5.RxFinishFlag = 1;    // 接受完成标志位置1    
     }
}
//串口5初始化
void U5_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE); //使能IDLE中断
    HAL_UART_Receive_DMA(&huart5,(uint8_t *)U5.RxBuf, UART_MAX_LEN);	
}
//串口5DMA发送
void U5_DMA_Send(uint8_t* uBuf, uint8_t uTxCount)
{
	HAL_UART_Transmit_DMA(&huart5, uBuf, uTxCount);
	while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) == RESET)
	{
		osDelay(1);
	}
}
/****************************************U7****************************************/
//串口7空闲中断回调
void UART7_IDLE_Callback(void )
{
    uint32_t tmp_flag = 0;
    uint32_t surplus_Count; //记录DMA中未传输的数据个数
    tmp_flag =__HAL_UART_GET_FLAG(&huart7,UART_FLAG_IDLE); //获取IDLE标志位
    if((tmp_flag != RESET))//idle标志被置位
    { 
        __HAL_UART_CLEAR_IDLEFLAG(&huart7);//清除标志位

		
        HAL_UART_DMAStop(&huart7); //
        surplus_Count  =  __HAL_DMA_GET_COUNTER(&hdma_uart7_rx);// 获取DMA中未传输的数据个数   

		
        U7.RxCount =  UART_MAX_LEN - surplus_Count; //总计数减去未传输的数据个数，得到已经接收的数据个数
        U7.RxFinishFlag = 1;    // 接受完成标志位置1    
     }
}
//串口7初始化
void U7_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE); //使能IDLE中断
    HAL_UART_Receive_DMA(&huart7,(uint8_t *)U7.RxBuf,UART_MAX_LEN);	
}
//串口7DMA发送
void U7_DMA_Send(uint8_t* uBuf, uint8_t uTxCount)
{
	HAL_UART_Transmit_DMA(&huart7, uBuf, uTxCount);
	while(__HAL_UART_GET_FLAG(&huart7, UART_FLAG_TC) == RESET)
	{
		osDelay(1);
	}
}
/****************************************U8****************************************/
//串口8空闲中断回调
void UART8_IDLE_Callback(void )
{
    uint32_t tmp_flag = 0;
    uint32_t surplus_Count; //记录DMA中未传输的数据个数
    tmp_flag =__HAL_UART_GET_FLAG(&huart8, UART_FLAG_IDLE); //获取IDLE标志位
    if((tmp_flag != RESET))//idle标志被置位
    { 
        __HAL_UART_CLEAR_IDLEFLAG(&huart8);//清除标志位

		
        HAL_UART_DMAStop(&huart8); //
        surplus_Count  =  __HAL_DMA_GET_COUNTER(&hdma_uart8_rx);// 获取DMA中未传输的数据个数   

		
        U8.RxCount =  UART_MAX_LEN - surplus_Count; //总计数减去未传输的数据个数，得到已经接收的数据个数
        U8.RxFinishFlag = 1;    // 接受完成标志位置1    
     }
}
//串口8初始化
void U8_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart8, UART_IT_IDLE); //使能IDLE中断
    HAL_UART_Receive_DMA(&huart8,(uint8_t *)U8.RxBuf,UART_MAX_LEN);	
}
//串口8DMA发送
void U8_DMA_Send(uint8_t* uBuf, uint8_t uTxCount)
{
	HAL_UART_Transmit_DMA(&huart8, uBuf, uTxCount);
	while(__HAL_UART_GET_FLAG(&huart8, UART_FLAG_TC) == RESET)
	{
		osDelay(1);
	}
}



//串口初始化
void Ux_ALL_Init(void )
{
	U1_Init();
//	U2_Init();
	U3_Init();
	U5_Init();
	U7_Init();
	U8_Init();
}



