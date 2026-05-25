#include "app_GB11579.h"

#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_sysparam.h"
#include "bsp_motor.h"
#include "bsp_uart.h"
#include "string.h"
#include "bsp_can.h"
#include "bsp_pid.h"
#include "bsp_DAC.h"
#include "app_uart5.h"
#include "app_SendData.h"

static uint8_t IS_load = 0;
static uint8_t ucMotor_Mode = 0;


//设置开环运行
void vAPP_RunOpenLoop_GB11579(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; 				//加上等待上位机读取的时间

    const CAN_Send_t stCAN_Send_Set_Open_Loop_Voltage_CW2800 = {
        .uiCAN_ID = 0X31,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x6D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}                                  //设置开环电压CW28V
    };//CAN发送数据结构体，包含CAN ID、数据长度和数据内容等信息

    const CAN_Send_t stCAN_Send_Set_Open_Loop_Voltage_CCW2800 = {
        .uiCAN_ID = 0X31,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x92, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}                                  //设置开环电压CCW28V
    };//CAN发送数据结构体，包含CAN ID、数据长度和数据内容等信息

    const CAN_Send_t stCAN_Send_Set_Speed_Loop_CW120RPM = {
        .uiCAN_ID = 0X31,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}                                  //设置闭环速度CW120RPM
    };//CAN发送数据结构体，包含CAN ID、数据长度和数据内容等信息

     const CAN_Send_t stCAN_Send_Set_Speed_Loop_CCW120RPM = {
        .uiCAN_ID = 0X31,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0xFF, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}                                  //设置闭环速度CCW120RPM
    };//CAN发送数据结构体，包含CAN ID、数据长度和数据内容等信息

    uint64_t ullCurrent_ms;						//当前时间，相对于任务开始时

    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP ||
            stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();		//本函数内部
        stTargetPara.ullMultiStepStart_ms = ullBsp_TimeBase_ms();		//上传
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;			//每次开始一个新的步骤，重置时间起点
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;							//本函数内部运行时间
    stTargetPara.ullMultiStepCurrent_ms =  ullBsp_TimeBase_ms() - stTargetPara.ullMultiStepStart_ms;	//上传当前步骤运行时间
    uSysROParam.fRunOpenLoop_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;						//上传当前步骤运行时间，单位s

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)						//时间未到，且状态不是停止
    {  	

		uSysROParam.SetPPS_On = 1;				//上电
		uSysROParam.SetPPS_Volt = Set_Volt;		//设置电压
		
        switch (ucMotor_Mode)
        {
            case 0:
            {
                if(SET_MOTOR_CW == ucSetMotorRotation)
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                                      //CAN1
                        0x01,                                                           //开启发送
                        stCAN_Send_Set_Open_Loop_Voltage_CW2800.uiCAN_ID,               //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Open_Loop_Voltage_CW2800.aucCAN_Buf,   //CAN发送数据缓存
                        stCAN_Send_Set_Open_Loop_Voltage_CW2800.ucCAN_Len,              //CAN发送数据长度
                        0,                                                              //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
                else
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                                      //CAN1
                        0x01,                                                           //开启发送
                        stCAN_Send_Set_Open_Loop_Voltage_CCW2800.uiCAN_ID,              //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Open_Loop_Voltage_CCW2800.aucCAN_Buf,  //CAN发送数据缓存
                        stCAN_Send_Set_Open_Loop_Voltage_CCW2800.ucCAN_Len,             //CAN发送数据长度
                        0,                                                              //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
            }
            break;

            case 1:
            {
                if(SET_MOTOR_CW == ucSetMotorRotation)
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                                      //CAN1
                        0x01,                                                           //开启发送
                        stCAN_Send_Set_Speed_Loop_CW120RPM.uiCAN_ID,                    //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Speed_Loop_CW120RPM.aucCAN_Buf,        //CAN发送数据缓存
                        stCAN_Send_Set_Speed_Loop_CW120RPM.ucCAN_Len,                   //CAN发送数据长度
                        0,                                                              //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
                else
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                                      //CAN1
                        0x01,                                                           //开启发送
                        stCAN_Send_Set_Speed_Loop_CCW120RPM.uiCAN_ID,              //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Speed_Loop_CCW120RPM.aucCAN_Buf,  //CAN发送数据缓存
                        stCAN_Send_Set_Speed_Loop_CCW120RPM.ucCAN_Len,             //CAN发送数据长度
                        0,                                                              //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
            }
            break;

            default:
            {

            }
            break;
        }

		if (IS_load == 1)					//如果负载为1，执行负载控制
		{
			if (ullCurrent_ms < 3000)		//前3秒负载为0
			{
				DAC_Set_Percentage(0);		//设置负载为0
				
			}
			else							//3秒后负载为24.5%
			{
				DAC_Set_Percentage(80);	//设置负载为24.5%
			}
			
		}
		else								//如果负载为0，执行停止控制
		{
			DAC_Set_Percentage(0);			//设置负载为0
		}

		/*参数获取*/
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)			//每RunTime_ms-500 ms执行一次参数获取，避免在上位机读取数据时发送干扰命令
		{
			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.01;							//计算电流，单位A，减去基座电流损耗
			// uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;										        //计算电压，单位V
			System_Uploading_Params.stMSG.fSpeed                 = stMotor_Read_MSG.fSpeed;													//电机转速，单位rpm	
            System_Uploading_Params.stMSG.fMCU_Temp              = stMotor_Read_MSG.fMCU_Temp;
            System_Uploading_Params.stMSG.fMOS_Temp              = stMotor_Read_MSG.fMOS_Temp;
            System_Uploading_Params.stMSG.fWire_Winding_Temp     = stMotor_Read_MSG.fWire_Winding_Temp;
            System_Uploading_Params.stMSG.fBus_Current           = stMotor_Read_MSG.fBus_Current * 0.01;												//计算母线电流，单位A
            System_Uploading_Params.stMSG.fVoltage               = uSysROParam.GetPPS_Volt * 0.01;

		}
    }

    else
    {	
        uSysROParam.Current_Status = STATUS_DONE;	            //结束

    }
}

//完成复位
void vAPP_SetTestFinish_GB11579(void)
{
    const CAN_Send_t stCAN_Send_Set_Motor_Disable = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}                                  //设置电机失能
    };//CAN发送数据结构体，包含CAN ID、数据长度和数据内容等信息

	DAC_Set_Percentage(0);				//设置负载为0
    uSysROParam.SetPPS_On = 0;			//电源断电
    ICS_POWER_CTRL(CTRL_CLOSE);			//关闭负载
    uSysROParam.usSystemStatus = 0;		//设置系统状态为0
	AIR_Cylinder_CTRL(CTRL_CLOSE);		//关闭气缸
    stTargetPara.ucIsSetTakStop = 1;	//设置任务停止标记，停止当前正在执行的任务

    vSystem_Send_Cfg(
        CAN1_MASK,                                              //CAN1
        0x01,                                                   //开启发送
        stCAN_Send_Set_Motor_Disable.uiCAN_ID,                  //CAN消息ID
        (uint8_t*)stCAN_Send_Set_Motor_Disable.aucCAN_Buf,      //CAN发送数据缓存
        stCAN_Send_Set_Motor_Disable.ucCAN_Len,                 //CAN发送数据长度
        3,                                                      //发送周期，单位为发送次数，0表示无限发送
        100
    );	//系统CAN发送配置函数，立即生效配置

}

//设置气缸
void vAPP_SetCylinder_GB11579(uint32_t RunTime_ms, uint16_t usSetCyl)
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
            AIR_Cylinder_CTRL(CTRL_OPEN);
        }
        else
        {
            AIR_Cylinder_CTRL(CTRL_CLOSE);
        }
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}



//设置负载
void vAPP_SetLoad_GB11579(uint32_t RunTime_ms, uint16_t usSetLoad)
{
    uint64_t ullCurrent_ms;		//当前时间，相对于任务开始时

    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)	//测试暂停后,重新开始计算时间起点
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();	//函数内部初始时间
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;		//每次开始一个新的步骤，重置时间起点
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;	//函数内部运行时间

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)	//时间
    {
        if(1 == usSetLoad)		//如果负载为1，执行负载控制
        {
			IS_load = 1;		//设置负载标记为1，在开环运行函数中执行负载控制
        }
        else					//如果负载为0，执行停止控制
        {
			IS_load = 0;		//设置负载标记为0，在开环运行函数中执行停止控制
        }
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;	//结束
    }
}

//设置电机暂停
void vAPP_SetMotorStop_GB11579(uint32_t RunTime_ms,uint16_t Set_Volt)
{
    const CAN_Send_t stCAN_Send_Set_Motor_Disable = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}                                  //设置电机失能
    };//CAN发送数据结构体，包含CAN ID、数据长度和数据内容等信息

    uint64_t ullCurrent_ms; 							//当前时间，相对于任务开始时


    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)	//测试暂停后,重新开始计算时间起点
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();	//函数内部初始时间
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;		//每次开始一个新的步骤，重置时间起点
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;		//函数内部运行时间		
    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)	//时间未到，且状态不是停止
    {
		uSysROParam.SetPPS_On = 1;					//上电
		uSysROParam.SetPPS_Volt = Set_Volt;			//设置电压，保持与开环运行时相同的电压，避免在停止过程中电压突变对设备造成冲击

        vSystem_Send_Cfg(
            CAN1_MASK,                                              //CAN1
            0x01,                                                   //开启发送
            stCAN_Send_Set_Motor_Disable.uiCAN_ID,                  //CAN消息ID
            (uint8_t*)stCAN_Send_Set_Motor_Disable.aucCAN_Buf,      //CAN发送数据缓存
            stCAN_Send_Set_Motor_Disable.ucCAN_Len,                 //CAN发送数据长度
            3,                                                      //发送周期，单位为发送次数，0表示无限发送
            100
        );	//系统CAN发送配置函数，立即生效配置
		
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;	//结束
    }
}

//驱动器上电
void vAPP_DrivePowerOn_GB11579(uint32_t RunTime_ms, uint16_t Set_Volt)
{
	uint64_t ullCurrent_ms;		                                                                                //当前时间，相对于任务开始时

    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)	//测试暂停后,重新开始计算时间起点
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();	                                            //函数内部初始时间
        stTargetPara.PrevCheckStep       = stTargetPara.CheckStep;		                                        //每次开始一个新的步骤，重置时间起点

    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;	                                //函数内部运行时间

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)	                            //时间未到，且状态不是停止
	{
		uSysROParam.SetPPS_On   = 1;				                                                            //上电
		uSysROParam.SetPPS_Volt = Set_Volt;		                                                                //设置电压
	}
	else
    {
        uSysROParam.Current_Status = STATUS_DONE;               //结束

    }
}

//电机设置模式
void vAPP_motor_set_mode_GB11579(uint32_t time, uint16_t set_mode)
{
    //读取固件版本
    const CAN_Send_t stCAN_Send_Read_Firmware_Version = {
        .uiCAN_ID = 0X70,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}                                  
    };

    //设置返回包类型，速度，mos温度，绕线温度，MCU温度
    const CAN_Send_t stCAN_Send_Set_Return_Package_1 = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x07, 0x01, 0x01, 0x03, 0x04, 0x09, 0x00}                                  
    };

    //设置返回包类型，速度，mos温度，绕线温度，母线电流
    const CAN_Send_t stCAN_Send_Set_Return_Package_2 = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x07, 0x01, 0x01, 0x03, 0x04, 0x07, 0x00}                                  
    };

    //设置开环模式
    const CAN_Send_t stCAN_Send_Set_Motor_Open_Loop_Mode = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}                                  
    };

    //设置速度闭环模式
    const CAN_Send_t stCAN_Send_Set_Motor_Speed_Loop_Mode = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00}                                  
    };

    //设置电机使能
    const CAN_Send_t stCAN_Send_Set_Motor_Enable = {
        .uiCAN_ID = 0X71,	                                                                            //CAN消息ID
        .ucCAN_Len = 8,	                                                                                //CAN发送数据长度，根据实际需要填写
        .aucCAN_Buf = {0x01, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00}                                  
    };


	uint64_t ullCurrent_ms; //当前时间，相对于任务开始时
    static uint8_t  ucPrevious_Step = 0;	                                                                    //记录上一个步骤，确保在步骤切换时执行一次配置
    static uint8_t  ucStep = 0;	                                                                                //步骤标记，确保在步骤内只执行一次配置


    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < time && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
        ucMotor_Mode = set_mode;
#if 0
        switch (set_mode)
        {
            case 0:
            {
                switch(ucStep)
                {
                    case 0: //读取固件版本，验证通信
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                              //CAN1
                            0x01,                                                   //开启发送
                            stCAN_Send_Read_Firmware_Version.uiCAN_ID,              //CAN消息ID
                            (uint8_t*)stCAN_Send_Read_Firmware_Version.aucCAN_Buf,  //CAN发送数据缓存
                            stCAN_Send_Read_Firmware_Version.ucCAN_Len,             //CAN发送数据长度
                            3,                                                       //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置

                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }   
                    }
                    break;

                    case 1: //设置返回包类型，速度，mos温度，绕线温度，MCU温度
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                              //CAN1
                            0x01,                                                   //开启发送          
                            stCAN_Send_Set_Return_Package_1.uiCAN_ID,                 //CAN消息ID
                            (uint8_t*)stCAN_Send_Set_Return_Package_1.aucCAN_Buf,     //CAN发送数据缓存
                            stCAN_Send_Set_Return_Package_1.ucCAN_Len,                //CAN发送数据长度
                            3,                                                       //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置

                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }  
                    }
                    break;

                    case 2: //设置电机使能
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                              //CAN1
                            0x01,                                                   //开启发送          
                            stCAN_Send_Set_Motor_Enable.uiCAN_ID,                   //CAN消息ID
                            (uint8_t*)stCAN_Send_Set_Motor_Enable.aucCAN_Buf,       //CAN发送数据缓存
                            stCAN_Send_Set_Motor_Enable.ucCAN_Len,                  //CAN发送数据长度
                            3,                                                       //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置

                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }  
                    }

                    case 3: //设置电机电压开环模式28V
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                                 //CAN1
                            0x01,                                                      //开启发送
                            stCAN_Send_Set_Motor_Open_Loop_Mode.uiCAN_ID,              //CAN消息ID
                            (uint8_t*)stCAN_Send_Set_Motor_Open_Loop_Mode.aucCAN_Buf,  //CAN发送数据缓存
                            stCAN_Send_Set_Motor_Open_Loop_Mode.ucCAN_Len,             //CAN发送数据长度
                            3,                                                         //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置
                    
                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }   
                    }

                    case 0xFF:
                    {
                        if (ucGet_CAN_State(CAN1_MASK) == CAN_STOP)	    //如果CAN1停止，说明已经完成一次发送，可以进入下一个步骤
                        {
                            ucStep = ucPrevious_Step + 1;	            //步骤标记加1，继续执行当前步骤的配置
                        }
                    }
                    break;

                    default:
                    {
                        //步骤结束，不执行任何操作
                    }
                    break;
                }
            }
            break;

            case 1:
            {
                switch(ucStep)
                {
                    case 0: //设置返回包类型，速度，mos温度，绕线温度，MCU温度
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                              //CAN1
                            0x01,                                                   //开启发送          
                            stCAN_Send_Set_Return_Package_2.uiCAN_ID,                 //CAN消息ID
                            (uint8_t*)stCAN_Send_Set_Return_Package_2.aucCAN_Buf,     //CAN发送数据缓存
                            stCAN_Send_Set_Return_Package_2.ucCAN_Len,                //CAN发送数据长度
                            3,                                                       //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置

                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }  
                    }
                    break;

                    case 1: //设置电机使能
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                              //CAN1
                            0x01,                                                   //开启发送          
                            stCAN_Send_Set_Motor_Enable.uiCAN_ID,                   //CAN消息ID
                            (uint8_t*)stCAN_Send_Set_Motor_Enable.aucCAN_Buf,       //CAN发送数据缓存
                            stCAN_Send_Set_Motor_Enable.ucCAN_Len,                  //CAN发送数据长度
                            3,                                                       //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置

                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }  
                    }

                    case 2: //设置电机速度闭环模式
                    {
                        vSystem_Send_Cfg(
                            CAN1_MASK,                                                 //CAN1
                            0x01,                                                      //开启发送
                            stCAN_Send_Set_Motor_Speed_Loop_Mode.uiCAN_ID,              //CAN消息ID
                            (uint8_t*)stCAN_Send_Set_Motor_Speed_Loop_Mode.aucCAN_Buf,  //CAN发送数据缓存
                            stCAN_Send_Set_Motor_Speed_Loop_Mode.ucCAN_Len,             //CAN发送数据长度
                            3,                                                         //发送周期，单位为发送次数，0表示无限发送
                            100
                        );	//系统CAN发送配置函数，立即生效配置
                    
                        if (ucGet_CAN_State(CAN1_MASK) == CAN_RUNING)	//如果CAN1已经在运行，说明配置已经生效，可以进入下一个步骤
                        {
                            ucPrevious_Step = ucStep;                   //记录当前步骤
                            ucStep = 0xFF;	                            //设置步骤标记为1，进入下一个步骤
                        }   
                    }

                    case 0xFF:
                    {
                        if (ucGet_CAN_State(CAN1_MASK) == CAN_STOP)	    //如果CAN1停止，说明已经完成一次发送，可以进入下一个步骤
                        {
                            ucStep = ucPrevious_Step + 1;	            //步骤标记加1，继续执行当前步骤的配置
                        }
                    }
                    break;

                    default:
                    {
                        //步骤结束，不执行任何操作
                    }
                    break;
                }
            }
            break;
        
            default:
            {
                
            }
            break;
        }
#endif
        switch (set_mode)
        {
            case 0:
            {
                if (ullCurrent_ms <= 500)   //读取固件版本，验证通信
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                              //CAN1
                        0x01,                                                   //开启发送
                        stCAN_Send_Read_Firmware_Version.uiCAN_ID,              //CAN消息ID
                        (uint8_t*)stCAN_Send_Read_Firmware_Version.aucCAN_Buf,  //CAN发送数据缓存
                        stCAN_Send_Read_Firmware_Version.ucCAN_Len,             //CAN发送数据长度
                        0,                                                       //发送周期，单位为发送次数，0表示无限发送
                        1000
                    );	//系统CAN发送配置函数，立即生效配置
                }
                else if(ullCurrent_ms > 500 && ullCurrent_ms <= 1000)   //设置返回包类型，速度，mos温度，绕线温度，MCU温度
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                              //CAN1
                        0x01,                                                   //开启发送          
                        stCAN_Send_Set_Return_Package_1.uiCAN_ID,                 //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Return_Package_1.aucCAN_Buf,     //CAN发送数据缓存
                        stCAN_Send_Set_Return_Package_1.ucCAN_Len,                //CAN发送数据长度
                        0,                                                       //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
                else if(ullCurrent_ms > 1000 && ullCurrent_ms <= 1500)  ////设置电机电压开环模式28V 
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                                 //CAN1
                        0x01,                                                      //开启发送
                        stCAN_Send_Set_Motor_Open_Loop_Mode.uiCAN_ID,              //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Motor_Open_Loop_Mode.aucCAN_Buf,  //CAN发送数据缓存
                        stCAN_Send_Set_Motor_Open_Loop_Mode.ucCAN_Len,             //CAN发送数据长度
                        0,                                                         //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置

                }
                else if(ullCurrent_ms > 1500 && ullCurrent_ms <= 2000)  //设置电机使能
                {

                    vSystem_Send_Cfg(
						CAN1_MASK,                                              //CAN1
						0x01,                                                   //开启发送          
						stCAN_Send_Set_Motor_Enable.uiCAN_ID,                   //CAN消息ID
						(uint8_t*)stCAN_Send_Set_Motor_Enable.aucCAN_Buf,       //CAN发送数据缓存
						stCAN_Send_Set_Motor_Enable.ucCAN_Len,                  //CAN发送数据长度
						0,                                                       //发送周期，单位为发送次数，0表示无限发送
						100
                    );	//系统CAN发送配置函数，立即生效配置
                }

            }
            break;

            case 1:
            {
                if (ullCurrent_ms <= 500)   //设置返回包类型，速度，mos温度，绕线温度，MCU温度
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                              //CAN1
                        0x01,                                                   //开启发送          
                        stCAN_Send_Set_Return_Package_2.uiCAN_ID,                 //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Return_Package_2.aucCAN_Buf,     //CAN发送数据缓存
                        stCAN_Send_Set_Return_Package_2.ucCAN_Len,                //CAN发送数据长度
                        0,                                                       //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
                else if(ullCurrent_ms > 500 && ullCurrent_ms <= 1000)    //设置电机速度闭环模式
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                                 //CAN1
                        0x01,                                                      //开启发送
                        stCAN_Send_Set_Motor_Speed_Loop_Mode.uiCAN_ID,              //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Motor_Speed_Loop_Mode.aucCAN_Buf,  //CAN发送数据缓存
                        stCAN_Send_Set_Motor_Speed_Loop_Mode.ucCAN_Len,             //CAN发送数据长度
                        0,                                                         //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
                else if(ullCurrent_ms > 1000 && ullCurrent_ms <= 1500)  //设置电机使能
                {
                    vSystem_Send_Cfg(
                        CAN1_MASK,                                              //CAN1
                        0x01,                                                   //开启发送          
                        stCAN_Send_Set_Motor_Enable.uiCAN_ID,                   //CAN消息ID
                        (uint8_t*)stCAN_Send_Set_Motor_Enable.aucCAN_Buf,       //CAN发送数据缓存
                        stCAN_Send_Set_Motor_Enable.ucCAN_Len,                  //CAN发送数据长度
                        0,                                                       //发送周期，单位为发送次数，0表示无限发送
                        100
                    );	//系统CAN发送配置函数，立即生效配置
                }
            }
            break;
        
        }

        System_Uploading_Params.stMSG.uiFirmware_Version = stMotor_Read_MSG.uiFirmware_Version;

			
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
        ucStep = 0;                                             //复位步骤标记
        ucPrevious_Step = 0;                                    //复位上一个步骤记录
    }
}