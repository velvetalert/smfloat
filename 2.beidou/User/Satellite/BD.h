//#define S2RI 0x01	//串口2接收中断请求标志位
//#define S2TI 0x02	//串口2发送中断请求标志位
#define CONVERT_HEX_CHAR(c, h, l) l = 0x0F&c;h=0x0F&(c>>4);l =(l<=9)?('0'+l):('A'+l-10);h=(h<=9)?('0'+h):('A'+h-10)

//void Delay1ms(int);//要更换成Delay_ms()，61s Delay_ms(61000);
//void UartInit(void);
//void Delay(int);
//void OutChar0(unsigned char);
//void OutChar1(unsigned char);
void HXHD_SZTH(void);
void TTCA (void);//北斗sbd通信用子函数
//void RD(void);
//void PG(void);
//void ESOUT(void);
//void PSOUT(void);
//void ICOUT(void);

/************************************CRC校验**********************************/
unsigned char BDCRC(char *buf, int size, char h[2]);

extern int i,ii,iii,iiii;
extern char SendDat0[101];
extern char UART_Buffer_Size0;
extern char UART_Buffer0[200];
extern char SendDat1[101];
extern char UART_Buffer_Size1;
extern char UART_Buffer1[200];
extern char CRCtemp[99];
extern char C[2];
extern unsigned char count0,count1,k;
extern unsigned char TTCA_1,TTCA_2;
extern unsigned char PG_1;
extern unsigned char RD_1,RD_2;
extern unsigned char ES_1;
extern unsigned char PS_1;
extern unsigned char IC_1;
extern unsigned char CRCVALUE;
extern unsigned char Flag0,Flag1;
