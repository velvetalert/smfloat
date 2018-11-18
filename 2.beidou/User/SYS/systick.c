#include "systick.h"
#include "stdlib.h"
#include "./usart/bsp_debug_usart.h"

volatile int32_t tc_timestamp;

void  BASIC_TIMx_IRQHandler (void)
{
	if ( TIM_GetITStatus( BASIC_TIMx, TIM_IT_Update) != RESET ) 
	{	
		tc_timestamp++;   //   计时器1S时间戳
		TIM_ClearITPendingBit(BASIC_TIMx, TIM_IT_Update); 
	}
        				 	
}

static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIMx_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void BASIC_TIMx_Mode_Config(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  RCC_APB1PeriphClockCmd(BASIC_TIMx_CLK, ENABLE);

	  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  //TIM_TimeBaseStructure.TIM_Period = 53000-1;       //一个周期设成了1S
	TIM_TimeBaseStructure.TIM_Period = 10000-1;
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  //TIM_TimeBaseStructure.TIM_Prescaler = 53000-1;       	//定时器频率   90M/(PSC+1)=10000HZ
	TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;  
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BASIC_TIMx, &TIM_TimeBaseStructure);
	
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(BASIC_TIMx, TIM_FLAG_Update);
	
	// 开启定时器更新中断
	TIM_ITConfig(BASIC_TIMx,TIM_IT_Update,ENABLE);
	
	// 使能定时器
	TIM_Cmd(BASIC_TIMx, ENABLE);	
}



void BASIC_TIMx_Config(void)
{
	TIMx_NVIC_Configuration();
	BASIC_TIMx_Mode_Config();
}
/****************************************************************/
static void TIM5_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM5_IRQn; 	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM5_Mode_Config(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(GENERAL_TIM5_CLK, ENABLE); 
		TIM_TimeBaseStructure.TIM_Period = 22500-1;       
		TIM_TimeBaseStructure.TIM_Prescaler = 40000-1;	
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
		TIM_TimeBaseInit(GENERAL_TIM5, &TIM_TimeBaseStructure);
		TIM_ClearFlag(GENERAL_TIM5, TIM_FLAG_Update);
		TIM_ITConfig(GENERAL_TIM5,TIM_IT_Update,ENABLE);
		TIM_Cmd(GENERAL_TIM5, ENABLE);	
}

void GENERAL_TIM5_Config(void)
{
	TIM5_NVIC_Configuration();	
  
  TIM5_Mode_Config();
}

/*********************************************************************/
void delay()
{
    u32 volatile time_s = 0;
	while(time_s++ <500000);
}

void delay_s(u8 t)
{
	int32_t delay_timestamp = tc_timestamp;
	while(tc_timestamp - delay_timestamp < t)
	{
		delay();
	}
}
