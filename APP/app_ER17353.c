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


static uint8_t IS_load = 0;

//设置开环运行
void vAPP_RunOpenLoop_ER17353(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间

    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时
	
	static uint8_t stop_flag = 0;
	
	//每10ms动作一次
	static uint64_t ullCurrent_ms_10 = 0;
	
    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP ||
            stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();//本函数内部
        stTargetPara.ullMultiStepStart_ms = ullBsp_TimeBase_ms();//上传
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
		
		uSysROParam.cMotorRotationIn	= 0;
		uSysROParam.cFCT_MotorRotation	= 0;
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
		//开启继电器
		DO_24V_1_CTRL(CTRL_OPEN);
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			if (stop_flag == 1)
			{
				SetMotorSpeed_AccDec(0);
				
			}
			else
			{
				if(ullCurrent_ms < 1000)
				{
					SetMotorSpeed_AccDec(200);
				}
				else
				{
					SetMotorSpeed_AccDec(1000);
				}
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
                SetMotorSpeed_AccDec(-1000);
            }
			
		}
		
		
		//负载控制
		if(1 == IS_load)
        {
			

			if (ullCurrent_ms - ullCurrent_ms_10 >= 40)
			{
				ullCurrent_ms_10 = ullCurrent_ms;
				
				PID_Controller(50, uSysROParam.fRunOpenLoop_Curr, &pid_pwm_set_ER17353);
				DAC_Set_Percentage(10 + pid_pwm_set_ER17353.fOutput);	
			}

			
			//参数获取
			if(ullCurrent_ms % RunTime_ms < RunTime_ms - 3000)
			{
				uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.01;//*100
				uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;
				uSysROParam.sRunOpenLoop_spd_MSG = DriverBoard_DATA.RealSpd / 250.0f;//驱动器转速转换系数1/25
				uSysROParam.isDIR_Flag = uSysROParam.cMotorRotationIn;
				
				NumForDriverBoard_Send = MD_TX_SetRunOpenLood;//驱动器:电压开环模式 1000 CW
			}
			else
			{

				DAC_Set_Percentage(0);
				
			}
				
			
        }
		//空载
        else
        {
            //参数获取
			if(ullCurrent_ms % RunTime_ms < RunTime_ms - 1000)
			{
				uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.01;//*100
				uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;
				uSysROParam.sRunOpenLoop_spd_MSG = DriverBoard_DATA.RealSpd / 250.0f;//驱动器转速转换系数1/25
				uSysROParam.isDIR_Flag = uSysROParam.cFCT_MotorRotation;
				
				NumForDriverBoard_Send = MD_TX_SetRunOpenLood;//驱动器:电压开环模式 1000 CW
			}
			else
			{
				if (SET_MOTOR_CW == ucSetMotorRotation)
				{
					stop_flag = 1;
				}
				
				
				
			}
        }

		

		        
    }
    else
    {
		SetMotorSpeed_AccDec(0);
		pid_pwm_set_ER17353.fError 			= 0;
		pid_pwm_set_ER17353.fOutput 		= 0;
		pid_pwm_set_ER17353.fProportion 	= 0;
		pid_pwm_set_ER17353.fIntegral 		= 0;
		pid_pwm_set_ER17353.fDerivative 	= 0;
		
		stop_flag = 0;
		IS_load = 0;
		
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_ER17353(void)
{
	//设转速为0
	SetMotorSpeed_AccDec(0);
	//清空PID
	pid_pwm_set_ER17353.fError 			= 0;
	pid_pwm_set_ER17353.fOutput 		= 0;
	pid_pwm_set_ER17353.fProportion 	= 0;
	pid_pwm_set_ER17353.fIntegral 		= 0;
	pid_pwm_set_ER17353.fDerivative 	= 0;
	
	//关停FDCAN发送
	NumForDriverBoard_Send = MD_TX_NONE;
	
	//关闭电源
    uSysROParam.SetPPS_On = 0;
	//关闭继电器
	DO_24V_8_CTRL(CTRL_CLOSE);
	//关闭负载
    ICS_POWER_CTRL(CTRL_CLOSE);
	//设置系统状态为0
    uSysROParam.usSystemStatus = 0;
	//设置步骤为1
    stTargetPara.ucIsSetTakStop = 1;
}



//驱动器上电
void vAPP_DrivePowerOn_ER17353(uint32_t RunTime_ms, uint16_t Set_Volt)
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


//设置负载
void vAPP_SetLoad_ER17353(uint32_t RunTime_ms, uint16_t usSetLoad)
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
            DAC_Set_Percentage(10);
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

