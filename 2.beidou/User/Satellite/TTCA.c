#include "./usart/bsp_debug_usart.h"
#include "./Satellite/BD.h"
void TTCA(void)
{
	TTCA_1=6,k=0,iiii=0;//TTCA_2计算电文实际长度（但是没有计算最后校验位，校验位最后加即可），TTCA_1用于选择一条指令里的字符位置
	SendDat0[0]='$';//报头$TXSQ
	SendDat0[1]='T';
	SendDat0[2]='X';
	SendDat0[3]='S';
	SendDat0[4]='Q';
	//5,6位给指令长度用，在下面一起计算
	//为什么定义TTCA_1=6？因为从第6位开始才是会变化的，前面都是$号的固定指令识别开头
	TTCA_1++;//数组是从0开始定义的，$TXSQ一共是5位，再加上2位指令长度，一共7位。
					 //这里TTCA_1++;执行完后等于7,TTCA_1是数组下标的值
	TTCA_2=TTCA_1+7;//这里加完之后应该是包括了“用户地址	 消息类别	用户地址”三部分内容
									//执行完TTCA_2=TTCA_1+7;后，TTCA_2为14，正好到了计算“电文长度”的位置
								  //完全和北斗2.0的例子对应，只是这里的数值不太一样有了变化
  //7,8,9用户号码
	//用户ID：028BA2（16进制），166818（10进制）,char的形式可以直接定义成16进制0x24的形式，输出直接对应为$号
	SendDat0[7]=0x02;
	SendDat0[8]=0x8B;
	SendDat0[9]=0xA2;
	//第10位消息类别
	SendDat0[10]=0x46;
	//11,12,13用户地址,314192
	SendDat0[11]=0x04;
	SendDat0[12]=0xCB;
	SendDat0[13]=0x50;
	//14,15下面计算“电文长度”
	TTCA_2++;
	TTCA_2++;
	//16是否应答，默认为
	SendDat0[16]=0x00;
	TTCA_2++;//原程序也是在case语句中对后面添加的内容自动在TTCA_2中加1，用于计算总长度
	//17往后是电文内容，需要根据实际情况来定，
	SendDat0[17]=0xA4;
	TTCA_2++;
	SendDat0[18]='h';
	TTCA_2++;
	SendDat0[19]='e';
	TTCA_2++;
	SendDat0[20]='l';
	TTCA_2++;
	SendDat0[21]='l';
	TTCA_2++;
	SendDat0[22]='o';
	TTCA_2++;//这里测试暂时这样用，后面真正发送内容时，需要用循环语句来，不用这样一句一句的写

	SendDat0[5]=((TTCA_2+1)/100);//总长度十位
	SendDat0[6]=((TTCA_2+1)%100);//总长度个位
	//14,15为电文长度，TTCA_2-TTCA_1-6，-TTCA_1是减去固定的“$TXSQ+长度占位”的部分，
	//-10是去除中间的“用户地址	 消息类别	用户地址	 电文长度	是否应答”,报文内容之前的位
	SendDat0[14]=(((TTCA_2-TTCA_1-10)*8)/100);//报文长度十位
	SendDat0[15]=(((TTCA_2-TTCA_1-10)*8)%100);//报文长度个位


//计算校验和，北斗2.0和北斗4.0这个应该是一样的
	for(iiii=0;iiii<TTCA_2;iiii++)//校验是从$开始，还是从$号之后开始？从$号开始
	{
		CRCtemp[iiii]=SendDat0[iiii];
	}
	CRCVALUE=BDCRC(CRCtemp,TTCA_2,C);
	
	SendDat0[TTCA_2]=CRCVALUE;
	//SendDat0[TTCA_2+1]=C[0];	//校验值高四位
	//SendDat0[TTCA_2+2]=C[1];	//校验值低四位
	//SendDat0[TTCA_2+3]=0x0D;	//结束位
	//SendDat0[TTCA_2+4]=0x0A;  //结束位

	for(i=0;i<=TTCA_2;i++)
		{//OutChar1(SendDat0[i]);//来自HXHD_SZTH.c中void OutChar1(unsigned char message) //发送给报文板
		printf("%02x",SendDat0[i]);//%d是整型输出，%c是字符形式输出
		}
//	Usart_SendString( UART4,SendDat0 );
		}
