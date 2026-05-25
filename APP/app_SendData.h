/// @file app_SendData.c
/// @brief 系统发送相关函数实现文件

#ifndef __APP_SHENDDATA_H__
#define __APP_SHENDDATA_H__
#include "main.h"
#include "bsp_uart.h"
#include "bsp_can.h"
#include "app_sysparam.h"

/*--------------------------------------------发送--------------------------------------------*/
#define USER_URAT_TX 0
#define USER_CAN_TX 1

typedef enum
{
    UART1_MASK = 0X01 << 0,
    UART2_MASK = 0X01 << 1,
    UART3_MASK = 0X01 << 2,
    UART4_MASK = 0X01 << 3,
    UART5_MASK = 0X01 << 4,
    UART6_MASK = 0X01 << 5,
    UART7_MASK = 0X01 << 6,
    UART8_MASK = 0X01 << 7,

}USER_UART_MASK;

typedef enum
{
    CAN1_MASK = 0X01 << 0,
    CAN2_MASK = 0X01 << 1,
}USER_CAN_MASK;

typedef struct 
{
#if USER_URAT_TX
    uint16_t    uSUart_COM_Mask;        //串口掩码开关
    UART_Parm_t stU1;
    UART_Parm_t stU2;
    UART_Parm_t stU3;
    UART_Parm_t stU4;
    UART_Parm_t stU5;
    UART_Parm_t stU6;
    UART_Parm_t stU7;
    UART_Parm_t stU8;
#endif

#if USER_CAN_TX
    uint16_t    usCAN_COM_Mask;         //CAN掩码开关
    CAN_Parm_t stCAN1;
    CAN_Parm_t stCAN2;
#endif

}System_Send_Params_t; //系统发送参数结构体
extern System_Send_Params_t stSys_send_Params;

/*--------------------------------------------接收--------------------------------------------*/
typedef struct 
{
    uint32_t uiFirmware_Version;            //固件版本号
    float    fSpeed;                        //速度
    float    fWire_Winding_Temp;            //绕线温度
    float    fMOS_Temp;                     //MOS管温度
    float    fMCU_Temp;                     //MCU温度
    float    fBus_Current;                  //母线电流
    float    fVoltage;                      //电压
}Motor_Read_MSG_t; //系统发送参数结构体
extern Motor_Read_MSG_t stMotor_Read_MSG;

typedef struct 
{
    Motor_Read_MSG_t stMSG;

}System_Uploading_Params_t; //系统发送参数结构体
extern System_Uploading_Params_t System_Uploading_Params;


extern void vSystem_Send_Loop(System_Send_Params_t* Param);                                     
extern void vUrat_switch(uint8_t ucUart_Mask, uint8_t ucSwitch);    
extern void vCAN_switch(uint8_t ucCAN_Mask, uint8_t ucSwitch);      
extern void vSystem_Send_Cfg(uint8_t ucCAN_Mask, uint8_t ucSwitch, uint32_t CAN_ID, uint8_t* aucbuf, uint8_t ucLen, uint8_t ucCycle, uint16_t usPeriodic_Time);   //系统CAN发送配置函数
extern uint8_t ucGet_CAN_State(USER_CAN_MASK CAN_Mask);   //获取CAN状态函数
#endif