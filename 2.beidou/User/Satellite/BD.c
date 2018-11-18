#include "./Satellite/BD.h"
#include "./systick/bsp_SysTick.h"

/*实际上，北斗的整个功能分了三部分：一是，固定指令的发送；二是，变长SBD指令的发送；三是，指令发送完成后的返回信息，
特别是指令发送完成后，返回的信息这一部分。实际上，可以不用像这里的程序一样，还要详细解码，可以直接将返回信息发送出去即可*/
/*这个程序是北斗2.1版的*/
//#include <STC12C5A60S2.H>
//#include <stdio.h>
//#include <string.h>
//#include <intrins.h>
//#include "HXHD_SZTH.h"


//CONVERT_HEX_CHA校验位转ASCII码
//CONVERT_HEX_CHAR参数说明，c代表循环校验；h代表高位；l代表低位
#define CONVERT_HEX_CHAR(c, h, l) l = 0x0F&c;h=0x0F&(c>>4);l =(l<=9)?('0'+l):('A'+l-10);h=(h<=9)?('0'+h):('A'+h-10)//ASCII码转换

void TTCA (void);//发送报文处理，报文长度

//*************************************CRC校验***********************************//
unsigned char BDCRC(char *buf, int size, char h[2])//BDCRC第1个参数是输入的校验字符串；第2个是长度；
//第3个也是字符串，代表校验值低位和高位
{
	unsigned char crc = 0;
	unsigned int i = 0;
	for (i = 0; i<size; i++)
	{
		crc^=buf[i];		
	}
	CONVERT_HEX_CHAR(crc, h[0], h[1]);			
	return crc;
}

/************************北斗SBD，定位所使用的变量定义*****************************/
char SendDat0[101];//原来格式char，考虑到后面还有电文内容，这里为了指令好弄16进制，定义成int型似乎不好
									//可以将这里分开，一种是int型，用于固定的指令内容部分。
									//另一种，是char型，用于指令发送
									//对于固定的北斗代码，可以这样弄，但是对于SBD的方式，因为还要加报文，所以需要char的形式
									//可以把要发的转成char的形式推送，关键是在定义时是char，printf时是%d的整型形式
char UART_Buffer_Size0;//串口0发送数据缓存大小
char UART_Buffer0[200];//={"$TTCA,1,136001,1,0,14,abcdefghijklmn,EA\r\n"};
											 //发送时，是不是已经转好到16进制了
char SendDat1[101];
char UART_Buffer_Size1;//串口1发送数据缓存大小
char UART_Buffer1[200];//={"$RD,28,424956,1,14,ABCDEFGHIJKLMN,EA\r\n"};
char CRCtemp[99];//CRC校验
char C[2];//?
unsigned char count0,count1,k;
unsigned char TTCA_1,TTCA_2;
unsigned char CRCVALUE;//CRC校验的值
unsigned char Flag0,Flag1;
int i,iiii;

extern uint8_t Rxflag;//串口接收到信息的标识符
extern uint8_t ucTemp;
uint8_t BDRxBuf[256];//ucaRxBuf[256]重复定义改成BDRxBuf[256]
uint16_t BDRxCount=0;// usRxCount=0重复定义改成BDRxCount

void HXHD_SZTH(void)
{

	

	
	
}
