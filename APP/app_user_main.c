#include "app_user_main.h"
#include "app_sysparam.h"
#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_main.h"
#include "debuger.h"
#include "bsp_cpu_flash.h"



uint16_t usTickForFlash = 0;
uint32_t uCurrentWriteAddrOffset = 0;

static uint32_t ulReadFromFlashData = 0;

//记录次数进flash里面
void vApp_SaveValue2Flash(uint32_t ulValue)
{
	//写入次数到FLASH里面  32表示H7最少写32个字节进去
    bsp_WriteCpuFlash(ADDR_FLASH_SECTOR_5_BANK2 + uCurrentWriteAddrOffset * 32, (uint8_t*)&ulValue, 4);
    //地址偏移
	uCurrentWriteAddrOffset++;

	//如果地址到底重新回头写
    if(uCurrentWriteAddrOffset == 0x1FFE0)
    {
		//擦除整个扇区
		bsp_EraseCpuFlash((uint32_t)ADDR_FLASH_SECTOR_5_BANK2);
        uCurrentWriteAddrOffset = 0;
    }
}

void vApp_RestValue2Flash(void)
{
//    usTickForFlash = 0;
//    uCurrentWriteAddrOffset = usTickForFlash;
//    stTargetPara.ulRunTimesTick = 0XFFFFFFFF;
//    STMFLASH_Write(FLASH_SAVE_ADDR, &stTargetPara.ulRunTimesTick, 1);

//    stTargetPara.ulRunTimesTick = 0;
}

//程序开始只会初始一次flash参数
void vApp_InitParaFromFlash(void)
{
	//读取整个扇区
    for(usTickForFlash = 0; usTickForFlash < DEF_RUNTIMESTICKVALUE_SUMNUM; usTickForFlash++)
    {
		//一次读取4个字节
        bsp_ReadCpuFlash(ADDR_FLASH_SECTOR_5_BANK2 + usTickForFlash * 4, (uint8_t *)&ulReadFromFlashData, 4);
		
		//检测是否清空
        if(ulReadFromFlashData == 0XFFFFFFFF)
        {
			//第一次进入进行重置
            if(usTickForFlash == 0)
            {
				//重置运行次数
                stTargetPara.ulRunTimesTick = 0;
            }
			
			//更新地址
            uCurrentWriteAddrOffset = usTickForFlash;
            break;
        }
        else
        {
			//读取FLASH里次数
            stTargetPara.ulRunTimesTick = ulReadFromFlashData;
        }
    }
}

void vApp_UserInit(void)
{ 
	//初始化循找上次运行的次数
    vApp_InitParaFromFlash();
    App_Init_SystemParam();
	
	//测试任务编号
	stTargetPara.ucMotorTestNumber = stTargetPara.switchCode;
	
    //设置开机运行任务
    if(SET_POWERON_RUN == 1)
    {
        stTargetPara.usSetShowTaskInfoColor = 1;
        stTargetPara.switchState = 2;
        vApp_InitTaskStartValue(stTargetPara.switchCode);
        stTargetPara.ucIsSetTakStop = 0;
    }
}
/**
  * @Name    vAsp_LCD_ShowInit
  * @brief
  * @param   None
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-06-07
 **/
void  vApps_Init(void)
{

    switch (SET_TARGETTYPE)
    {
       
     case APP_OBJ_GB3220:
			vTargetTest_AppInit((char*)uSysRWParam.uSetStationName_Flash);
            break;
     
        default:
            break;

    }
}

//用户任务循环
void vApp_UserLoop(void)
{
    switch (SET_TARGETTYPE)
    {       
        case APP_OBJ_GB3220:
            vTargetTest_AppLoop();
            break;
     	
        default:
            break;

    }

}
static float pre_data[10], output[10];
float LowPass_Filter(uint8_t ID, float force, float input)
{
    output[ID] = force * pre_data[ID] + (1.0 - force) * input;
    pre_data[ID] = output[ID];

    return output[ID];
}
void setLowPassInitValue(uint8_t ID, float value)
{
    pre_data[ID] = value;
}
void CleanLowPassValue(uint8_t ID)
{
    output[ID] = 0;
    pre_data[ID] = 0;
}




/**
  * @Name    ullBsp_TimeBase_ms
  * @brief   返回毫秒级时间基准
  * @param   None
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-05-19
 **/
uint64_t ullBsp_TimeBase_ms(void)
{
    static uint32_t ulLastCurrentTime = 0;
    static uint64_t ullTimeBase_ms = 0;
	//ullSYS_time_Count 与os同步每ms 自加1
//    uint32_t ulCurrentTime = ullSYS_time_Count;

//    if(ulCurrentTime < ulLastCurrentTime)
//    {
//        ullTimeBase_ms = ullTimeBase_ms + 0x100000000;
//    }

//    ulLastCurrentTime = ulCurrentTime;
//    return 	(ullTimeBase_ms + ulCurrentTime);
	return ullSYS_time_Count;
}
/**
  * @Name    ullBsp_TimeBase_us
  * @brief   返回微秒级时间基准
  * @param   None
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-05-19
 **/
uint64_t ullBsp_TimeBase_us(void)
{
    return  ullBsp_TimeBase_ms() * 1000 + htim4.Instance->CNT;
}


/**
  * @Name    vApp_ClockTick_1000ms
  * @brief
  * @param   None
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-06-07
 **/

void vApp_ClockTick_1000ms(void)
{
    static uint64_t ullTimeFor1000ms;

    if(ullBsp_TimeBase_ms() - ullTimeFor1000ms >= 1000)
    {
        ullTimeFor1000ms = ullBsp_TimeBase_ms();

        if(stTargetPara.ucSetEnTaskClockRun_1s)
        {
            stTargetPara.ulTaskClock_1s_ForCycle ++;
        }

        if(stTargetPara.switchState == 1 || stTargetPara.switchState == 0)
        {
            stTargetPara.ulTaskClock_1s_ForCycle = 0;
        }
    }
}



uint32_t ulSetMotorSpeed_Test = 0;
void vApp_SetMotorControl(uint8_t isSetMotorPowerON,
                          uint8_t ucSetNoStopRun,
                          uint8_t ucSetMotorRotation,
                          uint32_t ulSetMotorSpeed,
                          uint16_t usSetBrakePWM,
                          uint8_t ucIsUsePID,
                          uint16_t usSetAccDec)
{
    int16_t sTempMotorSpd = 0;

//	if(isSetMotorPowerON == SET_MOTOR_PWR_ON)
//    {
//        vBsp_IsSetMotorPowerON(0);
//    }
//    else
//    {
//        //vBsp_IsSetMotorPowerON(1);
//    }

    //TODO 考虑电机方向定义不同
    if(usSetAccDec != 0)
    {
        //TODO 加速度值与减速度值分开
        //加速过程
        if(ulSetMotorSpeed  >= stTargetPara.fMotorSpeed)
        {
            if(ulSetMotorSpeed  > stTargetPara.fMotorSpeed + usSetAccDec )
            {
                ulSetMotorSpeed = stTargetPara.fMotorSpeed + usSetAccDec;
            }
        }
        else
        {
            //减速过程
            sTempMotorSpd = stTargetPara.fMotorSpeed - usSetAccDec;

            if(sTempMotorSpd < 0)
            {
                ulSetMotorSpeed = 0;
            }
            else if(ulSetMotorSpeed  < stTargetPara.fMotorSpeed - usSetAccDec)
            {
                ulSetMotorSpeed = stTargetPara.fMotorSpeed - usSetAccDec;
            }
        }
    }

    ulSetMotorSpeed_Test = ulSetMotorSpeed;

	 if(ucSetNoStopRun == 0)
    {
        //ControlMotorDirection(fApp_MotorSpeedPID_Control(ulSetMotorSpeed)*100,ucSetMotorRotation);
    }
    else if(ucSetNoStopRun == 1)
    {
        //ControlMotorDirection(fApp_MotorSpeedPID_Control(ulSetMotorSpeed)*100,CLOCKWISE);
    }
    else
    {
        //ControlMotorDirection(fApp_MotorSpeedPID_Control(ulSetMotorSpeed)*100,ANTICLOCKWISE);
    }
  

}

uint16_t usPara_P = 60;
uint16_t usPara_I = 0;
uint16_t usPara_D = 4;
//uint16_t usSetMotorAcc = 300;
//uint16_t usSetMotorDec = 300;
//uint16_t usSetMotorSpeed    = 2000;
float    fOutPutMotorPWM = 0;
/**
  * @Name    fApp_MotorSpeedPID_Control
  * @brief
  * @param   usSetSpeed: [输入/出]
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-06-07
 **/

int16_t  sLastMotorSpeedError = 0;
int32_t  sMotorSpeedErrorSum = 0;
int32_t  sMotorSpeedErrorDt = 0;
float fApp_MotorSpeedPID_Control(uint16_t usSetSpeed)
{
    static int16_t  sMotorSpeedError = 0;

//	//加速过程
//	if(usSetSpeed  >= usMotorSpeed)
//    {
//        if(usSetSpeed  > usMotorSpeed + usSetMotorAcc )
//        {
//            usSetSpeed = usMotorSpeed + usSetMotorAcc;
//        }
//    }
//    else
//    {
//        //减速过程
//        usSetSpeed = usMotorSpeed - usSetMotorDec;

//        if(usSetSpeed < 0)
//        {
//            usSetSpeed = 0;
//        }
//        else if(usSetSpeed  < usMotorSpeed - usSetMotorDec)
//        {
//            usSetSpeed = usMotorSpeed - usSetMotorDec;
//        }
//    }

    sMotorSpeedError    = usSetSpeed - stTargetPara.fMotorSpeed; //误差
    sMotorSpeedErrorSum = sMotorSpeedError + sMotorSpeedErrorSum;//误差积分
    sMotorSpeedErrorDt  = sMotorSpeedError - sLastMotorSpeedError;//误差微分

    //积分限制
    if(sMotorSpeedErrorSum > 1000 )
    {
        sMotorSpeedErrorSum = 1000;
    }

    if(sMotorSpeedErrorSum < -1000)
    {
        sMotorSpeedErrorSum = -1000;
    }

    fOutPutMotorPWM += (usPara_P * sMotorSpeedError +
                        usPara_I * sMotorSpeedErrorSum +
                        usPara_D * sMotorSpeedErrorDt) / 10000000.0;

    sLastMotorSpeedError = sMotorSpeedError;

    if(fOutPutMotorPWM > 1)
    {
        fOutPutMotorPWM = 1;
    }
    else if(fOutPutMotorPWM < 0)
    {
        fOutPutMotorPWM = 0;
    }

    return fOutPutMotorPWM;
}
void vApp_ClearPID_OutPut(void)
{
    fOutPutMotorPWM = 0;
	sLastMotorSpeedError = 0;
    sMotorSpeedErrorSum = 0;
    sMotorSpeedErrorDt = 0;
}

void vApp_MotorRUN_Status(void)
{
    if(0)
    {
        //电机运行状态判断
        if(stTargetPara.fMotorSpeed > 500 )
        {

            //LCDShowRunInfo(ucIsStartTest, TestRunStatus, ulRunTimesTick);

        }
        else
        {
            vAPP_AddErr("MotorSpeedLow!");
            //电机异常时，暂停测试，等待 电压 电流 转速 在合理范围，再继续进行测试
        }
    }
}
struct ST_TASK_PARAM *stTask_debug;
uint16_t ucTaskFinishPercent;
void vApp_BasicMotorControl(struct ST_TASK_PARAM *stTask, uint8_t ucIsSetStop)
{
    static uint8_t IsReadyWriteFlash = 0;
    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时

    stTask_debug = stTask;

    if(ucIsSetStop)
    {
        // vApp_SetMotorControl(SET_MOTOR_PWR_OFF,
        //                     stTask->ucSetNoStopRunState, SET_MOTOR_CW,
        //                     0, 0);
       
        //vBsp_SetMotorPWM(0);
        //vBsp_SetMotorPWM(0);
        //vBsp_SetBrakePWM(0);
        return;
    }

    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
    }

    //电机重新开始测试的判断 TODO
    //重新开始的条件分析跟判断 TODO

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    ucTaskFinishPercent = stTargetPara.ulRunTimesTick * 50000 / stTask->ulSetRunTimesTick;

    if(stTargetPara.ulRunTimesTick < stTask->ulSetRunTimesTick && stTargetPara.ucTaskStatus != TASK_SET_STOP)//次数
    {
        //正转控制
        if((ullCurrent_ms % stTask->ulSetCircleTime_ms) <
                (stTask->ulSetCW_RunTime_ms +
                 stTask->ulSetCW_StopTime_ms))
        {
            //正转运行
            if(ullCurrent_ms % stTask->ulSetCircleTime_ms <
                    stTask->ulSetCW_RunTime_ms)
            {
                /* 先判断次数 */
                if(IsReadyWriteFlash == 1 )
                {
                    IsReadyWriteFlash = 0;
                    stTargetPara.ulRunTimesTick++;

                    if(stTask->ucIsSetRunTimesSaveFlash == 1)
                    {
                        vApp_SaveValue2Flash(stTargetPara.ulRunTimesTick);
                    }
                }

                if(stTargetPara.ulRunTimesTick ==  stTask->ulSetRunTimesTick)
                {
                    return;
                }

                /* 电机控制 */
                vApp_SetMotorControl(SET_MOTOR_PWR_ON,
                                     stTask->ucSetNoStopRunState, SET_MOTOR_CW,
                                     stTask->ulSetRunSpeed, stTask->usSetBrakePercent,
                                     stTask->ucIsUsePID, stTask->usSetAccDec);



                //电机运行状态判断
                vApp_MotorRUN_Status();
                stTargetPara.ucTaskStatus = TASK_RUN_CW_RUN;
                stTargetPara.ucSetEnTaskClockRun_1s = 1;
            }
            //正转停止
            else
            {
                /* 电机控制 */
                vApp_SetMotorControl(SET_MOTOR_PWR_OFF,
                                     stTask->ucSetNoStopRunState, SET_MOTOR_CW, 0, 0,
                                     stTask->ucIsUsePID, stTask->usSetAccDec);

                if(IsReadyWriteFlash == 0)
                {
                    IsReadyWriteFlash = 1;
                }

                stTargetPara.ucTaskStatus = TASK_RUN_CW_STOP;
                stTargetPara.ulTaskClock_1s_ForCycle = 0;
                stTargetPara.ucSetEnTaskClockRun_1s = 0;
            }
        }
        //反转控制
        else
        {
            //反转运行
            if((ullCurrent_ms % stTask->ulSetCircleTime_ms) <
                    (stTask->ulSetCW_RunTime_ms +
                     stTask->ulSetCW_StopTime_ms +
                     stTask->ulSetCCW_RunTime_ms))
            {
                /* 电机控制 */
                vApp_SetMotorControl(SET_MOTOR_PWR_ON,
                                     stTask->ucSetNoStopRunState, SET_MOTOR_CCW,
                                     stTask->ulSetRunSpeed, stTask->usSetBrakePercent,
                                     stTask->ucIsUsePID, stTask->usSetAccDec);

                /* 先判断次数 */
                if(stTask->ulSetCW_RunTime_ms == 0)
                {
                    if(IsReadyWriteFlash == 1 )
                    {
                        IsReadyWriteFlash = 0;
                        stTargetPara.ulRunTimesTick++;

                        if(stTask->ucIsSetRunTimesSaveFlash == 1)
                        {
                            vApp_SaveValue2Flash(stTargetPara.ulRunTimesTick);
                        }
                    }

                    if(stTargetPara.ulRunTimesTick ==  stTask->ulSetRunTimesTick)
                    {
                        return;
                    }
                }
                else
                {
                    if(IsReadyWriteFlash == 0)
                    {
                        IsReadyWriteFlash = 1;
                    }
                }

                stTargetPara.ucTaskStatus = TASK_RUN_CCW_RUN;
                stTargetPara.ucSetEnTaskClockRun_1s = 1;
            }
            //反转停止
            else
            {
                /* 电机控制 */
                vApp_SetMotorControl(SET_MOTOR_PWR_OFF,
                                     stTask->ucSetNoStopRunState, SET_MOTOR_CCW, 0, 0,
                                     stTask->ucIsUsePID, stTask->usSetAccDec);

                if(IsReadyWriteFlash == 0)
                {
                    IsReadyWriteFlash = 1;
                }

                stTargetPara.ucTaskStatus = TASK_RUN_CCW_STOP;
                stTargetPara.ucSetEnTaskClockRun_1s = 0;
                stTargetPara.ulTaskClock_1s_ForCycle = 0;
            }
        }

        stTargetPara.ullTestSumUseFromStart_ms  = ullCurrent_ms;
    }
    else
    {
        /* 电机控制 */
        vApp_SetMotorControl(SET_MOTOR_PWR_OFF,
                             stTask->ucSetNoStopRunState, SET_MOTOR_CW,
                             0, stTask->usSetBrakePercent,
                             stTask->ucIsUsePID, stTask->usSetAccDec);
        stTargetPara.ucTaskStatus = TASK_RUN_FINISH;
        stTargetPara.ucSetEnTaskClockRun_1s = 0;
    }
}

