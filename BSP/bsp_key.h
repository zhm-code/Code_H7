#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__
#include "main.h"


/*用户KEY值*/
enum {
	USER_KEY_NULL = 0, //空
	USER_KEY_UP   = 1,	//上
	USER_KEY_DWN  = 2,	//下
	USER_KEY_LET  = 3,	//左
	USER_KEY_RHT  = 4,	//右
	USER_KEY_MID  = 5,	//中
	USER_KEY_SET  = 6,	//SET
	USER_KEY_RESET = 7,	//RESET
};

/*定义USER_KEY_VAL用于用户KEY值*/
typedef uint32_t USER_KEY_VAL;
/*声明外部变量*/
extern USER_KEY_VAL xKEY_Val;
/*拨码器ID*/
extern uint8_t uUser_ID;

							           									   
#define KEY0_PRES    1                  /* KEY0按下 */
#define KEY1_PRES    2                  /* KEY1按下 */
#define KEY2_PRES    3                  /* KEY2按下 */
#define WKUP_PRES    4                  /* KEY_UP按下(即WK_UP) */





/*API*/
/*检测按键组函数*/
extern USER_KEY_VAL xGet_Key_Val(uint8_t mode );
extern USER_KEY_VAL Encoder_Scan(void );
extern uint8_t uCode_Dipper_ID(void);

extern void vGetStartButton(void);
extern void vGetPressButton(void);

#endif //end of __BSP_KEY_H__

