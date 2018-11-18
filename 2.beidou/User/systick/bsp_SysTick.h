#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);
void TimingDelay_Decrement(void);
//void Delay(__IO uint32_t nCount);
#define Delay_ms(x) Delay_us(100*x)	 //µ¥Î»ms

#endif /* __SYSTICK_H */
