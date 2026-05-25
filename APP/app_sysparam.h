#ifndef __APP_SYSPARAM_H__
#define __APP_SYSPARAM_H__

#include "app_alt_basic.h"
#include "app_sysparam_ex.h"


/*----------------枚举------------------*/
//各项目编号
typedef enum {
	ProjNULL = 0,
	GB10847,
	GB10865,
	GB1402S,
	GB3001,
	GB3002,
	GB5023,
	ER17353,
	ZC17353,
	GB5745X,
	GB5753X,
	TK100,
	IR108175,
    GB7639,
	GB5745Z,
	GB5753Z,
    GB11579,
    GB6816
	
}User_ProjNumber;

//函数索引
typedef enum 
{
	FUNC_Default = 0, 		        //默认
	FUNC_SetProject = 1,	        //设置项目
	FUNC_StartButton,		        //等待按键
	FUNC_SetCylinder,		        //设置气缸
	FUNC_SetLoad,			        //设置负载
	FUNC_SetMotorStop,		        //设置电机停止
	FUNC_RunOpenLoop,		        //开环
	FUNC_TestFinish,		        //测试结束
	FUNC_SAVE_RWPARAM_FLASH,        //写入FLASH
	FUNC_EncoderIdentification,     //编码器识别
	FUNC_RB_Distance_Test,		    //抬升主件_距离测试
	FUNC_RB_AD_Test,			    //抬升主件_AD信号测试
	FUNC_RB_Process_Test,		    //抬升主件_过程测试
	FUNC_DCL_Distance_Test,		    //深度清洁_距离测试
	FUNC_DCL_AD_Test,			    //深度清洁_AD信号测试
	FUNC_DCL_Process_Test,		    //深度清洁_过程测试
	FUNC_Decoupling_Test,		    //整机脱钩测试
	FUNC_Set_Zeor,				    //整机归位测试
	FUNC_ReadTmp,				    //读取温度
	FUNC_DrivePowerOn,		        //驱动器上电
    FUNC_MOTOR_SET_MODE,            //设置电机模式
    FUNC_MOTOR_ENABLED,             //电机使能
    FUNC_MOTOR_DISABLED,            //电机失能
	
}EN_STEP;

//运行状态
typedef enum 
{
	STATUS_START = 0,
	STATUS_DONE ,
	STATUS_OUTTIME,

}RUN_STATUS;



/*----------------start SYSTEM_RW_PARAMETER---------------------------*/
#define SYSTEM_RW_PARAMETER_NUM 0X100
#define MACHINE_SETTINGS_NUM (0X64)//100个寄存器

enum E_RUNMODE
{
    BUTTON_CONTROL = 0,
    AUTO_CONTROL   = 1,
    MODULE_TEST    = 2,
};

#define STATION_NAME_LEN (28)

#pragma pack(2) //设置2字节对齐
#pragma pack(push,1)
#pragma anon_unions

typedef union
{
    //位域先存低再存高
    //modbus帧字节显示先显示高再显示低
    uint16_t Buff[MACHINE_SETTINGS_NUM];
    struct
    {
        uint8_t uSetStationName[STATION_NAME_LEN];//站点名称

    };

} MACHINE_SETTINGS;

typedef union
{
    //位域先存低再存高
    //modbus帧字节显示先显示高再显示低
    uint16_t Buff[SYSTEM_RW_PARAMETER_NUM];
    struct
    {
        /*单片机系统配置--------------------------*/
        uint16_t IsSetSaveRWPARAM2Flash  : 1; //是否保存读写参数到Flash 0X00 ADDR0
        uint16_t IsSetResetRWPARAM2Flash : 1; //是否重置保存读写参数到Flash
        uint16_t resx : 14;

        uint16_t ucModbusAddr: 8; //设置Modbus站点地址 ADDR1
        uint16_t ucModbusBUAD: 8; //设置Modbus通信波特率

        MACHINE_SETTINGS stAppSet;//ADDR2-ADDR31
		uint8_t uSetStationName_Flash[STATION_NAME_LEN];//站点名称
        //固件升级操作,地址跳转
        //串口波特率配置
        //modbus地址配置
        //Flash操作
        //加密解密
        /*程序应用--------------------------------*/
        uint16_t usRUN_Mode;//控制模式  ADDR16 0:按键控制 1:Flash或者上位机控制 2:模块测试模式
        uint16_t usD17;//设置启动 D17
        uint16_t usSetCalibration_ms;//设置校准的时间ms D18
        uint16_t usTestCW_ms;//设置正转的时间ms D19
        uint16_t usTestCCW_ms;//设置反转的时间ms D20
        uint16_t usLoadTestCW_ms;//设置带载正转的时间ms D21
        uint16_t usLoadTestCCW_ms;//设置带载反转的时间ms D22

        uint16_t usSetGB5023_IntegrateTest_Mode;//D23

        uint16_t usRes[47 - 23]; //ADDR17-47

        struct ST_TASK_PARAM stBasicTask; //ADDR48

        uint8_t ucSetTaskRUN_FromFlash_NUM: 8; //设置任务参数从Flash获得的编号
        uint8_t ucSetRES: 8;

        //LCD显示控制，产线使用，测试部门验证，品质部使用
        struct ST_TASK_PARAM arstTask[10]; //ADDR49

        uint16_t usSetAirCylinder;//气缸控制
        uint16_t usSetLED_Ctrl;//LED控制
        float fSetLoadPWM_Percent;//负载PWM输出控制

        //最大地址D255 0XFF

    };
} SYSTEM_RW_PARAMETER;
#pragma pack(pop)
#pragma pack() //恢复编译器默认对齐

extern SYSTEM_RW_PARAMETER uSysRWParam;
/*-----------------end SYSTEM_RW_PARAMETER------------------------------*/




/*----------------start SYSTEM_RO_PARAMETER---------------------------*/
#define SYSTEM_RO_PARAMETER_NUM 0X100

#pragma pack(2) //设置2字节对齐
#pragma pack(push,1)
#pragma anon_unions
typedef union
{
    uint16_t Buff[SYSTEM_RO_PARAMETER_NUM];
    struct
    {
        uint16_t MainBoardFirmwareVersion1;	//固件版本号 ADDR2048 0X800
        uint16_t MainBoardFirmwareVersion2; //固件版本号 ADDR2049
        uint16_t MainBoardFirmwareVersion3; //固件版本号 ADDR2050
        //数据来自Flash?
        //时间基准
        //产品识别代码
        //软件识别代码
        uint16_t aucReserveROReg1; //预留区

        uint16_t aucReserveROReg2[4]; //预留区
        uint16_t aucReserveROReg3[4]; //预留区
        uint16_t aucReserveROReg4[4]; //预留区

        uint64_t ullSystemTimeBase;//系统时间基准 ms ADDR2064-2067

        uint8_t ucMotorTestNumber;//测试任务编号  ADDR2068 0:默认停止 1:Flash single 2:Flash List
        uint8_t ucTaskStatus ;//任务状态：启动，停止

        uint32_t ulRunTimesTick;//任务运行次数计数 ADDR2069-2070
        uint32_t ulTaskClock_1s_ForCycle;//1秒计时器，  ADDR2071-2072
        uint64_t ullTestSumUseFromStart_ms;//任务运行时间，相对于任务起始时间 ms ADDR2073-2076

        uint16_t ucTaskFinishPercent;//任务执行进度 500X [0,50000] ADDR2077

        uint16_t usMotorVoltage;//电机母线电压V  ADDR2078
        int16_t sMotorCurrent;//电机母线电流A   100X ADDR2079
        int16_t sMotorTemperature;//温度℃   100X ADDR2080

        int8_t cFCT_MotorRotation;//电机旋转方向 -1:CW  1:CCW  ADDR2081
        int8_t cMotorRotationIn;//红外光电测得电机转向;
		
		

        uint16_t usMotorSpeed;//电机速度 无方向 rpm    ADDR2082
        int16_t  sMotorSpeed;//电机速度 ±方向 rpm   ADDR2083


        /******************程控电源 寄存器*************/
        uint16_t SetPPS_On;// D2084 //设置程控电源开关
        uint16_t SetPPS_Volt;//D2085 //设置程控电源电压
        uint16_t SetPPS_Current;//D2086 //设置程控电源电流

        uint16_t PrevSetPPS_On;//  D2087 //上次设置程控电源开关
        uint16_t PrevSetPPS_Volt;//  D2088 //上次设置程控电源电压
        uint16_t PrevSetSetPPS_Current;// D2089 //上次设置程控电源电流

        uint16_t GetPPS_Volt;//D2090 //当前电压值 X100
        uint16_t GetPPS_Current;//D2091 //当前电流值 X100

        uint16_t D2092;// D2092
        uint16_t D2093;// D2093
        int16_t  D2094;// 100X D2094
        uint16_t D2095;// D2095
        uint16_t D2096;// D2096
        int16_t  D2097;// 100X D2097
        uint16_t D2098;// D2098

        uint16_t D2099;//备用 D2099



        #if (SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3223)
        uint16_t usCommunicationMak;//通信识别标记 //地址 绝对地址(DEC)2100  相对地址(DEC)52
        uint16_t usSystemStatus;//系统状态
        uint16_t usHardWareVersion;//硬件版本
        uint16_t usSoftWareVersion1;//软件版本
        uint16_t usSoftWareVersion2;//软件版本
        uint16_t usSoftWareVersion3;//软件版本
        uint8_t ucBuffMachineName[STATION_NAME_LEN];//站点名称 len=28
     
		uint16_t usEncoder_Status; 			//(码盘状态,无) 1：正常 0：不正常		
		
        #endif
		

        uint16_t usEndAddrMark;//地址结束标记，用于数据清零

        uint16_t usDialSW_Value;//拨码开关值	
		
		
		/*************Json CMD******************/
		RUN_STATUS Current_Status;//当前执行状态
		EN_STEP usCurrentFunc;//当前函数
		
		
		uint16_t usSetCMD_id;//当前执行 索引 
		uint16_t usPrevCMD_id;//上一个执行 索引
		
		char cProjectName[STATION_NAME_LEN];//设置项目名称
		
		User_ProjNumber ProjNumber; 		//项目编号
		User_ProjNumber Last_ProjNumber;	//保存最后得到项目编号
		uint32_t PECoefficient; 	//光电测数分频系数
		float MagneticPole;		//磁对极数量
		float ReductionRatio;	//减速比
		
		
		uint32_t lSetLoadTimeMS;//设置负载时间	
		uint16_t usSetLoadValue;//设置负载值
		uint16_t LoadStep;	//加载步骤
		uint16_t usSeClyValue;	//设置气缸
		uint16_t Step;	//步骤
		uint16_t usSetZeroFlag;	//调零标志
		int16_t  isDIR_Flag;	//方向
		
		uint16_t Test_Flag; //测试标志
		
		uint16_t usSet_MotorCalibrationVolt;//设置电机校准电压
		uint32_t lSet_RunOpenLoopTimeMS;//设置开环测试时间
		uint16_t usSet_RunOpenLoopFR;//设置开环测试方向
		uint16_t usSet_RunOpenLoopVolt;//设置开环测试电压
		
		uint16_t usSet_StopVolt;//设置电机停止电压
		uint16_t usReadTempVolt;//读取温度电压
		
		float fRunOpenLoop_Time_s;//测试时间
        float fRunOpenLoop_Volt;//测试电压 
        float fRunOpenLoop_spd_dev;//测试转速_治具
		float fRunOpenLoop_spd_motor;//测试转速_电机
        float fRunOpenLoop_Curr;//测试电流
		float fRunOpenLoop_Curr_mean;//测试平均电流
		float fRunOpenLoop_Curr_STD;//测试标准差电流
		float fRunOpenLoop_Curr_MAX;
		
		short sRunOpenLoop_spd_MSG;
		
		float MotorTemp;				//电机温度
		float MotorDriverTemp;			//启动器温度
		uint16_t usTemperature;
		
		
		
		//TODO数据封装
		DrivePowerOn_DATA_t 		DrivePowerOn_DATA;
		EncoderIdentification_DATA_t EncoderIdentification_DATA;
		GB1402S_DATA_t				GB1402S_DATA;
		
		uint32_t lSetMotorStopTimeMS;		//设置电机停止时间
		uint32_t lReadTmepTimeMS;		//设置电机停止时间
		uint32_t lSet_TestFinishTimeMS;//设置测试结束时间
		uint16_t usSet_TestFinishVolt;//设置测试结束电压

        uint32_t ui_time;             //时间
        uint16_t us_motor_set_mode; 

		
		uint16_t usTickForRxJson;//Json命令计数
		uint16_t usPrevTickForRxJson;//上一个Json命令计数
		
        //RW的数据RO映射
        //最大地址 D2303 0X8FF
    };

} SYSTEM_RO_PARAMETER;
#pragma pack(pop)
#pragma pack() //恢复编译器默认对齐



extern SYSTEM_RO_PARAMETER uSysROParam;
/*-----------------end SYSTEM_RO_PARAMETER------------------------------*/

/*----------------start Debug_RW_PARAMETER---------------------------*/
#define DEBUG_RW_PARAMETER_NUM 0X100

#pragma pack(2) //设置2字节对齐
#pragma pack(push,1)
#pragma anon_unions
typedef union
{
    uint16_t Buff[DEBUG_RW_PARAMETER_NUM];
    struct
    {
        float fMotorVoltage_RawADC;//电机母线电压V

    };

} DEBUG_RW_PARAMETER;
#pragma pack(pop)
#pragma pack() //恢复编译器默认对齐


extern DEBUG_RW_PARAMETER uDebugRWParam;
/*-----------------end SYSTEM_RO_PARAMETER------------------------------*/

//从Flash中读取系统参数
void App_Read_SysParam_FromFlash(void);

//接收参数保存命令，将参数保存到Flash
void App_Save_SysParam_ToFlash(void);

//初始化系统参数
void App_Init_SystemParam(void);

//系统参数处理
void App_SystemParam_Process(void);
//系统故障处理
void App_SystemFault_Process(void);

void App_SystemParam_Version(void);

#endif