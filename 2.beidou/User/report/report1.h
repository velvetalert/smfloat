#ifndef _REPORT1_H
#define _REPORT1_H
#include "stm32f4xx.h"


extern uint32_t SendAddr;
extern uint32_t MyAdder;
//void ByteUtill();
//void crc_data(uint8_t data[], uint8_t startIndex, uint16_t length);
extern int32_t last_send_tc;
void BD_REPORT(uint8_t length,uint8_t BD_SendData[]);

uint8_t crc_test_control(uint8_t data[], uint8_t startIndex, uint16_t length);
uint8_t crc_last(uint8_t data[], uint8_t startIndex, uint16_t length);
uint8_t crc_test(uint8_t data[], uint8_t startIndex, uint16_t length);

























#endif