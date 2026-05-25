#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "main.h"
#include "adc.h"

typedef struct
{
	uint64_t ADC_sum; 		//adc总和
	float ADC_mean_value; //通过均值得出的ADC值
	float ADC_conversion_pin_vol; //转换的pin引脚电压
	float actual_value; //实际值

}ADC_Typedef;

extern ADC_Typedef xMOT_Vol_ADC;		//电机电压
extern ADC_Typedef xMOT_Cur_ADC;		//电机电流
extern ADC_Typedef External_Vol_ADC;	//外部电压



#define VOL_SLOPE 0.167f //电压斜率
//#define CUR_REF	  2.48f				//电流芯片REF引脚电压
#define CUR_R	  0.01				//采样电阻	单位殴
#define CUR_GAIN  20				//增益选项



#define ADC_BUF_MAX 400
extern uint16_t ulADC1_value[ADC_BUF_MAX];
extern uint16_t ulADC2_value[ADC_BUF_MAX];

extern uint8_t ADC1_DMA_TX_Finish_Flag;				//DMA传输ADC1完成标志
extern uint8_t ADC2_DMA_TX_Finish_Flag;				//DMA传输ADC1完成标志






extern void vADC1_Process(void );
extern void vADC2_Process(void );

#endif

