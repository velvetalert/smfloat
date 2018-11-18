#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./report/report1.h"
#include <string.h>
#include "./SYS/systick.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "stm32f4xx_it.h"

/*�洢��ش����������ݺ����ݳ���*/
u16 USART4_COUNT_NOW;
u8 USART4_BUF_NOW[200];


/*�洢���������������ݺ����ݳ���*/
u8 RDSS_COUNT_NOW;
u8 RDSS_BUF_NOW[200];


/*�洢��ش�������͸������վ�����ݺ����ݳ���*/
u8 repeat_count;              //���ݳ���
u8 repeat_send = 0;           //�ظ����͵Ĵ���������3�Σ�
u8 repeat_buf[200];           //�洢͸��������

u8 first_send = 0;            //��ط���͸������ʱ��1������ʱ����
u8 next_data = 0;             //�յ���վ��40ָ��ʱ��1����ʼ͸������
u8 last_data = 0;             //�յ���ص�42�����һ������ָ��ʱ��1
u8 send_last_gps = 0;         //�յ�01ʱ��1���յ�00������

int32_t next_send_time = 0;   //����������һ��ָ���ʱ���
int32_t test_send5 = 0;       //�����õ�5��ʱ���
int32_t close5 = 0;
u8 open = 0;


u8 TIM5_Flag=0;
u8 NO_FKXX_FLAG=0;
/*��һ��ָ��52*/
uint8_t NEXT_DATA[] = {0xEC, 0x01, 0x52, 0x01, 0x01, 0x03, 0x50, 0xEB};


uint8_t LAST_DATA[] = {0x01, 0x42, 0x01, 0x01, 0x03};
/*************************************
��������hextochar��
���ܣ����ڽ����յ�ʮ��������ʾ��ָ��תΪASCII����ڵ���
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
��������hextochar1��
���ܣ����ڽ����յİ�վʮ��������ʾ��ָ��תΪASCII����ڵ���
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
��������init��
���ܣ���ʼ��������־
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
    /*��������*/
    RNSS_USART_Config();    //ʵ��û���õ�������ɾ��
    RDSS_USART_Config();    //����6���ԽӵĴ��ڣ�������115200
    CENTER_USART_Config();  //��ش��� ������9600
    Debug5_USART_Config();  //����GPS���ڣ�������9600
    Debug_USART_Config();   //����RDSS���ڣ����ڶ̱��ģ�������115200
    //SysTick_Init();
	
    BASIC_TIMx_Config();    //��ʱ������ʱ1s�ж����ڶ�ʱʱ��
    GENERAL_TIM5_Config();
	TIM_Cmd(TIM5,DISABLE);
    LED_GPIO_Config();      //IO�����ã����ڴ򿪺͹رձ���ģ���5V��12V����
    TIM_Cmd(TIM6, ENABLE);  //ʹ�ܶ�ʱ��
    USART_Cmd(UART5, DISABLE); //�ر�GPS����

    Usart_SendString(USART6, "DEBUG_START\r\n");
    Usart_SendString(UART4, "UART4\r\n");
   //Usart_SendString(USART1,"UART1\r\n");
    while(1)
    {
        /************************************************************************
        ��ش���ʵʱ��Ⲣ����
        Э��������ʱ��ͼ������word�ļ�
        ************************************************************************/
		if(USART4_RX_STA & 0x80)
        {
			USART4_RX_STA = 0;
			hextochar(USART4_BUF_NOW, USART4_COUNT_NOW);
			Usart_SendString(USART6, "\r\n");
			if(USART4_BUF_NOW[0] == 0xEC && USART4_BUF_NOW[USART4_COUNT_NOW - 1] == 0xEB)
			{
				/*����ָ��*/
				if(USART4_BUF_NOW[2] == 0xDD)
				{
					LED1(1);				 //��5v12v
					LED2(1);
					init();
					Usart_SendStr_length(UART4, USART4_BUF_NOW, USART4_COUNT_NOW);
					USART_Cmd(UART5, ENABLE); //��ʱ��GPS���ڣ�׼������GPS���ݸ���վ
				}
				/*����ָ��*/
				else if(USART4_BUF_NOW[2] == 0xAA)
				{
					LED1(0);				 //�ر�5v12v
					LED2(0);
					init();
					Usart_SendStr_length(UART4, USART4_RX_BUF, USART4_COUNT_NOW);
				}
				/*͸�������һ����*/
				else if(USART4_BUF_NOW[2] == 0x42)
				{
					LED1(1);				 //��5v12v
					LED2(1);
					next_data = 0;           //����������һ��ָ��
					last_data = 1;           //��ʱ��1
					first_send = 1;          //׼������
					repeat_count = USART4_COUNT_NOW - 3;
					STRNCPY(repeat_buf, USART4_BUF_NOW, 1, USART4_COUNT_NOW - 2);
				}
				/*ȫ�����͸���ɣ�����͸��*/
				else if(USART4_BUF_NOW[2] == 0x43)
				{
					LED1(1);				 //��5v12v
					LED2(1);
					next_data = 0;           //ȫ�����ճɹ�����0����������һ��ָ��
					first_send = 0;
				}
				/*͸�����ݺ����ݳ���*/
				else
				{
					first_send = 1;
					repeat_send = 0;
					repeat_count = USART4_COUNT_NOW - 3;
					STRNCPY(repeat_buf, USART4_BUF_NOW, 1, USART4_COUNT_NOW - 2);
				}
			}
        }

        /*ʵʱ���GPS���ڵ�����*/
        if(RNSS_RX_STA == 1)             //RNSS_RX_STA��������ɱ�־
        {
            if(RNSS_RX_BUF[47] == '1')   //λ����Ч����ȡʱ��λ����Ϣ
            {
                uint8_t t;

                RNSS_BUF[0] = 39;
                if(!send_last_gps)
                    RNSS_BUF[1] = 1;
                else
                {
                    RNSS_BUF[1] = 0x50;  //�����µ�GPSָ��
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
                first_send = 1;                               //׼�����ͣ���־��1
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
        
        /*����RDSS����ʵʱ���*/
        if(RDSS_RX_STA & 0x80)
        {  	
			Usart_SendString(USART6, "main***\r\n");
			Usart_SendStr_length(USART6, RDSS_BUF_NOW, RDSS_COUNT_NOW);
			Usart_SendString(USART6, "***\r\n");
			if(RDSS_RX_STA & 0x04)                    //���ͨ������RDSS���ڷ��͸���վʱ��ָ���
			{
				RDSS_RX_STA = 0;
				if(RDSS_BUF_NOW[10] == 0)
				{

					repeat_send = 0;                                      //���ͳɹ�����־��0
					Usart_SendString(USART6, "send success!\r\n");
				}
				else
				{
					repeat_send++;                                        //����ʧ�ܣ���־++
					Usart_SendString(USART6, "send fail!\r\n");
				}
			}

			if(RDSS_RX_STA & 0x02)                    //���հ�վ�ı�������ָ��
			{
				RDSS_RX_STA = 0;
				uint8_t i;
				uint8_t RDSS_Tlen = (RDSS_BUF_NOW[16] * 256 + RDSS_BUF_NOW[17]) / 8;
				/*******************************************************
				�յ��ؼ���Ϣ��40�����ط�ָ�41��������һ��ָ�52��
				���һ��ָ���־��0������׼����0���ظ���־��0
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
				/*�յ���λ��ʧ��GPS����*/
				if(RDSS_BUF_NOW[20] == 0x51)
				{
					USART_Cmd(UART5, DISABLE);
					first_send = 0;
					repeat_send = 0;
				}
				/*�յ����һ����*/
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
				/*��վָ��͸�������*/
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
        if(next_data == 0)                                     //���ָ��͸���վ �����������ݰ�
        {
			if(last_data == 1) //����׼�������һ������42��
			{
				 if((tc_timestamp - last_send_tc) >= 65)        //���65s����
                {
                    first_send = 0;                            //��0
					
                    BD_REPORT(5, LAST_DATA);       //���͸���վ
                }
				
			  
			}
					
            if(first_send == 1)             
            {			
                if((tc_timestamp - last_send_tc) >= 65)        //���65s����
                {
                    first_send = 0;                            //��0
					
                    BD_REPORT(repeat_count, repeat_buf);       //���͸���վ
                }
            }

            if(repeat_send > 0 && repeat_send < 4)             //����ʧ��ʱ�ظ�����
            {
                if((tc_timestamp - last_send_tc) >= 65)
                {
                    BD_REPORT(repeat_count, repeat_buf);
                }
            }                                                 //�����ظ�����3�κ���0
            else
            {
                repeat_send = 0;
            }
            if(tc_timestamp - test_send5 >= 5)                 //������
            {
                test_send5 = tc_timestamp;
                Usart_SendString(USART6, "52 off\r\n");
            }
        }
        else                                                   //���ݰ�͸��
        {
            if(first_send == 1)
            {
                if((tc_timestamp - last_send_tc) >= 65)        //���65s����
                {
                    first_send = 0;
                    BD_REPORT(repeat_count, repeat_buf);
                    next_send_time = tc_timestamp;             //��һ��ָ��ʱ�����ֵ��tc_timestampÿ���1
                    //Delay_us(10000);
					open = 1;
                    close5 = tc_timestamp+6;
//					LED1(0);				                   //�ر�5v12v
//                    LED2(0);
                }
            }

            else if(repeat_send > 0 && repeat_send < 4)        //�ظ�����
            {
                if((tc_timestamp - last_send_tc) >= 60)
                {
                    LED1(1);				                   //open5v12v
                    LED2(1);
                    delay_s(5);
                    BD_REPORT(repeat_count, repeat_buf);

                }
            }
            else                                               //�����ظ�3����0
            {
                repeat_send = 0;
                if((tc_timestamp - next_send_time) >= 60)
                {
                    LED1(1);				                   //open5v12v
                    LED2(1);
                    delay_s(5);
                    next_send_time = tc_timestamp;             //������һ��ָ��ʱ���
                    Usart_SendStr_length(UART4, NEXT_DATA, 8); //������һ��ָ��


                    Usart_SendString(USART6, "uart4->center: ");
                    hextochar(NEXT_DATA, 8);
                    Usart_SendString(USART6, "\r\n");
                }
            }
            if(tc_timestamp - test_send5 >= 5)                //������
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
//            Usart_SendString(USART6, "5min  ����\r\n");
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
		      
			  Usart_SendString(USART6, "�޷�����Ϣ����\r\n"); 
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

