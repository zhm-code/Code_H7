#include "bsp_DAC.h"
#include "dac.h"




void DAC_Set_Percentage(float Percentage)
{
	uint32_t DAC_set_val = 0;
	
	if(Percentage > 0)
		DAC_set_val = 4090 * (Percentage / 100.f) ;
	
	
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_set_val);
	
}
