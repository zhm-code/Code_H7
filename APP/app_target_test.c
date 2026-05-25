#include "app_sysparam.h"

//结构体定义初始化
struct ST_TARGET_TEST_PARAM stTargetPara =
{
    .CheckStep = 0,//当前测试步骤
    .PrevCheckStep = 255,//上一个测试步骤
    .ucCurrentStation = 1,//当前测试站编号
    .ucEndurenceTestStep = 0,//寿命测试步骤
	.ucMotorTestNumber = 0,//测试任务编号
	.ullTestTaskStart_ms = 0,//测试任务开始时间
	.ullTestSumUseFromStart_ms = 0,//测试任务从开始经过的总时间
	.ullTaskStopTime_ms = 0, //任务停止时间        
    .ullTestStepStart_ms = 0,//测试步骤开始时间
    .ulTaskClock_1s_ForCycle = 0,//任务计时器运行时间
    .ucSetEnTaskClockRun_1s = 0,//任务计时运行停止标记 0、
	.ulRunTimesTick = 0,//运行次数计数
	.fMotorSpeed = 0,//电机反馈速度当前值
    .usMotorSpeedMax = 0,//电机反馈速度最大值
    .Pre_usMotorSpeed = 0,//上一次电机反馈速度
	.ucTaskStatus = TASK_SET_STOP,//任务状态：启动，停止
    .ucTaskStatusLast = TASK_SET_STOP,//上一次任务状态：启动，停止	
	.ucIsSetTakStop = 1,//是否设置任务停止    
	.switchCode = INIT_SWITCHCODE,//按键关联的task号
    .switchState = 0, //按键状态 短按、长按
	.ucStartButtonNum = 0,//启动按键编号，按一次加1
	.usEnStartButtonAdd = 1,//使能启动按键编号增加
	.usSetShowTaskInfoColor = 0,//设置LCD屏幕显示的任务名称的颜0色	
	.cLCDShowStr = {' '},//LCD的显示字符串
	.strRunModeName = {' '},//LCD显示的任务模式名称
	.strRunStepName = {' '},//LCD显示的步骤名称
	.fMotorVoltageVal = 0.0,//母线电压
	.fMotorCurrentVal = 0.0,//母线电流
	.ullMultiStepStart_ms = 0,  //分类任务启动时间
    .ullMultiStepCurrent_ms = 0, //分类任务时间
	.PPS_Current_MOS_Base = 0,//程控电源电流MOS损耗
	.PPS_Current_Machine_Base = 0,//程控电源电流治具损耗
};





void vTargetTest_AppInit(char *pTarget)
{
    
}


void vApp_LCD_Update_TargetTest(void)
{
    
}




//开始测试前	综合测试寄存器参数置0
void vApp_Parameter_Clear(void)
{
	
}