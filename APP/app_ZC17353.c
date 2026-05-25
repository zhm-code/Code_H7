#include "app_ZC17353.h"

#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_sysparam.h"
#include "bsp_motor.h"
#include "bsp_uart.h"
#include "string.h"
#include "bsp_can.h"
#include "bsp_pid.h"
#include "bsp_DAC.h"


int SetSpeedMaxPercent = 970;//解决电机异音问题 如果100.0% 3.1Khz左右会有异音 
//设置开环运行
void vAPP_RunOpenLoop_ZC17353(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间

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
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		DO_24V_1_CTRL(CTRL_OPEN);
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			if(ullCurrent_ms < 1000)
            {
                SetMotorSpeed_AccDec(200);
            }
            else
            {
                SetMotorSpeed_AccDec(SetSpeedMaxPercent);
            }
			
		}
		else
		{
			if(ullCurrent_ms < 1000)
            {
                SetMotorSpeed_AccDec(-200);
            }
            else
            {
                SetMotorSpeed_AccDec(-SetSpeedMaxPercent);
            }
			
		}
		

		

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.01;//*100
			uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;
			uSysROParam.sRunOpenLoop_spd_MSG = DriverBoard_DATA.RealSpd / 250.0f;//驱动器转速转换系数1/25
			
			
			NumForDriverBoard_Send = MD_TX_SetRunOpenLood;//驱动器:电压开环模式 1000 CW
			
			
		}

		        
    }
    else
    {
		SetMotorSpeed_AccDec(0);
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_ZC17353(void)
{
	SetMotorSpeed_AccDec(0);
	
	//关停FDCAN发送
	NumForDriverBoard_Send = MD_TX_NONE;
	
	//关闭电源
    uSysROParam.SetPPS_On = 0;
	DO_24V_8_CTRL(CTRL_CLOSE);
	//关闭负载
    ICS_POWER_CTRL(CTRL_CLOSE);
	//设置系统状态为0
    uSysROParam.usSystemStatus = 0;
    //关闭电机
//    Morot_DIR_PWM_Ctrl(0);
	//关闭气缸
	AIR_Cylinder_CTRL(CTRL_CLOSE);
	//设置步骤为1
    stTargetPara.ucIsSetTakStop = 1;
}



//驱动器上电
void vAPP_DrivePowerOn_ZC17353(uint32_t RunTime_ms, uint16_t Set_Volt)
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
		//前1s启动电源
		if(ullCurrent_ms < 1000)
        {
            uSysROParam.SetPPS_On = 1;
            uSysROParam.SetPPS_Volt = Set_Volt;//100x
			DO_24V_1_CTRL(CTRL_OPEN);
        }
        else
        {
            if(ullCurrent_ms < 2000)
            {
                NumForDriverBoard_Send = MD_TX_SET_EN;//驱动器:使能
            }
            else
            {
                NumForDriverBoard_Send = MD_TX_SETFB_Data;//驱动器:设置返回数据
            }
        }
		
	}
	else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
	
	
}



