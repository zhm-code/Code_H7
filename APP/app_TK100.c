#include "app_TK100.h"

#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_sysparam.h"
#include "bsp_motor.h"
#include "bsp_uart.h"
#include "string.h"
#include "bsp_can.h"
#include "bsp_pid.h"
#include "bsp_DAC.h"



static uint8_t IS_load = 0;



//设置开环运行
void vAPP_RunOpenLoop_TK100(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间

    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时

	static uint64_t ullCurrent_ms_100 = 0;
	static uint64_t ullCurrent_ms_10 = 0;
	static int sing_flag = 1;
	
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
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			sing_flag = 1;
			ICS_POWER_CTRL(CTRL_CLOSE);
    
		}
		else
		{
			sing_flag = -1;
			ICS_POWER_CTRL(CTRL_OPEN);
		}
		


		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.001;//*100
			uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;

			uSysROParam.fRunOpenLoop_spd_dev = sing_flag * stTargetPara.fMotorSpeed;
			
			
		}

		        
    }
    else
    {	

        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_TK100(void)
{
	//负载为0
	DAC_Set_Percentage(0);

	
	//关停FDCAN发送
	NumForDriverBoard_Send = MD_TX_NONE;
	
	//关闭电源
    uSysROParam.SetPPS_On = 0;
	//关闭负载
    ICS_POWER_CTRL(CTRL_CLOSE);
	//设置系统状态为0
    uSysROParam.usSystemStatus = 0;
    //关闭电机
    Morot_DIR_PWM_Ctrl(0);
//		DO_24V_1_CTRL(CTRL_CLOSE);
	
	//关闭气缸
	AIR_Cylinder_CTRL(CTRL_CLOSE);
	//设置步骤为1
    stTargetPara.ucIsSetTakStop = 1;

}


//设置负载
void vAPP_SetLoad_TK100(uint32_t RunTime_ms, uint16_t usSetLoad)
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
        if(1 == usSetLoad)
        {
			DAC_Set_Percentage(27);
			IS_load = 1;
        }
        else
        {
			DAC_Set_Percentage(0);
			IS_load = 0;
        }

    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

//设置电机停止
void vAPP_SetMotorStop_TK100(uint32_t RunTime_ms,uint16_t Set_Volt)
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
		//电源设置
		uSysROParam.SetPPS_On = 0;
		uSysROParam.SetPPS_Volt = Set_Volt;


    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}
