#include "i2c.h"
#include "usart.h"

#define I2C_SPEED 100000
#define I2C_TIMEOUT 100000

void I2C_Setup(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* I2C1 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);


    /* I2C1 SDA and SCL configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //SCL is pin06 and SDA is pin 07 for I2C1



    /*enable I2C*/
    I2C_Cmd(I2C1,ENABLE);

    I2C_DeInit(I2C1);

    I2C_StructInit(&I2C_InitStructure);
    /* I2C1 configuration */
//    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//    I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
//    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    I2C_Init(I2C1, &I2C_InitStructure);



     //I2C_AcknowledgeConfig(I2C1, ENABLE);
    /*!< Enable SMBus Alert interrupt */
    //I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);    //dont think this is necessary


}

uint8_t I2C_ReadShort(uint16_t reg_address, uint16_t dev_address)
{
	uint16_t data = 0;
	uint32_t timeout = I2C_TIMEOUT;
	dev_address = dev_address << 1;

	I2C_AcknowledgeConfig(I2C1, ENABLE);

	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) {
		if (! timeout--)
			Usart1Put('q');
	}


	I2C_GenerateSTART(I2C1, ENABLE);

	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)){
		if (! timeout--)
			Usart1Put('w');
	}


	I2C_Send7bitAddress(I2C1, dev_address, I2C_Direction_Transmitter);

	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if (! timeout--)
			Usart1Put('e');
	}

	I2C_SendData(I2C1, reg_address);	// write register address
	timeout = I2C_TIMEOUT;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		if (! timeout--)
			Usart1Put('r');

	}

	/* restart */
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)){
			if (! timeout--)
				Usart1Put('t');
	}

	I2C_Send7bitAddress(I2C1, dev_address, I2C_Direction_Receiver);
	timeout = I2C_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
		if (! timeout--)
			Usart1Put('y');
	}

	/* before last one, disable ACK */
	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);
	I2C_AcknowledgeConfig(I2C1, DISABLE);


	/* Receive */
	I2C_ReceiveData(I2C1);
	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){
		if (! timeout--)
			Usart1Put('u');
	}

	/* Receive */
	data =	I2C_ReceiveData(I2C1);
	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){
		if (! timeout--)
			Usart1Put('i');
	}

	I2C_GenerateSTOP(I2C1, ENABLE);
	timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)){
		if (! timeout--)
			Usart1Put('o');
	}


	return data;
}

uint8_t I2C_WriteShort(uint16_t reg_address, uint16_t dev_address, uint8_t data)
{
	uint32_t timeout = I2C_TIMEOUT;
	dev_address = dev_address << 1;

	I2C_AcknowledgeConfig(I2C1, ENABLE);

	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) {
		if (! timeout--)
			Usart1Put('q');
	}


	I2C_GenerateSTART(I2C1, ENABLE);

	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)){
		if (! timeout--)
			Usart1Put('w');
	}


	I2C_Send7bitAddress(I2C1, dev_address, I2C_Direction_Transmitter);

	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if (! timeout--)
			Usart1Put('e');
	}

	I2C_SendData(I2C1, reg_address);	// write register address
	timeout = I2C_TIMEOUT;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		if (! timeout--)
			Usart1Put('r');

	}


	/* send */
	I2C_SendData(I2C1, data);
	timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		if (! timeout--)
			Usart1Put('t');
	}

	I2C_GenerateSTOP(I2C1, ENABLE);
	timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)){
		if (! timeout--)
			Usart1Put('y');
	}


	return 0;
}
