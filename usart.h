#ifndef __USART_H
#define __USART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdint.h>


void Usart1Init(void);
void Usart1Put(uint8_t ch);
uint8_t Usart1Get(void);
void Usart1WriteAsHex(uint32_t data, uint8_t bytes);
void Usart1WriteAsHex8(uint8_t data);
void Usart1WriteNLCR(void);
void UsartDelay(__IO uint32_t nCount);

#endif
