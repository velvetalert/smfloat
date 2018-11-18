#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./report/report1.h"
#include <string.h>
#include "./SYS/systick.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "stm32f4xx_it.h"

/*存储电控串口来的数据和数据长度*/
u16 USART4_COUNT_NOW;
u8 USART4_BUF_NOW[200];


/*存储北斗串口来的数据和数据长度*/
u8 RDSS_COUNT_NOW;
u8 RDSS_BUF_NOW[200];


/*存储电控串口用来透传给岸站的数据和数据长度*/
u8 repeat_count;              //数据长度
u8 repeat_send = 0;           //重复发送的次数（至多3次）
u8 repeat_buf[200];           //存储透传的数据

u8 first_send = 0;            //电控发送透传数据时置1，发送时清零
u8 next_data = 0;             //收到岸站的40指令时置1，开始透传数据
u8 last_data = 0;             //收到电控的42（最后一个包）指令时置1
u8 send_last_gps = 0;         //收到01时置1，收到00后清零

int32_t next_send_time = 0;   //北斗发送下一条指令的时间戳
int32_t test_send5 = 0;       //测试用的5秒时间戳
int32_t close5 = 0;
u8 open = 0;


u8 TIM5_Flag=0;
u8 NO_FKXX_FLAG=0;
/*下一条指令52*/
uint8_t NEXT_DATA[] = {0xEC, 0x01, 0x52, 0x01, 0x01, 0x03, 0x50, 0xEB};


uint8_t LAST_DATA[] = {0x01, 0x42, 0x01, 0x01, 0x03};
/*************************************
函数名：hextochar；
功能：用于将接收的十六进制显示的指令转为ASCII码便于调试
**************************************/

void hextochar(u8 hex[], u8 t)
{
    u8 i = 0;
    const u8 hexc[] = "0123456789ABCDEF";
    while(i < t)
    {
        Usart_SendByte(USART6, hexc[hex[i] >> 4]);
        Usart_SendByte(USART6, hexc[hex[i] & 0x0F]);
        i++;
    }

}
/*************************************
函数名：hextochar1；
功能：用于将接收的岸站十六进制显示的指令转为ASCII码便于调试
**************************************/
void hextochar1(u8 hex[], u8 t)
{
    u8 i = 0;
    const u8 hexc[] = "0123456789ABCDEF";
    while(i < t)
    {
        Usart_SendByte(USART6, hexc[hex[19 + i] >> 4]);
        Usart_SendByte(USART6, hexc[hex[19 + i] & 0x0F]);
        i++;
    }
}
/*************************************
函数名：init；
功能：初始化变量标志
**************************************/
void init()
{
    first_send = 0;
    next_data = 0;
    repeat_send = 0;
    last_data = 0;
}

int main(void)
{
    /*串口配置*/
    RNSS_USART_Config();    //实际没有用到，可以删除
    RDSS_USART_Config();    //串口6调试接的串口，波特率115200
    CENTER_USART_Config();  //电控串口 波特率9600
    Debug5_USART_Config();  //北斗GPS串口，波特率9600
    Debug_USART_Config();   //北斗RDSS串口，用于短报文，波特率115200
    //SysTick_Init();
	
    BASIC_TIMx_Config();    //定时器，定时1s中断用于定时时间
    GENERAL_TIM5_Config();
	TIM_Cmd(TIM5,DISABLE);
    LED_GPIO_Config();      //IO口配置，用于打开和关闭北斗模块的5V和12V供电
    TIM_Cmd(TIM6, ENABLE);  //使能定时器
    USART_Cmd(UART5, DISABLE); //关闭GPS串口

    Usart_SendString(USART6, "DEBUG_START\r\n");
    Usart_SendString(UART4, "UART4\r\n");
   //Usart_SendString(USART1,"UART1\r\n");
    while(1)
    {
        /************************************************************************
        电控串口实时检测并处理
        协议具体详见时序图和流程word文件
        ************************************************************************/
		if(USART4_RX_STA & 0x80)
        {
			USART4_RX_STA = 0;
			hextochar(USART4_BUF_NOW, USART4_COUNT_NOW);
			Usart_SendString(USART6, "\r\n");
			if(USART4_BUF_NOW[0] == 0xEC && USART4_BUF_NOW[USART4_COUNT_NOW - 1] == 0xEB)
			{
				/*唤醒指令*/
				if(USART4_BUF_NOW[2] == 0xDD)
				{
					LED1(1);				 //打开5v12v
					LED2(1);
					init();
					Usart_SendStr_length(UART4, USART4_BUF_NOW, USART4_COUNT_NOW);
					USART_Cmd(UART5, ENABLE); //此时打开GPS串口，准备发送GPS数据给岸站
				}
				/*休眠指令*/
				else if(USART4_BUF_NOW[2] == 0xAA)
				{
					LED1(0);				 //关闭5v12v
					LED2(0);
					init();
					Usart_SendStr_length(UART4, USART4_RX_BUF, USART4_COUNT_NOW);
				}
				/*透传的最后一个包*/
				else if(USART4_BUF_NOW[2] == 0x42)
				{
					LED1(1);				 //打开5v12v
					LED2(1);
					next_data = 0;           //结束发送下一条指令
					last_data = 1;           //此时置1
					first_send = 1;          //准备发送
					repeat_count = USART4_COUNT_NOW - 3;
					STRNCPY(repeat_buf, USART4_BUF_NOW, 1, USART4_COUNT_NOW - 2);
				}
				/*全部发送给完成，结束透传*/
				else if(USART4_BUF_NOW[2] == 0x43)
				{
					LED1(1);				 //打开5v12v
					LED2(1);
					next_data = 0;           //全部接收成功，置0结束发送下一条指令
					first_send = 0;
				}
				/*透传数据和数据长度*/
				else
				{
					first_send = 1;
					repeat_send = 0;
					repeat_count = USART4_COUNT_NOW - 3;
					STRNCPY(repeat_buf, USART4_BUF_NOW, 1, USART4_COUNT_NOW - 2);
				}
			}
        }

        /*实时检测GPS串口的数据*/
        if(RNSS_RX_STA == 1)             //RNSS_RX_STA：接收完成标志
        {
            if(RNSS_RX_BUF[47] == '1')   //位置有效，截取时间位置信息
            {
                uint8_t t;

                RNSS_BUF[0] = 39;
                if(!send_last_gps)
                    RNSS_BUF[1] = 1;
                else
                {
                    RNSS_BUF[1] = 0x50;  //发送新的GPS指令
                    send_last_gps = 0;
                    USART_Cmd(UART5, DISABLE);
                }


                RNSS_BUF[2] = 1;
                RNSS_BUF[3] = 1;

                for(t = 7; t < 46; t++)
                {
                    RNSS_BUF[t - 3] = RNSS_RX_BUF[t];
                }
                repeat_count = 43;
                STRNCPY(repeat_buf, RNSS_BUF, 0, 43);
                first_send = 1;                               //准备发送，标志置1
                repeat_send = 0;
                //BD_REPORT(repeat_count,repeat_buf);
                Usart_SendString(USART6, "A:\r\n");
            }
            else
            {
                Usart_SendString(USART6, "N:\r\n");
            }

            RNSS_RX_STA = 0;
            RNSS_RX_COUNT = 0;
            send_last_gps = 0;
        }
        
        /*北斗RDSS串口实时检测*/
        if(RDSS_RX_STA & 0x80)
        {  	
			Usart_SendString(USART6, "main***\r\n");
			Usart_SendStr_length(USART6, RDSS_BUF_NOW, RDSS_COUNT_NOW);
			Usart_SendString(USART6, "***\r\n");
			if(RDSS_RX_STA & 0x04)                    //电控通过北斗RDSS串口发送给岸站时的指令反馈
			{
				RDSS_RX_STA = 0;
				if(RDSS_BUF_NOW[10] == 0)
				{

					repeat_send = 0;                                      //发送成功，标志置0
					Usart_SendString(USART6, "send success!\r\n");
				}
				else
				{
					repeat_send++;                                        //发送失败，标志++
					Usart_SendString(USART6, "send fail!\r\n");
				}
			}

			if(RDSS_RX_STA & 0x02)                    //接收岸站的北斗数据指令
			{
				RDSS_RX_STA = 0;
				uint8_t i;
				uint8_t RDSS_Tlen = (RDSS_BUF_NOW[16] * 256 + RDSS_BUF_NOW[17]) / 8;
				/*******************************************************
				收到关键信息（40）和重发指令（41），打开下一条指令（52）
				最后一条指令标志置0，发送准备清0，重复标志清0
				********************************************************/
				if(RDSS_BUF_NOW[20] == 0x40 || RDSS_BUF_NOW[20] == 0x41)
				{
					//USART_Cmd(USART3,DISABLE);
					next_data = 1;
					last_data = 0;
					//next_send_time = tc_timestamp + 5;
					first_send = 0;
					repeat_send = 0;
				}
				/*收到定位，失能GPS串口*/
				if(RDSS_BUF_NOW[20] == 0x51)
				{
					USART_Cmd(UART5, DISABLE);
					first_send = 0;
					repeat_send = 0;
				}
				/*收到最后一条包*/
				if(RDSS_BUF_NOW[20] == 0x43)
				{
					first_send = 0;
					repeat_send = 0;
					last_data = 0;
				
				}
				if(RDSS_BUF_NOW[20] == 0x50)
				{
					send_last_gps = 1;
					USART_Cmd(UART5, ENABLE);
				}
				/*岸站指令透传给电控*/
				if(!send_last_gps)
				{
					Usart_SendByte(UART4, 0xEC);
					for(i = 0; i < RDSS_Tlen - 1; i++)
					{
						Usart_SendByte(UART4, RDSS_BUF_NOW[19 + i]);
					}
					Usart_SendByte(UART4, crc_last(RDSS_BUF_NOW, 19, RDSS_Tlen + 18));
					Usart_SendByte(UART4, 0xEB);
				}


				Usart_SendString(USART6, "uart4->center: ");
				hextochar1(RDSS_BUF_NOW, RDSS_Tlen - 1);
				Usart_SendString(USART6, "\r\n");
			}

        }
        /*************************************************************************************************/
        if(next_data == 0)                                     //电控指令发送给岸站 ，不发送数据包
        {
			if(last_data == 1) //发送准备或最后一个包（42）
			{
				 if((tc_timestamp - last_send_tc) >= 65)        //间隔65s发送
                {
                    first_send = 0;                            //清0
					
                    BD_REPORT(5, LAST_DATA);       //发送给岸站
                }
				
			  
			}
					
            if(first_send == 1)             
            {			
                if((tc_timestamp - last_send_tc) >= 65)        //间隔65s发送
                {
                    first_send = 0;                            //清0
					
                    BD_REPORT(repeat_count, repeat_buf);       //发送给岸站
                }
            }

            if(repeat_send > 0 && repeat_send < 4)             //发送失败时重复发送
            {
                if((tc_timestamp - last_send_tc) >= 65)
                {
                    BD_REPORT(repeat_count, repeat_buf);
                }
            }                                                 //至多重复发送3次后清0
            else
            {
                repeat_send = 0;
            }
            if(tc_timestamp - test_send5 >= 5)                 //调试用
            {
                test_send5 = tc_timestamp;
                Usart_SendString(USART6, "52 off\r\n");
            }
        }
        else                                                   //数据包透传
        {
            if(first_send == 1)
            {
                if((tc_timestamp - last_send_tc) >= 65)        //间隔65s发送
                {
                    first_send = 0;
                    BD_REPORT(repeat_count, repeat_buf);
                    next_send_time = tc_timestamp;             //下一条指令时间戳赋值，tc_timestamp每秒加1
                    //Delay_us(10000);
					open = 1;
                    close5 = tc_timestamp+6;
//					LED1(0);				                   //关闭5v12v
//                    LED2(0);
                }
            }

            else if(repeat_send > 0 && repeat_send < 4)        //重复发送
            {
                if((tc_timestamp - last_send_tc) >= 60)
                {
                    LED1(1);				                   //open5v12v
                    LED2(1);
                    delay_s(5);
                    BD_REPORT(repeat_count, repeat_buf);

                }
            }
            else                                               //至多重复3次清0
            {
                repeat_send = 0;
                if((tc_timestamp - next_send_time) >= 60)
                {
                    LED1(1);				                   //open5v12v
                    LED2(1);
                    delay_s(5);
                    next_send_time = tc_timestamp;             //更新下一条指令时间戳
                    Usart_SendStr_length(UART4, NEXT_DATA, 8); //发送下一条指令


                    Usart_SendString(USART6, "uart4->center: ");
                    hextochar(NEXT_DATA, 8);
                    Usart_SendString(USART6, "\r\n");
                }
            }
            if(tc_timestamp - test_send5 >= 5)                //调试用
            {
                test_send5 = tc_timestamp;
                Usart_SendString(USART6, "52 on\r\n");
            }
			if(open)
			{
				
				if(close5  <  tc_timestamp)
				{
					open = 0;
					LED1(0);
					LED2(0);
				}
			}
			
        }
//       if(tc_timestamp - last_send_tc >= 300)
//		   
//        {
//            Usart_SendString(USART6, "5min  重启\r\n");
//            
//			  LED1(0);
//            LED2(0);
//            delay_s(5);
//            LED1(1);
//            LED2(1);
//            last_send_tc = tc_timestamp;
//        }
		if(TIM5_Flag == 1)
		{
		   TIM_Cmd(TIM5,DISABLE);
		   TIM5_Flag = 0;
		   if(NO_FKXX_FLAG == 0)
		   {
		      
			  Usart_SendString(USART6, "无反馈信息重启\r\n"); 
			  LED1(0);
              LED2(0);
              delay_s(5);
              LED1(1);
              LED2(1);
		   }
		   NO_FKXX_FLAG=0;
		   
			
		}


    }



}



/*********************************************END OF FILE**********************/

