#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f10x.h"

void BlueTooth_Init(void);
void BlueTooth_SendData(uint8_t Byte);
uint8_t BlueTooth_ReceiveData(void);
uint8_t BlueTooth_GetRxFlag(void);

#endif