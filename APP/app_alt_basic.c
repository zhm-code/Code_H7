#include "math.h"
#include "stdint.h"
#include "stdlib.h"
#include "adc.h"
#include "app_target_test.h"
#include "app_sysparam.h"
#include "app_pps_ctrl.h"
#include "bsp_key.h"


#include "app_GB10847.h"
#include "app_GB10865.h"
#include "app_GB1402S.h"
#include "app_GB3001_GB3002.h"
#include "app_ER17353.h"
#include "app_ZC17353.h"
#include "app_GB5745X.h"
#include "app_TK100.h"
#include "app_IR108175.h"
#include "app_GB5753X.h"
#include "app_GB7639_GB5753Z_GB5745Z.h"
#include "app_GB11579.h"


void vApp_Json_BasicTest(void);
void vApp_Target_ChooseTestTask(uint8_t ucTestMode);

//设置项目执行函数
void vAPP_SetProj(uint32_t RunTime_ms, char * str);
//设置气缸执行函数
void vAPP_SetCylinder(uint32_t RunTime_ms, uint16_t usSetCyl);
//设置负载执行函数
void vAPP_SetLoad(uint32_t RunTime_ms, uint16_t usSetLoad );
//设置开环跑执行函数
void vAPP_RunOpenLoop(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt);
//设置测试完成执行函数
void vAPP_SetTestFinish(uint32_t RunTime_ms,
                        uint16_t Set_Volt);
//设置电机停止
void vAPP_SetMotorStop(uint32_t RunTime_ms,uint16_t Set_Volt);

void vAPP_DrivePowerOn(uint32_t RunTime_ms,uint16_t Set_Volt);

void vAPP_EncoderIdentification(uint32_t RunTime_ms);

void vAPP_SetTestFinish_Basic(void);
//抬升主件_距离测试
void vAPP_RB_Distance_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//抬升主件_AD信号测试
void vAPP_RB_AD_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//抬升主件_过程测试
void vAPP_RB_Process_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//深度清洁_距离测试
void vAPP_DCL_Distance_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//深度清洁_AD信号测试
void vAPP_DCL_AD_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//深度清洁_过程测试
void vAPP_DCL_Process_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//整机脱钩测试
void vAPP_Decoupling_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);
//读取温度
void vAPP_ReadTmp(uint32_t RunTime_ms,uint16_t Set_Volt);
//整机归位测试
void vAPP_Set_Zeor(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt);

void vAPP_motor_set_mode(uint32_t time, 
                    uint16_t _motor_set_mode);

void vAPP_motor_enabled(uint32_t time);

void vAPP_motor_disabled(uint32_t time);

#if (SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3223)
    void vApp_Motor_Integrated_TestProcess(void);
#endif




void vTargetTest_AppLoop(void)
{
    static uint64_t ullTimeFor10ms = 0;
    static uint64_t ullTimeFor100ms = 0;

	//每10ms进入一次
    if(ullBsp_TimeBase_ms() - ullTimeFor10ms >= 10)//100hz
    {
		//更新时间，为下步相减作基准
        ullTimeFor10ms = ullBsp_TimeBase_ms();
		

        App_SystemParam_Process();//系统参数处理

        /****主要差异*****/
		//根据测试任务编号
        vApp_Target_ChooseTestTask(stTargetPara.ucMotorTestNumber);
    }

	
	//每100ms进入一次
    if(ullBsp_TimeBase_ms() - ullTimeFor100ms >= 100)//10hz
    {
		//更新时间，为下步相减作基准
        ullTimeFor100ms = ullBsp_TimeBase_ms();
		
        #if (SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3223)
			Control_ProgramPowerSupply_TX();//控制 程控电源
        #endif
    }

    vGetPressButton();//按键检测
    vGetStartButton();//检测到按键按下，重新启动测试程序

    vApp_ClockTick_1000ms();

}





void vApp_InitTaskStartValue(uint8_t ucSWCode)
{
    if(stTargetPara.CheckStep == 0)
    {
        stTargetPara.ucMotorTestNumber = ucSWCode;
        stTargetPara.ullTestTaskStart_ms =  ullBsp_TimeBase_ms();
        stTargetPara.ucTaskStatus = TASK_RUN_START;
        stTargetPara.CheckStep = 1;//设置步骤开始值
        stTargetPara.PrevCheckStep = 255;
    }
}

void vApp_SetTaskStopValue(uint8_t ucSWCode)
{
    stTargetPara.ucMotorTestNumber = ucSWCode;
    stTargetPara.ullTaskStopTime_ms = ullBsp_TimeBase_ms();
    stTargetPara.ucTaskStatus = TASK_SET_STOP;
    stTargetPara.ullTestSumUseFromStart_ms = 0;
    stTargetPara.CheckStep = 0;
}

void vApp_TaskStop(uint8_t ucTestMode)
{
    if(ullBsp_TimeBase_ms() - stTargetPara.ullTaskStopTime_ms < 1000)
    {
        //vAsp_IsSetAircylinderON(1);//此时继电器闭合,PWM开关控制
        ////ControlMotorStop(0);
    }
    else
    {
        ////MotorPowerSwitch(MOTOR_POWER_OFF);
        //vBsp_SetMotorRotation(SET_MOTOR_CW);
    }
}




void vApp_Target_ChooseTestTask(uint8_t ucTestMode)
{
    struct ST_TASK_PARAM stTask_Trans;


    if(uSysRWParam.usRUN_Mode == BUTTON_CONTROL)
    {
        switch (ucTestMode)
        {
            case 0:
            {
                #ifdef APP_OBJ_GB5023
                strncpy(stTargetPara.strRunModeName, stTargetMotorTask[ucTestMode].ucSetArTaskName);
                vApp_BasicMotorControl(&stTargetMotorTask[ucTestMode], ucIsSetTakStop);
                #endif
            }
            break;

            case 1://
            {

                //RO 结构体 设备名称
                uint8_t ucLength = 0;
                ucLength = snprintf((char *)uSysROParam.ucBuffMachineName, sizeof(uSysROParam.ucBuffMachineName), uSysRWParam.uSetStationName_Flash);


                vApp_Json_BasicTest();


            }
            break;



            default:
                //strncpy(strRunModeName, ">>>NoTask<<<");
                break;
        }
    }
    //自动控制
    else if(uSysRWParam.usRUN_Mode == AUTO_CONTROL)
    {
        stTargetPara.ucTaskStatus = TASK_RUN_START;
        stTargetPara.usSetShowTaskInfoColor = 1;

        if(uSysRWParam.ucSetTaskRUN_FromFlash_NUM <= MAX_TASK_NUM)
        {
            strncpy((char *)stTargetPara.strRunModeName, uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ucSetArTaskName, LCD_MAX_Num);
            stTask_Trans.ucSetTaskNumber          = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ucSetTaskNumber ;
            stTask_Trans.ulSetRunSpeed            = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetRunSpeed ;
            stTask_Trans.usSetBrakePercent        = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].usSetBrakePercent  ;
            stTask_Trans.ulSetCircleTime_ms       = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetCircleTime_ms ;
            stTask_Trans.ulSetRunTimesTick        = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetRunTimesTick ;
            stTask_Trans.ulSetCW_RunTime_ms       = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetCW_RunTime_ms ;
            stTask_Trans.ulSetCW_StopTime_ms      = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetCW_StopTime_ms ;
            stTask_Trans.ulSetCCW_RunTime_ms      = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetCCW_RunTime_ms  ;
            stTask_Trans.ulSetCCW_StopTime_ms     = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ulSetCCW_StopTime_ms ;
            stTask_Trans.ucSetNoStopRunState      = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ucSetNoStopRunState  ;
            stTask_Trans.ucIsSetRunTimesSaveFlash = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ucIsSetRunTimesSaveFlash ;
            stTask_Trans.ucIsUsePID               = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].ucIsUsePID;//是否使用闭环PID
            stTask_Trans.usSetAccDec              = uSysRWParam.arstTask[uSysRWParam.ucSetTaskRUN_FromFlash_NUM].usSetAccDec,//加减速值
            vApp_BasicMotorControl(&stTask_Trans, stTargetPara.ucIsSetTakStop);
        }
    }

    stTargetPara.ucTaskStatusLast = stTargetPara.ucTaskStatus;
}

void vApp_Json_BasicTest(void)
{
    static uint64_t ullTimeForJsonCMD = 0;


    //切换命令时，初始化状态
    if(uSysROParam.usPrevCMD_id != uSysROParam.usSetCMD_id)
    {
        uSysROParam.Current_Status = STATUS_START;
        stTargetPara.CheckStep++;
        stTargetPara.ucTaskStatus = TASK_RUN_START;

    }

    //标记收到Json命令时间
    if(uSysROParam.usTickForRxJson  != uSysROParam.usPrevTickForRxJson)
    {
        uSysROParam.usPrevTickForRxJson = uSysROParam.usTickForRxJson;
        ullTimeForJsonCMD = ullBsp_TimeBase_ms();
    }

    //超时设置 测试停止
    if(ullBsp_TimeBase_ms() - ullTimeForJsonCMD > 1000)
    {
        uSysROParam.usCurrentFunc = FUNC_Default;
        uSysROParam.usPrevCMD_id = 65535;
        uSysROParam.usSetCMD_id = 65535;
    }


    //具体执行函数
    switch(uSysROParam.usCurrentFunc )
    {

        case FUNC_StartButton://读取按键

            strncpy(stTargetPara.strRunModeName, "StartButton", LCD_MAX_Num);
			
			//此处未进 case判断时 stTargetPara.ucPrevStartButtonNum有时员工抢按导致提前改变 只能等于stTargetPara.ucStartButtonNum
			//这里只会发送一次 
            if(stTargetPara.ucPrevStartButtonNum != stTargetPara.ucStartButtonNum)
            {
				
                uSysROParam.Current_Status = STATUS_DONE;//结束
            }
            break;


        //设置项目
        case FUNC_SetProject:

            strncpy(stTargetPara.strRunModeName, "SetProjectName", LCD_MAX_Num);

            //从Flah中获得名称
            vAPP_SetProj(500, uSysRWParam.uSetStationName_Flash);//从JSOn 获得uSysROParam.cProjectName
            break;

        case FUNC_SAVE_RWPARAM_FLASH:

            strncpy(stTargetPara.strRunModeName, "SAVE_RWPARAM_FLASH", LCD_MAX_Num);
            static uint8_t isSetRWPARAM_ToFlash = 0;

			//只进行一次
            if(0 == isSetRWPARAM_ToFlash)
            {
                strncpy(uSysRWParam.uSetStationName_Flash, uSysRWParam.stAppSet.uSetStationName, STATION_NAME_LEN);
                uSysRWParam.IsSetSaveRWPARAM2Flash = SET;
				//一次性锁
                isSetRWPARAM_ToFlash = SET;
            }

            uSysROParam.Current_Status = STATUS_DONE;//结束
            break;

    }



    //具体执行函数
    switch(uSysROParam.usCurrentFunc )
    {
        //默认初始
        case FUNC_Default:
            vAPP_SetTestFinish_Basic();
            break;

        case FUNC_SetCylinder:				//气缸
            // strncpy(stTargetPara.strRunModeName, "SetCyl", LCD_MAX_Num);
            vAPP_SetCylinder(uSysROParam.lSetLoadTimeMS, uSysROParam.usSeClyValue);


            break;
		
		case FUNC_SetLoad:					//设置负载
			
			strncpy(stTargetPara.strRunModeName, "MotorCalibration", LCD_MAX_Num);
            vAPP_SetLoad(uSysROParam.lSetLoadTimeMS,uSysROParam.usSetLoadValue);
            break;

		case FUNC_DrivePowerOn:				//驱动器上电
			
            strncpy(stTargetPara.strRunModeName, "DrivePowerOn", LCD_MAX_Num);
            vAPP_DrivePowerOn(uSysROParam.DrivePowerOn_DATA.RunTime,
							uSysROParam.DrivePowerOn_DATA.SetVol);


            break;
		
		case FUNC_EncoderIdentification:	//编码器识别

            strncpy(stTargetPara.strRunModeName, "EncoderIdentification", LCD_MAX_Num);
            vAPP_EncoderIdentification(uSysROParam.EncoderIdentification_DATA.RunTime);


            break;
		
        case FUNC_RunOpenLoop://开环运行

            strncpy(stTargetPara.strRunModeName, "RunOpenLoop", LCD_MAX_Num);
            vAPP_RunOpenLoop(uSysROParam.lSet_RunOpenLoopTimeMS,
                             uSysROParam.usSet_RunOpenLoopFR,
                             uSysROParam.usSet_RunOpenLoopVolt);

            break;
		
		case FUNC_RB_Distance_Test://抬升主件_距离测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_RB_Distance_Test(uSysROParam.GB1402S_DATA.RB_Set_Test_Time_s,
								  uSysROParam.GB1402S_DATA.RB_Test_FR,
								  uSysROParam.GB1402S_DATA.RB_Set_Test_Volt);

            break;
		
		case FUNC_RB_AD_Test://抬升主件_AD信号测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_RB_AD_Test(uSysROParam.GB1402S_DATA.RB_Set_Test_Time_s,
							uSysROParam.GB1402S_DATA.RB_Test_FR,
							uSysROParam.GB1402S_DATA.RB_Set_Test_Volt);

            break;
		
		case FUNC_RB_Process_Test://抬升主件_过程测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_RB_Process_Test(uSysROParam.GB1402S_DATA.RB_Set_Test_Time_s,
								 uSysROParam.GB1402S_DATA.RB_Test_FR,
								 uSysROParam.GB1402S_DATA.RB_Set_Test_Volt);

            break;
		
		case FUNC_DCL_Distance_Test://深度清洁_距离测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_DCL_Distance_Test(uSysROParam.GB1402S_DATA.DCL_Set_Test_Time_s,
								 uSysROParam.GB1402S_DATA.DCL_Test_FR,
								 uSysROParam.GB1402S_DATA.DCL_Set_Test_Volt);

            break;
		
		case FUNC_DCL_AD_Test://抬升主件_AD信号测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_DCL_AD_Test(uSysROParam.GB1402S_DATA.DCL_Set_Test_Time_s,
							uSysROParam.GB1402S_DATA.DCL_Test_FR,
							uSysROParam.GB1402S_DATA.DCL_Set_Test_Volt);

            break;
		
		case FUNC_DCL_Process_Test://深度清洁_过程测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_DCL_Process_Test(uSysROParam.GB1402S_DATA.DCL_Set_Test_Time_s,
								 uSysROParam.GB1402S_DATA.DCL_Test_FR,
								 uSysROParam.GB1402S_DATA.DCL_Set_Test_Volt);

            break;
		
		case FUNC_Decoupling_Test://整机脱钩测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_Decoupling_Test(uSysROParam.GB1402S_DATA.Whole_Engine_Set_Test_Time_s,
								 uSysROParam.GB1402S_DATA.Whole_Engine_Test_FR,
								 uSysROParam.GB1402S_DATA.Whole_Engine_Set_Volt);

            break;
		
		case FUNC_Set_Zeor://整机归位测试

            strncpy(stTargetPara.strRunModeName, "  ", LCD_MAX_Num);
            vAPP_Set_Zeor(uSysROParam.GB1402S_DATA.Whole_Engine_Set_Test_Time_s,
								 uSysROParam.GB1402S_DATA.Whole_Engine_Test_FR,
								 uSysROParam.GB1402S_DATA.Whole_Engine_Set_Volt);

            break;
		
		case FUNC_SetMotorStop://电机停止

            strncpy(stTargetPara.strRunModeName, "SetMotorStop", LCD_MAX_Num);
            vAPP_SetMotorStop(uSysROParam.lSetMotorStopTimeMS,
							  uSysROParam.usSet_StopVolt);


            break;
		
		case FUNC_ReadTmp://读取温度

            strncpy(stTargetPara.strRunModeName, "ReadTmp", LCD_MAX_Num);
			vAPP_ReadTmp(uSysROParam.lReadTmepTimeMS,uSysROParam.usReadTempVolt);


            break;

        case FUNC_MOTOR_SET_MODE:
            
            vAPP_motor_set_mode(uSysROParam.ui_time, uSysROParam.us_motor_set_mode);

            break;

        case FUNC_MOTOR_ENABLED:

            vAPP_motor_enabled(uSysROParam.ui_time);

            break;

		case FUNC_MOTOR_DISABLED:

            vAPP_motor_disabled(uSysROParam.ui_time);

            break;

        case FUNC_TestFinish://测试结束

            strncpy(stTargetPara.strRunModeName, "TestFinish", LCD_MAX_Num);
            vAPP_SetTestFinish(uSysROParam.lSet_TestFinishTimeMS,
                               uSysROParam.usSet_TestFinishVolt);

            break;
		
		
    }

    uSysROParam.usPrevCMD_id = uSysROParam.usSetCMD_id;
    stTargetPara.ucPrevStartButtonNum = stTargetPara.ucStartButtonNum;
}

//切换命令清除参数
void vClearTestData(void)
{
    uSysROParam.lSetLoadTimeMS = 0;
    uSysROParam.usSetLoadValue = 0;

    uSysROParam.lSet_RunOpenLoopTimeMS = 0;
    uSysROParam.usSet_RunOpenLoopFR = 0;
    uSysROParam.usSet_RunOpenLoopVolt = 0;

    uSysROParam.fRunOpenLoop_Time_s = 0;
    uSysROParam.fRunOpenLoop_Volt = 0;
    uSysROParam.fRunOpenLoop_spd_dev = 0;
    uSysROParam.fRunOpenLoop_spd_motor = 0;
    uSysROParam.fRunOpenLoop_Curr = 0;

    uSysROParam.lSet_TestFinishTimeMS = 0;
    uSysROParam.usSet_TestFinishVolt = 0;
}

/*-----------------------------------------------------------------------------------*
*																					 																					 \
*									项目任务函数												 															 \
*																					 																					 \
*-----------------------------------------------------------------------------------*/

//设置项目
void vAPP_SetProj(uint32_t RunTime_ms, char * str)
{
    uint64_t ullCurrent_ms; //当前时间，相对于任务开始时

    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {

    }
    else
    {
        //寻找匹配项目名称赋予项目编号
        if (strcmp("GB10847-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB10847;
        }
		
		else if (strcmp("GB10865-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB10865;
        }
		
		else if (strcmp("GB1402S-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB1402S;
        }
		
		else if (strcmp("GB3001-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB3001;
        }
		
		else if (strcmp("GB3002-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB3002;
        }
		
		else if (strcmp("GB5023-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB5023;
        }
		
		else if (strcmp("ER17353-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = ER17353;
        }
		
		else if (strcmp("ZC17353-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = ZC17353;
        }
		else if (strcmp("GB5745X-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB5745X;
        }
		
		else if (strcmp("GB5753X-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB5753X;
        }
		
		else if (strcmp("TK100-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = TK100;
        }
        
		else if (strcmp("IR108175-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = IR108175;
        }

        else if (strcmp("GB7639-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB7639;
        }
				
        else if (strcmp("GB5753Z-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB5753Z;
        }
				
        else if (strcmp("GB5745Z-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB5745Z;
        }

        else if (strcmp("GB11579-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB11579;
        }

        else if (strcmp("GB6816-Integrate", str) == 0)
        {
            uSysROParam.ProjNumber = GB6816;
        }
		
        else
        {
            uSysROParam.ProjNumber = ProjNULL;
        }
		
		
        //是否寻找到任务
        if (uSysROParam.ProjNumber != ProjNULL)
        {
            uSysROParam.Current_Status = STATUS_DONE;//结束
        }
        else
        {
            uSysROParam.Current_Status = STATUS_OUTTIME;
            //如果没有下一个命令，执行停止流程
        }
    }

}

//设置电机停止
void vAPP_SetMotorStop(uint32_t RunTime_ms,uint16_t Set_Volt)
{
    switch ((uint16_t)uSysROParam.ProjNumber)
	{
		case TK100:
			vAPP_SetMotorStop_TK100(RunTime_ms,Set_Volt);
			break;
		case GB5745X:
			vAPP_SetMotorStop_GB5745X(RunTime_ms,Set_Volt);
			break;
		case GB5753X:
			vAPP_SetMotorStop_GB5753X(RunTime_ms,Set_Volt);
			break;
		case IR108175:
			vAPP_SetMotorStop_IR108175(RunTime_ms,Set_Volt);
			break;
        case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
			vAPP_SetMotorStop_GB7639_GB5753Z_GB5745Z(RunTime_ms,Set_Volt);
			break;
        case GB11579:
			vAPP_SetMotorStop_GB11579(RunTime_ms,Set_Volt);
			break;  

	}
}

//读取温度
void vAPP_ReadTmp(uint32_t RunTime_ms,uint16_t Set_Volt)
{
    switch ((uint16_t)uSysROParam.ProjNumber)
	{
		case GB5745X:
			vAPP_ReadTmp_GB5745X(RunTime_ms,Set_Volt);
			break;
		case GB5753X:
			vAPP_ReadTmp_GB5745X(RunTime_ms,Set_Volt);
			break;
		case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
			vAPP_ReadTmp_GB7639_GB5753Z_GB5745Z(RunTime_ms,Set_Volt);
			break;
	}
}

//设置负载
void vAPP_SetLoad(uint32_t RunTime_ms, uint16_t usSetLoad )
{
	switch (uSysROParam.ProjNumber)
    {
		case GB10847:
        {
            vAPP_SetLoad_GB10847(RunTime_ms, usSetLoad);
        }
        break;
		
		case GB10865:
        {
            vAPP_SetLoad_GB10865(RunTime_ms, usSetLoad);
        }
        break;
		
		case GB3001:
		case GB3002:
        {
            vAPP_SetLoad_GB3001_GB3002(RunTime_ms, usSetLoad);
        }
        break;
		
		case GB5023:
        {
//            vAPP_SetLoad_GB5023(RunTime_ms, usSetLoad);
        }
        break;
		
		case ER17353:
        {
            vAPP_SetLoad_ER17353(RunTime_ms, usSetLoad);
        }
        break;
		
		case GB5745X:
        {
            vAPP_SetLoad_GB5745X(RunTime_ms, usSetLoad);
        }
        break;
		
		case GB5753X:
        {
            vAPP_SetLoad_GB5753X(RunTime_ms, usSetLoad);
        }
        break;
		
		case TK100:
        {
            vAPP_SetLoad_TK100(RunTime_ms, usSetLoad);
        }
        break;
		
		case IR108175:
        {
            vAPP_SetLoad_IR108175(RunTime_ms, usSetLoad);
        }
        break;

        case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
        {
            vAPP_SetLoad_GB7639_GB5753Z_GB5745Z(RunTime_ms, usSetLoad);
        }
        break;

        case GB11579:
        {
            vAPP_SetLoad_GB11579(RunTime_ms, usSetLoad);
        }
        break;

        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;
	}
}

//设置气缸
void vAPP_SetCylinder(uint32_t RunTime_ms, uint16_t usSetCyl)
{
    switch (uSysROParam.ProjNumber)
    {
		case GB10847:
        {
            vAPP_SetCylinder_GB10847(RunTime_ms, usSetCyl);
        }
        break;
		
		case GB10865:
        {
            vAPP_SetCylinder_GB10865(RunTime_ms, usSetCyl);
        }
        break;
		
		case GB3001:
		case GB3002:
        {
            vAPP_SetCylinder_GB3001_GB3002(RunTime_ms, usSetCyl);
        }
        break;
		
		case GB1402S:
        {
            vAPP_SetCylinder_GB1402S(RunTime_ms, usSetCyl);
        }
        break;

        case GB11579:
        {
            vAPP_SetCylinder_GB11579(RunTime_ms, usSetCyl);
        }
        break;

        case GB6816:
        {
            vAPP_SetCylinder_GB6816(RunTime_ms, usSetCyl);
        }
        break;



        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;
	}
}

//启动器上电
void vAPP_DrivePowerOn(uint32_t RunTime_ms,uint16_t Set_Volt)
{
    switch ((uint16_t)uSysROParam.ProjNumber)
	{
		case GB10847:
			vAPP_DrivePowerOn_GB10847(RunTime_ms,Set_Volt);
			break;
		case GB10865:
			vAPP_DrivePowerOn_GB10865(RunTime_ms,Set_Volt);
			break;
		case ER17353:
			vAPP_DrivePowerOn_ER17353(RunTime_ms,Set_Volt);
			break;
		case ZC17353:
			vAPP_DrivePowerOn_ZC17353(RunTime_ms,Set_Volt);
			break;
		case GB5745X:
			vAPP_DrivePowerOn_GB5745X(RunTime_ms,Set_Volt);
			break;
		case GB5753X:
			vAPP_DrivePowerOn_GB5753X(RunTime_ms,Set_Volt);
			break;
		case IR108175:
            vAPP_DrivePowerOn_IR108175(RunTime_ms, Set_Volt);
        break;
        case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
            vAPP_DrivePowerOn_GB7639_GB5753Z_GB5745Z(RunTime_ms, Set_Volt);
        case GB11579:
            vAPP_DrivePowerOn_GB11579(RunTime_ms, Set_Volt);
        break;
	}
}

//编码器识别
void vAPP_EncoderIdentification(uint32_t RunTime_ms)
{
    switch ((uint16_t)uSysROParam.ProjNumber)
	{
		case GB10847:
			vAPP_EncoderIdentification_GB10847(RunTime_ms);
			break;
		case GB10865:
			vAPP_EncoderIdentification_GB10865(RunTime_ms);
			break;
	}
}

//设置开环运行
void vAPP_RunOpenLoop(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    //匹配项目
    switch (uSysROParam.ProjNumber)
    {
		case GB10847:
        {
            vAPP_RunOpenLoop_GB10847(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case GB10865:
        {
            vAPP_RunOpenLoop_GB10865(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case GB1402S:
        {
            vAPP_RunOpenLoop_GB1402S(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case GB3001:
		case GB3002:
        {
            vAPP_RunOpenLoop_GB3001_GB3002(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case GB5023:
        {
//            vAPP_RunOpenLoop_GB5023(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case ER17353:
        {
            vAPP_RunOpenLoop_ER17353(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case ZC17353:
        {
            vAPP_RunOpenLoop_ZC17353(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case GB5745X:
        {
            vAPP_RunOpenLoop_GB5745X(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case GB5753X:
        {
            vAPP_RunOpenLoop_GB5753X(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;
		
		case TK100:
        {
            vAPP_RunOpenLoop_TK100(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;

		case IR108175:
        {
            vAPP_RunOpenLoop_IR108175(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;

        case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
        {
            vAPP_RunOpenLoop_GB7639_GB5753Z_GB5745Z(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;

        case GB11579:
        {
            vAPP_RunOpenLoop_GB11579(RunTime_ms, ucSetMotorRotation, Set_Volt);

        }
        break;

        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;

    }
}

//抬升主件_距离测试
void vAPP_RB_Distance_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_RB_Distance_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//抬升主件_AD信号测试
void vAPP_RB_AD_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_RB_AD_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//抬升主件_过程测试
void vAPP_RB_Process_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_RB_Process_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//深度清洁_距离测试
void vAPP_DCL_Distance_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_DCL_Distance_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//深度清洁_AD信号测试
void vAPP_DCL_AD_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_DCL_AD_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//深度清洁_过程测试
void vAPP_DCL_Process_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_DCL_Process_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//整机脱钩测试
void vAPP_Decoupling_Test(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_Decoupling_Test_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//整机归位测试
void vAPP_Set_Zeor(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	switch (uSysROParam.ProjNumber)
    {
    	case GB1402S:
    	{
    		vAPP_Set_Zeor_GB1402S(RunTime_ms,ucSetMotorRotation,Set_Volt);
    	}
    	break;

    }
}

//电机设置模式
void vAPP_motor_set_mode(uint32_t time, 
                    uint16_t _motor_set_mode)
{
    switch (uSysROParam.ProjNumber)
    {
		case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
        {
            vAPP_motor_set_mode_GB7639_GB5753Z_GB5745Z(time,_motor_set_mode);
        }
        break;

        case GB11579:
        {
            vAPP_motor_set_mode_GB11579(time,_motor_set_mode);
        }
        break;
		

        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;
	}
}

void vAPP_motor_enabled(uint32_t time)
{
    switch (uSysROParam.ProjNumber)
    {
		case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
        {
            vAPP_motor_enabled_GB7639_GB5753Z_GB5745Z(time);
        }
        break;
		

        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;
	}
}

void vAPP_motor_disabled(uint32_t time)
{
    switch (uSysROParam.ProjNumber)
    {
		case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
        {
            vAPP_motor_disabled_GB7639_GB5753Z_GB5745Z(time);
        }
        break;
		

        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;
	}
}

//结束运行
void vAPP_SetTestFinish(uint32_t RunTime_ms,
                        uint16_t Set_Volt)
{

    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时

    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP ||
            stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();//本函数内部
        stTargetPara.ullMultiStepStart_ms = ullBsp_TimeBase_ms();//上传
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;
    //计算分类任务经历时间
    stTargetPara.ullMultiStepCurrent_ms =  ullBsp_TimeBase_ms() - stTargetPara.ullMultiStepStart_ms;
    uSysROParam.fRunOpenLoop_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
        vAPP_SetTestFinish_Basic();
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
        stTargetPara.ucTaskStatus = TASK_SET_STOP;
        //如果没有下一个命令，执行停止流程
    }
}

//结束运行
void vAPP_SetTestFinish_Basic(void)
{
    //匹配项目
    switch (uSysROParam.ProjNumber)
    {
		case GB10847:
        {
            vAPP_SetTestFinish_GB10847();

        }
        break;
		
		case GB10865:
        {
            vAPP_SetTestFinish_GB10865();

        }
        break;
		
		case GB1402S:
        {
            vAPP_SetTestFinish_GB1402S();

        }
        break;
		
		case GB3001:
		case GB3002:
        {
            vAPP_SetTestFinish_GB3001_GB3002();

        }
        break;
		
		case GB5023:
        {
//            vAPP_SetTestFinish_GB5023();

        }
        break;
		
		case ER17353:
        {
            vAPP_SetTestFinish_ER17353();

        }
        break;
		
		case ZC17353:
        {
            vAPP_SetTestFinish_ZC17353();

        }
        break;
		
		case GB5745X:
        {
            vAPP_SetTestFinish_GB5745X();

        }
        break;
		
		case GB5753X:
        {
            vAPP_SetTestFinish_GB5753X();

        }
        break;
		
		case TK100:
        {
            vAPP_SetTestFinish_TK100();

        }
        break;
		
		case IR108175:
        {
            vAPP_SetTestFinish_IR108175();

        }
        break;

        case GB7639:
        case GB5745Z:
        case GB5753Z:
        case GB6816:
        {
            vAPP_SetTestFinish_GB7639_GB5753Z_GB5745Z();

        }
        break;

        case GB11579:
        {
            vAPP_SetTestFinish_GB11579();

        }
        break;
		
        default:
        {
            //错误处理
            uSysROParam.Current_Status = STATUS_OUTTIME;

        }
        break;

    }
}


struct ST_TASK_PARAM stTask_Trans;
void  vApp_BasicTask_ByFlashSetPara(void)
{
    if(uSysRWParam.ucSetTaskRUN_FromFlash_NUM <= MAX_TASK_NUM)
    {
        //strncpy(stTargetPara.strRunModeName, uSysRWParam.stBasicTask.ucSetArTaskName);
        for(uint8_t Tick = 0; Tick < LCD_MAX_Num; Tick = Tick + 2)
        {
            stTargetPara.strRunModeName[Tick] = uSysRWParam.stBasicTask.ucSetArTaskName[Tick + 1];
            stTargetPara.strRunModeName[Tick + 1] = uSysRWParam.stBasicTask.ucSetArTaskName[Tick];
        }

        stTask_Trans.ucSetTaskNumber          = uSysRWParam.stBasicTask.ucSetTaskNumber ;
        stTask_Trans.ulSetRunSpeed            = uSysRWParam.stBasicTask.ulSetRunSpeed ;
        stTask_Trans.usSetBrakePercent        = uSysRWParam.stBasicTask.usSetBrakePercent  ;
        stTask_Trans.ulSetCircleTime_ms       = uSysRWParam.stBasicTask.ulSetCircleTime_ms ;
        stTask_Trans.ulSetRunTimesTick        = uSysRWParam.stBasicTask.ulSetRunTimesTick ;
        stTask_Trans.ulSetCW_RunTime_ms       = uSysRWParam.stBasicTask.ulSetCW_RunTime_ms ;
        stTask_Trans.ulSetCW_StopTime_ms      = uSysRWParam.stBasicTask.ulSetCW_StopTime_ms ;
        stTask_Trans.ulSetCCW_RunTime_ms      = uSysRWParam.stBasicTask.ulSetCCW_RunTime_ms  ;
        stTask_Trans.ulSetCCW_StopTime_ms     = uSysRWParam.stBasicTask.ulSetCCW_StopTime_ms ;
        stTask_Trans.ucSetNoStopRunState      = uSysRWParam.stBasicTask.ucSetNoStopRunState  ;
        stTask_Trans.ucIsSetRunTimesSaveFlash = uSysRWParam.stBasicTask.ucIsSetRunTimesSaveFlash ;
        stTask_Trans.ucIsUsePID               = uSysRWParam.stBasicTask.ucIsUsePID;//是否使用闭环PID
        stTask_Trans.usSetAccDec              = uSysRWParam.stBasicTask.usSetAccDec;//加减速值


        vApp_BasicMotorControl(&stTask_Trans, stTargetPara.ucIsSetTakStop);
    }
}






