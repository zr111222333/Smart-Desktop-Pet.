#ifndef __SERIAL_H
#define __SERIAL_H
#include "stm32f10x.h"                  // Device header


void Serial_Init(void);
void Serial_SendData(uint8_t Byte);
uint8_t Serial_ReceiveData(void);
uint8_t Serial_GetRxFlag(void);

#endif
