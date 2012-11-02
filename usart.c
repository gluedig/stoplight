/*
 * usart.c
 *
 *  Created on: Aug 31, 2012
 *      Author: raber
 */

#include "usart.h"

void Usart1Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_InitTypeDef USART_InitStructure;

	USART_ClockInitTypeDef USART_ClockInitStructure;

	//enable bus clocks

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	//Set USART1 Tx (PA.09) as AF push-pull

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Set USART1 Rx (PA.10) as input floating

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_ClockStructInit(&USART_ClockInitStructure);

	USART_ClockInit(USART1, &USART_ClockInitStructure);

	USART_InitStructure.USART_BaudRate = 9600;

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

	USART_InitStructure.USART_StopBits = USART_StopBits_1;

	USART_InitStructure.USART_Parity = USART_Parity_No ;

	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	//Write USART1 parameters

	USART_Init(USART1, &USART_InitStructure);

	//Enable USART1

	USART_Cmd(USART1, ENABLE);

}

void Usart1Put(uint8_t ch)
{
      USART_SendData(USART1, (uint8_t) ch);
      //Loop until the end of transmission
      while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {
      }
}
uint8_t Usart1Get(void){
     while ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        return (uint8_t)USART_ReceiveData(USART1);
}

void Usart1WriteAsHex(uint32_t data, uint8_t bytes)
{
	int i;
	for (i = bytes*2; i>0; i--){
	    uint8_t ch = 0;
	    uint8_t shift = 4 * (i-1);
	   	ch = (data & (0xf << shift)) >> shift;
	   	if (ch < 0xa) {
	   		Usart1Put(0x30+ch);
	   	} else {
	   		Usart1Put(0x61+(ch-0xa));
	   	}
	}
}

void Usart1WriteAsHex8(uint8_t data)
{
	Usart1WriteAsHex(data, 1);
}

void Usart1WriteAsHex16(uint16_t data)
{
	Usart1WriteAsHex(data, 2);
}

void Usart1WriteNLCR(void)
{
	Usart1Put(0xa);
	Usart1Put(0xd);
	UsartDelay(0xFF);
}

void UsartDelay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


