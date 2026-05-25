#include "app_GB5753X.h"

#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_sysparam.h"
#include "bsp_motor.h"
#include "bsp_uart.h"
#include "string.h"
#include "bsp_can.h"
#include "bsp_pid.h"
#include "bsp_DAC.h"


pid_t pid_pwm_set_GB5753X = {
	.fKp = 0.01,
	.fKi = 0.01,
	.fKd = 0.01,
};

static uint8_t IS_load = 0;


//指令
static uint8_t fdcan1_TX_NWT_TO_Operational[8] 		= {0x01,0x01};									//01 01		  				NMT网络管理	
static uint8_t fdcan1_TX_Shutdown[8] 				= {0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};	//2B 40 60 00 06 00 00 00	402状态机 Shutdown
static uint8_t fdcan1_TX_Switch_On[8] 				= {0x2B,0x40,0x60,0x00,0x07,0x00,0x00,0x00};	//2B 40 60 00 07 00 00 00 	Switch On
static uint8_t fdcan1_TX_Velocity_Mode[8] 			= {0x2F,0x60,0x60,0x00,0x03,0x00,0x00,0x00};	//2F 60 60 00 03 00 00 00	设置模式为Velocity Mode
static uint8_t fdcan1_TX_Operation_Enabled[8] 		= {0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};	//2B 40 60 00 0F 00 00 00	Operation Enabled
static uint8_t fdcan1_TX_Set_Speed_CW60[8] 		    = {0x23,0xFF,0x60,0x00,0x00,0x00,0x80,0x3F};	//23 FF 60 00 00 00 80 3F	带载60RPM
static uint8_t fdcan1_TX_Set_Speed_CCW60[8] 		= {0x23,0xFF,0x60,0x00,0x00,0x00,0x80,0xbF};	//23 FF 60 00 00 00 80 BF	带载-60RPM
static uint8_t fdcan1_TX_Set_Vs_Mode[8]				= {0x2F,0x60,0x60,0x00,0x08,0x00,0x00,0x00};	//2F 60 60 00 08 00 00 00	设置模式为 Vs Mode
static uint8_t fdcan1_TX_Set_Open_Loop_CW[8]		= {0x23,0x20,0x20,0x01,0x00,0x00,0xC8,0x42};	//23 20 20 01 00 00 C8 42	开环+100%
static uint8_t fdcan1_TX_Set_Open_Loop_CCW[8]		= {0x23,0x20,0x20,0x01,0x00,0x00,0xC8,0xC2};	//23 20 20 01 00 00 C8 C2	开环-100%
static uint8_t fdcan1_TX_Set_Read_Driver_tmp[8]		= {0x40,0x04,0x22,0x01,0x00,0x00,0x80,0xF3};	//40 04 22 01 00 00 00 00	读取驱动器温度
static uint8_t fdcan1_TX_Set_Read_Motor_tmp[8]		= {0x40,0x04,0x22,0x02,0x00,0x00,0x00,0x00};	//40 04 22 02 00 00 00 00	读取电机温度

enum 
{
	/*ID 0*/
	NWT_TO_Operational 	= 0, 	//切换 NMT 到 Operational 	
	/*ID 601*/
	Shutdown 			= 1,	//切换到 402 状态机 Shutdown  
	Switch_On 			= 2,	//切换到 Switch On 			
	Velocity_Mode 		= 3, 	//设置模式为 Velocity Mode
	Operation_Enabled 	= 4,	//切换到 Operation Enabled 	
	Set_Speed_CW60 	    = 5,	//设置目标速度 +60 RPM 		
	Set_Speed_CCW60 	= 6,	//设置目标速度 -60 RPM 		
	Vs_Mode				= 7,	//设置模式为 Vs Mode
	Open_Loop_CW		= 8,	//开环+100%
	Open_Loop_CCW		= 9,    //开环-100%
	Read_Driver_tmp		= 10,   //读取驱动器温度
	Read_Motor_tmp		= 11,   //读取电机温度
	
}GB5753_CAN_Step;	//发送步骤枚举




//设置开环运行
void vAPP_RunOpenLoop_GB5753X(uint32_t RunTime_ms,
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
			
			if (  (ullCurrent_ms - ullCurrent_ms_100 >= 1000))
			{
				if (1 == IS_load )
				{
					FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Speed_CW60, 0x08);
					
				}
				else
				{
					FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Open_Loop_CW, 0x08);
					
				}
				ullCurrent_ms_100 = ullCurrent_ms;
				
				sing_flag = 1;
				
				
			}
    
		}
		else
		{
			if ( (ullCurrent_ms - ullCurrent_ms_100 >= 1000))
			{
				if (1 == IS_load)
				{
					FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Speed_CCW60, 0x08);
					
				}
				else
				{
					FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Open_Loop_CCW, 0x08);
					
				}
				ullCurrent_ms_100 = ullCurrent_ms;
				
				sing_flag = -1;
			}
			
		}
		
		//负载控制
		if(1 == IS_load)
        {
			if ( ullCurrent_ms < 3000)
			{
				DAC_Set_Percentage(0);
				
			}
			else 
			{
				if (  (ullCurrent_ms - ullCurrent_ms_10 >= 10))
				{
					ullCurrent_ms_10 = ullCurrent_ms;

					PID_Controller(3.4, uSysROParam.fRunOpenLoop_Curr, &pid_pwm_set_GB5745X);
					DAC_Set_Percentage(25 + pid_pwm_set_GB5745X.fOutput);
				}
				
			}
            
			
        }
        else
        {
            DAC_Set_Percentage(0);
        }
		

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.01;//*100
			uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;

			uSysROParam.fRunOpenLoop_spd_dev = sing_flag * stTargetPara.fMotorSpeed;
			
			
		}

		        
    }
    else
    {	
		pid_pwm_set_GB5745X.fError 			= 0;
		pid_pwm_set_GB5745X.fOutput 		= 0;
		pid_pwm_set_GB5745X.fProportion 	= 0;
		pid_pwm_set_GB5745X.fIntegral 		= 0;
		pid_pwm_set_GB5745X.fDerivative 	= 0;
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_GB5753X(void)
{
	DAC_Set_Percentage(0);
	pid_pwm_set_GB5745X.fError 			= 0;
	pid_pwm_set_GB5745X.fOutput 		= 0;
	pid_pwm_set_GB5745X.fProportion 	= 0;
	pid_pwm_set_GB5745X.fIntegral 		= 0;
	pid_pwm_set_GB5745X.fDerivative 	= 0;
	
	//关停FDCAN发送
	NumForDriverBoard_Send = MD_TX_NONE;
	
	//关闭电源
    uSysROParam.SetPPS_On = 0;
	//关闭负载
    ICS_POWER_CTRL(CTRL_CLOSE);
	//设置系统状态为0
    uSysROParam.usSystemStatus = 0;
    //关闭电机
//    Morot_DIR_PWM_Ctrl(0);
//		DO_24V_1_CTRL(CTRL_CLOSE);
	
	//关闭气缸
	AIR_Cylinder_CTRL(CTRL_CLOSE);
	//设置步骤为1
    stTargetPara.ucIsSetTakStop = 1;

}


//设置负载
void vAPP_SetLoad_GB5753X(uint32_t RunTime_ms, uint16_t usSetLoad)
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
			DAC_Set_Percentage(0);
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

//驱动器上电
void vAPP_DrivePowerOn_GB5753X(uint32_t RunTime_ms, uint16_t Set_Volt)
{
	uint64_t ullCurrent_ms; //当前时间，相对于任务开始时

	static uint64_t ullCurrent_ms_50; 
	static uint8_t IS_One_time_Program = 0;
	static uint8_t One_time_Step = 0;
	static uint8_t Send_Count = 0;
	
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
	{
		//前1s启动电源
		if(ullCurrent_ms < 2000)
        {
            uSysROParam.SetPPS_On = 1;
            uSysROParam.SetPPS_Volt = Set_Volt;//100x

        }
        else
        {
			if ( (IS_One_time_Program == 0) && (ullCurrent_ms - ullCurrent_ms_50 >= 100) )
			{
				ullCurrent_ms_50 = ullCurrent_ms;
				
				switch (One_time_Step)
				{
					case NWT_TO_Operational: //## 1. 切换 NMT 到 Operational 01 01
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x00, fdcan1_TX_NWT_TO_Operational, 0x02);
						
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step ++;
						}
						
					}
					break;
					
					
					case Shutdown:			//## 2. 切换到 402 状态机 Shutdown 
					{
						
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Shutdown, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step ++;
						}
					}
					break;
					
					case Switch_On:			//## 3. 切换到 Switch On 
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Switch_On, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step ++;
						}
					}
					break;
					
					
					case Velocity_Mode:		//## 4. 设置模式为 Velocity Mode 	
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Velocity_Mode, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step ++;
						}
					}
					break;
					
					
					case Operation_Enabled:	//## 5. 切换到 Operation Enabled  
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Operation_Enabled, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step ++;
						}
						
					}
					break;
					
					case Set_Speed_CW60:	//## 6. 设置目标速度 +60 RPM 		
					case Set_Speed_CCW60:  //## 6. 设置目标速度 -60 RPM 		
					{                       
						if (One_time_Step == Set_Speed_CW60)
						{
							FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Speed_CW60, 0x08);
							
							
						}
						else if (One_time_Step == Set_Speed_CCW60)
						{
							FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Speed_CCW60, 0x08);
							
						}
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step ++;
						}
						IS_One_time_Program = 1;
					}                       
					break;
					
					default:
					{
						
					}
					break;
				}

			}
        }
		
	}
	else
    {
		One_time_Step = NWT_TO_Operational;
		IS_One_time_Program = 0;
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
	
	
}

//设置电机停止
void vAPP_SetMotorStop_GB5753X(uint32_t RunTime_ms,uint16_t Set_Volt)
{
    uint64_t ullCurrent_ms; //当前时间，相对于任务开始时

	static uint64_t ullCurrent_ms_50; 
	static uint8_t IS_One_time_Program = 0;
	static uint8_t One_time_Step = Shutdown;
	static uint8_t Send_Count = 0;
	
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
	{
		//前1s启动电源
		if(ullCurrent_ms < 500)
        {
            uSysROParam.SetPPS_On = 1;
            uSysROParam.SetPPS_Volt = Set_Volt;//100x

        }
        else
        {
			if ( (IS_One_time_Program == 0) && (ullCurrent_ms - ullCurrent_ms_50 >= 100) )
			{
				ullCurrent_ms_50 = ullCurrent_ms;
				
				switch (One_time_Step)
				{
					case Shutdown:			//## 1. 切换到 402 状态机 Shutdown 
					{
						
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Shutdown, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step = Switch_On;
						}
					}
					break;
					
					case Switch_On:			//## 2. 切换到 Switch On 
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Switch_On, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step = Vs_Mode;
						}
					}
					break;
					
					
					case Vs_Mode:		//## 3. 设置模式为 Vs Mode 	
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Vs_Mode, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step = Operation_Enabled;
						}
					}
					break;
					
					
					case Operation_Enabled:	//## 4. 切换到 Operation Enabled  
					{
						FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Operation_Enabled, 0x08);
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step = Open_Loop_CW;
						}
						
					}
					break;
					
					case Open_Loop_CW:	//## 5. 开环+100%		
					case Open_Loop_CCW:  //## 5.开环-100% 		
					{                       
						if (One_time_Step == Open_Loop_CW)
						{
							FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Open_Loop_CW, 0x08);
							
							
						}
						else if (One_time_Step == Open_Loop_CCW)
						{
							FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Open_Loop_CCW, 0x08);
							
						}
						if (Send_Count < 3)
						{
							
							Send_Count++;
						}
						else
						{
							Send_Count = 0;
							One_time_Step = Shutdown;
						}
						IS_One_time_Program = 1;
					}                       
					break;
					
					default:
					{
						
					}
					break;
				}

			}
        }
		
	}
	else
    {
		One_time_Step = Shutdown;
		IS_One_time_Program = 0;
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

//读取温度
void vAPP_ReadTmp_GB5753X(uint32_t RunTime_ms,uint16_t Set_Volt)
{
	uint64_t ullCurrent_ms; //当前时间，相对于任务开始时
	static uint64_t ullCurrent_ms_100 = 0; //当前时间，相对于任务开始时
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
		if (ullCurrent_ms - ullCurrent_ms_100 >= 100)
		{
			ullCurrent_ms = ullCurrent_ms_100;
			static uint8_t flag = 0;
			if (flag == 0)
			{
				FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Read_Driver_tmp, 0x08);
				flag = 1;
				
			}
			else if (flag == 1)
			{
				FDCAN_TX_Send(FDCAN1_TxFrame,0x601, fdcan1_TX_Set_Read_Motor_tmp, 0x08);
				flag = 0;
			}
			
		}
		
		
		uSysROParam.MotorDriverTemp = DriverBoard_DATA.MotorDriverTemp /10.f ;
		uSysROParam.MotorTemp		= DriverBoard_DATA.MotorTemp /10.f; 
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

