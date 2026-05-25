#include "bsp_key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "bsp_ENC.h"
#include "app_user_main.h"
#include "app_sysparam.h"



//记录 key值 
USER_KEY_VAL xKEY_Val;
//模式锁
static uint8_t MODE_LOCK = RESET; 
//拨码器ID
uint8_t uUser_ID = 0x00; 

uint8_t switchVal = 0;//按键开关状态
uint8_t switchLock = 0;//按键是否一直按下

uint32_t switchCnt_20ms = 0;//按键按下后20ms计数
uint32_t switchCnt_Pre = 0;//上一次按键按下20ms计数

extern  struct ST_TARGET_TEST_PARAM stTargetPara;


/*检测按键组函数*/
//mode SET 支持连按模式  RESET 不支持
USER_KEY_VAL xGet_Key_Val(uint8_t mode )
{
	/*初始化key值为零*/
	xKEY_Val = USER_KEY_NULL;
	
	
	if(mode == SET)
	{
		MODE_LOCK = USER_OPEN;
	}
	if(
		(
			( MODE_LOCK == USER_OPEN ) &&
			(
				(	IS_KEY_UP_Press() 	 == 	true   	)||
				(	IS_KEY_DWN_Press()  == 	true 	)||
				(	IS_KEY_LET_Press()  == 	true 	)||
				(	IS_KEY_RHT_Press()  == 	true 	)||
				(	IS_KEY_MID_Press()  == 	true	)
			)
		)
	)
	{
		
		MODE_LOCK = USER_CLOSE;
		//上 是否被按下？
		if		 ( IS_KEY_UP_Press() )
		{                                                                               																		    
			xKEY_Val = USER_KEY_UP;                                                                                                              																				    
		}                                                                               
		//下                                                                            
		else if( IS_KEY_DWN_Press() )
		{                                                                               																	    
			xKEY_Val = USER_KEY_DWN;                                                                                                            																		    
		}                                                                               
		//左                                                                            
		else if( IS_KEY_LET_Press() )
		{                                                                               																				    
			xKEY_Val = USER_KEY_LET;                                                                                                            
																					    
		}                                                                               
		//右                                                                            
		else if( IS_KEY_RHT_Press() )
		{                                                                               																			    
			xKEY_Val = USER_KEY_RHT;                                                                                                            																    
		}                                                                               
		//中                                                                            
		else if( IS_KEY_MID_Press() )
		{
			xKEY_Val = USER_KEY_MID;
		}
		osDelay(10);
	}
	else if(
			( MODE_LOCK == USER_CLOSE ) &&        
	        ( IS_KEY_UP_Press()  == false  	)   	&&
	        ( IS_KEY_DWN_Press() == false 	)		&&
	        ( IS_KEY_LET_Press() == false	)   	&&
	        ( IS_KEY_RHT_Press() == false	)   	&&
	        ( IS_KEY_MID_Press() == false  	)
	        
		)
	{
		MODE_LOCK = USER_OPEN;
	}

	return xKEY_Val;
}

//此函数作LVGL 旋转AB编码器输入读取回调
USER_KEY_VAL Encoder_Scan(void )
{
	//获取旋转编码器方向
	DIR_VAL dir = Get_ENC_DIR(&xKnob_ENC, &htim5);
	
	
	/*初始化key值为零*/
	xKEY_Val = USER_KEY_NULL;
	
	if(
		(
			(	IS_Rotary_Knob_MID_Press() 	 == 	true   	) &&
			(	dir == NONDIR 	)
		)
	)
	{                                                               
		if( IS_Rotary_Knob_MID_Press() )
		{
			xKEY_Val = USER_KEY_MID;
		}
		
	}
	else 
	{
		if (dir == LEFT)
		{
			xKEY_Val = USER_KEY_LET;
		}
		else if(dir == RIGHT)
		{
			xKEY_Val = USER_KEY_RHT;
		}
	}

	return xKEY_Val;
	
}



//读取编码器ID
uint8_t uCode_Dipper_ID(void)
{
	uint8_t id = 0x00;
	//是否B0被置1？
	if (  IS_DialSW_B0_SET() )
	{                   
		id = id | ( 0x01 << 0 );		    
	}                   
	if (  IS_DialSW_B1_SET() )
	{                   
		id = id | ( 0x01 << 1 );				    
	}                   
	if (  IS_DialSW_B2_SET() )
	{                   
		id = id | ( 0x01 << 2 );				    
	}                   
	if (  IS_DialSW_B3_SET() )
	{
		id = id | ( 0x01 << 3 );	
	}
	
	return id;
}

#define MAX_STARTBTN_NUM (2)

/***
 * 检测到按键按下，重新启动程序
***/
void vGetStartButton(void)
{
    static uint64_t ullLastPressTime_ms = 0;
    static uint8_t ucButtonState = 0;
	
	//判断按键是否按下
    if (IS_Start_Button_Press() == true)
    {
        if(0 == ucButtonState)
        {
            ullLastPressTime_ms = ullBsp_TimeBase_ms();
        }

        ucButtonState = 1;
    }
    else
    {
        if(1 == ucButtonState && ullBsp_TimeBase_ms() - ullLastPressTime_ms > 50)
        {
            if(1 == stTargetPara.usEnStartButtonAdd)//在具体应用中禁能
            {
                //启动测试任务
                stTargetPara.usSetShowTaskInfoColor = 1;
                stTargetPara.switchState = 1;
                vApp_InitTaskStartValue(stTargetPara.switchCode);
                stTargetPara.ucIsSetTakStop = 0;

                //先执行启动再累加
                stTargetPara.ucStartButtonNum++;

                if(MAX_STARTBTN_NUM  < stTargetPara.ucStartButtonNum)
                {
                    stTargetPara.ucStartButtonNum = 0;
                }
				
            }
        }

        ucButtonState = 0;
    }
}


void vGetPressButton(void)
{
    static uint64_t ullTimeFor20ms = 0;
    static uint64_t ullLastPressTime_ms = 0;

	//20ms进入一次
    if(ullBsp_TimeBase_ms() - ullTimeFor20ms >= 20)
    {
		//更新时间
        ullTimeFor20ms = ullBsp_TimeBase_ms();

		//判断是否按下
        switchVal = IS_Start_Button_Press();

        if(switchVal == true )//按钮被按下，开始计时
        {
            if(stTargetPara.switchState == 2 && ullBsp_TimeBase_ms() - ullLastPressTime_ms < 1500)
            {
                return;
            }

			//是否在长按
            if(switchLock == 0)
            {
                switchLock = 1;
				
                switchCnt_Pre = switchCnt_20ms;
                stTargetPara.ucIsSetTakStop = 1;
//                vApp_SetTaskStopValue(stTargetPara.switchCode);

            }
            else if(switchLock == 1 && (switchCnt_20ms - switchCnt_Pre) > 50
                    && (switchCnt_20ms - switchCnt_Pre) < 100)//50*20ms = 1s
            {
                stTargetPara.usSetShowTaskInfoColor = 1;
            }
            else if(switchLock == 1 && (switchCnt_20ms - switchCnt_Pre) >= 100)
                //100*20ms = 2s 清零测试次数计数
            {
                stTargetPara.usSetShowTaskInfoColor = 0;
            }

            ullLastPressTime_ms = ullBsp_TimeBase_ms();
        }
        else if(switchVal == false && switchLock == 1)//按钮抬起，结束计时
        {
            //短按按钮
            if((switchCnt_20ms - switchCnt_Pre) < 50)
            {
                stTargetPara.switchState = 1;
                stTargetPara.switchCode ++;

                stTargetPara.usSetShowTaskInfoColor = 0;

                if(stTargetPara.switchCode > END_SWITCHCODE)
                {
                    stTargetPara.switchCode = START_SWITCHCODE;
                }

                stTargetPara.ucMotorTestNumber = stTargetPara.switchCode;
                //任务数据清零
            }
            //长按按钮 50*20ms = 1s
            else if((switchCnt_20ms - switchCnt_Pre) >= 50 &&
                    (switchCnt_20ms - switchCnt_Pre) < 100)
            {
                //启动测试任务
                stTargetPara.switchState = 2;
                vApp_InitTaskStartValue(stTargetPara.switchCode);
                stTargetPara.ucIsSetTakStop = 0;
            }
            else
            {
                //长按大于两秒清零
                vApp_RestValue2Flash();
            }

            switchLock = 0;
        }

        switchCnt_20ms ++;
    }
}








