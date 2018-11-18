/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �ض���c��printf������usart�˿ڣ��жϽ���ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./usart/bsp_debug_usart.h"



/*************************************************/
uint8_t RNSS_RX_BUF[200];
uint8_t RNSS_RX_COUNT;
uint8_t dLan[29];
uint8_t RNSS_BUF[200];
uint8_t RNSS_RX_STA;
/********************************************/
uint8_t RDSS_RX_STA;
uint8_t RDSS_RX_COUNT;
uint8_t RDSS_RX_BUF[200];
uint8_t RDSS_RX_REPORT[200];
uint16_t RDSS_Alen;
/**********************************************/
uint8_t USART4_RX_STA;
uint8_t USART4_RX_COUNT;
uint8_t USART4_Alen;
uint8_t USART4_RX_BUF[200];
/*********************************************/
/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */


int fputc(int ch, FILE *f)
{
    // ???????????1
    USART_SendData(USART6, (uint8_t) ch);
    //USART_SendData(OUTPUT_USART, (uint8_t) ch);//?????printf????6??

    // ??????
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
    //while (USART_GetFlagStatus(OUTPUT_USART, USART_FLAG_TXE) == RESET);

    return (ch);
}

///???c???scanf???RS232_USART,???????scanf?getchar???
int fgetc(FILE *f)
{
    // ????????
    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USART6);
}





void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;
    /* �������ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* �����ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1 ���жϽ���ģʽ
 * @param  ��
 * @retval ��
 */
void Debug_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_GPIO_CLK | DEBUG_USART_TX_GPIO_CLK, ENABLE);

    /* ʹ�� USART ʱ�� */
    RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);

    /* GPIO��ʼ�� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ����Tx����Ϊ���ù���  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* ���� PXx �� USARTx_Tx*/
    GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

    /*  ���� PXx �� USARTx__Rx*/
    GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_SOURCE, DEBUG_USART_TX_AF);

    /* ���ô�DEBUG_USART ģʽ */
    /* ���������ã�DEBUG_USART_BAUDRATE */
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
    /* �ֳ�(����λ+У��λ)��8 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* ֹͣλ��1��ֹͣλ */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* У��λѡ�񣺲�ʹ��У�� */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* Ӳ�������ƣ���ʹ��Ӳ���� */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* ���USART��ʼ������ */
    USART_Init(DEBUG_USART, &USART_InitStructure);

    /* Ƕ�������жϿ�����NVIC���� */

    NVIC_Configuration();

    /* ʹ�ܴ��ڽ����ж� */
    USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);

    /* ʹ�ܴ��� */
    USART_Cmd(DEBUG_USART, ENABLE);
}


/*******************************************************/
void NVIC_Configuration1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn ;
    /* �������ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* �����ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}
void RNSS_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    RNSS_RX_COUNT = 0;
    RNSS_RX_STA = 0;
    RCC_AHB1PeriphClockCmd(RNSS_USART_RX_GPIO_CLK | RNSS_USART_TX_GPIO_CLK, ENABLE);

    /* ʹ�� USART ʱ�� */
    RCC_APB1PeriphClockCmd(RNSS_USART_CLK, ENABLE);

    /* GPIO��ʼ�� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ����Tx����Ϊ���ù���  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = RNSS_USART_TX_PIN  ;
    GPIO_Init(RNSS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = RNSS_USART_RX_PIN;
    GPIO_Init(RNSS_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* ���� PXx �� USARTx_Tx*/
    GPIO_PinAFConfig(RNSS_USART_RX_GPIO_PORT, RNSS_USART_RX_SOURCE, RNSS_USART_RX_AF);

    /*  ���� PXx �� USARTx__Rx*/
    GPIO_PinAFConfig(RNSS_USART_TX_GPIO_PORT, RNSS_USART_TX_SOURCE, RNSS_USART_TX_AF);

    /* ���ô�RNSS_USART ģʽ */
    /* ���������ã�RNSS_USART_BAUDRATE */
    USART_InitStructure.USART_BaudRate = RNSS_USART_BAUDRATE;
    /* �ֳ�(����λ+У��λ)��8 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* ֹͣλ��1��ֹͣλ */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* У��λѡ�񣺲�ʹ��У�� */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* Ӳ�������ƣ���ʹ��Ӳ���� */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* ���USART��ʼ������ */
    USART_Init(RNSS_USART, &USART_InitStructure);

    /* Ƕ�������жϿ�����NVIC���� */
    NVIC_Configuration1();

    /* ʹ�ܴ��ڽ����ж� */
    //USART_ITConfig(RNSS_USART, USART_IT_RXNE, ENABLE);

    /* ʹ�ܴ��� */
    USART_Cmd(RNSS_USART, ENABLE);
}
/*****************  ����һ���ַ� **********************/
void NVIC_Configuration2(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn ;
    /* �������ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* �����ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}
void RDSS_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    RDSS_RX_STA = 0;
    RDSS_RX_COUNT = 0;
    RCC_AHB1PeriphClockCmd(RDSS_USART_RX_GPIO_CLK | RDSS_USART_TX_GPIO_CLK, ENABLE);

    /* ʹ�� USART ʱ�� */
    RCC_APB2PeriphClockCmd(RDSS_USART_CLK, ENABLE);

    /* GPIO��ʼ�� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ����Tx����Ϊ���ù���  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = RDSS_USART_TX_PIN  ;
    GPIO_Init(RDSS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = RDSS_USART_RX_PIN;
    GPIO_Init(RDSS_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* ���� PXx �� USARTx_Tx*/
    GPIO_PinAFConfig(RDSS_USART_RX_GPIO_PORT, RDSS_USART_RX_SOURCE, RDSS_USART_RX_AF);

    /*  ���� PXx �� USARTx__Rx*/
    GPIO_PinAFConfig(RDSS_USART_TX_GPIO_PORT, RDSS_USART_TX_SOURCE, RDSS_USART_TX_AF);

    /* ���ô�RDSS_USART ģʽ */
    /* ���������ã�RDSS_USART_BAUDRATE */
    USART_InitStructure.USART_BaudRate = RDSS_USART_BAUDRATE;
    /* �ֳ�(����λ+У��λ)��8 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* ֹͣλ��1��ֹͣλ */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* У��λѡ�񣺲�ʹ��У�� */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* Ӳ�������ƣ���ʹ��Ӳ���� */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* ���USART��ʼ������ */
    USART_Init(RDSS_USART, &USART_InitStructure);

    /* Ƕ�������жϿ�����NVIC���� */
    NVIC_Configuration2();

    /* ʹ�ܴ��ڽ����ж� */
    //USART_ITConfig(RDSS_USART, USART_IT_RXNE, ENABLE);

    /* ʹ�ܴ��� */
    USART_Cmd(RDSS_USART, ENABLE);
}
/*******************************************************/
/*****************  ����һ���ַ� **********************/
void NVIC_Configuration3(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn ;
    /* �������ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* �����ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}
void CENTER_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    USART4_RX_STA = 0;
    USART4_RX_COUNT = 0;
    RCC_AHB1PeriphClockCmd(CENTER_USART_RX_GPIO_CLK | CENTER_USART_TX_GPIO_CLK, ENABLE);

    /* ʹ�� USART ʱ�� */
    RCC_APB1PeriphClockCmd(CENTER_USART_CLK, ENABLE);

    /* GPIO��ʼ�� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ����Tx����Ϊ���ù���  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = CENTER_USART_TX_PIN  ;
    GPIO_Init(CENTER_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = CENTER_USART_RX_PIN;
    GPIO_Init(CENTER_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* ���� PXx �� USARTx_Tx*/
    GPIO_PinAFConfig(CENTER_USART_RX_GPIO_PORT, CENTER_USART_RX_SOURCE, CENTER_USART_RX_AF);

    /*  ���� PXx �� USARTx__Rx*/
    GPIO_PinAFConfig(CENTER_USART_TX_GPIO_PORT, CENTER_USART_TX_SOURCE, CENTER_USART_TX_AF);

    /* ���ô�CENTER_USART ģʽ */
    /* ���������ã�CENTER_USART_BAUDRATE */
    USART_InitStructure.USART_BaudRate = CENTER_USART_BAUDRATE;
    /* �ֳ�(����λ+У��λ)��8 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* ֹͣλ��1��ֹͣλ */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* У��λѡ�񣺲�ʹ��У�� */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* Ӳ�������ƣ���ʹ��Ӳ���� */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* ���USART��ʼ������ */
    USART_Init(CENTER_USART, &USART_InitStructure);

    /* Ƕ�������жϿ�����NVIC���� */
    NVIC_Configuration3();

    /* ʹ�ܴ��ڽ����ж� */
    USART_ITConfig(CENTER_USART, USART_IT_RXNE, ENABLE);

    /* ʹ�ܴ��� */
    USART_Cmd(CENTER_USART, ENABLE);
}
/*******************************************************/
/*****************  ?????? **********************/
void NVIC_Configuration5(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ???????????? */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ??USART???? */
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn ;
    /* ??????1 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* ?????1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ???? */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ?????NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

void Debug5_USART_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(DEBUG5_USART_RX_GPIO_CLK | DEBUG5_USART_TX_GPIO_CLK, ENABLE);

    /* ?? USART ?? */
    RCC_APB1PeriphClockCmd(DEBUG5_USART_CLK, ENABLE);

    /* GPIO??? */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* ??Tx???????  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DEBUG5_USART_TX_PIN  ;
    GPIO_Init(DEBUG5_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* ??Rx??????? */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DEBUG5_USART_RX_PIN;
    GPIO_Init(DEBUG5_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* ?? PXx ? USARTx_Tx*/
    GPIO_PinAFConfig(DEBUG5_USART_RX_GPIO_PORT, DEBUG5_USART_RX_SOURCE, DEBUG5_USART_RX_AF);

    /*  ?? PXx ? USARTx__Rx*/
    GPIO_PinAFConfig(DEBUG5_USART_TX_GPIO_PORT, DEBUG5_USART_TX_SOURCE, DEBUG5_USART_TX_AF);

    /* ???DEBUG5_USART ?? */
    /* ?????:DEBUG5_USART_BAUDRATE */
    USART_InitStructure.USART_BaudRate = DEBUG5_USART_BAUDRATE;
    /* ??(???+???):8 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* ???:1???? */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* ?????:????? */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* ?????:?????? */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* USART????:????????? */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* ??USART????? */
    USART_Init(DEBUG5_USART, &USART_InitStructure);

    /* ?????????NVIC?? */
    NVIC_Configuration5();

    /* ???????? */
    USART_ITConfig(DEBUG5_USART, USART_IT_RXNE, ENABLE);

    /* ???? */
    USART_Cmd(DEBUG5_USART, ENABLE);
}
/*******************************************************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* ����һ���ֽ����ݵ�USART */
    USART_SendData(pUSARTx, ch);

    /* �ȴ��������ݼĴ���Ϊ�� */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k) != '\0');

    /* �ȴ�������� */
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
    {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    /* ȡ���߰�λ */
    temp_h = (ch & 0XFF00) >> 8;
    /* ȡ���Ͱ�λ */
    temp_l = ch & 0XFF;

    /* ���͸߰�λ */
    USART_SendData(pUSARTx, temp_h);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

    /* ���͵Ͱ�λ */
    USART_SendData(pUSARTx, temp_l);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str, uint32_t strlen )
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}
///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
void STRNCPY(uint8_t a[], uint8_t b[], uint8_t first, uint8_t last)
{
    uint8_t i = 0;
    for(i = first; i < last; i++)
    {
        a[i - first] = b[i];
    }
}

