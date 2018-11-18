#include "./i2c/bsp_i2c_eeprom.h"

void Wait_for_EEPROM(void);

/*1. GPIO配置*/
void EEPROM_GPIO_Config(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 使能GPIO时钟*/	
  RCC_AHB1PeriphClockCmd(EEPROM_SCL_GPIO_CLK|EEPROM_SDA_GPIO_CLK ,ENABLE);
	/* 使能外设时钟*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置SCL引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = EEPROM_SCL_PIN  ;  
  GPIO_Init(EEPROM_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* 配置SDA引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = EEPROM_SDA_PIN;
  GPIO_Init(EEPROM_SDA_GPIO_PORT, &GPIO_InitStructure);
  
 
  GPIO_PinAFConfig(EEPROM_SCL_GPIO_PORT,EEPROM_SCL_SOURCE ,EEPROM_SCL_AF);
  GPIO_PinAFConfig(EEPROM_SDA_GPIO_PORT,EEPROM_SDA_SOURCE ,EEPROM_SDA_AF);
}



/*2.I2C模式配置 */
void EEPROM_I2C_Mode_Config(void)   
{
	I2C_InitTypeDef  I2C_InitStructure; 
 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		              
  I2C_InitStructure.I2C_OwnAddress1 =I2C_OWN_ADDR; 
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	
	/* I2C的寻址模式 */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	
	/* I2C1 初始化 */
  I2C_Init(EEPROM_I2C, &I2C_InitStructure);	 
  /* 使能 I2C1 */	
  I2C_Cmd(EEPROM_I2C, ENABLE);  	                                                

  I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);  	
}



/*3.byte write 函数 */
void EEPROM_Byte_Write(uint8_t* pDate,uint8_t addr)
{
  I2C_GenerateSTART(EEPROM_I2C, ENABLE); 
  
	while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS)    //EV5
{

}

  I2C_Send7bitAddress(EEPROM_I2C,EEPROM_ADDR,I2C_Direction_Transmitter);    //发送EEPROM设备地址

  while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS)  //EV6
{

}

  I2C_SendData(EEPROM_I2C,addr);    //发送要写入的EEPROM内部地址

  while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS)  //EV8
{

}

  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);

}


/*4. random read,读函数*/
uint8_t EEPROM_Byte_Read(uint8_t addr)
{
	uint8_t readTemp;
  I2C_GenerateSTART(EEPROM_I2C, ENABLE); 
  
	while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS)    //EV5
{

}

  I2C_Send7bitAddress(EEPROM_I2C,EEPROM_ADDR,I2C_Direction_Transmitter);    //发送EEPROM设备地址

  while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS)  //EV6
{

}

  I2C_SendData(EEPROM_I2C,addr);    //发送要写入的EEPROM内部地址

  while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS)  //EV8
{

}

  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
  while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS)    //EV5
{

}
  I2C_Send7bitAddress(EEPROM_I2C,EEPROM_ADDR,I2C_Direction_Receiver);    //发送EEPROM设备地址
  while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)!=SUCCESS)  //EV6
{

}

  I2C_AcknowledgeConfig(EEPROM_I2C,DISABLE);
 while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS)  //EV7
{

}
  readTemp=I2C_ReceiveData(EEPROM_I2C);
  
    

  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);


  return readTemp;

}


/*5.等待EEPROM内部写完毕*/
void Wait_for_EEPROM(void)
{
   do
	 {
	 I2C_GenerateSTART(EEPROM_I2C, ENABLE);
	 
//   while(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS) 
//	 {
//	 }
    I2C_Send7bitAddress(EEPROM_I2C,EEPROM_ADDR,I2C_Direction_Transmitter);
   
   }while(I2C_GetFlagStatus(EEPROM_I2C,I2C_FLAG_ADDR) == RESET);
//等待ADDR=1才执行下面的语句
	 
	 I2C_ClearFlag(EEPROM_I2C, I2C_FLAG_AF);
	 
	 I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
	 
	 while(I2C_GetFlagStatus(EEPROM_I2C,I2C_FLAG_BUSY)==SET);
	 //总线空闲，执行下面的语句
	 
}	



