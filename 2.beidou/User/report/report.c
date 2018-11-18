#include "./report/report.h"
#include <string.h>
//#include "stm32f4xx_it.h"
extern uint8_t CENTER_BUF[200];
extern uint8_t hexChars[200];
extern uint8_t CRC_BD[3];
void ByteUtill()
{
	const uint8_t hexArray[17]="0123456789ABCDEF";
	//static uint8_t hexChars[34];
	for (uint8_t j = 0; j < strlen((char*)CENTER_BUF); j++) {
		int v = CENTER_BUF[j] & 0xFF;
		hexChars[j * 2] = hexArray[(v >> 4)];
		hexChars[j * 2 + 1] = hexArray[v & 0x0F];
	}
}
void crc(uint8_t data[], uint8_t startIndex, uint16_t length) {
	uint8_t crc = 0x00;
	const uint8_t hexArray[17]="0123456789ABCDEF";
	for (uint8_t i = 0; i < length; i++) {
		crc ^= data[startIndex + i];
	}
	CRC_BD[0]=hexArray[(crc >> 4)];
	CRC_BD[1]=hexArray[(crc & 0x0F)];
}