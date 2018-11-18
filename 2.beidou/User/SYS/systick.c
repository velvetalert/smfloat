#include "systick.h"
#include "stdlib.h"
#include "./usart/bsp_debug_usart.h"

volatile int32_t tc_timestamp;

void  BASIC_TIMx_IRQHandler (void)
{
	if ( TIM_GetITStatus( BASIC_TIMx, TIM_IT_Update) != RESET ) 
	{	
		tc_timestamp++;   //   ��ʱ��1Sʱ���
		TIM_ClearITPendingBit(BASIC_TIMx, TIM_IT_Update); 
	}
        				 	
}

static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIMx_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void BASIC_TIMx_Mode_Config(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  RCC_APB1PeriphClockCmd(BASIC_TIMx_CLK, ENABLE);

	  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  //TIM_TimeBaseStructure.TIM_Period = 53000-1;       //һ�����������1S
	TIM_TimeBaseStructure.TIM_Period = 10000-1;
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  //TIM_TimeBaseStructure.TIM_Prescaler = 53000-1;       	//��ʱ��Ƶ��   90M/(PSC+1)=10000HZ
	TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;  
	// ��ʼ����ʱ��TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BASIC_TIMx, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(BASIC_TIMx, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(BASIC_TIMx,TIM_IT_Update,ENABLE);
	
	// ʹ�ܶ�ʱ��
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
