#include "./led/bsp_led.h"   

void LED_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED��ص�GPIO����ʱ��*/
		//RCC_AHB1PeriphClockCmd ( LED1_GPIO_CLK, ENABLE); 
    RCC_AHB1PeriphClockCmd ( LED1_GPIO_CLK||LED2_GPIO_CLK,ENABLE);
		/*ѡ��Ҫ���Ƶ�GPIO����*/															   
		//GPIO_InitStructure.GPIO_Pin = LED1_PIN;	
    GPIO_InitStructure.GPIO_Pin = (LED1_PIN|LED2_PIN);
		/*��������ģʽΪ���ģʽ*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*��������Ϊ����ģʽ*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*������������Ϊ2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

		/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
		GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);	
    /*ѡ��Ҫ���Ƶ�GPIO����*/															   
		
}
/*********************************************END OF FILE**********************/
