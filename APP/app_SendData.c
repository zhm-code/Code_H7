#include "app_SendData.h"

System_Send_Params_t stSys_send_Params;             //系统发送参数配置
Motor_Read_MSG_t stMotor_Read_MSG;                    //电机读取信息
System_Uploading_Params_t System_Uploading_Params;

static void __vCAN_Send_Loop(System_Send_Params_t* Param, USER_CAN_MASK CAN_Mask);     //CAN发送循环函数声明
static void __vUART_Send_Loop(System_Send_Params_t Param, USER_UART_MASK UART_Mask);  //UART发送循环函数声明


/// @brief 系统发送循环函数
/// @param Param 系统发送参数结构体
void vSystem_Send_Loop(System_Send_Params_t* Param)
{

#if USER_URAT_TX 
    if (Param.uSUart_COM_Mask & UART1_MASK)         //如果UART1_MASK位被置位，表示需要发送UART1的数据
    {
        __vUART_Send_Loop(Param, UART1_MASK);       //调用UART发送循环函数，传入参数和UART1_MASK
    }

    if (Param.uSUart_COM_Mask & UART3_MASK)         //如果UART3_MASK位被置位，表示需要发送UART3的数据
    {
        __vUART_Send_Loop(Param, UART3_MASK);       //调用UART发送循环函数，传入参数和UART3_MASK
    }

    if (Param.uSUart_COM_Mask & UART5_MASK)         //如果UART5_MASK位被置位，表示需要发送UART5的数据
    {
        __vUART_Send_Loop(Param, UART5_MASK);       //调用UART发送循环函数，传入参数和UART5_MASK
    }

#endif

#if  USER_CAN_TX
    if ((*Param).usCAN_COM_Mask & CAN1_MASK)            //如果CAN1_MASK位被置位，表示需要发送CAN1的数据
    {
        __vCAN_Send_Loop(Param, CAN1_MASK);             //调用CAN发送循环函数，传入参数和CAN1_MASK
    }

    if ((*Param).usCAN_COM_Mask & CAN2_MASK)            //如果CAN2_MASK位被置位，表示需要发送CAN2的数据
    {
        __vCAN_Send_Loop(Param, CAN2_MASK);             //调用CAN发送循环函数，传入参数和CAN2_MASK
    }
#endif

}



/// @brief UART开关函数
/// @param ucUart_Mask 串口掩码，用于指定哪个UART的开关
/// @param ucSwitch  开关状态，1表示开启发送，0表示关闭发送
void vUrat_switch(uint8_t ucUart_Mask, uint8_t ucSwitch)
{
#if USER_URAT_TX 
    if (ucSwitch)
    {
        stSys_send_Params.uSUart_COM_Mask |= ucUart_Mask;  //置位对应UART的掩码位，开启发送
    }
    else
    {
        stSys_send_Params.uSUart_COM_Mask &= ~ucUart_Mask; //清除对应UART的掩码位，关闭发送
    }
#endif
}

/// @brief CAN开关函数
/// @param ucCAN_Mask CAN掩码，用于指定哪个CAN的开关
/// @param ucSwitch  开关状态，1表示开启发送，0表示关闭发送
void vCAN_switch(uint8_t ucCAN_Mask, uint8_t ucSwitch)
{
    if (ucSwitch)
    {
        stSys_send_Params.usCAN_COM_Mask |= ucCAN_Mask;  //置位对应CAN的掩码位，开启发送
    }
    else
    {
        stSys_send_Params.usCAN_COM_Mask &= ~ucCAN_Mask; //清除对应CAN的掩码位，关闭发送
    }
}

/// @brief 系统CAN发送配置函数
/// @param ucCAN_Mask CAN掩码，用于指定哪个CAN的开关
/// @param ucSwitch  开关状态，1表示开启发送，0表示关闭发送
/// @param CAN_ID CAN ID
/// @param aucbuf CAN数据缓冲区
/// @param ucLen 数据长度
/// @param ucCycle 发送周期，单位为发送次数，0表示无限发送
/// @param usPeriodic_Time 发送间隔时间
void vSystem_Send_Cfg(uint8_t ucCAN_Mask, uint8_t ucSwitch, uint32_t CAN_ID, uint8_t* aucbuf, uint8_t ucLen, uint8_t ucCycle, uint16_t usPeriodic_Time)
{

    switch (ucCAN_Mask)
    {
        case CAN1_MASK:
        {
            stSys_send_Params.stCAN1.stCAN_Send.uiCAN_ID            = CAN_ID;			                            //设置CAN ID
            stSys_send_Params.stCAN1.stCAN_Send.ucCAN_Len           = ucLen;	                                    //设置CAN数据长度
            stSys_send_Params.stCAN1.usSend_Cycle                   = ucCycle;                                      //发送次数
            stSys_send_Params.stCAN1.usCAN_Sned_Periodic_Time       = usPeriodic_Time;                              //发送间隔时间
            memcpy(stSys_send_Params.stCAN1.stCAN_Send.aucCAN_Buf, aucbuf, ucLen);	                                //设置CAN数据内容
        }
        break;

        case CAN2_MASK:
        {
            stSys_send_Params.stCAN2.stCAN_Send.uiCAN_ID            = CAN_ID;			                            //设置CAN ID
            stSys_send_Params.stCAN2.stCAN_Send.ucCAN_Len           = ucLen;	                                    //设置CAN数据长度
            stSys_send_Params.stCAN2.usSend_Cycle                   = ucCycle;                                      //发送次数
            stSys_send_Params.stCAN2.usCAN_Sned_Periodic_Time       = usPeriodic_Time;                              //发送间隔时间
            memcpy(stSys_send_Params.stCAN2.stCAN_Send.aucCAN_Buf, aucbuf, ucLen);	                                //设置CAN数据内容
        }
        break;

        default:
        {
            stSys_send_Params.stCAN1.stCAN_Send.ucCAN_Len = 0;   //设置CAN1发送长度为0，表示不发送数据
            stSys_send_Params.stCAN2.stCAN_Send.ucCAN_Len = 0;   //设置CAN2发送长度为0，表示不发送数据
        }
        break;
    }

    vCAN_switch(ucCAN_Mask, ucSwitch);				                                                                //打开CAN1发送
}

/// @brief 获取CAN状态
/// @param CAN_Mask 掩码，用于指定哪个CAN的状态
/// @return 
uint8_t ucGet_CAN_State(USER_CAN_MASK CAN_Mask)
{
    uint8_t ucState = 0;

    switch (CAN_Mask)
    {
        case CAN1_MASK:
        {
            ucState = stSys_send_Params.stCAN1.ucCAN_State_Flag;   //获取CAN1状态标志位
        }
        break;

        case CAN2_MASK:
        {
            ucState = stSys_send_Params.stCAN2.ucCAN_State_Flag;   //获取CAN2状态标志位
        }
        break;

        default:
        {
            ucState = 0;   //默认返回0，表示未知状态
        }
        break;
    }

    return ucState;
}


/********************************************本地函数*********************************************************/

/// @brief UART发送循环函数
/// @param Param 系统发送参数结构体
/// @param UART_Mask UART掩码，用于指定发送哪个UART的数据
static void __vUART_Send_Loop(System_Send_Params_t Param, USER_UART_MASK UART_Mask)
{
#if USER_URAT_TX
    static uint64_t ullSned_Periodic_Time = 0;

    switch (UART_Mask)
    {
        case UART1_MASK:
        {
            if (ullSYS_time_Count - ullSned_Periodic_Time >= Param.stU1.usUart_Sned_Periodic_Time)
            {
                ullSned_Periodic_Time = ullSYS_time_Count;
                vUx_DMA_Send(
                            huart1,
                            Param.stU1.aucTx_Buf, 
                            Param.stU1.ucTx_Cnt
                        );
            }
        }
        break;

        case UART3_MASK:
        {
            if (ullSYS_time_Count - ullSned_Periodic_Time >= Param.stU3.usUart_Sned_Periodic_Time)
            {
                ullSned_Periodic_Time = ullSYS_time_Count;
                vUx_DMA_Send(
                            huart3,
                            Param.stU3.aucTx_Buf, 
                            Param.stU3.ucTx_Cnt
                        );
            }
        }
        break;

        case UART5_MASK:
        {
            if (ullSYS_time_Count - ullSned_Periodic_Time >= Param.stU5.usUart_Sned_Periodic_Time)
            {
                ullSned_Periodic_Time = ullSYS_time_Count;
                vUx_DMA_Send(
                            huart5,
                            Param.stU5.aucTx_Buf, 
                            Param.stU5.ucTx_Cnt
                        );
            }
        }
        break;

        default:
        {
            
        }
        break;
    }

#endif

}




/// @brief CAN发送循环函数
/// @param Param 系统发送参数结构体
/// @param CAN_Mask CAN掩码，用于指定发送哪个CAN的数据
static void __vCAN_Send_Loop(System_Send_Params_t* Param, USER_CAN_MASK CAN_Mask)
{
    static uint64_t ullSned_Periodic_Time = 0;
    static uint16_t usSend_Cycle_Count = 0;    //发送周期计数器

    switch (CAN_Mask)
    {
        case CAN1_MASK:
        {
            if ( (ullSYS_time_Count - ullSned_Periodic_Time >= (*Param).stCAN1.usCAN_Sned_Periodic_Time) 
                && ((*Param).stCAN1.stCAN_Send.ucCAN_Len > 0) )   //如果到达发送周期，并且发送长度大于0，才进行发送
            {
                ullSned_Periodic_Time = ullSYS_time_Count;
                FDCAN_TX_Send(
                              FDCAN1_TxFrame,               //FDCAN 发送帧的配置结构体
                              (*Param).stCAN1.stCAN_Send.uiCAN_ID,        //帧ID
                              (*Param).stCAN1.stCAN_Send.aucCAN_Buf,      //发送缓冲区
                              (*Param).stCAN1.stCAN_Send.ucCAN_Len        //发送长度
                            );
                if((*Param).stCAN1.usSend_Cycle > 0 && usSend_Cycle_Count < (*Param).stCAN1.usSend_Cycle) 
                {
                    usSend_Cycle_Count++;
                }
                else if((*Param).stCAN1.usSend_Cycle > 0 && usSend_Cycle_Count >= (*Param).stCAN1.usSend_Cycle)
                {
                    vCAN_switch(CAN1_MASK, 0x00);  //关闭CAN1发送
                    usSend_Cycle_Count = 0;        //重置发送周期计数器
					(*Param).stCAN1.ucCAN_State_Flag = CAN_STOP;
                }
                else if((*Param).stCAN1.usSend_Cycle == 0)
                {
                    //如果发送周期为0，表示无限发送，不进行周期计数
                }
                
                (*Param).stCAN1.ucCAN_State_Flag = CAN_RUNING;   //更新CAN状态标志位为运行
            }
        }
        break;

        case CAN2_MASK:
        {
            if ( (ullSYS_time_Count - ullSned_Periodic_Time >= (*Param).stCAN2.usCAN_Sned_Periodic_Time) 
                && ((*Param).stCAN2.stCAN_Send.ucCAN_Len > 0) )   //如果到达发送周期，并且发送长度大于0，才进行发送
            {
                ullSned_Periodic_Time = ullSYS_time_Count;
                FDCAN_TX_Send(
                              FDCAN2_TxFrame,               //FDCAN 发送帧的配置结构体
                              (*Param).stCAN2.stCAN_Send.uiCAN_ID,        //帧ID
                              (*Param).stCAN2.stCAN_Send.aucCAN_Buf,      //发送缓冲区
                              (*Param).stCAN2.stCAN_Send.ucCAN_Len        //发送长度
                            );
                if((*Param).stCAN2.usSend_Cycle > 0 && usSend_Cycle_Count < (*Param).stCAN2.usSend_Cycle) 
                {
                    usSend_Cycle_Count++;
                }
                else if((*Param).stCAN2.usSend_Cycle > 0 && usSend_Cycle_Count >= (*Param).stCAN2.usSend_Cycle)
                {
                    vCAN_switch(CAN2_MASK, 0x00);  //关闭CAN2发送
                    usSend_Cycle_Count = 0;        //重置发送周期计数器
					(*Param).stCAN2.ucCAN_State_Flag = CAN_STOP;
                }
                else if((*Param).stCAN2.usSend_Cycle == 0)
                {
                    //如果发送周期为0，表示无限发送，不进行周期计数
                }
                
                (*Param).stCAN2.ucCAN_State_Flag = CAN_RUNING;   //更新CAN状态标志位为运行
            }
        }
        break;

        default:
        {
            (*Param).stCAN1.ucCAN_State_Flag = CAN_STOP;    //更新CAN状态标志位为停止
            (*Param).stCAN2.ucCAN_State_Flag = CAN_STOP;    //更新CAN状态标志位为停止
        }
        break;
    }
}






