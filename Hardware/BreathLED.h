#ifndef __BREATHLED_H
#define __BREATHLED_H
#include "stm32f10x.h"                  // Device header

extern volatile uint8_t BreathLED_LEDState;

void BreathLED_UpdateLighTtime();

#endif
