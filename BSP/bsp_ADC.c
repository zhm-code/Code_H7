#include "bsp_ADC.h"
ADC_Typedef xMOT_Vol_ADC;		//电机电压
ADC_Typedef xMOT_Cur_ADC;		//电机电流
ADC_Typedef External_Vol_ADC;	//外部电压

uint8_t ADC1_DMA_TX_Finish_Flag = RESET; //DMA传输ADC完成标志
uint8_t ADC2_DMA_TX_Finish_Flag = RESET; //DMA传输ADC完成标志

uint16_t ulADC1_value[ADC_BUF_MAX];
uint16_t ulADC2_value[ADC_BUF_MAX];

void vADC1_Process(void )
{
	
	switch(ADC1_DMA_TX_Finish_Flag) //ADC1采集完成标志
	{
		case SET:	//数据传输完成处理
		{
			int ADDR_Count = 0; //数组地址计数
			while(ADDR_Count < ADC_BUF_MAX)
			{
				for(int i = 0; i < 2; i++) //求和
				{
					if ( i == 0 ) //第1顺序 ADC1 C9    电机电流
					{
						xMOT_Cur_ADC.ADC_sum += ulADC1_value[ADDR_Count ++];
					}
					else if ( i == 1)	//第2顺序 ADC1 C9    外部电压采集
					{
						External_Vol_ADC.ADC_sum += ulADC1_value[ADDR_Count ++];
					}
				}
			}
			
			//循环结束求均值
			xMOT_Cur_ADC.ADC_mean_value = xMOT_Cur_ADC.ADC_sum / (ADC_BUF_MAX / 2);
			External_Vol_ADC.ADC_mean_value = External_Vol_ADC.ADC_sum / (ADC_BUF_MAX / 2);
			
			
			//求引脚电压
			xMOT_Cur_ADC.ADC_conversion_pin_vol = xMOT_Cur_ADC.ADC_mean_value * 3.3f / 65535.0f;
			External_Vol_ADC.ADC_conversion_pin_vol = External_Vol_ADC.ADC_mean_value * 3.3f / 65535.0f;
			
			//求实际值
			
			//电机电流计算公式
			xMOT_Cur_ADC.actual_value = xMOT_Cur_ADC.ADC_conversion_pin_vol  /(CUR_R * CUR_GAIN);
			
			//外部电压计算公式
			//TODO
			External_Vol_ADC.actual_value = External_Vol_ADC.ADC_conversion_pin_vol /  3.33333333333f * 9.89;
			
			//转到上位机寄存器
			//TODO
			
			ADC1_DMA_TX_Finish_Flag = RESET; //复位
			xMOT_Cur_ADC.ADC_sum = 0;
			External_Vol_ADC.ADC_sum = 0;
			HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ulADC1_value, ADC_BUF_MAX); //重新开启DMA
			break;
		}
		case RESET:
		{
			
			
			break;
		}
	}
	
}


float Filter_Sum = 0;
int Filter_Count = 0;

void vADC2_Process(void )
{
	
	switch(ADC2_DMA_TX_Finish_Flag) //ADC1采集完成标志
	{
		case SET:	//数据传输完成处理
		{
//			float Filter_Buff[100] = {0};
			
			for(int i = 0; i < ADC_BUF_MAX; i++)	//求和
			{
				xMOT_Vol_ADC.ADC_sum += ulADC2_value[i];
				
			}

			
			
			
			//计算公式 16精度65535 / 2 得到中间值 32767.5    ( ADC值 - 32767.5) * 3.3v / 32767.5  正转为正数 反转为负数
//			xMOT_Vol_ADC.ADC_conversion_pin_vol = (xMOT_Vol_ADC.ADC_mean_value - 65535.0f / 2) * 3.3f / (65535.0f / 2);
			
			
			if(Filter_Count < 1000)
			{
				Filter_Sum += xMOT_Vol_ADC.ADC_sum / ADC_BUF_MAX;
				Filter_Count ++;
			}
			else
			{
				
				//循环结束求均值
				xMOT_Vol_ADC.ADC_mean_value = Filter_Sum / 1000.0f;
				
				//求引脚电压
				xMOT_Vol_ADC.ADC_conversion_pin_vol = (xMOT_Vol_ADC.ADC_mean_value - 35450) * 3.3f / 35450;
				
				//求实际值
				//电机电压计算公式	
				xMOT_Vol_ADC.actual_value = xMOT_Vol_ADC.ADC_conversion_pin_vol / 0.0786;
				
				//转到上位机寄存器
				//TODO
				
				
				Filter_Sum = 0;
				Filter_Count = 0;
			}
			
				
			
			
			
			
			
			
			ADC2_DMA_TX_Finish_Flag = RESET; //复位
			xMOT_Vol_ADC.ADC_sum = 0;
			HAL_ADC_Start_DMA(&hadc2, (uint32_t *)ulADC2_value, ADC_BUF_MAX); //重新开启DMA
			break;
		}
		case RESET:
		{
			
			
			break;
		}
	}
	
}

