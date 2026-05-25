#ifndef __APP_ALT_BASICK_H
#define __APP_ALT_BASICK_H

#include "app_user_main.h"
#include "app_target_test.h"

#define TASK_SET_STOP 0
#define TASK_RUN_START 1
#define TASK_RUN_CW_RUN 2
#define TASK_RUN_CW_STOP 3
#define TASK_RUN_CCW_RUN 4
#define TASK_RUN_CCW_STOP 5
#define TASK_RUN_FINISH 6

#pragma pack(2) //设置2字节对齐
#pragma pack(push,1)
struct ST_TASK_PARAM
{
	uint8_t ucSetArTaskName[26];//任务名称  ADDR0-13
	uint16_t ucSetTaskNumber;//任务编号      ADDR14	
	
	uint32_t ulSetRunSpeed;//设定转速       ADDR15-16
    uint16_t usSetBrakePercent;//设定负载   ADDR17
    uint32_t ulSetCircleTime_ms;//周期时间  ADDR18-19
    uint32_t ulSetRunTimesTick;//周期运行次数 ADDR20-21
    uint32_t ulSetCW_RunTime_ms;//正转运行时间 ADDR22-23
	uint32_t ulSetCW_StopTime_ms;//正转停止时间 ADDR24-25
	uint32_t ulSetCCW_RunTime_ms;//反转运行时间 ADDR26-27
	uint32_t ulSetCCW_StopTime_ms;//反转停止时间 ADDR28-29 
	
    uint8_t  ucSetNoStopRunState;//是否连续运行 0:不连续 1:连续正转 2:连续反转  ADDR30
	uint8_t  ucIsSetRunTimesSaveFlash;//是否保存当前运行次数 
	
	uint16_t  ucIsUsePID;//是否使用闭环PID ADDR31	
	
	uint16_t usSetAccDec;//加减速值 ADDR32
};
#pragma pack(pop)
#pragma pack() //恢复编译器默认对齐

void vApp_ChooseTestTask(uint8_t ucTestMode);

void vApp_InitTaskStartValue(uint8_t switchCode);
void vApp_TaskStop(uint8_t ucTestMode);
void vApp_SetTaskStopValue(uint8_t ucSWCode);

#define LIMIT_SPEED_TEST 2950
//#define LIMIT_SPEED_TEST 3100

#define MAX_RUNMODE_NAME_LEN 26

void vApp_BasicMotorControl(struct ST_TASK_PARAM *stTask,uint8_t ucIsSetStop);


void vGetMotorFeedbackSpeed(void);
//系统参数处理
void App_SystemParam_Process(void);
//系统故障处理
void App_SystemFault_Process(void);
//void vGetPressButton(void);
//void vAPP_Usart_Process(void);

void vApp_BaseChooseTestTask(uint8_t ucTestMode);

void vBasicTask_AppInit(char *pTarget);
void vBasicTask_AppLoop(void);

//任务相关函数
void vApp_BasicTask_ByFlashSetPara(void);

void vClearTestData(void);

extern void vTargetTest_AppLoop(void);


#endif /* __APP_TASK_H */