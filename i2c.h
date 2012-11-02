/*
 * i2c.h
 *
 *  Created on: Sep 1, 2012
 *      Author: raber
 */
#include "stm32f10x.h"
#include <stm32f10x_i2c.h>

#ifndef I2C_H_
#define I2C_H_

void I2C_Setup(void);
uint8_t I2C_ReadShort(uint16_t reg_address, uint16_t dev_address);
uint8_t I2C_WriteShort(uint16_t reg_address, uint16_t dev_address, uint8_t data);

#endif /* I2C_H_ */
