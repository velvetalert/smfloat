#include "./Satellite/BD.h"
#include "./systick/bsp_SysTick.h"

/*ʵ���ϣ��������������ܷ��������֣�һ�ǣ��̶�ָ��ķ��ͣ����ǣ��䳤SBDָ��ķ��ͣ����ǣ�ָ�����ɺ�ķ�����Ϣ��
�ر���ָ�����ɺ󣬷��ص���Ϣ��һ���֡�ʵ���ϣ����Բ���������ĳ���һ������Ҫ��ϸ���룬����ֱ�ӽ�������Ϣ���ͳ�ȥ����*/
/*��������Ǳ���2.1���*/
//#include <STC12C5A60S2.H>
//#include <stdio.h>
//#include <string.h>
//#include <intrins.h>
//#include "HXHD_SZTH.h"


//CONVERT_HEX_CHAУ��λתASCII��
//CONVERT_HEX_CHAR����˵����c����ѭ��У�飻h�����λ��l�����λ
#define CONVERT_HEX_CHAR(c, h, l) l = 0x0F&c;h=0x0F&(c>>4);l =(l<=9)?('0'+l):('A'+l-10);h=(h<=9)?('0'+h):('A'+h-10)//ASCII��ת��

void TTCA (void);//���ͱ��Ĵ������ĳ���

//*************************************CRCУ��***********************************//
unsigned char BDCRC(char *buf, int size, char h[2])//BDCRC��1�������������У���ַ�������2���ǳ��ȣ�
//��3��Ҳ���ַ���������У��ֵ��λ�͸�λ
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

/************************����SBD����λ��ʹ�õı�������*****************************/
char SendDat0[101];//ԭ����ʽchar�����ǵ����滹�е������ݣ�����Ϊ��ָ���Ū16���ƣ������int���ƺ�����
									//���Խ�����ֿ���һ����int�ͣ����ڹ̶���ָ�����ݲ��֡�
									//��һ�֣���char�ͣ�����ָ���
									//���ڹ̶��ı������룬��������Ū�����Ƕ���SBD�ķ�ʽ����Ϊ��Ҫ�ӱ��ģ�������Ҫchar����ʽ
									//���԰�Ҫ����ת��char����ʽ���ͣ��ؼ����ڶ���ʱ��char��printfʱ��%d��������ʽ
char UART_Buffer_Size0;//����0�������ݻ����С
char UART_Buffer0[200];//={"$TTCA,1,136001,1,0,14,abcdefghijklmn,EA\r\n"};
											 //����ʱ���ǲ����Ѿ�ת�õ�16������
char SendDat1[101];
char UART_Buffer_Size1;//����1�������ݻ����С
char UART_Buffer1[200];//={"$RD,28,424956,1,14,ABCDEFGHIJKLMN,EA\r\n"};
char CRCtemp[99];//CRCУ��
char C[2];//?
unsigned char count0,count1,k;
unsigned char TTCA_1,TTCA_2;
unsigned char CRCVALUE;//CRCУ���ֵ
unsigned char Flag0,Flag1;
int i,iiii;

extern uint8_t Rxflag;//���ڽ��յ���Ϣ�ı�ʶ��
extern uint8_t ucTemp;
uint8_t BDRxBuf[256];//ucaRxBuf[256]�ظ�����ĳ�BDRxBuf[256]
uint16_t BDRxCount=0;// usRxCount=0�ظ�����ĳ�BDRxCount

void HXHD_SZTH(void)
{

	

	
	
}
