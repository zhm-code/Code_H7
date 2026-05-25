#ifndef __BSP_UART2_H__
#define __BSP_UART2_H__

#include "main.h"
#include "cJSON.h"
#include "string.h"
#include "bsp_MB_slave.h"


#define UART_MAX_LEN 256

#define MSG_ERR_FLAG  0xFFFF    // 接收错误 字符间超时
#define MSG_IDLE      0x0000    // 空闲状态
#define MSG_RXING     0x0001    // 正在接收数据
#define MSG_COM       0x0002    // 接收完成
#define MSG_INC       0x8000    // 数据帧不完整(两字符间的空闲间隔大于1.5个字符时间)
#define TIME_OVERRUN  100       // 定义超时时间 ms

typedef struct
{
	int RPM;		//转速
	
	short    ER8138_RPM;
	uint16_t ER6886_RPM;
	
	
	uint8_t Torque; //扭矩
	uint8_t Temperature; //温度
	uint16_t Current;	//电流
	float LDO;
	float FG;
	float PWM;
	uint8_t HardwareVersion; //硬件件版本号
	uint8_t FirmwareVersion; //固件版本号
	uint8_t ProtocolVersion; //协议版本号
}MSG;

typedef struct
{
	uint8_t RxBuf[UART_MAX_LEN];
	uint8_t TxBuf[UART_MAX_LEN];
	uint8_t RxCount;
	uint8_t TxCount;
	uint8_t RxFinishFlag;
	uint8_t TxFinishFlag;
	MSG Motor_MSG;
	uint8_t data;
	
	
}xUART_t;


typedef struct
{
	uint8_t 	aucRx_Buf[UART_MAX_LEN];
	uint8_t 	aucTx_Buf[UART_MAX_LEN];
	uint8_t 	ucRx_Cnt;
	uint8_t 	ucTx_Cnt;
	uint8_t 	ucRx_receive_Finish_Flag : 1;
	uint8_t 	ucTx_receive_Finish_Flag : 1;
	uint8_t 	ucres : 6;
	uint16_t    usUart_BPS;                     //波特率
	uint16_t    usUart_Sned_Periodic_Time;      //间隔时间
	uint16_t    usSend_Cycle;              		//发送次数
}UART_Parm_t;



extern xUART_t U1;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

//extern xUART_t U2;
//extern UART_HandleTypeDef huart2;
//extern DMA_HandleTypeDef hdma_usart2_rx;

extern xUART_t U3;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

extern xUART_t U5;
extern UART_HandleTypeDef huart5;
extern DMA_HandleTypeDef hdma_uart5_rx;

extern xUART_t U7;
extern UART_HandleTypeDef huart7;
extern DMA_HandleTypeDef hdma_uart7_rx;

extern xUART_t U8;
extern UART_HandleTypeDef huart8;
extern DMA_HandleTypeDef hdma_uart8_rx;


extern void U1_Init(void);
extern void UART1_IDLE_Callback(void );
extern void U1_DMA_Send(uint8_t* uBuf, uint8_t uTxCount);

extern void U2_Init(void);
extern void UART2_IDLE_Callback(void );
extern void U2_DMA_Send(uint8_t* uBuf, uint8_t uTxCount);

extern void U3_Init(void);
extern void UART3_IDLE_Callback(void );
extern void U3_DMA_Send(uint8_t* uBuf, uint8_t uTxCount);

extern void U5_Init(void);
extern void UART5_IDLE_Callback(void );
extern void U5_DMA_Send(uint8_t* uBuf, uint8_t uTxCount);

extern void U7_Init(void);
extern void UART7_IDLE_Callback(void );
extern void U7_DMA_Send(uint8_t* uBuf, uint8_t uTxCount);

extern void U8_Init(void);
extern void UART8_IDLE_Callback(void );
extern void U8_DMA_Send(uint8_t* uBuf, uint8_t uTxCount);


extern void Ux_ALL_Init(void );

extern void vUx_IDLE_Callback(UART_HandleTypeDef huartx, DMA_HandleTypeDef DMA_Ux_RX, xUART_t Ux);
extern void vUx_Init(UART_HandleTypeDef huartx, xUART_t Ux);
extern void vUx_DMA_Send(UART_HandleTypeDef huartx, uint8_t* ucBuf, uint8_t ucTxCount);

#endif //end of __BSP_URAT_H__

