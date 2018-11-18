#include "./report/report1.h"
#include "./usart/bsp_debug_usart.h"
#include <string.h>
#include "./SYS/systick.h"
//#include "stm32f4xx_it.h"
extern uint8_t CENTER_BUF[200];
extern int32_t next_send_time;
//extern uint8_t BD_count;
uint8_t hexChars[200];
uint8_t CRC_BD[3];
uint32_t SendAddr = 360456;
uint32_t MyAdder = 360458;
int32_t last_send_tc = 0;
static void ByteUtill()
{
    const uint8_t hexArray[17] = "0123456789ABCDEF";
    //static uint8_t hexChars[34];
    uint8_t j;
    for ( j = 0; j < strlen((char*)CENTER_BUF); j++) {
        int v = CENTER_BUF[j] & 0xFF;
        hexChars[j * 2] = hexArray[(v >> 4)];
        hexChars[j * 2 + 1] = hexArray[v & 0x0F];
    }
}
static void crc_data(uint8_t data[], uint8_t startIndex, uint16_t length) {
    uint8_t crc = 0x00;
    uint8_t i;
    const uint8_t hexArray[17] = "0123456789ABCDEF";
    for ( i = 0; i < length; i++) {
        crc ^= data[startIndex + i];
    }
    CRC_BD[0] = hexArray[(crc >> 4)];
    CRC_BD[1] = hexArray[(crc & 0x0F)];
}
void BD_REPORT(uint8_t length, uint8_t BD_SendData[])
{
    uint8_t i;
    uint8_t send_txsq_data[512] = "$TXSQ";
    //uint8_t BDTXA[100]={0xA4};
    uint16_t alllen;
    alllen = 5 + 2 + 3 + 1 + 3 + 2 + 1 + length + 1 + 1;
    //strcat((char*)BDTXA,(char*)BD_SendData);
    send_txsq_data[5] = (alllen >> 8);
    send_txsq_data[6] = (alllen & 0xFF);

    send_txsq_data[7] = (MyAdder >> 16);
    send_txsq_data[8] = (MyAdder >> 8);
    send_txsq_data[9] = (MyAdder & 0xFF);
    send_txsq_data[10] = 0x46;
    send_txsq_data[11] = (SendAddr >> 16);
    send_txsq_data[12] = (SendAddr >> 8);
    send_txsq_data[13] = (SendAddr & 0xFF);
    send_txsq_data[14] = ((length + 1) * 8 >> 8);
    send_txsq_data[15] = ((length + 1) * 8 & 0xFF);
    send_txsq_data[16] = 0x00;
    send_txsq_data[17] = 0xA4;
    //strcat((char*)send_txsq_data,(char*)BD_SendData);
    for( i = 0; i < length + 1; i++)
    {
        send_txsq_data[18 + i] = BD_SendData[i];
    }
    send_txsq_data[18 + length] = crc_last(send_txsq_data, 0, 18 + length);
    Usart_SendStr_length( USART1, send_txsq_data, 19 + length );
	TIM_Cmd(TIM5,ENABLE);
    Usart_SendString(USART6, "uart1>>: ");
    Usart_SendStr_length(USART6, send_txsq_data, 19 + length);
    Usart_SendString(USART6, ">>\r\n");
    //strcat(BDTXA,BD_SendData);
    last_send_tc = tc_timestamp;
    next_send_time = tc_timestamp;
}
uint8_t crc_test_control(uint8_t data[], uint8_t startIndex, uint16_t length) {
    uint8_t crc = 0x00;
    uint8_t i;
    for ( i = startIndex; i < length - 2; i++) {
        crc ^= data[i];
    }
    if(data[length - 2] == crc)
    {
        return 1;
    }
    else return 0;
}
uint8_t crc_test(uint8_t data[], uint8_t startIndex, uint16_t length) {
    uint8_t crc = 0x00;
    uint8_t i;
    for ( i = 0; i < length - 1; i++) {
        crc ^= data[startIndex + i];
    }
    if(data[length - 1] == crc)
    {
        return 1;
    }
    else return 0;
}
uint8_t crc_last(uint8_t data[], uint8_t startIndex, uint16_t length) {
    uint8_t crc = 0x00;
    uint8_t i;
    for ( i = startIndex; i < length; i++) {
        crc ^= data[i];
    }
    return crc;
}