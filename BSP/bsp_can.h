#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__
#include "main.h"

typedef enum
{
	CAN_STOP   = 0x00,
	CAN_RUNING = 0x01,

}CAN_STATE;


/************************************************************************************************************
*										FDCAN发送接收句柄
************************************************************************************************************/
typedef struct {
    FDCAN_HandleTypeDef *hcan; 			// 指向FDCAN控制器的句柄，用于标识和操作FDCAN硬件[^1^]。
    FDCAN_TxHeaderTypeDef Header; 		// 发送帧的头部信息，包含帧的ID、数据长度、帧类型等[^1^]。
    uint8_t Data[8]; 					// 发送帧的数据部分，最大支持8字节[^1^]。
} FDCAN_TxFrame_TypeDef;

typedef struct {
    FDCAN_HandleTypeDef *hcan; 			// 指向FDCAN控制器的句柄，用于标识和操作FDCAN硬件[^1^]。
    FDCAN_RxHeaderTypeDef Header; 		// 接收帧的头部信息，包含帧的ID、数据长度、帧类型等[^1^]。
    uint8_t Data[8]; 					// 接收帧的数据部分，最大支持8字节[^1^]。
} FDCAN_RxFrame_TypeDef;

typedef struct
{
	uint32_t    		  	uiCAN_ID;                       //帧ID
    uint8_t    				ucCAN_Len;                      //长度
    uint8_t     			aucCAN_Buf[8];                  //缓冲区
}CAN_Send_t;

typedef struct 
{
	FDCAN_TxFrame_TypeDef 	CANx;
    uint16_t    		  	usCAN_BPS;                      //波特率
	CAN_Send_t				stCAN_Send;                     //发送数据结构体
    uint16_t    			usCAN_Sned_Periodic_Time;       //间隔时间
	uint16_t    			usSend_Cycle;              		//发送次数

	uint8_t     			ucCAN_State_Flag : 1;  			 //CAN状态标志位，0表示停止，1表示运行    
	uint8_t      			ucCAN_Run_Once_Flag : 1;         //CAN运行一次标志位，0表示未运行，1表示已运行一次（用于单次发送模式）         
	uint8_t     			ucCAN_Res : 6;          //预留7位

}CAN_Parm_t;

/************************************************************************************************************
*										开环测试发送共用体
************************************************************************************************************/
#pragma pack(1) //设置1字节对齐
#pragma pack(push,1)
#pragma anon_unions
typedef union
{
	uint8_t Buff[8];
	struct 
	{
		int32_t SetSpd;				//速度测试
		uint8_t SetWorkMode;		//工作模式
		uint8_t RES1;				//寄存器1
		uint8_t RES2;				//寄存器2
		uint8_t RES3;				//寄存器3
	};
} CAN_TX_SET_RUN_OPENLOOP;
#pragma pack(pop)
#pragma pack() //恢复编译器默认对齐

/************************************************************************************************************
*										驱动板接收数据句柄
************************************************************************************************************/
typedef struct {
	
	//功能码 60h：一号反馈数据包
	uint8_t MD_StatusCode;
	uint8_t MD_Res;
	uint16_t MD_ErrorCode;
	//功能码 61h：二号反馈数据包
	int32_t RealPos;				//实际位置 cnt 电机转一圈4096个脉冲
	int32_t RealSpd;				//实际速度 单位0.1Rpm
	//功能码 62h：三号反馈数据包
	int32_t RealTorqeCurr;			//实际力矩电流，单位 mA
	int16_t MotorDriverTemp;		//驱动器温度，单位℃
	int16_t MotorTemp;				//电机温度，单位℃
	uint16_t usEndAddrMark;			//地址结束标记，用于数据清零
	
}DriverBoard_DATA_TypeDef;


/************************************************************************************************************
*										驱动板相关功能枚举
************************************************************************************************************/
typedef enum {
	MD_TX_NONE 				= 0x00,		//空操作
	MD_TX_SET_EN 			= 0x01,		//驱动器使能
	MD_TX_SETFB_Data		= 0x02,		//设置返回数据
	MD_TX_SetRunOpenLood	= 0x03,		//电压开环模式
	MD_TX_SET_ENGINTEST		= 0x04,		//编码器辨识
	MD_TX_SetDIS			= 0x06,		//驱动器禁能
	MD_TX_OpenMOde 			= 0x07,
}MX_TX_ENUM;

/************************************************************************************************************
*											外部变量声明
************************************************************************************************************/
extern  FDCAN_TxFrame_TypeDef   FDCAN1_TxFrame;
extern  FDCAN_TxFrame_TypeDef   FDCAN2_TxFrame;
extern 	CAN_TX_SET_RUN_OPENLOOP can_TX_SetOpenLoop;
extern 	MX_TX_ENUM NumForDriverBoard_Send ;
extern 	DriverBoard_DATA_TypeDef DriverBoard_DATA;

/************************************************************************************************************
*											外部函数声明
************************************************************************************************************/
extern void BSP_FDCAN_Init(void);
extern void FDCAN_TX_Send(FDCAN_TxFrame_TypeDef Tx, uint32_t ID, uint8_t *msg, uint8_t msg_len);
extern void FDCAN_CTRL_DriverBoard_TX(void);
#endif


