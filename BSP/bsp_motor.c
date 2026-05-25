#include "bsp_motor.h"
#include "bsp_pwm.h"
#include "bsp_can.h"
#include "stdlib.h"
/************************************************************************************************************
*												双PWM控制电机正负电流
*	Percentage		占空比
************************************************************************************************************/
static float usSetPercentageAcc_DC = 2;
static float usSetPercentageDec_DC = 2;
static float usSetPercentage_DC    = 0;
void Morot_DIR_PWM_Ctrl(float Percentage )
{
	//	//加速过程
//	if(Percentage  >= usSetPercentage_DC)
//    {
//        if(Percentage  > usSetPercentage_DC + usSetPercentageAcc_DC )
//        {
//            Percentage = usSetPercentage_DC + usSetPercentageAcc_DC;
//        }
//    }
//    else
//    {
//        //减速过程   
//        if(Percentage  < usSetPercentage_DC - usSetPercentageDec_DC)
//        {
//            Percentage = usSetPercentage_DC - usSetPercentageDec_DC;
//        }
//    }	

	if (Percentage > 100.0)
	{
		Percentage = 100.0;		
	}
	else if(Percentage < -100.0)
	{
		Percentage = -100.0;
	}
		
	usSetPercentage_DC = Percentage;
	
	if (0 == Percentage)
	{		

		PWM_SetOutput(htim3, TIM_CHANNEL_1, PWM_0);
		PWM_SetOutput(htim3, TIM_CHANNEL_2, PWM_0);		
		
	}
	else if(0 < Percentage )
	{		

		PWM_SetOutput(htim3, TIM_CHANNEL_1, fabs(Percentage)) ;  //PMW1控制正转
		//注意下一指令，不可更改！！！
		PWM_SetOutput(htim3, TIM_CHANNEL_2, PWM_0);		  //正转时必须PWM2为0 
	}
	else
	{
		PWM_SetOutput(htim3, TIM_CHANNEL_2, fabs(Percentage) );  //PMW2控制反转
		//注意下一指令，不可更改！！！
		PWM_SetOutput(htim3, TIM_CHANNEL_1, PWM_0);		  
	}
}


void Morot_DIR_PWM_Ctrl_Direct(float Percentage )
{
	if (Percentage > 100.0)
	{
		Percentage = 100.0;		
	}
	else if(Percentage < -100.0)
	{
		Percentage = -100.0;
	}
		
	usSetPercentage_DC = Percentage;
	
	if (0 == Percentage)
	{		

		PWM_SetOutput(htim3, TIM_CHANNEL_1, PWM_0);
		PWM_SetOutput(htim3, TIM_CHANNEL_2, PWM_0);		
		
	}
	else if(0 < Percentage )
	{		

		PWM_SetOutput(htim3, TIM_CHANNEL_1, fabs(Percentage)) ;  //PMW1控制正转
		//注意下一指令，不可更改！！！
		PWM_SetOutput(htim3, TIM_CHANNEL_2, PWM_0);		  //正转时必须PWM2为0 
	}
	else
	{
		PWM_SetOutput(htim3, TIM_CHANNEL_2, fabs(Percentage) );  //PMW2控制反转
		//注意下一指令，不可更改！！！
		PWM_SetOutput(htim3, TIM_CHANNEL_1, PWM_0);		  
	}
}

/************************************************************************************************************
*												CAN控制电机速度
*	Spe		速度
************************************************************************************************************/
static float usSetPercentageAcc_CAN = 1000;
static float usSetPercentageDec_CAN = 1000;
static int32_t s32SetPercentage_CAN    = 0;

void SetMotorSpeed_AccDec(int32_t Spe)
{
	//	//加速过程
	if(Spe  >= s32SetPercentage_CAN)
    {
        if(Spe  > s32SetPercentage_CAN + usSetPercentageAcc_CAN )
        {
            Spe = s32SetPercentage_CAN + usSetPercentageAcc_CAN;
        }
    }
    else
    {
        //减速过程   
        if(Spe  < s32SetPercentage_CAN - usSetPercentageDec_CAN)
        {
            Spe = s32SetPercentage_CAN - usSetPercentageDec_CAN;
        }
    }	

	if (Spe > 20000)
	{
		Spe = 20000;		
	}
	else if(Spe < -20000)
	{
		Spe = -20000;
	}
		
	s32SetPercentage_CAN = Spe;
	
	can_TX_SetOpenLoop.SetSpd = s32SetPercentage_CAN;    
}

