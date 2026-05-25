#include "app_pps_ctrl.h"
#include "app_sysparam.h"
#include "bsp_uart.h"
#include "bsp_MB_slave.h"

#if (SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3220 || SET_TARGETTYPE == APP_OBJ_GB3223)
uint16_t TxCount_ForU6 = 0;
uint16_t crc_ForU6 = 0;
uint8_t isSetSendProgramPowerSupply = 0;


//程控电源通信读写控制
uint16_t TickForPPS_Send = 0;
void Control_ProgramPowerSupply_TX(void)
{

    if(0 == TickForPPS_Send)
    {
        if(uSysROParam.SetPPS_On != uSysROParam.PrevSetPPS_On)
        {
            SetProgramPowerSupplyOn(uSysROParam.SetPPS_On);
            uSysROParam.PrevSetPPS_On = uSysROParam.SetPPS_On;
        }
        else
        {
            TickForPPS_Send++;
        }
    }

    if(1 == TickForPPS_Send)
    {
        if(uSysROParam.SetPPS_Volt != uSysROParam.PrevSetPPS_Volt)
        {
            SetProgramPowerSupply_volt(uSysROParam.SetPPS_Volt);
            uSysROParam.PrevSetPPS_Volt = uSysROParam.SetPPS_Volt;
        }
        else
        {
            TickForPPS_Send++;
        }
    }

    if(2 == TickForPPS_Send)
    {
        ReadProgramPowerSupplyVoltCurr();
    }


}

//读取寄存器
void PPS_ReadReg(uint8_t SlaverAddr, uint16_t RegAddr, uint16_t Num)
{
    TxCount_ForU6 = 0;
    U7.TxBuf[TxCount_ForU6++] = SlaverAddr; /* 从站地址 */
    U7.TxBuf[TxCount_ForU6++] = 0X03; /* 功能码   */
    U7.TxBuf[TxCount_ForU6++] = RegAddr >> 8 ;
    U7.TxBuf[TxCount_ForU6++] = RegAddr ;
    U7.TxBuf[TxCount_ForU6++] = Num >> 8 ;
    U7.TxBuf[TxCount_ForU6++] = Num ;

    crc_ForU6 = MB_CRC16((uint8_t*)&U7.TxBuf, TxCount_ForU6);
    U7.TxBuf[TxCount_ForU6++] = crc_ForU6;/* crc 低字节 */
    U7.TxBuf[TxCount_ForU6++] = crc_ForU6 >> 8;/* crc 高字节 */
    U7_DMA_Send((uint8_t*)U7.TxBuf, TxCount_ForU6);
}
//写单个寄存器
void PPS_WriteSingleReg(uint8_t SlaverAddr, uint16_t RegAddr, uint16_t Value)
{
    TxCount_ForU6 = 0;
    U7.TxBuf[TxCount_ForU6++] = SlaverAddr; /* 从站地址 */
    U7.TxBuf[TxCount_ForU6++] = 0X06; /* 功能码   */
    U7.TxBuf[TxCount_ForU6++] = RegAddr >> 8 ;
    U7.TxBuf[TxCount_ForU6++] = RegAddr ;
    U7.TxBuf[TxCount_ForU6++] = Value >> 8 ;
    U7.TxBuf[TxCount_ForU6++] = Value ;

    crc_ForU6 = MB_CRC16((uint8_t*)&U7.TxBuf, TxCount_ForU6);
    U7.TxBuf[TxCount_ForU6++] = crc_ForU6;/* crc 低字节 */
    U7.TxBuf[TxCount_ForU6++] = crc_ForU6 >> 8;/* crc 高字节 */
    U7_DMA_Send((uint8_t*)U7.TxBuf, TxCount_ForU6);
}

//设置电源开关
void SetProgramPowerSupplyOn(uint8_t  isSetOn)
{
    PPS_WriteSingleReg(0x01, 0x0001, isSetOn);
}

//设置电压
void SetProgramPowerSupply_volt(uint16_t  Value)
{
    PPS_WriteSingleReg(0x01, 0x0030, Value);
}
//设置电流
void SetProgramPowerSupply_curr(uint16_t  Value)
{
    PPS_WriteSingleReg(0x01, 0x0031, Value);
}
//读取电压电流
void ReadProgramPowerSupplyVoltCurr(void)
{
    PPS_ReadReg(0x01, 0x0010, 0x0002);
}
#endif