#include "app_uart8.h"
#include "string.h"
#include "app_sysparam.h"
#include "app_SendData.h"

/************************************************************************************************************
*												Json命令执行函数
************************************************************************************************************/

//设置工程名字
static void Json_CMD_set_project_name(cJSON* cjson_src);
//等待按键按下
static void Json_CMD_wait_start_button(cJSON* cjson_src);
//设置气缸
static void Json_CMD_SetCylinder(cJSON* cjson_src);
//设置负载
static void Json_CMD_SetLoad(cJSON* cjson_src);
//设置电机开环
static void Json_CMD_RunOpenLoop(cJSON* cjson_src);
//设置电机测试完成
static void Json_CMD_TestFinish(cJSON* cjson_src);
//设置参数保存FLASH
static void Json_CMD_save_rw_param_flash(cJSON* cjson_src);
//驱动器上电
static void Json_CMD_DrivePowerOn(cJSON* cjson_src);
//编码器识别
static void Json_CMD_EncoderIdentification(cJSON* cjson_src);
//抬升主件_距离测试
static void Json_CMD_RB_Distance_Test(cJSON* cjson_src);
//抬升主件_AD信号测试
static void Json_CMD_RB_AD_Test(cJSON* cjson_src);
//抬升主件_过程测试
static void Json_CMD_RB_Process_Test(cJSON* cjson_src);
//深度清洁_距离测试
static void Json_CMD_DCL_Distance_Test(cJSON* cjson_src);
//深度清洁_到达AD信号测试
static void Json_CMD_DCL_AD_Test(cJSON* cjson_src);
//深度清洁_过程测试
static void Json_CMD_DCL_Process_Test(cJSON* cjson_src);
//整机脱钩测试
static void Json_CMD_Decoupling_Test(cJSON* cjson_src);
//整机归位测试
static void Json_CMD_Set_Zeor(cJSON* cjson_src);
//设置电机停止
static void Json_CMD_SetMotorStop(cJSON* cjson_src);
//读取温度
static void Json_CMD_ReadTmp(cJSON* cjson_src);
//电机设置模式
static void Json_CMD_motor_set_mode(cJSON* cjson_src);
//电机使能
static void Json_CMD_motor_enabled(cJSON* cjson_src);
//电机使能
static void Json_CMD_motor_disabled(cJSON* cjson_src);


//Json发送
static void Json_Send(cJSON *root);

extern REG_VALUE R_value;

/* -------------------------本地全局变量-----------------------------------*/
static uint16_t crc_check = 0; //Modbus CRC 检验判断值
static __IO uint16_t Rx_MSG = MSG_IDLE;  // 接收报文状态，系统初始为空闲
static uint8_t Ex_code = 0; //Modbus异常代码检测值


//Modebus初始
void vAPP_Modbus_Init(void)
{
    /* 申请内存空间作为保持寄存器地址,对应功能码03H、06H和10H */
    //PduData.PtrHoldingbase = (uint16_t*)malloc(sizeof(uint16_t)*0x125);
	
	//将Modbus首地址指定uSysRWParam.Buff首地址（注意：这是共用体 uint16_t uint16_t Buff[SYSTEM_RW_PARAMETER_NUM] 与 struct 共同） \
	意味着整个共用体存储在内部flash中
    PduData.PtrHoldingbase = (uint16_t*)(&uSysRWParam.Buff); //设置内存开始地址为系统读写参数句柄
    //FillBuf((uint8_t*)PduData.PtrHoldingbase,FUN_CODE_03H);//产生测试数据
    U8.RxFinishFlag = MSG_IDLE; //报文状态为空闲
}

/**
  * 函数功能: 填充内存
  * 输入参数: buf:内存空间首地址,Code:功能码
  * 返 回 值: 无
  * 说    明: 功能不同的功能码填充内容不一的内存空间,
  */
void FillBuf(uint8_t* buf, uint8_t Code)
{
    uint16_t i = 0;
    uint16_t j = 1;

    switch(Code)
    {
        case FUN_CODE_03H:
        case FUN_CODE_06H:
        case FUN_CODE_10H:
            j = 0x000F;

            for(i = 0; i < 0x250; i++)
                buf[i] = j++;

            break;
    }
}

uint32_t ulGetFrameTick = 0;


void vApp_Modbus_Loop(void)
{
    static uint8_t i;

    /* 接收到一帧的数据,对缓存提取数据 */
    if( U8.RxFinishFlag == SET )
    {
        for(i = 0; i < 8; i++)
        {
            
        }

        /* 收到非本机地址的响应请求 */
        if((U8.RxBuf[0] != MB_SLAVEADDR ) && (U8.RxBuf[0] != MB_ALLSLAVEADDR))
        {
            U8.RxFinishFlag = RESET;
            // continue;
            return;
        }
		
		if(U8.RxBuf[1] == 0x50)//JSon处理自定义命令
		{
			//进入Json处理
			JsonProcess();
			
			/* 重新标记为空闲状态 */
            U8.RxFinishFlag = RESET;
			return;
		}
		
		
        /* 解析数据帧 */
        MB_Parse_Data();
        if(PduData.Addr >= 0X800)
        {
            //只读寄存器
            PduData.PtrHoldingbase = (uint16_t*)(&uSysROParam.Buff);
            PduData.PtrHoldingOffset = PduData.PtrHoldingbase + PduData.Addr - 0X800; // 保持寄存器的起始地址
        }
        else
        {
            //读写寄存器
            PduData.PtrHoldingbase = (uint16_t*)(&uSysRWParam.Buff);
            PduData.PtrHoldingOffset = PduData.PtrHoldingbase + PduData.Addr; // 保持寄存器的起始地址
        }

        /* CRC 校验正确 */
        crc_check = ( (U8.RxBuf[U8.RxCount - 1] << 8) | U8.RxBuf[U8.RxCount - 2] );

        if(crc_check == PduData._CRC)
        {
            /* 分析数据帧并执行 */
            Ex_code = MB_Analyze_Execute();

            /* 出现异常 */
            if(Ex_code != EX_CODE_NONE)
            {
                //printf("异常响应\n");
                //vAPP_AddErr("ModbusRespone_NG!");
                MB_Exception_RSP(PduData.Code, Ex_code);
            }
            else
            {
                //printf("正常响应\n");
                //vAPP_AddErr("ModbusRespone_OK");
                MB_RSP(PduData.Code);
            }
        }

        /* 重新标记为空闲状态 */
        U8.RxFinishFlag = RESET;
    }

//	//接收完成判断

}

uint8_t JSonRx_Buf[256];

static void JsonProcess(void)
{
	MB_Parse_Data();
	 
	uint8_t JSonRxCount = U8.RxCount;

	memcpy(JSonRx_Buf,(const char*)U8.RxBuf,U8.RxCount);
	//接收格式为 uchar ucBuff[Max]={0x01,0x50,DataLen,0x00,0x00,CRC_L,CRC_H};
	crc_check = ( (JSonRx_Buf[JSonRxCount - 1] << 8) | JSonRx_Buf[JSonRxCount - 2] );
	if(crc_check == PduData._CRC)
	{				
//		if( JSonRx_Buf[JSonRxCount - 2] == 0)
//		{
//			cJSON_ParseWithLength((const char*)(JSonRx_Buf+3),JSonRx_Buf[2]);
//		}
		
		cJSON* cjson_Parse = NULL;		
			
		/* 解析整段JSO数据 */
		cjson_Parse = cJSON_ParseWithLength((const char*)(JSonRx_Buf+3),JSonRx_Buf[2]);
		if(cjson_Parse == NULL)
		{
			//printf("parse fail.\n");			
			return ;
		}

		cJSON* cjson_cmd = NULL;
        cJSON* cjson_id = NULL;		
		
		/* 依次根据名称提取JSON数据（键值对） */
		cjson_cmd = cJSON_GetObjectItem(cjson_Parse, "cmd");
		if(cjson_cmd)
		{
			 cjson_id = cJSON_GetObjectItem(cjson_Parse, "id");
			
			if(cjson_id)
			{
				 uSysROParam.usSetCMD_id =  cjson_id->valueint;
				if(uSysROParam.usPrevCMD_id != uSysROParam.usSetCMD_id)
				{
					vClearTestData();//切换命令时清空数据
				}
			}
			
			/**************根据命令调用不同函数********************/
			//设置项目名称
			if		(strcmp("set_project_name",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_set_project_name(cjson_Parse);
			}
			
			//等待按下
			else if(strcmp("wait_start_button",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_wait_start_button(cjson_Parse);		
			}	
			
			//设置气缸
			else if(strcmp("SetCyl",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_SetCylinder(cjson_Parse);
			}
			
			//设置负载
			else if(strcmp("SetLoad",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_SetLoad(cjson_Parse);
			}
			
			//设置开环跑
			else if(strcmp("RunOpenLoop",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_RunOpenLoop(cjson_Parse);
			}
			
			//设置测试完成
			else if(strcmp("TestFinish",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_TestFinish(cjson_Parse);
			}
			
			//设置保存RW 参数到Flash
			else if(strcmp("save_rw_param_flash",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_save_rw_param_flash(cjson_Parse);
			}
			
			//驱动器上电
			else if(strcmp("drive_power_on",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_DrivePowerOn(cjson_Parse);
			}
			
			//编码器识别
			else if(strcmp("encoder_identification",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_EncoderIdentification(cjson_Parse);
			}
			
			//抬升主件_距离测试
			else if(strcmp("RB_Distance_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_RB_Distance_Test(cjson_Parse);
			}
			
			//抬升主件_AD信号测试
			else if(strcmp("RB_AD_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_RB_AD_Test(cjson_Parse);
			}
			
			//抬升主件_过程测试
			else if(strcmp("RB_Process_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_RB_Process_Test(cjson_Parse);
			}
			
			//深度清洁_距离测试
			else if(strcmp("DCL_Distance_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_DCL_Distance_Test(cjson_Parse);
			}
			
			//深度清洁_AD信号测试
			else if(strcmp("DCL_AD_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_DCL_AD_Test(cjson_Parse);
			}
			
			//深度清洁_过程测试
			else if(strcmp("DCL_Process_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_DCL_Process_Test(cjson_Parse);
			}
			
			//整机脱钩测试
			else if(strcmp("Decoupling_Test",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_Decoupling_Test(cjson_Parse);
			}
			
			//设置电机停止
			if(strcmp("SetMotorStop",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_SetMotorStop(cjson_Parse);
			}
			
			//整机归位测试
			else if(strcmp("Set_Zeor",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_Set_Zeor(cjson_Parse);
			}
			
			//读取温度
			else if(strcmp("Read_Tmp",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_ReadTmp(cjson_Parse);
			}

			//设置电机模式
			else if(strcmp("motor_set_mode",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_motor_set_mode(cjson_Parse);
			}

			//设置电机使能
			else if(strcmp("motor_enabled",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_motor_enabled(cjson_Parse);
			}

			//设置电机使能
			else if(strcmp("motor_disabled",cjson_cmd->valuestring) == 0)
			{
				Json_CMD_motor_disabled(cjson_Parse);
			}

		  }
		
		
        uSysROParam.usTickForRxJson++;	
		  
			
		if(NULL != cjson_Parse)
		{
		    cJSON_Delete(cjson_Parse);
		}				
	}		
}

//设置项目
static void Json_CMD_set_project_name(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_SetProject;
	
	
	/************数据解析***************/
	cJSON* cjson_ProjectName 		= NULL;	//项目名称
	cJSON* cjson_PECoefficient 		= NULL;	//光电分辨率
	cJSON* cjson_MagneticPole 		= NULL;	//磁对极数量
	cJSON* cjson_ReductionRatio 	= NULL; //减速比
	
	//解析项目名称
	cjson_ProjectName = cJSON_GetObjectItem(cjson_src, "name");
	if(cjson_ProjectName)
	{
	    strcpy(uSysROParam.cProjectName , cjson_ProjectName->valuestring);
	}
	
	//解析光电传感多少个挡片（一圈触发多少个脉冲）
	cjson_PECoefficient = cJSON_GetObjectItem(cjson_src, "PECoefficient");
	if(cjson_PECoefficient)
	{
	    uSysROParam.PECoefficient = cjson_PECoefficient->valuedouble;
	}
	
	//解析磁对极数量
	cjson_MagneticPole = cJSON_GetObjectItem(cjson_src, "MagneticPole");
	if(cjson_MagneticPole)
	{
	    uSysROParam.MagneticPole = cjson_MagneticPole->valuedouble;
	}
	
	//解析减速比
	cjson_ReductionRatio = cJSON_GetObjectItem(cjson_src, "ReductionRatio");
	if(cjson_ReductionRatio)
	{
	    uSysROParam.ReductionRatio = cjson_ReductionRatio->valuedouble;
	}
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	Json_Send(root);//上传
	cJSON_Delete(root);
	
	
	
}

//等待按键启动
static void Json_CMD_wait_start_button(cJSON* cjson_src)
{

	uSysROParam.usCurrentFunc = FUNC_StartButton;
	
	/************数据解析***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//设置电机停止
static void Json_CMD_SetMotorStop(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_SetMotorStop;	

	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_StopVolt = NULL;	
	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	if(cjson_TimeMS)
	uSysROParam.lSetMotorStopTimeMS = cjson_TimeMS->valuedouble*1000;
	
	cjson_StopVolt = cJSON_GetObjectItem(cjson_src, "Volt");
	if(cjson_StopVolt)
	uSysROParam.usSet_StopVolt = cjson_StopVolt->valuedouble*100;
	
		
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}

//设置气缸
static void Json_CMD_SetCylinder(cJSON* cjson_src)
{

	uSysROParam.usCurrentFunc = FUNC_SetCylinder;	

	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_Cylinder = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	if(cjson_TimeMS)
	uSysROParam.lSetLoadTimeMS = cjson_TimeMS->valuedouble*1000;
	
	cjson_Cylinder = cJSON_GetObjectItem(cjson_src, "Cylinder");
	if(cjson_Cylinder)
	uSysROParam.usSeClyValue = cjson_Cylinder->valuedouble;
	
		
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
			
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}


//设置负载
static void Json_CMD_SetLoad(cJSON* cjson_src)
{

	uSysROParam.usCurrentFunc = FUNC_SetLoad;	

	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_Load = NULL;
	//设置负载步骤  说明：项目有时需不同的负载 此处作为上位机传达判断 1负载多少  2负载多少 （1、2可自定义具体实现在项目分文件）
	cJSON* cjson_LoadStep = NULL;
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	if(cjson_TimeMS)
	uSysROParam.lSetLoadTimeMS = cjson_TimeMS->valuedouble*1000;
	
	cjson_Load = cJSON_GetObjectItem(cjson_src, "Load");
	if(cjson_Load)
	uSysROParam.usSetLoadValue = cjson_Load->valuedouble;
	
	cjson_LoadStep = cJSON_GetObjectItem(cjson_src, "LoadStep");
	if(cjson_LoadStep)
	uSysROParam.LoadStep = cjson_LoadStep->valuedouble;
		
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
			
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}
//电机开环运行 上传太多对象会导致断线
static void Json_CMD_RunOpenLoop(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_RunOpenLoop;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	cJSON* cjson_Test_Flag = NULL;
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.lSet_RunOpenLoopTimeMS = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.usSet_RunOpenLoopFR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.usSet_RunOpenLoopFR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.usSet_RunOpenLoopVolt = cjson_Volt->valuedouble*100;
	
	cjson_Test_Flag = cJSON_GetObjectItem(cjson_src, "Test_Flag");
	uSysROParam.Test_Flag = cjson_Test_Flag->valuedouble;
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/

	switch((uint16_t)uSysROParam.ProjNumber)
	{
		case GB11579:
		{
			//时间
			cJSON_AddNumberToObject(root, "TestTime",			uSysROParam.fRunOpenLoop_Time_s						);
			cJSON_AddNumberToObject(root, "volt",				System_Uploading_Params.stMSG.fVoltage				);
			cJSON_AddNumberToObject(root, "curr",				uSysROParam.fRunOpenLoop_Curr						);
			cJSON_AddNumberToObject(root, "speed_motor",		System_Uploading_Params.stMSG.fSpeed 				);
			cJSON_AddNumberToObject(root, "MCU_Temp",			System_Uploading_Params.stMSG.fMCU_Temp 			);
			cJSON_AddNumberToObject(root, "MOS_Temp",			System_Uploading_Params.stMSG.fMOS_Temp 			);
			cJSON_AddNumberToObject(root, "Wire_Winding_Temp",	System_Uploading_Params.stMSG.fWire_Winding_Temp 	);
			cJSON_AddNumberToObject(root, "Bus_Current",		System_Uploading_Params.stMSG.fBus_Current		 	);

		}
		break;

		default:
		{
			//时间
			cJSON_AddNumberToObject(root, "TestTime",uSysROParam.fRunOpenLoop_Time_s);  
			//电压
			cJSON_AddNumberToObject(root, "volt",uSysROParam.fRunOpenLoop_Volt);  
			//治具转速
			cJSON_AddNumberToObject(root, "speed_dev",uSysROParam.fRunOpenLoop_spd_dev);  
			//电机转速
			cJSON_AddNumberToObject(root, "speed_motor",uSysROParam.fRunOpenLoop_spd_motor);	
			//电流
			cJSON_AddNumberToObject(root, "curr",uSysROParam.fRunOpenLoop_Curr);
		}
		break;
	}
	

	
	
/***---------------------------------------------------------------------------------***/
/*	                                项目细分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/	
	switch((uint16_t)uSysROParam.ProjNumber)
	{
		case GB10847:
		case GB10865:
		case ZC17353:
		case ER17353:
		{
			//串口反馈转速
			cJSON_AddNumberToObject(root, "speed_MSG",uSysROParam.sRunOpenLoop_spd_MSG);
			//串口反馈温度
			cJSON_AddNumberToObject(root, "temp",uSysROParam.usTemperature);
			//方向
			cJSON_AddNumberToObject(root, "DIR_Flag",uSysROParam.isDIR_Flag);
		}
		break;
		
		case GB1402S:
		{
			cJSON_AddNumberToObject(root, "mean_curr",uSysROParam.fRunOpenLoop_Curr_mean);
			cJSON_AddNumberToObject(root, "STD_curr",uSysROParam.fRunOpenLoop_Curr_STD);
			cJSON_AddNumberToObject(root, "AD_volt",uSysROParam.GB1402S_DATA.AD_DEFAULT_Volt);
			cJSON_AddNumberToObject(root, "value_curr",uSysROParam.GB1402S_DATA.Difference_Value_Cur);
		}
		break;
		
		case IR108175:
		{
			
			cJSON_AddNumberToObject(root, "temp",U5_MSG.Motor_Temp);
			if (U5_MSG.Motor_SR <= 51)
			cJSON_AddNumberToObject(root, "Motor_SR",U5_MSG.Motor_SR);
			if (U5_MSG.Motor_ESR <= 65535)
			cJSON_AddNumberToObject(root, "Motor_ESR",U5_MSG.Motor_ESR);
			if (U5_MSG.Motor_Type <= 4)
			cJSON_AddNumberToObject(root, "Motor_Type",U5_MSG.Motor_Type);

		}
	}
	
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//抬升主件_距离测试
static void Json_CMD_RB_Distance_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_RB_Distance_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.RB_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.RB_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.RB_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.RB_Set_Test_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//滚刷抬升主件_下降测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.RB_Test_Time_s);  
	//滚刷抬升主件_下降前距离
	cJSON_AddNumberToObject(root, "RB_DDIS_begin",uSysROParam.GB1402S_DATA.RB_DEFAULT_distance);  
	//滚刷抬升主件_下降后距离
	cJSON_AddNumberToObject(root, "RB_DDIS_end",uSysROParam.GB1402S_DATA.RB_END_distance);	
	//滚刷抬升主件_下降前后距离差
	cJSON_AddNumberToObject(root, "RB_RDIS",uSysROParam.GB1402S_DATA.RB_range_difference);

	

	
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//深度清洁_距离测试
static void Json_CMD_DCL_Distance_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_DCL_Distance_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.DCL_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.DCL_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.DCL_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.DCL_Set_Test_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//深度清洁_测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.DCL_Test_Time_s);  
	//深度清洁_前距离
	cJSON_AddNumberToObject(root, "DCL_DIS_end",uSysROParam.GB1402S_DATA.DCL_DEFAULT_distance);  
	//深度清洁_后距离
	cJSON_AddNumberToObject(root, "DCL_DIS_begin",uSysROParam.GB1402S_DATA.DCL_END_distance);	
	//深度清洁_前后距离差
	cJSON_AddNumberToObject(root, "DCL_RDIS",uSysROParam.GB1402S_DATA.DCL_range_difference);

	

	
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//抬升主件_AD信号测试
static void Json_CMD_RB_AD_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_RB_AD_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.RB_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.RB_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.RB_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.RB_Set_Test_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//滚刷抬升主件_滚刷底部AD信号测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.RB_Test_Time_s);  
	//滚刷抬升主件_滚刷底部AD信号测试电压
	cJSON_AddNumberToObject(root, "AD_volt",uSysROParam.GB1402S_DATA.AD_ROLLING_BRUSH_BOTTOM_Volt);  

	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//深度清洁_到达AD信号测试
static void Json_CMD_DCL_AD_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_DCL_AD_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.DCL_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.DCL_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.DCL_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.DCL_Set_Test_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//滚刷抬升主件_滚刷底部AD信号测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.DCL_Test_Time_s);  
	//滚刷抬升主件_滚刷底部AD信号测试电压
	cJSON_AddNumberToObject(root, "AD_volt",uSysROParam.GB1402S_DATA.AD_DEEP_CLEANING_BOTTOM_Volt);  

	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//抬升主件_过程测试
static void Json_CMD_RB_Process_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_RB_Process_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.RB_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.RB_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.RB_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.RB_Set_Test_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//滚刷抬升主件_过程测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.RB_Test_Time_s);  
	//滚刷抬升主件_到达滚刷底部时间
	cJSON_AddNumberToObject(root, "fall_time",uSysROParam.GB1402S_DATA.RB_Decline_Time);  
	//滚刷抬升主件_过程测试电流_均值
	cJSON_AddNumberToObject(root, "mean_curr",uSysROParam.GB1402S_DATA.RB_Curr_mean);  
	//滚刷抬升主件_过程测试电流_最大值
	cJSON_AddNumberToObject(root, "MAX_curr",uSysROParam.GB1402S_DATA.RB_Curr_MAX);
	//滚刷抬升主件_抬升顶部时间
	cJSON_AddNumberToObject(root, "lifting_time",uSysROParam.GB1402S_DATA.RB_Lifting_Time);
	
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//深度清洁_过程测试
static void Json_CMD_DCL_Process_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_DCL_Process_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.DCL_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.DCL_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.DCL_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.DCL_Set_Test_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//深度清洁_过程测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.DCL_Test_Time_s);  
	//深度清洁_到达时间
	cJSON_AddNumberToObject(root, "DCL_arrival_time",uSysROParam.GB1402S_DATA.DCL_Arrival_Time);  
	//深度清洁_过程测试电流_均值
	cJSON_AddNumberToObject(root, "mean_curr",uSysROParam.GB1402S_DATA.DCL_Curr_mean);  
	//深度清洁_过程测试电流_最大值
	cJSON_AddNumberToObject(root, "MAX_curr",uSysROParam.GB1402S_DATA.DCL_Curr_MAX);
	
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//整机脱钩测试
static void Json_CMD_Decoupling_Test(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_Decoupling_Test;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.Whole_Engine_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.Whole_Engine_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.Whole_Engine_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.Whole_Engine_Set_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//整机脱钩_测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.Whole_Engine_Test_Time_s);  
	//整机脱钩_脱钩脉冲检测_1
	cJSON_AddNumberToObject(root, "pulse_count_1",uSysROParam.GB1402S_DATA.pulse_count_1);  
	//整机脱钩_脱钩脉冲检测_2
	cJSON_AddNumberToObject(root, "pulse_count_2",uSysROParam.GB1402S_DATA.pulse_count_2);  

	
	
	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//整机归位测试
static void Json_CMD_Set_Zeor(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_Set_Zeor;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.GB1402S_DATA.Whole_Engine_Set_Test_Time_s = cjson_TimeMS->valuedouble*1000;
	
	cjson_FR = cJSON_GetObjectItem(cjson_src, "FR");	
	if(strcmp("CW",cjson_FR->valuestring) == 0)
	{
	  uSysROParam.GB1402S_DATA.Whole_Engine_Test_FR = SET_MOTOR_CW;
	}
	else
	{
	  uSysROParam.GB1402S_DATA.Whole_Engine_Test_FR = SET_MOTOR_CCW;
	}
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.GB1402S_DATA.Whole_Engine_Set_Volt = cjson_Volt->valuedouble*100;
	
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
	
	//上传数据
	
/***---------------------------------------------------------------------------------***/
/*	                                公共部分                                           */
/*                                                                                     */
/***---------------------------------------------------------------------------------***/
	//归位_过程测试时间
	cJSON_AddNumberToObject(root, "TestTime",uSysROParam.GB1402S_DATA.Whole_Engine_Test_Time_s);  
	//归位_到达时间
	cJSON_AddNumberToObject(root, "zero_flag",uSysROParam.GB1402S_DATA.zero_flag);  
	//归位_抬升距离差
	cJSON_AddNumberToObject(root, "RB_Homing_Distance",uSysROParam.GB1402S_DATA.RB_Homing_Distance); 
	//归位_深度清洁距离差
	cJSON_AddNumberToObject(root, "DCL_Homing_Distance",uSysROParam.GB1402S_DATA.DCL_Homing_Distance); 	

	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//测试结束
static void Json_CMD_TestFinish(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_TestFinish;
	
	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_FR = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	uSysROParam.lSet_TestFinishTimeMS = cjson_TimeMS->valuedouble*1000;
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	uSysROParam.usSet_TestFinishVolt = cjson_Volt->valuedouble*100;
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}

	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//参数保存到FLASH
static void Json_CMD_save_rw_param_flash(cJSON* cjson_src)
{
	uSysROParam.usCurrentFunc = FUNC_SAVE_RWPARAM_FLASH;
		
	
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}

	Json_Send(root);//上传
	cJSON_Delete(root); 
}

//驱动器上电
static void Json_CMD_DrivePowerOn(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_DrivePowerOn;	

	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	if(cjson_TimeMS)
	uSysROParam.DrivePowerOn_DATA.RunTime = cjson_TimeMS->valuedouble*1000;
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	if(cjson_Volt)
	uSysROParam.DrivePowerOn_DATA.SetVol = cjson_Volt->valuedouble*100;
	
		
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}

//读取温度
static void Json_CMD_ReadTmp(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_ReadTmp;	

	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	cJSON* cjson_Volt = NULL;	
	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	if(cjson_TimeMS)
	uSysROParam.lReadTmepTimeMS = cjson_TimeMS->valuedouble*1000;
	
	cjson_Volt = cJSON_GetObjectItem(cjson_src, "Volt");
	if(cjson_Volt)
	uSysROParam.usReadTempVolt = cjson_Volt->valuedouble*100;
	
		
	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		
	//读取驱动器温度
	cJSON_AddNumberToObject(root, "MotorDriverTemp",uSysROParam.MotorDriverTemp);  
	//读取电机温度
	cJSON_AddNumberToObject(root, "MotorTemp",uSysROParam.MotorTemp);  
	
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}

//编码器识别
static void Json_CMD_EncoderIdentification(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_EncoderIdentification;	

	/************数据解析***************/
	cJSON* cjson_TimeMS = NULL;	
	
	cjson_TimeMS = cJSON_GetObjectItem(cjson_src, "TimeS");
	if(cjson_TimeMS)
	uSysROParam.EncoderIdentification_DATA.RunTime = cjson_TimeMS->valuedouble*1000;
	

	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}

//设置电机模式
static void Json_CMD_motor_set_mode(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_MOTOR_SET_MODE;	

	/************数据解析***************/
	cJSON* st_cjson_time_ms 	   = NULL;	
	cJSON* st_cjson_motor_set_mode = NULL;

	
	//数据获取
	st_cjson_time_ms 		= cJSON_GetObjectItem(cjson_src, "time_ms");
	st_cjson_motor_set_mode = cJSON_GetObjectItem(cjson_src, "motor_set_mode");


	if( st_cjson_time_ms != NULL )
	{
		uSysROParam.ui_time = st_cjson_time_ms->valuedouble*1000;
	}

	if( st_cjson_motor_set_mode != NULL )
	{
		uSysROParam.us_motor_set_mode = st_cjson_motor_set_mode->valuedouble;
	}
	
	

	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		


	cJSON_AddNumberToObject(root, "Firmware_Version",System_Uploading_Params.stMSG.uiFirmware_Version);  


	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}

//设置电机使能
static void Json_CMD_motor_enabled(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_MOTOR_ENABLED;	

	/************数据解析***************/
	cJSON* st_cjson_time_ms 	   = NULL;	
	
	//数据获取
	st_cjson_time_ms 		= cJSON_GetObjectItem(cjson_src, "time_ms");

	if( st_cjson_time_ms != NULL )
	{
		uSysROParam.ui_time = st_cjson_time_ms->valuedouble*1000;
	}
	

	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}

//设置电机失能
static void Json_CMD_motor_disabled(cJSON* cjson_src)
{
	
	uSysROParam.usCurrentFunc = FUNC_MOTOR_DISABLED;	

	/************数据解析***************/
	cJSON* st_cjson_time_ms 	   = NULL;	
	
	//数据获取
	st_cjson_time_ms 		= cJSON_GetObjectItem(cjson_src, "time_ms");

	if( st_cjson_time_ms != NULL )
	{
		uSysROParam.ui_time = st_cjson_time_ms->valuedouble*1000;
	}
	

	/************数据上传***************/
	cJSON *root = cJSON_CreateObject();  
	
	if(STATUS_START == uSysROParam.Current_Status)
	{			
		cJSON_AddStringToObject(root, "status", "start");  
	}
	else if(STATUS_DONE == uSysROParam.Current_Status)	
	{
		cJSON_AddStringToObject(root, "status", "done");  
	}	
	else
	{
		cJSON_AddStringToObject(root, "status", "outtime");  
	}
		
	Json_Send(root);//上传
	cJSON_Delete(root); 	
	
}




uint16_t jsonlen = 0;

//Json发送
static void Json_Send(cJSON *JRoot)
{
	char *json_string = cJSON_PrintUnformatted(JRoot);	
	jsonlen = strlen(json_string);	
	strncpy((char *)U8.TxBuf+3, json_string,jsonlen);		
	cJSON_free(json_string);
	
	U8.TxBuf[0] = 0x01;
	U8.TxBuf[1] = 0x50;
	U8.TxBuf[2] = jsonlen;		
	uint16_t crc = 0;
	crc = MB_CRC16((uint8_t*)&U8.TxBuf, jsonlen+3);
	U8.TxBuf[jsonlen+3] = crc;	          /* crc 低字节 */
	U8.TxBuf[jsonlen+4] = crc >> 8;		    /* crc 高字节 */		
	U8_DMA_Send(U8.TxBuf, jsonlen+5);
	
}
void vApp_usart1_DMA_IDLE_SetFinish(void)
{
    Rx_MSG = MSG_COM;
    ulGetFrameTick++;
}


void U8_Receiving_Process(void)
{
	//判断是否接收到数据
	switch(U8.RxFinishFlag)
	{
		//接收到数据
		case SET:
		{
			//处理模版
//			U8_DMA_Send(U8.RxBuf,U8.RxCount);
			
			vApp_Modbus_Loop();
			
			
			//复位操作
			U8.RxFinishFlag = RESET;
			U8.RxCount = 0;
			memset(U8.RxBuf, 0 , sizeof(U8.RxBuf));
			
			//又再次打开DMA收发
			HAL_UART_Receive_DMA(&huart8,(uint8_t *)U8.RxBuf,UART_MAX_LEN);	
			break;
		}
		
		case RESET:
		{
			
			
			
			break;
		}
		
	}
}
