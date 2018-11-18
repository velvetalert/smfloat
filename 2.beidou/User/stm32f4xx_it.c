
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./report/report1.h"
#include <string.h>
#include "./SYS/systick.h"
#include "./led/bsp_led.h"
#include "./systick/bsp_SysTick.h"


/*******************************************************/



void DEBUG5_USART_IRQHandler(void)
{
    uint8_t ucTemp;
    if(USART_GetITStatus(DEBUG5_USART, USART_IT_RXNE) != RESET)
    {
        ucTemp = USART_ReceiveData(DEBUG5_USART);
        // UART4->DR = ucTemp;
    }
    if( RNSS_RX_STA == 0)
    {
        if(ucTemp == '$')//0xEC
        {
            RNSS_RX_BUF[0] = ucTemp;
            RNSS_RX_COUNT = 0;
        }
        if(RNSS_RX_BUF[0] == '$') //0xEC
        {

            RNSS_RX_BUF[RNSS_RX_COUNT] = ucTemp;
            RNSS_RX_COUNT++;
            if(ucTemp == '\n')
            {
                if(strncmp((char*)RNSS_RX_BUF, "$GNGGA", 6) == 0)
                {
                    RNSS_RX_STA = 1;
                }

            }
        }
    }
    if(RNSS_RX_COUNT > 150)
    {
        RNSS_RX_STA = 0;
        RNSS_RX_COUNT = 0;
    }

}

/***************************************************************/
extern u8 RDSS_BUF_NOW[200];
extern u8 RDSS_COUNT_NOW;
extern u8 NO_FKXX_FLAG;
void DEBUG_USART_IRQHandler(void)
{
    uint8_t ucTemp;
    if(USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET)
    {
        ucTemp = USART_ReceiveData(DEBUG_USART);
        if(ucTemp == '$')
        {
            RDSS_RX_BUF[RDSS_RX_COUNT] = ucTemp;
            RDSS_RX_STA |= 0x01;
        }
        if(RDSS_RX_STA & 0x01)
        {
			if(RDSS_RX_COUNT == 1)
            {
				if(ucTemp == 'T')
				RDSS_RX_STA |=0x02;
				else if(ucTemp == 'F')
				{
					RDSS_RX_STA |=0x04;
				    NO_FKXX_FLAG = 1;
				}
				else
                {
					Usart_SendString(USART1, "hh");
					RDSS_RX_STA = 0;		
					RDSS_RX_COUNT = 0;
				}
            }

            if(RDSS_RX_COUNT == 6 && (RDSS_RX_STA > 0x01))
            {
                RDSS_Alen = ucTemp;
				RDSS_RX_STA |=0x10;
            }
			if(RDSS_RX_STA != 0)
            RDSS_RX_BUF[RDSS_RX_COUNT++] = ucTemp;
			if(RDSS_RX_STA > 0x10)
			{
				if(RDSS_RX_COUNT == RDSS_Alen)
				{
					STRNCPY(RDSS_BUF_NOW, RDSS_RX_BUF, 0, RDSS_Alen);
					RDSS_COUNT_NOW = RDSS_Alen;
					RDSS_RX_COUNT = 0;
					RDSS_RX_STA |= 0x80;
				}
			}
            
        }

        if(RDSS_RX_COUNT > 100)
        {
            RDSS_RX_COUNT = 0;
        }
    }

}


/******************************************************************/


/******************************************************************/
extern u8 USART4_BUF_NOW[200];
extern u16 USART4_COUNT_NOW;
void CENTER_USART_IRQHandler(void)
{
    uint8_t res;
    if((UART4->SR & (1 << 5)))
    {
        res = UART4->DR;

//		USART4_RX_BUF[USART4_RX_COUNT++]=res;
        if (res == 0xEC)
        {
            USART4_RX_STA |= 0x10;
            USART4_RX_BUF[USART4_RX_COUNT] = 0xEC;
        }
        if(USART4_RX_STA & 0x10)
        {
            if(USART4_RX_COUNT == 1)
            {
                USART4_Alen = res + 7;
            }
            USART4_RX_BUF[USART4_RX_COUNT++] = res;
            if(USART4_RX_COUNT == USART4_Alen)
            {
                STRNCPY(USART4_BUF_NOW, USART4_RX_BUF, 0, USART4_Alen);
                USART4_COUNT_NOW = USART4_Alen;
                USART4_RX_COUNT = 0;
                USART4_RX_STA |= 0x80;
            }
        }
        if(USART4_RX_COUNT > 150)
        {
            USART4_RX_COUNT = 0;
        }
    }
}


extern u8 TIM5_Flag;
void  GENERAL_TIM5_IRQHandler (void)
{	
	
    if ( TIM_GetITStatus( GENERAL_TIM5, TIM_IT_Update) != RESET )
    {
        TIM5_Flag	= 1;
        TIM_Cmd(TIM5, DISABLE);
        TIM_ClearITPendingBit(GENERAL_TIM5 , TIM_IT_Update);
    }
}


void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
