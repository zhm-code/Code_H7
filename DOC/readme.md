/----------------------------项目说明文档 [倒叙]---------------------------------------------/
|
|
2025/3/25/17:08
1.此项目需开启电源才能烧录，注意：不要接 烧录器 VCC   
2.在开启rtos时，自动增加计时  递增单位ms 请查看 freertos.c   void vApplicationTickHook( void ) 
3.bsp_interrupt_callback.c	存放中断回调函数的c文件 注意：定时器溢出回调函数在main.c中  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 由于MX自动生成导致
4.main.h 中定义了IO读取和控制
5.所有用户硬件驱动都在 app_user_init中  
6.所有串口通讯驱动放在bsp_urat.c