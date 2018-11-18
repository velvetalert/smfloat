#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f4xx.h"


#define BASIC_TIMx           		TIM6
#define BASIC_TIMx_CLK       		RCC_APB1Periph_TIM6

#define BASIC_TIMx_IRQn					TIM6_DAC_IRQn
#define BASIC_TIMx_IRQHandler   TIM6_DAC_IRQHandler

#define GENERAL_TIM5           		TIM5
#define GENERAL_TIM5_CLK       		RCC_APB1Periph_TIM5

#define GENERAL_TIM5_IRQn			TIM5_IRQn
#define GENERAL_TIM5_IRQHandler     TIM5_IRQHandler

extern volatile int32_t tc_timestamp;

void GENERAL_TIM5_Config(void);

void BASIC_TIMx_Config(void);
void delay();

void delay_s(u8 t);

#endif /* __BASIC_TIM_H */
