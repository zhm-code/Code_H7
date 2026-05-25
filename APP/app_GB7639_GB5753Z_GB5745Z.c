#include "app_GB7639_GB5753Z_GB5745Z.h"

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

#include "app_XR_API.h"

//负载标志位
static uint8_t IS_load = 0;

static uint8_t sg_motor_mode;

//PID设置
static pid_t pid_pwm_set_5745Z = {
	.fKp = 0.2,
	.fKi = 0.0f,
	.fKd = 0.0f,
};
static pid_t pid_pwm_set_7639 = {
	.fKp = 0.1,
	.fKi = 0.0f,
	.fKd = 0.0f,
};

static pid_t pid_pwm_set_5753Z = {
	.fKp = 0.1,
	.fKi = 0.0f,
	.fKd = 0.0f,
};

static float state_val = 0;
static float end_val = 0;

//设置开环运行
void vAPP_RunOpenLoop_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间

    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时

	/*控制时间进入频率*/
	static uint64_t ullCurrent_ms_100_load 		= 0;			//加载频率 10HZ		
	static uint64_t ullCurrent_ms_100_send_cmd  = 0;			//发送命令频率 10HZ

	static uint8_t	s_u_cnt				= 0;									


    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP ||
            stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();//本函数内部
        stTargetPara.ullMultiStepStart_ms = ullBsp_TimeBase_ms();//上传
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
		
		switch ((uint16_t)uSysROParam.ProjNumber)
		{
			case GB7639:
			{
				state_val = 0;
				end_val = 46;
			}
			break;
			
			case GB5753Z:
			{
				state_val = 0;
				end_val = 41;
			}
			break;
		
			case GB5745Z:
			{
				state_val = 0;
				end_val = 41;
			}
			break;
		}

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

		//正反转控制
		switch (ucSetMotorRotation)
        {
			//正转
        	case SET_MOTOR_CW:
        	{
				switch (sg_motor_mode)
				{
					case 2:	//位置模式
					{

					}
					break;
					
					case 3: //速度模式
					{
						if (IS_load == 0)
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								
								if(s_u_cnt <= 0)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_speed(2.0f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_speed(2.0f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_speed(2.0f);
										}
										break;

										case GB6816:
										{
											motor_set_speed(2.0f);
										}
										break;
									}
									

								}
								else if(s_u_cnt > 0 && s_u_cnt <= 11)
								{

								}
								else if(s_u_cnt > 11 && s_u_cnt <= 12)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_speed(5.236f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_speed(5.236f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_speed(5.236f);
										}
										break;

										case GB6816:
										{
											motor_set_speed(5.236f);
										}
										break;
									}
								}
								s_u_cnt++;
							}
						}
						else
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								switch ((uint16_t)uSysROParam.ProjNumber)
								{
									case GB7639:
									{
										motor_set_speed(5.236f);
									}
									break;
									
									case GB5753Z:
									{
										motor_set_speed(5.236f);
									}
									break;
								
									case GB5745Z:
									{
										motor_set_speed(5.236f);
									}
									break;

									case GB6816:
									{
										motor_set_speed(5.236f);
									}
									break;
								}
							}
						}
						
					}
					break;
					
					case 6: //力矩模式
					{
						if (IS_load == 0)
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								
								if(s_u_cnt <= 0)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_torque(4.0f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_torque(4.0f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_torque(4.0f);
										}
										break;

										case GB6816:
										{
											motor_set_torque(4.0f);
										}
										break;
									}
									
								}
								else if(s_u_cnt > 0 && s_u_cnt <= 21)
								{

								}
								else if(s_u_cnt > 21 && s_u_cnt <= 22)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_torque(120.0f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_torque(120.0f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_torque(120.0f);
										}
										break;

										case GB6816:
										{
											motor_set_torque(120.0f);
										}
										break;
									}
								}
								s_u_cnt++;
							}
						}
						else
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								switch ((uint16_t)uSysROParam.ProjNumber)
								{
									case GB7639:
									{
										motor_set_torque(120.0f);
									}
									break;
									
									case GB5753Z:
									{
										motor_set_torque(120.0f);
									}
									break;
								
									case GB5745Z:
									{
										motor_set_torque(120.0f);
									}
									break;
									
									case GB6816:
									{
										motor_set_torque(120.0f);
									}
									break;
								}
							}
						}

					}
					break;

				}

				
        	}
        	break;
			
			//反转
        	case SET_MOTOR_CCW:
        	{
        		switch (sg_motor_mode)
				{
					case 2:	//位置模式
					{

					}
					break;
					
					case 3: //速度模式
					{
						if (IS_load == 0)
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								
								if(s_u_cnt <= 0)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_speed(-2.0f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_speed(-2.0f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_speed(-2.0f);
										}
										break;
									
										case GB6816:
										{
											motor_set_speed(-2.0f);
										}
										break;
									}
								}
								else if(s_u_cnt > 0 && s_u_cnt <= 11)
								{

								}
								else if(s_u_cnt > 11 && s_u_cnt <= 12)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_speed(-5.236f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_speed(-5.236f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_speed(-5.236f);
										}
										break;
									
										case GB6816:
										{
											motor_set_speed(-5.236f);
										}
										break;
									}
									
								}
								s_u_cnt++;
							}
						}
						else
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								switch ((uint16_t)uSysROParam.ProjNumber)
								{
									case GB7639:
									{
										motor_set_speed(-5.236f);
									}
									break;
									
									case GB5753Z:
									{
										motor_set_speed(-5.236f);
									}
									break;
								
									case GB5745Z:
									{
										motor_set_speed(-5.236f);
									}
									break;
									
									case GB6816:
									{
										motor_set_speed(-5.236f);
									}
									break;
								}
							}
						}
					}
					break;
					
					case 6: //力矩模式
					{
						if (IS_load == 0)
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								
								if(s_u_cnt <= 0)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_torque(-4.0f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_torque(-4.0f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_torque(-4.0f);
										}
										break;
									
										case GB6816:
										{
											motor_set_torque(-4.0f);
										}
										break;
									}
								}
								else if(s_u_cnt > 0 && s_u_cnt <= 21)
								{

								}
								else if(s_u_cnt > 21 && s_u_cnt <= 22)
								{
									switch ((uint16_t)uSysROParam.ProjNumber)
									{
										case GB7639:
										{
											motor_set_torque(-120.0f);
										}
										break;
										
										case GB5753Z:
										{
											motor_set_torque(-120.0f);
										}
										break;
									
										case GB5745Z:
										{
											motor_set_torque(-120.0f);
										}
										break;
									
										case GB6816:
										{
											motor_set_torque(-120.0f);
										}
										break;
									}
									
								}
								s_u_cnt++;
							}
						}
						else
						{
							if (ullCurrent_ms - ullCurrent_ms_100_send_cmd >= 100) //10HZ
							{
								ullCurrent_ms_100_send_cmd = ullCurrent_ms;
								switch ((uint16_t)uSysROParam.ProjNumber)
								{
									case GB7639:
									{
										motor_set_torque(-120.0f);
									}
									break;
									
									case GB5753Z:
									{
										motor_set_torque(-120.0f);
									}
									break;
								
									case GB5745Z:
									{
										motor_set_torque(-120.0f);
									}
									break;
									
									case GB6816:
									{
										motor_set_torque(-120.0f);
									}
									break;
								}
							}
						}

					}
					break;

				}
        	}
        	break;
			
        }
		
		//负载控制
		switch (IS_load)
		{
			//加载
			case 1:
			{
				switch ((uint16_t)uSysROParam.ProjNumber)
				{
					case GB7639:
					{
						if (ullCurrent_ms - ullCurrent_ms_100_load >= 100)
						{
							ullCurrent_ms_100_load = ullCurrent_ms;
							DAC_Set_Percentage(state_val );
							if (state_val < end_val)
							{
								state_val += 1;
								
							}
							if (state_val > end_val)
							{
								state_val = end_val;
							}
							
						}
					}
					break;
					
					case GB5753Z:
					{
						if (ullCurrent_ms < 2000)
						{
							
							DAC_Set_Percentage(0);
							
						}
						else
						{
							DAC_Set_Percentage(78);
						}
						
					}
					break;
				
					case GB5745Z:
					{
						if (ullCurrent_ms < 2000)
						{
							DAC_Set_Percentage(pid_pwm_set_5745Z.fInitial_Value);
						}
						else
						{
//							//每100ms进入一次
//							if (ullCurrent_ms - ullCurrent_ms_100_load >= 100)
//							{
//								ullCurrent_ms_100_load = ullCurrent_ms;
//								if(U5_MSG.Torque_Sensor_Torque < 0)
//								{
//									U5_MSG.Torque_Sensor_Torque = -U5_MSG.Torque_Sensor_Torque;
//									
//								}
//								
//								PID_Controller(3, U5_MSG.Torque_Sensor_Torque, &pid_pwm_set_5745Z);
//								DAC_Set_Percentage(pid_pwm_set_5745Z.fActual_Value );
//								pid_pwm_set_5745Z.fActual_Value = pid_pwm_set_5745Z.fActual_Value + pid_pwm_set_5745Z.fOutput;
//							}
							
							DAC_Set_Percentage(26);
						}
					}
					break;
				}
				

				
				
				
			}
			break;
			
			//空载
			case 0:
			{
				DAC_Set_Percentage(0);
			}
			break;

		}
		
		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{

			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.01;//*100
			uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;
			uSysROParam.fRunOpenLoop_spd_dev =  uSysROParam.cMotorRotationIn * stTargetPara.fMotorSpeed;
		}
			
		

		        
    }
    else
    {	
		s_u_cnt = 0;
		state_val = 0;
		
		pid_pwm_set_5745Z.fError 		= 0;
		pid_pwm_set_5745Z.fOutput 		= 0;
		pid_pwm_set_5745Z.fProportion 	= 0;
		pid_pwm_set_5745Z.fIntegral 	= 0;
		pid_pwm_set_5745Z.fDerivative 	= 0;
		pid_pwm_set_5745Z.fActual_Value = 0;
		
		pid_pwm_set_7639.fError 		= 0;
		pid_pwm_set_7639.fOutput 		= 0;
		pid_pwm_set_7639.fProportion 	= 0;
		pid_pwm_set_7639.fIntegral 		= 0;
		pid_pwm_set_7639.fDerivative 	= 0;
		pid_pwm_set_7639.fActual_Value 	= 0;
		
		DAC_Set_Percentage(0);
		
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_GB7639_GB5753Z_GB5745Z(void)
{
	//负载为0
	DAC_Set_Percentage(0);	
	//关闭电源
    uSysROParam.SetPPS_On = 0;
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
	state_val = 0;
	/*电机差异*/
//	motor_disabled();
	
	//清零操作
	uSysROParam.MotorDriverTemp 	= 0;			//电机驱动器温度
	uSysROParam.MotorTemp 			= 0;			//电机温度
	motor_status2.mos_temperature 	= 0;			//MOS温度
	motor_status2.motor_temperature = 0;			//电机温度


}


//设置负载
void vAPP_SetLoad_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms, uint16_t usSetLoad)
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
			
			IS_load = 1;
			ICS_POWER_CTRL(CTRL_OPEN);
        }
        else
        {
			
			IS_load = 0;
			DAC_Set_Percentage(0);
			ICS_POWER_CTRL(CTRL_CLOSE);
        }

    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

//设置电机停止
void vAPP_SetMotorStop_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms,uint16_t Set_Volt)
{
    uint64_t ullCurrent_ms; //当前时间，相对于任务开始时
	static uint64_t ullCurrent_ms_100 = 0;
	
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;
	
    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		if (ullCurrent_ms - ullCurrent_ms_100 >= 200)
		{
			ullCurrent_ms_100 = ullCurrent_ms;
			switch (sg_motor_mode)
			{
				case 3:
				{
					motor_set_speed(0.0f);
				}
				break;

				case 6:
				{
					motor_set_torque(0);
				}
				break;
			}
		}
    }
    else
    {
		ullCurrent_ms_100 = 0;
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

//驱动器上电
void vAPP_DrivePowerOn_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms, uint16_t Set_Volt)
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
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;//100x
		
	}
	else
    {
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
	
	
}


void vAPP_motor_set_mode_GB7639_GB5753Z_GB5745Z(uint32_t time, uint16_t set_mode)
{
	 uint64_t ullCurrent_ms; //当前时间，相对于任务开始时
	 static uint64_t ullCurrent_ms_100; //当前时间，相对于任务开始时
	
	 static uint8_t s_u_once_flag = 0;

    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < time && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
			if (ullCurrent_ms - ullCurrent_ms_100 >=100)
			{
				ullCurrent_ms_100 = ullCurrent_ms;
				
				if(s_u_once_flag <= 10)
				{
					s_u_once_flag++;
					motor_set_mode(set_mode);
					sg_motor_mode = set_mode;
				}
			}
			
    }
    else
    {
				s_u_once_flag = 0;
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}


void vAPP_motor_enabled_GB7639_GB5753Z_GB5745Z(uint32_t time)
{
	 uint64_t ullCurrent_ms; //当前时间，相对于任务开始时
	 static uint8_t s_u_once_flag = 0;
	 static uint64_t ullCurrent_ms_100;
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < time && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
			if (ullCurrent_ms - ullCurrent_ms_100 >= 100)
			{
				ullCurrent_ms_100 = ullCurrent_ms;
				
				if(s_u_once_flag <= 3)
				{
					s_u_once_flag++;
					motor_enabled();
				}
			}
			
    }
    else
    {
		s_u_once_flag = 0;
      	uSysROParam.Current_Status = STATUS_DONE;		//结束
    }
}

void vAPP_motor_disabled_GB7639_GB5753Z_GB5745Z(uint32_t time)
{
	 uint64_t ullCurrent_ms; //当前时间，相对于任务开始时
	 static uint8_t s_u_once_flag = 0;
	 static uint64_t ullCurrent_ms_100;
	
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < time && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {
				if (ullCurrent_ms - ullCurrent_ms_100 >= 100)
				{
					ullCurrent_ms_100 = ullCurrent_ms;
					
					if(s_u_once_flag <= 3)
					{
						s_u_once_flag++;
						motor_disabled();
					}
				}
    }
    else
    {
			s_u_once_flag = 0;
        uSysROParam.Current_Status = STATUS_DONE;//结束
    }
}

//读取温度
void vAPP_ReadTmp_GB7639_GB5753Z_GB5745Z(uint32_t RunTime_ms,uint16_t Set_Volt)
{
	uint64_t ullCurrent_ms; 
	static uint64_t ullCurrent_ms_100 = 0;
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP || stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;

    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)
    {
		if (ullCurrent_ms - ullCurrent_ms_100 >= 100)
		{
			ullCurrent_ms_100 = ullCurrent_ms;

			motor_get_state();
		}
		
		
		uSysROParam.MotorDriverTemp = motor_status2.mos_temperature;
		
		uSysROParam.MotorTemp		= motor_status2.motor_temperature ;
    }
    else
    {
        uSysROParam.Current_Status 	= STATUS_DONE;	//结束


    }
}

//设置气缸
void vAPP_SetCylinder_GB6816(uint32_t RunTime_ms, uint16_t usSetCyl)
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

