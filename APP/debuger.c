#include "debuger.h"
#include "app_user_main.h"

char cLCDShowFlowStr[LCDShowFlowStrSubMax] = "A123456789B123456789";//LCD的显示字符串
/**
  * @Name    vApp_ShowStringFlow
  * @brief
* @param   OutPutStr: [输出] 输出的字符串，用于显示屏显示
**			 ucOutLenLimt: [输入] 显示屏每行显示的字符长度最大值，跟字号有关
**			 InputStr: [输入] 等待显示的长字符串
**			 ucInputLen: [输入] 等待显示的长字符串的长度，如果是全局数组，注意有效字符长度
**			 usFlowSpeedms: [输入] 流动频率，多少毫秒移动一个字符
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-06-15
 **/
void vApp_ShowStringFlow(char *OutPutStr,
                         uint8_t ucOutLenLimt,
                         char *InputStr,
                         uint8_t ucInputLen,
                         uint16_t usFlowSpeedms)
{
    uint8_t ucTick = 0;
    uint8_t ucStartPoint = (ullBsp_TimeBase_ms() / usFlowSpeedms) % ucInputLen;

    for(uint8_t ucSub = ucStartPoint; ucTick < ucOutLenLimt; ucTick++)
    {
        OutPutStr[ucTick] = InputStr[(ucSub % ucInputLen)];
        ucSub++;

    }
}


char cErrStrBuff[ErrStrLines][ErrStrBuffSub] ;
/**
  * @Name    vAPP_AddErr
  * @brief
  * @param   p: [输入:限制为ASCII字符]
  * @retval
  * @author  Andrew Coulson
  * @Date    2022-06-15
 **/
void vAPP_AddErr(const char *p)
{
    if(strlen(p) < ErrStrBuffSub)
    {
        for(uint8_t ucTick = 0; ucTick < ErrStrLines; ucTick++)
        {
            if(strncmp(p, cErrStrBuff[ucTick], strlen(p)) == 0)
            {
                return;
            }
        }

        for(uint8_t ucTick = 0; ucTick < ErrStrLines; ucTick++)
        {
            if(cErrStrBuff[ucTick][0] == 0 && cErrStrBuff[ucTick][2] == 0)
            {
                strncpy(cErrStrBuff[ucTick], p, strlen(p));
                return;
            }
        }
    }
}
