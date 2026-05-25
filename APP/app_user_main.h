#ifndef __APP_USER_MAIN_H
#define __APP_USER_MAIN_H
#include "main.h"


#define APP_OBJ_GB3220   4//GB3220 测试等

// <<< Use Configuration Wizard in Context Menu >>>

//<e>DebugMode
//<i>Set:1,Open 0,Close
#define DEF_IS_DEBUG_MODE 0  //是否在调试模式
//</e>

/*------------------------版本号设置 开始---------------------*/
//<o>DEF_MAJOR_VERSION
//<0000-0255>
//<i>.
#define DEF_MAJOR_VERSION 1

//<o>DEF_SUB_VERSION
//<0000-0255>
//<i>.
#define DEF_SUB_VERSION 0

//<o>DEF_REVISE_VERSION
//<0000-0255>
//<i>.
#define DEF_REVISE_VERSION 20

//<o>DEF_REVISE_VERSION
//<0000-0255>
//<i>.
#define DEF_REVISE_YEAR 25

//<o>DEF_REVISE_MONTH
//<0000-0255>
//<i>.
#define DEF_REVISE_MONTH 8

//<o>DEF_REVISE_DAY
//<0000-0255>
//<i>.
#define DEF_REVISE_DAY 1

/*------------------------版本号设置 结束---------------------*/
/*------------------------任务设置 开始---------------------*/
//<o>ChooseTargetType
//<i>4:GB3220,
//<4=>GB3220
#define SET_TARGETTYPE	4

//<o>Set_MaxTaskNum RealNumber=Num+1
//<0000-0100>
//<i>total 4 digit
#define MAX_TASK_NUM (20) //数量从0开始算,即：实际数量 = MAX_TASK_NUM + 1
//<o>Init_SWITCHCODE
//<0000-0100>
//<i>total 4 digit
#define INIT_SWITCHCODE 1

//限制任务选择
//<o>START_SWITCHCODE
//<0000-0100>
//<i>total 4 digit
#define START_SWITCHCODE 1

//<o>END_SWITCHCODE
//<0000-0100>
//<i>total 4 digit
#define END_SWITCHCODE 1

//<o>Set Poweron run
//<i>0:pause 1:PowerOn run
//<0=>SET_PowerON_Stop <1=>SET_PowerON_Run
#define SET_POWERON_RUN	0

/*------------------------任务设置 结束---------------------*/
// <<< end of configuration section >>>


#define MAINBOARDAPPLICATIONNAME ("IsolatedTestBox")//固件应用名称

#define PULSE_WIDTH_MAX                         350
#define LOAD_MODE                               0
#define CURRENT_GAIN                            44//运放增益
//#define Voltage_GAIN                            17.5//运放增益
//#define OHM                                     0.001//采样电阻阻值
//#define Voltage_Calibration_Coefficient         0.012158//0.00882;//电压系数标定而来
//float Voltage_GAIN = 17.25;


#define SET_MOTOR_CW    (0)
#define SET_MOTOR_CCW   (1)

#define SET_MOTOR_PWR_OFF (0)
#define SET_MOTOR_PWR_ON (1)


/*时基 延时                        */
uint64_t ullBsp_TimeBase_ms(void);
uint64_t ullBsp_TimeBase_us(void);


/*LCD显示                     */
void vAsp_LCD_ShowInit(void);
void vApp_ClockTick_1000ms(void);
void vApp_LCD_Update_15042ALT(void);
/*Flash读写操作                      */
//设置FLASH 保存地址(必须为4的倍数，且所在扇区,要大于本代码所占用到的扇区.
//MAX: 0X08040000
#define FLASH_SAVE_ADDR  ((uint32_t)0x08010000) //ADDR_FLASH_SECTOR_4 	
#define FLASH_SAVE_ADDR_SYSPARAM  ((u32)0x08020000) //ADDR_FLASH_SECTOR_5 
//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.
#define DEF_RUNTIMESTICKVALUE_SUMNUM 16380 //16380*4 = 65520 = 0XFFF0 < 64KB


extern uint32_t uCurrentWriteAddrOffset; //记录次数的写入flash地址
uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//读出字
void STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead);   		//从指定地址开始读出指定长度的数据
void  vApp_RestValue2Flash(void);
void  vApp_SaveValue2Flash(uint32_t ulValue);

//未分类
float LowPass_Filter(uint8_t ID, float force, float input);
float fApp_MotorSpeedPID_Control(uint16_t usSetSpeed);


void vApps_Init(void);
void vApp_UserLoop(void);
extern char cLCDShowStr[50];
void vApp_UserInit(void);


void  vTIM4_1msCallBack(void);
#endif