#ifndef __AD_H
#define __AD_H 

#include "stm32f10x.h"

void AD_Init(void);
uint16_t AD_GetValue(void);
float AD_GetVoltage(void);
float AD_GetFilteredADC(void);

#endif