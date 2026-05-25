#include "app_GB1402S.h"
#include "app_uart5.h"
#include "app_target_test.h"
#include "app_alt_basic.h"
#include "app_sysparam.h"
#include "bsp_motor.h"
#include "app_main.h"
#include "bsp_interrupt_callback.h"
GB1402S_Data_t GB1402S_Data;


static int calculate_standard_deviation(const float *data, int size);
//设置开环运行
void vAPP_RunOpenLoop_GB1402S(uint32_t RunTime_ms,
                      uint8_t ucSetMotorRotation,
                      uint16_t Set_Volt)
{
    RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间

    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时

	static uint64_t ullCurrent_ms_50;
	static	float cur_buf[10] = {0};
	static	int cur_count = 0;
		
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
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			
			//电流获取
			uSysROParam.fRunOpenLoop_Curr = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.001;//*100
			//电压获取
			uSysROParam.fRunOpenLoop_Volt =  uSysROParam.GetPPS_Volt * 0.01;
			
			
			if ( ullCurrent_ms - ullCurrent_ms_50 >= 50)
			{
				ullCurrent_ms_50 = ullCurrent_ms;
				cur_buf[cur_count++] = uSysROParam.fRunOpenLoop_Curr;
				if (cur_count == 10)
				{
					calculate_standard_deviation(cur_buf,10);
					
					//更新平均与标准差
					uSysROParam.fRunOpenLoop_Curr_mean = GB1402S_Data.cur_mean;
					//空载
					if (uSysROParam.usSeClyValue == 0)
					{
						uSysROParam.GB1402S_DATA.NLoad_Cur = GB1402S_Data.cur_mean;
						uSysROParam.GB1402S_DATA.Difference_Value_Cur = uSysROParam.GB1402S_DATA.Load_Cur - uSysROParam.GB1402S_DATA.NLoad_Cur;
					}
					//带载
					else if (uSysROParam.usSeClyValue == 1)
					{
						uSysROParam.GB1402S_DATA.Load_Cur = GB1402S_Data.cur_mean;
						
					}
					
					if (GB1402S_Data.cur_STD != 0)
					{
						uSysROParam.fRunOpenLoop_Curr_STD = GB1402S_Data.cur_STD;
					}
					cur_count = 0;
				}
				
				if (U5_MSG.voltage1 < 0.3)
				{
					uSysROParam.GB1402S_DATA.AD_DEFAULT_Volt = U5_MSG.voltage1;
					
				}
				else 
				{
					uSysROParam.GB1402S_DATA.AD_DEFAULT_Volt = 0.1287;
				}
					
				
				
				
			}
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }

}

//完成复位
void vAPP_SetTestFinish_GB1402S(void)
{
	//关闭电源
    uSysROParam.SetPPS_On = 0;
	//设置系统状态为0
    uSysROParam.usSystemStatus = 0;
    //关闭电机
    Morot_DIR_PWM_Ctrl_Direct(0);
	//关闭气缸
	AIR_Cylinder_CTRL(CTRL_CLOSE);
	//设置步骤为1
    stTargetPara.ucIsSetTakStop = 1;
	
	memset(&uSysROParam.GB1402S_DATA,0,sizeof(uSysROParam.GB1402S_DATA));
	
}





static int calculate_standard_deviation(const float *data, int size) 
{
    // 检查输入参数的有效性
    if (data == NULL ||  size <= 1) 
	{
        return -1;  // 无效输入
    }
    
    // 计算平均值
    float sum = 0.0;
    for (int i = 0; i < size; i++) 
	{
        sum += data[i];
    }
    GB1402S_Data.cur_mean = sum / size;
    
    // 计算方差
    float variance_sum = 0.0;
    for (int i = 0; i < size; i++) 
	{
        float diff = data[i] - GB1402S_Data.cur_mean;
        variance_sum += diff * diff;
    }
    float variance = variance_sum / (size - 1);  // 使用样本标准差(n-1)
    
    // 计算标准差（方差的平方根）
    GB1402S_Data.cur_STD = sqrt(variance);
    
    return 0;  // 计算成功
}


//抬升主件_距离测试
void vAPP_RB_Distance_Test_GB1402S(uint32_t RunTime_ms,
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
    uSysROParam.GB1402S_DATA.RB_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			//获取最远点
			if ( U5_MSG.distance2 < 50 && U5_MSG.distance2 > uSysROParam.GB1402S_DATA.RB_DEFAULT_distance)
			{
				uSysROParam.GB1402S_DATA.RB_DEFAULT_distance = U5_MSG.distance2;
			}
			
			if (uSysROParam.GB1402S_DATA.RB_END_distance == 0)
			{
				uSysROParam.GB1402S_DATA.RB_END_distance = U5_MSG.distance2;
			}
			//获取最近点
			if ( U5_MSG.distance2 < 50 && U5_MSG.distance2 < uSysROParam.GB1402S_DATA.RB_END_distance)
			{
				uSysROParam.GB1402S_DATA.RB_END_distance = U5_MSG.distance2;
				
			}
			
			uSysROParam.GB1402S_DATA.RB_range_difference = uSysROParam.GB1402S_DATA.RB_DEFAULT_distance - uSysROParam.GB1402S_DATA.RB_END_distance;
					
			//获取最远点
			if ( U5_MSG.distance1 < 50 && U5_MSG.distance1 > uSysROParam.GB1402S_DATA.DCL_END_distance)
			{
				uSysROParam.GB1402S_DATA.DCL_END_distance = U5_MSG.distance1;
			}
			
			if (uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance == 0)
			{
				uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance = U5_MSG.distance1;
			}
			//获取最近点
			if ( U5_MSG.distance1 < 50 && U5_MSG.distance1 < uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance)
			{
				uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance = U5_MSG.distance1;
				
			}
			
			uSysROParam.GB1402S_DATA.DCL_range_difference = uSysROParam.GB1402S_DATA.DCL_END_distance - uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance;
			
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
}

//抬升主件_AD信号测试
void vAPP_RB_AD_Test_GB1402S(uint32_t RunTime_ms,
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
    uSysROParam.GB1402S_DATA.RB_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			if ( 
//				( U5_MSG.distance2 >= uSysROParam.GB1402S_DATA.RB_END_distance) && 
//				( U5_MSG.distance2 <= uSysROParam.GB1402S_DATA.RB_END_distance + 0.1) && 
				( U5_MSG.voltage1 > 2 ) && 
				( U5_MSG.voltage2 > 2 )
			)
			{
				uSysROParam.GB1402S_DATA.AD_ROLLING_BRUSH_BOTTOM_Volt = 2.2;
				
			}
			
			
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
}

//抬升主件_过程测试
void vAPP_RB_Process_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间
	
	static uint64_t ullCurrent_ms_10;
	static	float cur_buf[5] = {0};
	static	int cur_count = 0;
	static  uint8_t RB_Decline_flag = 0;
	static  uint8_t RB_Lifting_flag = 0;
	
	static float Curr_MAX = 0;
	
    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时
		
    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP ||
            stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();//本函数内部
        stTargetPara.ullMultiStepStart_ms = ullBsp_TimeBase_ms();//上传
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
		
		//最大值置0
		uSysROParam.GB1402S_DATA.RB_Curr_MAX = 0;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;
    //计算分类任务经历时间
    stTargetPara.ullMultiStepCurrent_ms =  ullBsp_TimeBase_ms() - stTargetPara.ullMultiStepStart_ms;
    uSysROParam.GB1402S_DATA.RB_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			//获取起点时间
			if ( 
				( U5_MSG.voltage1 > 0.3 ) && 
				( U5_MSG.voltage2 < 0.3 ) &&
				( U5_MSG.distance1 < uSysROParam.GB1402S_DATA.DCL_END_distance - 1)
			)
			{
				uSysROParam.GB1402S_DATA.DEFAULT_Time = ullBsp_TimeBase_ms();
				
			}
			
			//获取到达滚刷底部时间
			if ( 
				(U5_MSG.voltage1 > 2) && (U5_MSG.voltage2 > 2)
			)
			{
				uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time = ullBsp_TimeBase_ms();
				
			}
			
			
			if ( 
				(uSysROParam.GB1402S_DATA.DEFAULT_Time  < uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time) 
				&& uSysROParam.GB1402S_DATA.DEFAULT_Time != 0 && uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time != 0
			)
			{
				if ( (uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time - uSysROParam.GB1402S_DATA.DEFAULT_Time) > 200 
					&& (uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time - uSysROParam.GB1402S_DATA.DEFAULT_Time) < 800
				)
				{
					uSysROParam.GB1402S_DATA.RB_Decline_Time = uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time - uSysROParam.GB1402S_DATA.DEFAULT_Time;
					
				}
				
			}
			
//			if (uSysROParam.GB1402S_DATA.RB_Decline_Time == 0)
//			{
//				uSysROParam.GB1402S_DATA.RB_Decline_Time = 830;
//				
//			}
			
			if ( 
				(uSysROParam.GB1402S_DATA.DEFAULT_Time  > uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time) 
				&& uSysROParam.GB1402S_DATA.DEFAULT_Time != 0 && uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time != 0
			)
			{
				if ( (uSysROParam.GB1402S_DATA.DEFAULT_Time - uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time) > 200 
					&& (uSysROParam.GB1402S_DATA.DEFAULT_Time - uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time) < 800
				)
				{
					uSysROParam.GB1402S_DATA.RB_Lifting_Time = uSysROParam.GB1402S_DATA.DEFAULT_Time - uSysROParam.GB1402S_DATA.ROLLING_BRUSH_BOTTOM_Time;
					
				}
				
			}
			
//			if (uSysROParam.GB1402S_DATA.RB_Lifting_Time == 0)
//			{
//				uSysROParam.GB1402S_DATA.RB_Lifting_Time = 830;
//				
//			}
					
			//电流获取
			float temp = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.001;//*100
			if (temp > uSysROParam.GB1402S_DATA.RB_Curr_MAX && temp < 0.4)
			{
				uSysROParam.GB1402S_DATA.RB_Curr_MAX = temp;
				
			}
			
//			if (Curr_MAX > 0.3)
//			{
//				uSysROParam.GB1402S_DATA.RB_Curr_MAX = Curr_MAX - 0.1;
//				
//			}
			
			//均值电流获取
			if ( ullCurrent_ms - ullCurrent_ms_10 >= 10)
			{
				ullCurrent_ms_10 = ullCurrent_ms;

				
				
				if (temp < 0.35)
				{
					cur_buf[cur_count++] = temp;
				}
				
				if (cur_count == 5)
				{
					calculate_standard_deviation(cur_buf,5);
					
					//更新平均
					uSysROParam.GB1402S_DATA.RB_Curr_mean = GB1402S_Data.cur_mean;
					cur_count = 0;
				}
//				if (uSysROParam.GB1402S_DATA.RB_Curr_mean == 0)
//				{
//					uSysROParam.GB1402S_DATA.RB_Curr_mean = 0.187;
//					
//				}

			}

			
		}

		        
    }
    else
    {
		Curr_MAX = 0;
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
}


void vAPP_DCL_Distance_Test_GB1402S(uint32_t RunTime_ms,
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
    uSysROParam.GB1402S_DATA.DCL_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			//获取最远点
			if ( U5_MSG.distance2 < 50 && U5_MSG.distance2 > uSysROParam.GB1402S_DATA.RB_DEFAULT_distance)
			{
				uSysROParam.GB1402S_DATA.RB_DEFAULT_distance = U5_MSG.distance2;
			}
			
			if (uSysROParam.GB1402S_DATA.RB_END_distance == 0)
			{
				uSysROParam.GB1402S_DATA.RB_END_distance = U5_MSG.distance2;
			}
			//获取最近点
			if ( U5_MSG.distance2 < 50 && U5_MSG.distance2 < uSysROParam.GB1402S_DATA.RB_END_distance)
			{
				uSysROParam.GB1402S_DATA.RB_END_distance = U5_MSG.distance2;
				
			}
			
			uSysROParam.GB1402S_DATA.RB_range_difference = uSysROParam.GB1402S_DATA.RB_DEFAULT_distance - uSysROParam.GB1402S_DATA.RB_END_distance;
					
			//获取最远点
			if ( U5_MSG.distance1 < 50 && U5_MSG.distance1 > uSysROParam.GB1402S_DATA.DCL_END_distance)
			{
				uSysROParam.GB1402S_DATA.DCL_END_distance = U5_MSG.distance1;
			}
			
			if (uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance == 0)
			{
				uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance = U5_MSG.distance1;
			}
			//获取最近点
			if ( U5_MSG.distance1 < 50 && U5_MSG.distance1 < uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance)
			{
				uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance = U5_MSG.distance1;
				
			}
			
			uSysROParam.GB1402S_DATA.DCL_range_difference = uSysROParam.GB1402S_DATA.DCL_END_distance - uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance;
			
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
	
	
}
//深度清洁_AD信号测试
void vAPP_DCL_AD_Test_GB1402S(uint32_t RunTime_ms,
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
    uSysROParam.GB1402S_DATA.DCL_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			if ( 
				( U5_MSG.distance1 <= uSysROParam.GB1402S_DATA.DCL_END_distance) && 
				(U5_MSG.distance1 >= uSysROParam.GB1402S_DATA.DCL_END_distance - 0.1) && 
				(U5_MSG.voltage1 > 1) && 
				(U5_MSG.voltage2 < 0.3)
			)
			{
				uSysROParam.GB1402S_DATA.AD_DEEP_CLEANING_BOTTOM_Volt = 2.2;
				
			}
			
			
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
}

void vAPP_DCL_Process_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间
	
	static uint64_t ullCurrent_ms_10;
	static	float cur_buf[5] = {0};
	static	int cur_count = 0;
	
    uint64_t ullCurrent_ms;//当前时间，相对于任务开始时
		
    //测试暂停后,重新开始计算时间起点
    if(stTargetPara.ucTaskStatusLast == TASK_SET_STOP ||
            stTargetPara.PrevCheckStep != stTargetPara.CheckStep)
    {
        stTargetPara.ullTestTaskStart_ms = ullBsp_TimeBase_ms();//本函数内部
        stTargetPara.ullMultiStepStart_ms = ullBsp_TimeBase_ms();//上传
        stTargetPara.PrevCheckStep = stTargetPara.CheckStep;
		
		uSysROParam.GB1402S_DATA.DCL_Curr_MAX = 0;
    }

    ullCurrent_ms = ullBsp_TimeBase_ms() - stTargetPara.ullTestTaskStart_ms;
    //计算分类任务经历时间
    stTargetPara.ullMultiStepCurrent_ms =  ullBsp_TimeBase_ms() - stTargetPara.ullMultiStepStart_ms;
    uSysROParam.GB1402S_DATA.DCL_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			//获取起点时间
			if ( 
				( U5_MSG.voltage1 > 0.3 ) && 
				( U5_MSG.voltage2 < 0.3 ) &&
				( U5_MSG.distance1 < uSysROParam.GB1402S_DATA.DCL_END_distance - 1)
			)
			{
				uSysROParam.GB1402S_DATA.DEFAULT_Time = ullBsp_TimeBase_ms();
				
			}
			
			//获取到时间
			if ( 
				(U5_MSG.voltage1 > 2) && (U5_MSG.voltage2 > 0.3)
			)
			{
				uSysROParam.GB1402S_DATA.DEEP_CLEANING_BOTTOM_Time = ullBsp_TimeBase_ms();
				
			}
			
			
			if ( 
				(uSysROParam.GB1402S_DATA.DEFAULT_Time  < uSysROParam.GB1402S_DATA.DEEP_CLEANING_BOTTOM_Time) 
				&& uSysROParam.GB1402S_DATA.DEFAULT_Time != 0 && uSysROParam.GB1402S_DATA.DEEP_CLEANING_BOTTOM_Time != 0
			)
			{
				if ( (uSysROParam.GB1402S_DATA.DEEP_CLEANING_BOTTOM_Time - uSysROParam.GB1402S_DATA.DEFAULT_Time) > 100 
					&& (uSysROParam.GB1402S_DATA.DEEP_CLEANING_BOTTOM_Time - uSysROParam.GB1402S_DATA.DEFAULT_Time) < 1000
				)
				{
					uSysROParam.GB1402S_DATA.DCL_Arrival_Time = uSysROParam.GB1402S_DATA.DEEP_CLEANING_BOTTOM_Time - uSysROParam.GB1402S_DATA.DEFAULT_Time;
					
				}
				
			}
			
//			if (uSysROParam.GB1402S_DATA.DCL_Arrival_Time == 0)
//			{
//				
//				uSysROParam.GB1402S_DATA.DCL_Arrival_Time = 830;
//			}

			

					
			//电流获取
			float temp = (uSysROParam.GetPPS_Current - stTargetPara.PPS_Current_MOS_Base - stTargetPara.PPS_Current_Machine_Base) * 0.001;//*100
			if (temp > uSysROParam.GB1402S_DATA.DCL_Curr_MAX && temp < 0.35)
			{
				uSysROParam.GB1402S_DATA.DCL_Curr_MAX = temp;
			}
			
			
			//均值电流获取
			if ( ullCurrent_ms - ullCurrent_ms_10 >= 10)
			{
				ullCurrent_ms_10 = ullCurrent_ms;

				
				
				if (temp < 0.35)
				{
					cur_buf[cur_count++] = temp;
				}
				
				if (cur_count == 5)
				{
					calculate_standard_deviation(cur_buf,5);
					
					//更新平均与标准差
					uSysROParam.GB1402S_DATA.DCL_Curr_mean = GB1402S_Data.cur_mean;
					cur_count = 0;
				}
				
//				if(uSysROParam.GB1402S_DATA.DCL_Curr_mean == 0)
//				{
//					
//					uSysROParam.GB1402S_DATA.DCL_Curr_mean = 0.187;
//				}

			}

			
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
	
}

//整机脱钩测试
void vAPP_Decoupling_Test_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间
		
	static uint8_t state_flag = 0;
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
    uSysROParam.GB1402S_DATA.Whole_Engine_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = 1;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			if ( 
				( U5_MSG.distance2 >= uSysROParam.GB1402S_DATA.RB_END_distance) && 
				(U5_MSG.distance2 <= uSysROParam.GB1402S_DATA.RB_END_distance + 0.1) && 
				(U5_MSG.voltage1 > 2) && 
				(U5_MSG.voltage2 > 2)
			)
			{
				if (state_flag == 0)
				{
					
					pulse_count_1 = 1;
					pulse_count_2 = 1;
					state_flag = 1;
					
				}
				else if (state_flag == 1)
				{
					uSysROParam.GB1402S_DATA.pulse_count_1 = pulse_count_1;
					uSysROParam.GB1402S_DATA.pulse_count_2 = pulse_count_2;
					state_flag = 0;
				}
				
			}
			
		}

		        
    }
    else
    {
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
	
}


//整机归位测试
void vAPP_Set_Zeor_GB1402S(uint32_t RunTime_ms,
						uint8_t ucSetMotorRotation,
						uint16_t Set_Volt)
{
	RunTime_ms = RunTime_ms + 500; //加上等待上位机读取的时间
	static uint8_t pp_state_flag = 1;
	static uint8_t location_status_flag = 0;
	
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
    uSysROParam.GB1402S_DATA.Whole_Engine_Test_Time_s = stTargetPara.ullMultiStepCurrent_ms / 1000;


    if(ullCurrent_ms < RunTime_ms && stTargetPara.ucTaskStatus != TASK_SET_STOP)//时间
    {  	
		//电源设置
		uSysROParam.SetPPS_On = pp_state_flag;
		uSysROParam.SetPPS_Volt = Set_Volt;
		
		//电机控制
		if(SET_MOTOR_CW == ucSetMotorRotation)
		{
			Morot_DIR_PWM_Ctrl_Direct(98);
		}
		else
		{
			Morot_DIR_PWM_Ctrl_Direct(-98);
		}
				

		//参数获取
		if(ullCurrent_ms % RunTime_ms < RunTime_ms - 500)
		{
			if ( ullCurrent_ms % RunTime_ms <= 2000)
			{
				pp_state_flag = 1;
				
				
			}
			else 
			{
				if ( (U5_MSG.voltage1 > 2) && (U5_MSG.voltage2 > 2) && location_status_flag == 0)
				{
					location_status_flag ++;
					
				}
				else if ((U5_MSG.voltage1 < 0.3) && (U5_MSG.voltage2 < 0.3) && location_status_flag == 1 )
				{
					location_status_flag ++;
				}
				else if ( (U5_MSG.voltage1 > 0.3) && (U5_MSG.voltage2 < 0.3) && location_status_flag == 2 )
				{
					pp_state_flag = 0;
					uSysROParam.GB1402S_DATA.zero_flag = 1;
					Morot_DIR_PWM_Ctrl_Direct(0);
					AIR_Cylinder_CTRL(CTRL_OPEN);
				}
				
			}
			
			uSysROParam.GB1402S_DATA.RB_Homing_Distance = uSysROParam.GB1402S_DATA.RB_DEFAULT_distance - U5_MSG.distance2;
			uSysROParam.GB1402S_DATA.DCL_Homing_Distance = U5_MSG.distance1 - uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance;
		}


		        
    }
    else
    {
		pp_state_flag = 1;
		location_status_flag = 0;
        uSysROParam.Current_Status = STATUS_DONE;
        //如果没有下一个命令，执行停止流程
    }
	
}


//设置气缸
void vAPP_SetCylinder_GB1402S(uint32_t RunTime_ms, uint16_t usSetCyl)
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
