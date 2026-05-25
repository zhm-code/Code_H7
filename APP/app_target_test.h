#ifndef __APP_TARGET_TEST_H
#define __APP_TARGET_TEST_H
#include "main.h"
#include "app_user_main.h"
#define LCD_MAX_LINES (6)
#define LCD_MAX_Num (26)

/*************通用部分 开始 *********************/
struct ST_TARGET_TEST_PARAM
{
    uint8_t CheckStep;//当前测试步骤
    uint8_t PrevCheckStep;//上一个测试步骤
    uint8_t ucCurrentStation ;//当前测试站编号
    uint8_t ucEndurenceTestStep;//寿命测试步骤
	uint8_t ucMotorTestNumber;//测试任务编号
	uint64_t ullTestTaskStart_ms;//测试任务开始时间
	uint64_t ullTestSumUseFromStart_ms;//测试任务从开始经过的总时间 
  	uint64_t ullTaskStopTime_ms;//任务停止时间   
    uint64_t ullTestStepStart_ms;//测试步骤开始时间    
	uint32_t ulTaskClock_1s_ForCycle;//任务计时器运行时间
    uint8_t ucSetEnTaskClockRun_1s;//任务计时器行使能标记 0、1
	uint32_t ulRunTimesTick;//运行次数计数
	float fMotorSpeed;//电机反馈速度当前值
	float fFG_MotorSpeed;//电机反馈速度当前值
    uint16_t usMotorSpeedMax;//电机反馈速度最大值
    uint16_t Pre_usMotorSpeed;//上一次电机反馈速度
	uint8_t ucTaskStatus ;//任务状态：启动，停止
    uint8_t ucTaskStatusLast;//上一次任务状态：启动，停止	
	uint8_t ucIsSetTakStop;//是否设置任务停止    
	uint8_t switchCode;//按键关联的task号
    uint8_t switchState; //按键状态 短按、长按
	uint8_t ucStartButtonNum;//启动按键编号，按一次加1
	uint8_t usEnStartButtonAdd;//使能启动按键编号增加
	uint8_t usSetShowTaskInfoColor;//设置LCD屏幕显示的任务名称的颜色	
	char cLCDShowStr[LCD_MAX_Num];//LCD的显示字符串
	uint8_t strRunModeName[LCD_MAX_Num];//LCD显示的任务模式名称
	uint8_t strRunStepName[LCD_MAX_Num];//LCD显示的步骤名称
	float fMotorVoltageVal;//母线电压
	float fMotorCurrentVal;//母线电流
	uint64_t ullMultiStepStart_ms; //分类任务启动时间
    uint64_t ullMultiStepCurrent_ms; //分类任务时间
	uint16_t PPS_Current_MOS_Base;//程控电源电流MOS损耗
	uint16_t PPS_Current_Machine_Base;//程控电源电流治具损耗
	
	uint8_t ucPrevStartButtonNum;//上一个启动按键编号
};

extern struct ST_TARGET_TEST_PARAM stTargetPara;


void vApp_GB3220_EndurenceTest( uint32_t ulCW_RunTime_ms,
    uint32_t ulCW_StopTime_ms,
    uint32_t ulCCW_RunTime_ms,
    uint32_t ulCCW_StopTime_ms,
    uint32_t ulSetRunTimesTick,
    uint8_t ucIsNoStopRun,
    uint8_t ucIsEnWriteFlash);

void vApp_GB3223_EndurenceTest( uint32_t ulCW_RunTime_ms,
    uint32_t ulCW_StopTime_ms,
    uint32_t ulCCW_RunTime_ms,
    uint32_t ulCCW_StopTime_ms,
    uint32_t ulSetRunTimesTick,
    uint8_t ucIsNoStopRun,
    uint8_t ucIsEnWriteFlash);

void vTargetTest_AppInit(char *pTarget);
void vApp_LCD_Update_TargetTest(void);
void vApp_Parameter_Clear(void);
#endif
