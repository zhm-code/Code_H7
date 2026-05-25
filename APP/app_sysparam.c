#include "main.h"
#include "app_sysparam.h"
#include "app_user_main.h"
#include "bsp_cpu_flash.h"


extern uint16_t ucTaskFinishPercent;
//系统读写参数句柄
SYSTEM_RW_PARAMETER uSysRWParam;
//系统只读参数句柄
SYSTEM_RO_PARAMETER uSysROParam ;

DEBUG_RW_PARAMETER uDebugRWParam;

void vApp_InitFlashSysPara()
{
    //设置默认的值
    uSysRWParam.usRUN_Mode = 0;
    //保存到Flash
    App_Save_SysParam_ToFlash();
    //读出Flash
    App_Read_SysParam_FromFlash();
}


//从Flash中读取系统参数
void App_Read_SysParam_FromFlash(void)
{
    if( *((unsigned int *)(ADDR_FLASH_SECTOR_6_BANK2 )) != 0xFFFFFFFF) //已初始化
    {
        //参数来自Flash
//        STMFLASH_Read(ADDR_FLASH_SECTOR_6_BANK2, (uint32_t*)&uSysRWParam, sizeof(uSysRWParam.Buff) / 4); //从Flash中读出参数
			bsp_ReadCpuFlash(ADDR_FLASH_SECTOR_6_BANK2, (uint8_t*)&uSysRWParam, sizeof(uSysRWParam.Buff) );
    }
    else//未初始化,则初始化
    {
        //参数不是来自Flash 先写入再读出
        vApp_InitFlashSysPara();
    }

}

//接收参数保存命令，将参数保存到Flash
void App_Save_SysParam_ToFlash(void)
{
//    STMFLASH_Write(FLASH_SAVE_ADDR_SYSPARAM, (uint32_t*)&uSysRWParam, sizeof(uSysRWParam.Buff) / 4);
	/* 擦除扇区 */
	bsp_EraseCpuFlash((uint32_t)ADDR_FLASH_SECTOR_6_BANK2);
	bsp_WriteCpuFlash((uint32_t)ADDR_FLASH_SECTOR_6_BANK2,  (uint8_t *)&uSysRWParam, sizeof(uSysRWParam));
}

//初始化系统参数
void App_Init_SystemParam(void)
{
    //注意判断联合体中的结构体是否超出限制 TODO
    //注意判断联合体中的结构体是否超出限制 TODO
	
	//记录软件版本日期
    uSysROParam.MainBoardFirmwareVersion1 = DEF_MAJOR_VERSION  << 8 | DEF_SUB_VERSION;
    uSysROParam.MainBoardFirmwareVersion2 = DEF_REVISE_VERSION << 8 | DEF_REVISE_YEAR;
    uSysROParam.MainBoardFirmwareVersion3 = DEF_REVISE_MONTH   << 8 | DEF_REVISE_DAY;

    if(1)
    {
        App_Read_SysParam_FromFlash();
    }
    else
    {
        //设置默认的值
        //uSysRWParam.IsSetTestHALL_DutyCycle     = 0; //是否检测霍尔占空比
    }


    //设置一下不使用Flash的参数的值

    for(uint16_t Tick = 0; Tick < MACHINE_SETTINGS_NUM; Tick++)
    {
        uSysRWParam.stAppSet.Buff[Tick] = 0;
    }


    uSysROParam.cMotorRotationIn = 1;
    uSysROParam.SetPPS_On = 0;//上电发一次关闭指令
    uSysROParam.PrevSetPPS_On = 1;

    uSysROParam.usCommunicationMak = 54321;//通信识别标记
    App_SystemParam_Version();

	uSysROParam.usPrevCMD_id = 65535;
	uSysROParam.usSetCMD_id = 65535;
}

//软硬件版本号设置
void App_SystemParam_Version(void)
{
    uSysROParam.usHardWareVersion =  01 << 8 | 02; //硬件版本
    uSysROParam.usSoftWareVersion1 = DEF_MAJOR_VERSION << 8 | DEF_SUB_VERSION; //软件版本
    uSysROParam.usSoftWareVersion2 = DEF_REVISE_VERSION << 8 | DEF_REVISE_YEAR; //软件版本
    uSysROParam.usSoftWareVersion3 = DEF_REVISE_MONTH << 8 | DEF_REVISE_DAY; //软件版本
}

//系统参数处理
void App_SystemParam_Process(void)
{
    //参数保存到Flash
    if( uSysRWParam.IsSetSaveRWPARAM2Flash ==  SET )
    {
		//复位防止重复写
        uSysRWParam.IsSetSaveRWPARAM2Flash = RESET;
		
		//写入Flash
        App_Save_SysParam_ToFlash();
    }

	
    //使用默认参数重置Flash
    if( uSysRWParam.IsSetResetRWPARAM2Flash == SET )
    {
		//复位防止重复写
        uSysRWParam.IsSetResetRWPARAM2Flash = RESET;
		
		//参数初始化
        vApp_InitFlashSysPara();
    }


    //手动模式时，清除自动模式的配置参数，LCD显示等 TODO

    //转速为零时，清除FCT检测的结果，重置为0 TODO

    uSysROParam.ullSystemTimeBase = ullBsp_TimeBase_ms();

    //uSysROParam.usMotorSpeed   = usMotorSpeed;
    uSysROParam.ulRunTimesTick = stTargetPara.ulRunTimesTick;
    uSysROParam.ulTaskClock_1s_ForCycle = stTargetPara.ulTaskClock_1s_ForCycle;
    uSysROParam.ullTestSumUseFromStart_ms = stTargetPara.ullTestSumUseFromStart_ms;
    uSysROParam.ucMotorTestNumber = stTargetPara.ucMotorTestNumber;
    uSysROParam.ucTaskStatus = stTargetPara.ucTaskStatus;//任务状态：启动，停止

    uSysROParam.ucTaskFinishPercent = ucTaskFinishPercent;

}
//系统故障处理
void App_SystemFault_Process(void);
