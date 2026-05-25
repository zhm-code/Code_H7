#include "app_GB3001_GB3002.h"

#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_sysparam.h"
#include "bsp_motor.h"
#include "bsp_uart.h"
#include "string.h"
#include "bsp_can.h"
#include "bsp_pid.h"
#include "bsp_DAC.h"


//设置开环运行
void vAPP_RunOpenLoop_GB3001_GB3002(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间

    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时
	int FG_flag = 0;
	static float data[10] = {0};
	static int count = 0;
	static uint64_t ullCurrent_ms_10;
	
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
//			HAL_GPIO_WritePin(FR_CTRL_GPIO_Port,FR_CTRL_Pin,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(BRAKE_CTRL_GPIO_Port,BRAKE_CTRL_Pin,GPIO_PIN_SET);
			FG_flag = 1;
		}
		else
		{
//			HAL_GPIO_WritePin(FR_CTRL_GPIO_Port,FR_CTRL_Pin,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(BRAKE_CTRL_GPIO_Port,BRAKE_CTRL_Pin,GPIO_PIN_SET);
			FG_flag = -1;
		}

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.001;//*100
			uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;

//			uSysROParam.fRunOpenLoop_spd_dev = uSysROParam.cMotorRotationIn * stTargetPara.fMotorSpeed;
//			uSysROParam.fRunOpenLoop_spd_dev = FG_flag * stTargetPara.fMotorSpeed;
			
			
//			uSysROParam.fRunOpenLoop_spd_motor = FG_flag * stTargetPara.fFG_MotorSpeed
			if ( ullCurrent_ms - ullCurrent_ms_10 >= 10)
			{
				ullCurrent_ms_10 = ullCurrent_ms;
				data[count++] = FG_flag * stTargetPara.fFG_MotorSpeed;
				if (count == 10)
				{
					// 计算平均值
					float sum = 0.0;
					for (int i = 0; i < 10; i++) 
					{
						sum += data[i];
					}
					
					uSysROParam.fRunOpenLoop_spd_motor = sum / 10.0f;
					uSysROParam.fRunOpenLoop_spd_dev = sum / 10.0f;
					count = 0;
				}
			}
			
			
			
			
		}

		        
    }
    else
    {	
		memset(data,0,10);
		count = 0;
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_GB3001_GB3002(void)
{
	//关闭电源
    uSysROParam.SetPPS_On = 0;
	//关闭负载
    ICS_POWER_CTRL(CTRL_CLOSE);
	//设置系统状态为0
    uSysROParam.usSystemStatus = 0;
    //关闭电机
//    Morot_DIR_PWM_Ctrl(0);
	//关闭气缸
	switch ((uint16_t)uSysROParam.ProjNumber)
		{
			case GB3001:
			{
				DO_24V_8_CTRL(0);
			}
			break;
			
			case GB3002:
			{
				DO_24V_7_CTRL(0);
			}
			break;
		}
	//设置步骤为1
    stTargetPara.ucIsSetTakStop = 1;
}


//设置负载
void vAPP_SetLoad_GB3001_GB3002(uint32_t RunTime_ms, uint16_t usSetLoad)
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
			ICS_POWER_CTRL(1);
        }
        else
        {
			ICS_POWER_CTRL(0);
        }

    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

//设置气缸
void vAPP_SetCylinder_GB3001_GB3002(uint32_t RunTime_ms, uint16_t usSetCyl)
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
        if(1 == usSetCyl)
        {
			switch ((uint16_t)uSysROParam.ProjNumber)
			{
				case GB3001:
				{
					DO_24V_8_CTRL(1);
				}
				break;
				
				case GB3002:
				{
					DO_24V_7_CTRL(1);
				}
				break;
			}
        }
        else
        {
			switch ((uint16_t)uSysROParam.ProjNumber)
			{
				case GB3001:
				{
					DO_24V_8_CTRL(0);
				}
				break;
				
				case GB3002:
				{
					DO_24V_7_CTRL(0);
				}
				break;
			}
        }

    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

