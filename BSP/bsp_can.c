#include "bsp_can.h"
#include "fdcan.h"
#include "app_sysparam.h"
#include "app_SendData.h"

/************************************************************************************************************
*												FDCANRX初始化
************************************************************************************************************/
FDCAN_RxFrame_TypeDef FDCAN1_RxFrame;
FDCAN_RxFrame_TypeDef FDCAN2_RxFrame;

/************************************************************************************************************
*												FDCANTX初始化
************************************************************************************************************/
FDCAN_TxFrame_TypeDef FDCAN1_TxFrame = {
    .hcan 	= &hfdcan1, 								// 指向FDCAN控制器的句柄，用于标识和操作FDCAN硬件
	
    .Header = {      									// 发送帧的头部信息
		.Identifier 			= 0x00,					// 设置ID
        .IdType 				= FDCAN_STANDARD_ID, 	// 设置为标准ID（11位）
        .TxFrameType 			= FDCAN_DATA_FRAME, 	// 设置为数据帧
        .DataLength 			= 8, 					// 数据长度为8字节
        .ErrorStateIndicator 	= FDCAN_ESI_ACTIVE, 	// 错误状态指示器为活动状态
        .BitRateSwitch 			= FDCAN_BRS_OFF, 		// 禁用比特率切换
        .FDFormat 				= FDCAN_CLASSIC_CAN, 	// 使用经典CAN格式
        .TxEventFifoControl 	= FDCAN_NO_TX_EVENTS, 	// 禁用发送事件FIFO
        .MessageMarker 			= 0 					// 消息标记为0
    },
	
    .Data 	= {0} 										// 数据部分，最大支持8字节
};


FDCAN_TxFrame_TypeDef FDCAN2_TxFrame = {
    .hcan 	= &hfdcan2, 								// 指向FDCAN控制器的句柄，用于标识和操作FDCAN硬件实例
    
	.Header = {      									// 发送帧的头部信息
        .Identifier 			= 0x00,					// 设置ID
		.IdType 				= FDCAN_STANDARD_ID, 	// 设置为标准ID（11位）
        .TxFrameType 			= FDCAN_DATA_FRAME, 	// 设置为数据帧
        .DataLength 			= 8, 					// 数据长度为8字节
        .ErrorStateIndicator 	= FDCAN_ESI_ACTIVE, 	// 错误状态指示器为活动状态
        .BitRateSwitch 			= FDCAN_BRS_OFF, 		// 禁用比特率切换
        .FDFormat 				= FDCAN_CLASSIC_CAN, 	// 使用经典CAN格式
        .TxEventFifoControl 	= FDCAN_NO_TX_EVENTS, 	// 禁用发送事件FIFO
        .MessageMarker 			= 0 					// 消息标记为0
    },
	
    .Data 	= {0} 										// 数据部分，最大支持8字节
};

/************************************************************************************************************
*											FDCAN驱动控制板相关
************************************************************************************************************/
CAN_TX_SET_RUN_OPENLOOP can_TX_SetOpenLoop;
MX_TX_ENUM NumForDriverBoard_Send = 0;
DriverBoard_DATA_TypeDef DriverBoard_DATA;

//指令
static uint8_t fdcan1_TX_SetEN[8] 			= {0x01};				//驱动器使能
static uint8_t fdcan1_TX_SetDIS[8] 			= {0x00};				//驱动器禁能
static uint8_t fdcan1_TX_SetFB_Data[8] 		= {0x64, 0x64, 0x64};	//设置返回数据
static uint8_t fdcan1_TX_SetEnginTest[8] 	= {0x0C};				//工程测试模式 - 0X0C 编码器辨识
static uint8_t fdcan1_TX_OpenMode[8] 		= {0xE8, 0x03, 0x00, 0x00, 0x21,0x00,0x00};				//工程测试模式 - 0X0C 编码器辨识

/************************************************************************************************************
*												FDCAN初始化
************************************************************************************************************/
void BSP_FDCAN_Init(void)
{
    // 定义 FDCAN1 的过滤器配置结构体
    FDCAN_FilterTypeDef FDCAN1_FilterConfig;

    // 配置 FDCAN1 的过滤器参数
    FDCAN1_FilterConfig.IdType 				= FDCAN_STANDARD_ID; 		// 设置为标准ID（11位）
    FDCAN1_FilterConfig.FilterIndex 		= 0;            			// 设置过滤器索引为0
    FDCAN1_FilterConfig.FilterType 			= FDCAN_FILTER_MASK; 		// 设置过滤器类型为掩码过滤
    FDCAN1_FilterConfig.FilterConfig 		= FDCAN_FILTER_TO_RXFIFO0;  // 将匹配的消息存储到接收 FIFO 0
    FDCAN1_FilterConfig.FilterID1 			= 0x00000000;     			// 设置过滤器 ID1 为 0
    FDCAN1_FilterConfig.FilterID2 			= 0x00000000;     			// 设置过滤器 ID2 为 0

    // 调用 HAL_FDCAN_ConfigFilter 函数配置 FDCAN1 的过滤器
    if(HAL_FDCAN_ConfigFilter(&hfdcan1, &FDCAN1_FilterConfig) != HAL_OK)
    {
        Error_Handler(); // 如果配置失败，调用错误处理函数
    }

    // 配置 FDCAN1 的全局过滤器，拒绝不匹配的标准和扩展数据帧，以及标准和扩展远程帧
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
    {
        Error_Handler(); // 如果配置失败，调用错误处理函数
    }

    // 使能 FDCAN1 的接收 FIFO 0 新消息中断
    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
        Error_Handler(); // 如果使能失败，调用错误处理函数
    }

    // 启动 FDCAN1
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        Error_Handler(); // 如果启动失败，调用错误处理函数
    }

    // 定义 FDCAN2 的过滤器配置结构体
    FDCAN_FilterTypeDef FDCAN2_FilterConfig;

    // 配置 FDCAN2 的过滤器参数
    FDCAN2_FilterConfig.IdType 				= FDCAN_STANDARD_ID; 		// 设置为标准ID（11位）
    FDCAN2_FilterConfig.FilterIndex 		= 0;            			// 设置过滤器索引为0
    FDCAN2_FilterConfig.FilterType 			= FDCAN_FILTER_MASK; 		// 设置过滤器类型为掩码过滤
    FDCAN2_FilterConfig.FilterConfig 		= FDCAN_FILTER_TO_RXFIFO1;  // 将匹配的消息存储到接收 FIFO 1
    FDCAN2_FilterConfig.FilterID1 			= 0x00000000;     			// 设置过滤器 ID1 为 0
    FDCAN2_FilterConfig.FilterID2 			= 0x00000000;     			// 设置过滤器 ID2 为 0

    // 调用 HAL_FDCAN_ConfigFilter 函数配置 FDCAN2 的过滤器
    if(HAL_FDCAN_ConfigFilter(&hfdcan2, &FDCAN2_FilterConfig) != HAL_OK)
    {
        Error_Handler(); // 如果配置失败，调用错误处理函数
    }

    // 配置 FDCAN2 的全局过滤器，拒绝不匹配的标准和扩展数据帧，以及标准和扩展远程帧
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
    {
        Error_Handler(); // 如果配置失败，调用错误处理函数
    }

    // 使能 FDCAN2 的接收 FIFO 1 新消息中断
    if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK)
    {
        Error_Handler(); // 如果使能失败，调用错误处理函数
    }

    // 启动 FDCAN2
    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
    {
        Error_Handler(); // 如果启动失败，调用错误处理函数
    }
}
/************************************************************************************************************
*												FDCANTX发送
*	FDCAN_TxFrame_TypeDef Tx	FDCAN发送结构体
*	uint32_t ID					发送帧ID
*	uint8_t *msg				发送缓冲区
*	uint8_t msg_len				发送长度
************************************************************************************************************/
void FDCAN_TX_Send(FDCAN_TxFrame_TypeDef Tx, uint32_t ID, uint8_t *msg, uint8_t msg_len)
{
	Tx.Header.Identifier = ID;									//设置ID
	Tx.Header.DataLength = msg_len;								//设置长度
	HAL_FDCAN_AddMessageToTxFifoQ(Tx.hcan, &Tx.Header, msg);	//底层FDCAN发送
}

/************************************************************************************************************
*												FDCANTX控制驱动板
************************************************************************************************************/
void FDCAN_CTRL_DriverBoard_TX(void)
{
	switch ((uint16_t) NumForDriverBoard_Send)	//功能码对应操作
	{
		case MD_TX_NONE:				//空操作
		{
			
			
		}
		break;
		
		case MD_TX_SET_EN:				//驱动器使能
		{
			FDCAN_TX_Send(FDCAN1_TxFrame, 0x501, fdcan1_TX_SetEN, 0x01);
			
		}
		break;
		
		case MD_TX_SET_ENGINTEST:		//编码器辨识
		{
			FDCAN_TX_Send(FDCAN1_TxFrame, 0x5E1, fdcan1_TX_SetEnginTest, 0x01);
			NumForDriverBoard_Send = MD_TX_NONE;	//发送一次,置空操作	
		}
		break;
		
		case MD_TX_SETFB_Data:			//设置返回数据
		{
			FDCAN_TX_Send(FDCAN1_TxFrame, 0x521, fdcan1_TX_SetFB_Data, 0x03);
			
		}
		break;
		
		case MD_TX_SetRunOpenLood:		//电压开环模式
		{
			switch ((uint16_t)uSysROParam.ProjNumber)
            {
            	case GB10847:
				case GB10865:
				
            	{
//            		can_TX_SetOpenLoop.SetWorkMode = 0x31;	//轮廓速度模式	
					can_TX_SetOpenLoop.SetWorkMode = 0x21;	//轮廓速度模式
            	}
            	break;
            	case ZC17353:
				case ER17353:
            	{
            		can_TX_SetOpenLoop.SetWorkMode = 0x21;	//轮廓速度模式	
            	}
            	break;
            }
				
			FDCAN_TX_Send(FDCAN1_TxFrame, 0x531, can_TX_SetOpenLoop.Buff, 0x07);
			
		}
		break;
		
		case MD_TX_SetDIS:				//驱动器禁能
		{
			FDCAN_TX_Send(FDCAN1_TxFrame,0x501, fdcan1_TX_SetDIS, 0x01);
			
		}
		break;
		
		case MD_TX_OpenMOde:				//开环电压模式
		{
			FDCAN_TX_Send(FDCAN1_TxFrame,0x531, fdcan1_TX_OpenMode, 0x07);
			
		}
		break;
		

		
	}
	
}



/************************************************************************************************************
*										FDCAN接收FIFO中断回调
*	RXFIFO_0:设置为FDCAN1接收因此直接写死
*	RXFIFO_1:设置为FDCAN2接收因此直接写死
*
************************************************************************************************************/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{ 
	if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN1_RxFrame.Header, FDCAN1_RxFrame.Data) != HAL_OK)	//接收信息
	{
		return ;	//接收不成功返回
	}

	switch(FDCAN1_RxFrame.Header.Identifier)	//判断帧ID进入相应接收
    {
		case 0x531:			//设置目标值
		{
			
			
		}
		break;
		
		case 0x5E1:			//工程测试模式 编码器辨识
		{
			
			
		}
		break;
		
		case 0x601:			//一号反馈数据包 状态错误码
		{
			if(FDCAN1_RxFrame.Header.DataLength == 3)
            {
                DriverBoard_DATA.MD_StatusCode 	= FDCAN1_RxFrame.Data[0];										
				DriverBoard_DATA.MD_ErrorCode 	= (uint16_t)(FDCAN1_RxFrame.Data[1]|FDCAN1_RxFrame.Data[2]<<8);	
            }
			
		}
		break;
		
		case 0x611:			//二号反馈数据包 实际位置和实际速度
		{
			if(FDCAN1_RxFrame.Header.DataLength == 8)
            {
                DriverBoard_DATA.RealPos = (int32_t)(FDCAN1_RxFrame.Data[0]|FDCAN1_RxFrame.Data[1]<<8|FDCAN1_RxFrame.Data[2]<<16|FDCAN1_RxFrame.Data[3]<<24);
				DriverBoard_DATA.RealSpd = (int32_t)(FDCAN1_RxFrame.Data[4]|FDCAN1_RxFrame.Data[5]<<8|FDCAN1_RxFrame.Data[6]<<16|FDCAN1_RxFrame.Data[7]<<24);
            }
			
		}
		break;
		
		case 0x621:			//三号反馈数据包 实际力矩电流 驱动器温度 电机温度
		{
			if(FDCAN1_RxFrame.Header.DataLength == 8)
            {
                DriverBoard_DATA.RealTorqeCurr 		= (int32_t)(FDCAN1_RxFrame.Data[0]|FDCAN1_RxFrame.Data[1]<<8|FDCAN1_RxFrame.Data[2]<<16|FDCAN1_RxFrame.Data[3]<<24);
				DriverBoard_DATA.MotorDriverTemp 	= (int16_t)(FDCAN1_RxFrame.Data[4]|FDCAN1_RxFrame.Data[5]<<8);
				DriverBoard_DATA.MotorTemp 			= (int16_t)(FDCAN1_RxFrame.Data[6]|FDCAN1_RxFrame.Data[7]<<8);
            }
			
		}
		break;
		
		case 0x581:			
		{
			if(FDCAN1_RxFrame.Header.DataLength == 8)
			{
				switch (FDCAN1_RxFrame.Data[3])
                {
                	case 0x01: //驱动器温度
                	{
                		DriverBoard_DATA.MotorDriverTemp = (int16_t)(FDCAN1_RxFrame.Data[4]|FDCAN1_RxFrame.Data[5]<<8) ;
                	}
                	break;
                	case 0x02:	//电机温度
                	{
                		DriverBoard_DATA.MotorTemp		 = (int16_t)(FDCAN1_RxFrame.Data[4]|FDCAN1_RxFrame.Data[5]<<8) ;
			
                	}
                	break;
                	default:
                	{
                		
                	}
                	break;
                }
				
			}
		}
		break;

		//GB11579
		case 0x81:
		{
			if (FDCAN1_RxFrame.Data[0] == 0x01 && FDCAN1_RxFrame.Data[1] == 0xE0 && FDCAN1_RxFrame.Data[2] == 0x00)
			{
				stMotor_Read_MSG.uiFirmware_Version = (uint32_t)(FDCAN1_RxFrame.Data[3] << 24 | FDCAN1_RxFrame.Data[4] << 16
												| FDCAN1_RxFrame.Data[5] << 8 | FDCAN1_RxFrame.Data[6] << 0);
			}
			
		}
		break;

		case 0x201:
		{
			stMotor_Read_MSG.fSpeed 				= (int16_t)(FDCAN1_RxFrame.Data[0] << 8 | FDCAN1_RxFrame.Data[1] << 0) / 10.0f;
			stMotor_Read_MSG.fMOS_Temp 				= (int16_t)(FDCAN1_RxFrame.Data[2] << 8 | FDCAN1_RxFrame.Data[3] << 0) / 10.0f;
			stMotor_Read_MSG.fWire_Winding_Temp     = (int16_t)(FDCAN1_RxFrame.Data[4] << 8 | FDCAN1_RxFrame.Data[5] << 0) / 10.0f;
			stMotor_Read_MSG.fMCU_Temp 				= (int16_t)(FDCAN1_RxFrame.Data[6] << 8 | FDCAN1_RxFrame.Data[7] << 0) / 10.0f;
			stMotor_Read_MSG.fBus_Current 			= (int16_t)(FDCAN1_RxFrame.Data[6] << 8 | FDCAN1_RxFrame.Data[7] << 0) / 10.0f;
		}
		break;
		
		default:
		{
			
			
		}
        break;
		
	}

}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{ 
    // 从FDCAN的FIFO 1中获取接收到的消息
    // hfdcan: 指向FDCAN句柄的指针，标识当前操作的FDCAN实例
    // RxFifo0ITs: FIFO 1中断标志，表示触发回调的具体事件
    // FDCAN2_RxFrame: 用于存储接收到的消息的头部信息和数据
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN2_RxFrame.Header, FDCAN2_RxFrame.Data);
    // 此处可以根据FDCAN2_RxFrame中的数据进行进一步处理，例如解析数据、执行相关任务等
}


