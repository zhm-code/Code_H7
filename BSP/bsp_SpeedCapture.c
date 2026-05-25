#include "bsp_SpeedCapture.h"
#include "app_sysparam.h"
#include "math.h"

SpeedCapture_t PE_SpeedCapture; //光电测速
SpeedCapture_t FG_SpeedCapture; //FG反馈


Zero_Set_t Zero_Set = {
	.uStart_Flag = 0,
	.fSet_PWM = 17.0f
};
STD_t user_spd_STD;

//速度运算
void Spe_Calculation(void)
{
	if(uSysROParam.PECoefficient == 0)
	{
		uSysROParam.PECoefficient = 1;
		
	}
	//判断数据是否准备好
	if(PE_SpeedCapture.Data_Busy_Flag == SET)
    {
	  float Spe_Tmp = (60*1000*10) / (float)PE_SpeedCapture.Cycle_Tim_Count / (float)uSysROParam.PECoefficient;
	  if(Spe_Tmp != 0 ) //滤波
	  {
			stTargetPara.fMotorSpeed = Spe_Tmp;

	  }
	  //数据处理完成复位等待下次进入
	  PE_SpeedCapture.Data_Busy_Flag = RESET;
	  
    }
	
	if(uSysROParam.ReductionRatio == 0)
	{
		uSysROParam.ReductionRatio = 1;
	}
	if (uSysROParam.MagneticPole == 0)
	{
		uSysROParam.MagneticPole = 1;
		
	}
	
	//判断数据是否准备好
	if(FG_SpeedCapture.Data_Busy_Flag == SET)
    {
	  float Spe_Tmp = (60*1000*1000) / (float)FG_SpeedCapture.Cycle_Tim_Count / uSysROParam.MagneticPole /uSysROParam.ReductionRatio;
	  if(Spe_Tmp != 0 ) //滤波
	  {
			stTargetPara.fFG_MotorSpeed = Spe_Tmp;

	  }
	  //数据处理完成复位等待下次进入
	  FG_SpeedCapture.Data_Busy_Flag = RESET;
	  
    }
	  
}


