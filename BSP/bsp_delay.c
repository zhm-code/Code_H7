#include "bsp_delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

//nus为要延时的us数.		    								   
void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;						//LOAD的值	    	 
	ticks=nus*(SystemCoreClock/1000000); 	//需要的节拍数	  		 
	tcnt=0;
	vTaskSuspendAll();									//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;						//时间超过/等于要延迟的时间,则退出.
		}  
	};
	xTaskResumeAll();										//恢复OS调度									    
}



